/**
 * @file weather.h
 * @brief Менеджер погоды (OpenWeatherMap API)
 * @author Clock Informer Team
 *
 * Обеспечивает получение данных о погоде с OpenWeatherMap API
 * с периодическим обновлением и обработкой ошибок.
 */

#pragma once

#include <Arduino.h>
#include <OpenWeatherMapOneCall.h>

/**
 * @struct WeatherData
 * @brief Структура для хранения данных о погоде
 */
struct WeatherData {
    float temperature;          ///< Температура в градусах
    float feelsLike;            ///< Ощущаемая температура
    float humidity;             ///< Влажность в процентах
    float pressure;             ///< Давление в hPa
    float windSpeed;            ///< Скорость ветра в м/с
    String description;         ///< Описание погоды
    String icon;                ///< Код иконки погоды
    bool valid;                 ///< Флаг валидности данных
    uint32_t timestamp;         ///< Время последнего обновления
};

/**
 * @class WeatherManager
 * @brief Управляет получением данных о погоде
 *
 * Предоставляет интерфейс для получения данных с OpenWeatherMap API
 * с периодическим обновлением.
 *
 * @example
 * @code
 * WeatherManager weather;
 *
 * void setup() {
 *     weather.begin();
 * }
 *
 * void loop() {
 *     if (weather.needsUpdate()) {
 *         weather.update();
 *     }
 *
 *     if (weather.hasValidData()) {
 *         float temp = weather.getTemperature();
 *     }
 * }
 * @endcode
 */
class WeatherManager {
public:
    /**
     * @brief Конструктор
     */
    WeatherManager();

    /**
     * @brief Деструктор
     */
    ~WeatherManager();

    /**
     * @brief Инициализация менеджера погоды
     */
    void begin();

    /**
     * @brief Проверка необходимости обновления
     *
     * @return true если прошло достаточно времени с последнего обновления
     */
    bool needsUpdate() const;

    /**
     * @brief Обновить данные о погоде
     *
     * Выполняет запрос к OpenWeatherMap API.
     * ВНИМАНИЕ: Этот метод блокирует выполнение на несколько секунд!
     *
     * @param force Принудительное обновление
     * @return true если обновление успешно
     */
    bool update(bool force = false);

    // ========== Получение данных ==========

    /**
     * @brief Проверка валидности данных
     *
     * @return true если данные валидны
     */
    bool hasValidData() const;

    /**
     * @brief Получить температуру
     *
     * @return float температура в градусах Цельсия
     */
    float getTemperature() const;

    /**
     * @brief Получить ощущаемую температуру
     *
     * @return float ощущаемая температура
     */
    float getFeelsLike() const;

    /**
     * @brief Получить влажность
     *
     * @return float влажность в процентах
     */
    float getHumidity() const;

    /**
     * @brief Получить описание погоды
     *
     * @return const String& описание погоды
     */
    const String& getDescription() const;

    /**
     * @brief Получить все данные
     *
     * @return WeatherData структура с данными
     */
    WeatherData getData() const;

    /**
     * @brief Получить время последнего обновления
     *
     * @return uint32_t время в миллисекундах
     */
    uint32_t getLastUpdate() const;

    /**
     * @brief Получить строку для отображения на дисплее
     *
     * @param buffer Буфер для записи
     * @param size Размер буфера
     */
    void getDisplayString(char* buffer, size_t size) const;

private:
    OpenWeatherMapOneCall* _client;     ///< Клиент API
    OpenWeatherMapOneCallData _apiData; ///< Данные от API
    WeatherData _data;                  ///< Обработанные данные
    uint32_t _lastUpdate;               ///< Время последнего обновления
    bool _initialized;                  ///< Флаг инициализации
};
