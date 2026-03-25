/**
 * @file network.h
 * @brief Менеджер сетевого подключения (WiFi + NTP)
 * @author Clock Informer Team
 *
 * Обеспечивает асинхронное подключение к WiFi и синхронизацию времени
 * через NTP серверы.
 */

#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

/**
 * @enum NetworkState
 * @brief Состояния сетевого подключения
 */
enum class NetworkState {
    DISCONNECTED,   ///< Отключено от WiFi
    CONNECTING,     ///< Подключение к WiFi
    CONNECTED,      ///< Подключено к WiFi
    ERROR           ///< Ошибка подключения
};

/**
 * @class NetworkManager
 * @brief Управляет WiFi подключением и NTP синхронизацией
 *
 * Предоставляет неблокирующие методы для подключения к WiFi
 * и получения текущего времени через NTP.
 *
 * @example
 * @code
 * NetworkManager network;
 *
 * void setup() {
 *     network.begin();
 * }
 *
 * void loop() {
 *     network.update();
 *
 *     if (network.isConnected()) {
 *         String time = network.getFormattedTime();
 *     }
 * }
 * @endcode
 */
class NetworkManager {
public:
    /**
     * @brief Конструктор по умолчанию
     */
    NetworkManager();

    /**
     * @brief Деструктор
     */
    ~NetworkManager();

    /**
     * @brief Инициализация сетевого менеджера
     *
     * Настраивает WiFi в режиме станции и инициализирует NTP клиент.
     * Не начинает подключение автоматически.
     */
    void begin();

    /**
     * @brief Начать подключение к WiFi
     *
     * Запускает асинхронное подключение к WiFi сети.
     * Используйте isConnected() или update() для проверки статуса.
     *
     * @param ssid Имя WiFi сети (если nullptr, использует из config.h)
     * @param password Пароль WiFi (если nullptr, использует из config.h)
     */
    void connect(const char* ssid = nullptr, const char* password = nullptr);

    /**
     * @brief Обновление состояния (вызывать в loop())
     *
     * Проверяет статус WiFi подключения и обновляет NTP клиент.
     *
     * @return true если подключено к WiFi
     */
    bool update();

    /**
     * @brief Проверка подключения к WiFi
     *
     * @return true если подключено к WiFi
     */
    bool isConnected() const;

    /**
     * @brief Получить текущее состояние сети
     *
     * @return NetworkState текущее состояние
     */
    NetworkState getState() const;

    /**
     * @brief Получить IP адрес устройства
     *
     * @return IPAddress или 0.0.0.0 если не подключено
     */
    IPAddress getLocalIP() const;

    /**
     * @brief Получить уровень сигнала WiFi
     *
     * @return int32_t RSSI в dBm, или 0 если не подключено
     */
    int32_t getRSSI() const;

    // ========== NTP методы ==========

    /**
     * @brief Обновить время с NTP сервера
     *
     * Неблокирующий метод. Проверяет, нужно ли обновление.
     *
     * @param force Принудительное обновление (игнорировать период)
     * @return true если обновление успешно или не требуется
     */
    bool updateNTP(bool force = false);

    /**
     * @brief Получить текущий час
     *
     * @return uint8_t часы (0-23)
     */
    uint8_t getHours() const;

    /**
     * @brief Получить текущую минуту
     *
     * @return uint8_t минуты (0-59)
     */
    uint8_t getMinutes() const;

    /**
     * @brief Получить текущую секунду
     *
     * @return uint8_t секунды (0-59)
     */
    uint8_t getSeconds() const;

    /**
     * @brief Получить день недели
     *
     * @return uint8_t день недели (0 = воскресенье, 6 = суббота)
     */
    uint8_t getDayOfWeek() const;

    /**
     * @brief Получить форматированное время (ЧЧ:ММ)
     *
     * @return String время в формате "HH:MM"
     */
    String getFormattedTime() const;

    /**
     * @brief Получить форматированную дату и время
     *
     * @return String дата и время в формате "YYYY-MM-DDTHH:MM:SS"
     */
    String getFormattedDate() const;

    /**
     * @brief Получить год
     *
     * @return int год (например, 2026)
     */
    int getYear() const;

    /**
     * @brief Получить месяц (1-12)
     *
     * @return int месяц
     */
    int getMonth() const;

    /**
     * @brief Получить день месяца (1-31)
     *
     * @return int день
     */
    int getDay() const;

    /**
     * @brief Получить метку времени Unix
     *
     * @return unsigned long секунды с 1970-01-01
     */
    unsigned long getEpochTime() const;

private:
    WiFiUDP _ntpUDP;                ///< UDP клиент для NTP
    NTPClient* _timeClient;         ///< NTP клиент
    NetworkState _state;            ///< Текущее состояние
    uint32_t _connectStartTime;     ///< Время начала подключения
    bool _ntpInitialized;           ///< Флаг инициализации NTP
};
