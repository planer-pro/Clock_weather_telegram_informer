/**
 * @file sensors.cpp
 * @brief Реализация менеджера датчиков
 */

#include "sensors.h"
#include "constants.h"
#include <Arduino.h>

SensorsManager::SensorsManager()
    : _dht(nullptr)
    , _lastRead(0)
{
    _data.temperature = NAN;
    _data.humidity = NAN;
    _data.valid = false;
    _data.timestamp = 0;
}

SensorsManager::~SensorsManager() {
    if (_dht != nullptr) {
        delete _dht;
        _dht = nullptr;
    }
}

void SensorsManager::begin() {
    _dht = new DHT(PIN_DHT, DHT22);
    _dht->begin();

    Serial.println(F("[Sensors] DHT22 initialized"));

    // Начальное чтение данных
    update(true);
}

bool SensorsManager::update(bool force) {
    uint32_t now = millis();

    // Проверка периода обновления
    if (!force && (now - _lastRead < DHT_READ_PERIOD)) {
        return false;
    }

    return readNow();
}

bool SensorsManager::readNow() {
    if (_dht == nullptr) {
        _data.valid = false;
        return false;
    }

    // Чтение данных
    float h = _dht->readHumidity();
    float t = _dht->readTemperature();

    // Проверка на ошибки чтения
    if (isnan(h) || isnan(t)) {
        Serial.println(F("[Sensors] DHT read error!"));
        _data.valid = false;
        _data.temperature = NAN;
        _data.humidity = NAN;
        _lastRead = millis();
        return false;
    }

    // Сохранение данных
    _data.temperature = t;
    _data.humidity = h;
    _data.valid = true;
    _data.timestamp = millis();
    _lastRead = millis();

    Serial.print(F("[Sensors] T="));
    Serial.print(t);
    Serial.print(F("C, H="));
    Serial.print(h);
    Serial.println(F("%"));

    return true;
}

bool SensorsManager::hasValidData() const {
    return _data.valid;
}

float SensorsManager::getTemperature() const {
    return _data.valid ? _data.temperature : NAN;
}

float SensorsManager::getHumidity() const {
    return _data.valid ? _data.humidity : NAN;
}

SensorData SensorsManager::getData() const {
    return _data;
}

uint32_t SensorsManager::getLastUpdate() const {
    return _lastRead;
}

void SensorsManager::getDisplayString(char* buffer, size_t size) const {
    if (buffer == nullptr || size == 0) {
        return;
    }

    if (!_data.valid) {
        strncpy(buffer, " Ошибка датчика DHT", size - 1);
        buffer[size - 1] = '\0';
        return;
    }

    // Форматирование температуры (1 знак после запятой)
    int tempInt = (int)_data.temperature;
    int tempFrac = abs((int)((_data.temperature - tempInt) * 10)) % 10;

    // Форматирование влажности
    int humInt = (int)_data.humidity;
    int humFrac = abs((int)((_data.humidity - humInt) * 10)) % 10;

    snprintf(buffer, size, " Температура: %d.%dC, Влажность: %d.%d%%",
             tempInt, tempFrac, humInt, humFrac);
}
