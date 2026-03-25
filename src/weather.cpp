/**
 * @file weather.cpp
 * @brief Реализация менеджера погоды
 */

#include "weather.h"
#include "config.h"
#include "constants.h"
#include <Arduino.h>

WeatherManager::WeatherManager()
    : _client(nullptr)
    , _lastUpdate(0)
    , _initialized(false)
{
    _data.temperature = 0.0f;
    _data.feelsLike = 0.0f;
    _data.humidity = 0.0f;
    _data.pressure = 0.0f;
    _data.windSpeed = 0.0f;
    _data.valid = false;
    _data.timestamp = 0;
}

WeatherManager::~WeatherManager() {
    if (_client != nullptr) {
        delete _client;
        _client = nullptr;
    }
}

void WeatherManager::begin() {
    _initialized = true;
    Serial.println(F("[Weather] Initialized"));
}

bool WeatherManager::needsUpdate() const {
    if (!_initialized) {
        return false;
    }

    // Если данных нет - нужно обновить
    if (!_data.valid) {
        return true;
    }

    // Проверка периода обновления
    return (millis() - _lastUpdate >= WEATHER_UPDATE_PERIOD);
}

bool WeatherManager::update(bool force) {
    if (!_initialized) {
        return false;
    }

    // Проверка периода (если не принудительное)
    if (!force && !needsUpdate()) {
        return _data.valid;
    }

    Serial.println(F("[Weather] Updating..."));

    // Создание клиента для запроса
    // ВНИМАНИЕ: new/delete используются из-за архитектуры библиотеки
    _client = new OpenWeatherMapOneCall();

    _client->setMetric(OPENWEATHER_METRIC);
    _client->setLanguage(OPENWEATHER_LANGUAGE);

    // Выполнение запроса (блокирующий вызов!)
    _client->update(&_apiData, OPENWEATHER_API_KEY,
                    LOCATION_LATITUDE, LOCATION_LONGITUDE);

    // Освобождение клиента
    delete _client;
    _client = nullptr;

    // Обработка полученных данных
    _data.temperature = _apiData.current.temp;
    _data.feelsLike = _apiData.current.feels_like;
    _data.humidity = _apiData.current.humidity;
    _data.pressure = _apiData.current.pressure;
    _data.windSpeed = _apiData.current.windSpeed;
    _data.description = String(_apiData.current.weatherDescription);
    _data.valid = true;
    _data.timestamp = millis();
    _lastUpdate = millis();

    Serial.print(F("[Weather] Updated: "));
    Serial.print(_data.temperature);
    Serial.print(F("C, "));
    Serial.println(_data.description);

    return true;
}

bool WeatherManager::hasValidData() const {
    return _data.valid;
}

float WeatherManager::getTemperature() const {
    return _data.valid ? _data.temperature : 0.0f;
}

float WeatherManager::getFeelsLike() const {
    return _data.valid ? _data.feelsLike : 0.0f;
}

float WeatherManager::getHumidity() const {
    return _data.valid ? _data.humidity : 0.0f;
}

const String& WeatherManager::getDescription() const {
    static const String empty = "";
    return _data.valid ? _data.description : empty;
}

WeatherData WeatherManager::getData() const {
    return _data;
}

uint32_t WeatherManager::getLastUpdate() const {
    return _lastUpdate;
}

void WeatherManager::getDisplayString(char* buffer, size_t size) const {
    if (buffer == nullptr || size == 0) {
        return;
    }

    if (!_data.valid) {
        strncpy(buffer, " Погода недоступна", size - 1);
        buffer[size - 1] = '\0';
        return;
    }

    // Форматирование: " На улице: XX.XC, описание"
    int tempInt = (int)_data.temperature;
    int tempFrac = abs((int)((_data.temperature - tempInt) * 10)) % 10;

    snprintf(buffer, size, " На улице: %d.%dC, %s",
             tempInt, tempFrac, _data.description.c_str());
}
