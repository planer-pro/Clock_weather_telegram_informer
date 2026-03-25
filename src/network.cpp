/**
 * @file network.cpp
 * @brief Реализация менеджера сетевого подключения
 */

#include "network.h"
#include "config.h"
#include "constants.h"
#include <ArduinoOTA.h>

NetworkManager::NetworkManager()
    : _timeClient(nullptr)
    , _state(NetworkState::DISCONNECTED)
    , _connectStartTime(0)
    , _ntpInitialized(false)
{
}

NetworkManager::~NetworkManager() {
    if (_timeClient != nullptr) {
        delete _timeClient;
        _timeClient = nullptr;
    }
}

void NetworkManager::begin() {
    // Режим станции (подключение к точке доступа)
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);

    // Настройка OTA
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPort(OTA_PORT);
    ArduinoOTA.begin();

    Serial.println(F("[Network] Initialized"));
}

void NetworkManager::connect(const char* ssid, const char* password) {
    const char* s = (ssid != nullptr) ? ssid : WIFI_SSID;
    const char* p = (password != nullptr) ? password : WIFI_PASSWORD;

    Serial.print(F("[Network] Connecting to "));
    Serial.println(s);

    WiFi.begin(s, p);
    _state = NetworkState::CONNECTING;
    _connectStartTime = millis();
}

bool NetworkManager::update() {
    // Обработка OTA
    ArduinoOTA.handle();

    // Проверка статуса WiFi
    if (_state == NetworkState::CONNECTING) {
        if (WiFi.status() == WL_CONNECTED) {
            _state = NetworkState::CONNECTED;
            Serial.println(F("[Network] Connected!"));
            Serial.print(F("[Network] IP: "));
            Serial.println(WiFi.localIP());

            // Инициализация NTP после подключения
            if (!_ntpInitialized) {
                _timeClient = new NTPClient(_ntpUDP);
                _timeClient->begin();
                _timeClient->setTimeOffset(NTP_TIME_OFFSET);
                _ntpInitialized = true;
                Serial.println(F("[Network] NTP client started"));
            }
        }
        else if (millis() - _connectStartTime > WIFI_CONNECT_TIMEOUT) {
            _state = NetworkState::ERROR;
            Serial.println(F("[Network] Connection timeout!"));
        }
    }
    else if (_state == NetworkState::CONNECTED) {
        // Проверка на потерю соединения
        if (WiFi.status() != WL_CONNECTED) {
            _state = NetworkState::DISCONNECTED;
            Serial.println(F("[Network] Connection lost!"));
            // WiFi.setAutoReconnect(true) автоматически переподключит
        }
    }

    return isConnected();
}

bool NetworkManager::isConnected() const {
    return _state == NetworkState::CONNECTED && WiFi.status() == WL_CONNECTED;
}

NetworkState NetworkManager::getState() const {
    return _state;
}

IPAddress NetworkManager::getLocalIP() const {
    return WiFi.localIP();
}

int32_t NetworkManager::getRSSI() const {
    return isConnected() ? WiFi.RSSI() : 0;
}

// ========== NTP методы ==========

bool NetworkManager::updateNTP(bool force) {
    if (!isConnected() || _timeClient == nullptr) {
        return false;
    }

    // NTP клиент использует неблокирующее обновление
    // forceUpdate() может блокировать, используем update()
    if (force) {
        _timeClient->forceUpdate();
    } else {
        _timeClient->update();
    }

    return true;
}

uint8_t NetworkManager::getHours() const {
    return (_timeClient != nullptr) ? _timeClient->getHours() : 0;
}

uint8_t NetworkManager::getMinutes() const {
    return (_timeClient != nullptr) ? _timeClient->getMinutes() : 0;
}

uint8_t NetworkManager::getSeconds() const {
    return (_timeClient != nullptr) ? _timeClient->getSeconds() : 0;
}

uint8_t NetworkManager::getDayOfWeek() const {
    return (_timeClient != nullptr) ? _timeClient->getDay() : 0;
}

String NetworkManager::getFormattedTime() const {
    return (_timeClient != nullptr) ? _timeClient->getFormattedTime() : String("--:--");
}

String NetworkManager::getFormattedDate() const {
    if (_timeClient == nullptr) {
        return String("----");
    }
    return _timeClient->getFormattedDate();
}

int NetworkManager::getYear() const {
    if (_timeClient == nullptr) {
        return 0;
    }

    String dateStr = _timeClient->getFormattedDate();
    int splitT = dateStr.indexOf("T");
    if (splitT <= 0) {
        return 0;
    }

    String yearStr = dateStr.substring(0, 4);
    return yearStr.toInt();
}

int NetworkManager::getMonth() const {
    if (_timeClient == nullptr) {
        return 0;
    }

    String dateStr = _timeClient->getFormattedDate();
    int splitT = dateStr.indexOf("T");
    if (splitT <= 0) {
        return 0;
    }

    // Формат: YYYY-MM-DDTHH:MM:SS
    String monthStr = dateStr.substring(5, 7);
    return monthStr.toInt();
}

int NetworkManager::getDay() const {
    if (_timeClient == nullptr) {
        return 0;
    }

    String dateStr = _timeClient->getFormattedDate();
    int splitT = dateStr.indexOf("T");
    if (splitT <= 0) {
        return 0;
    }

    // Формат: YYYY-MM-DDTHH:MM:SS
    String dayStr = dateStr.substring(8, 10);
    return dayStr.toInt();
}

unsigned long NetworkManager::getEpochTime() const {
    return (_timeClient != nullptr) ? _timeClient->getEpochTime() : 0;
}
