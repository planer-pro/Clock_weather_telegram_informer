/**
 * @file sensors.h
 * @brief Менеджер датчиков (DHT22)
 * @author Clock Informer Team
 *
 * Обеспечивает чтение данных с датчика температуры и влажности DHT22
 * с проверкой ошибок и кэшированием результатов.
 */

#pragma once

#include <Arduino.h>
#include <DHT.h>

/**
 * @struct SensorData
 * @brief Структура для хранения данных датчика
 */
struct SensorData {
    float temperature;      ///< Температура в градусах Цельсия
    float humidity;         ///< Влажность в процентах
    bool valid;             ///< Флаг валидности данных
    uint32_t timestamp;     ///< Время последнего чтения (millis)
};

/**
 * @class SensorsManager
 * @brief Управляет датчиками температуры и влажности
 *
 * Предоставляет интерфейс для чтения данных с DHT22 с автоматической
 * проверкой ошибок и периодическим обновлением.
 *
 * @example
 * @code
 * SensorsManager sensors;
 *
 * void setup() {
 *     sensors.begin();
 * }
 *
 * void loop() {
 *     sensors.update();
 *
 *     if (sensors.hasValidData()) {
 *         float temp = sensors.getTemperature();
 *         float hum = sensors.getHumidity();
 *     }
 * }
 * @endcode
 */
class SensorsManager {
public:
    /**
     * @brief Конструктор
     */
    SensorsManager();

    /**
     * @brief Деструктор
     */
    ~SensorsManager();

    /**
     * @brief Инициализация датчика
     *
     * Настраивает DHT22 для чтения данных.
     */
    void begin();

    /**
     * @brief Обновление данных датчика
     *
     * Проверяет, прошло ли достаточно времени с последнего
     * обновления, и при необходимости считывает новые данные.
     *
     * @param force Принудительное чтение (игнорировать период)
     * @return true если данные были обновлены
     */
    bool update(bool force = false);

    /**
     * @brief Принудительное чтение данных
     *
     * Немедленно считывает данные с датчика.
     *
     * @return true если чтение успешно
     */
    bool readNow();

    // ========== Получение данных ==========

    /**
     * @brief Проверка валидности данных
     *
     * @return true если последние данные валидны
     */
    bool hasValidData() const;

    /**
     * @brief Получить температуру
     *
     * @return float температура в градусах Цельсия, или NAN при ошибке
     */
    float getTemperature() const;

    /**
     * @brief Получить влажность
     *
     * @return float влажность в процентах, или NAN при ошибке
     */
    float getHumidity() const;

    /**
     * @brief Получить все данные датчика
     *
     * @return SensorData структура с данными
     */
    SensorData getData() const;

    /**
     * @brief Получить время последнего обновления
     *
     * @return uint32_t время в миллисекундах
     */
    uint32_t getLastUpdate() const;

    /**
     * @brief Получить строку с данными для отображения
     *
     * @param buffer Буфер для записи строки
     * @param size Размер буфера
     */
    void getDisplayString(char* buffer, size_t size) const;

private:
    DHT* _dht;              ///< Объект DHT датчика
    SensorData _data;       ///< Кэшированные данные
    uint32_t _lastRead;     ///< Время последнего чтения
};
