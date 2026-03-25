/**
 * @file display.h
 * @brief Менеджер светодиодной матрицы MAX7219
 * @author Clock Informer Team
 *
 * Обеспечивает неблокирующее отображение текста с различными
 * эффектами анимации на LED матрице.
 */

#pragma once

#include <Arduino.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include "constants.h"

/**
 * @enum DisplayMode
 * @brief Режимы отображения информации
 */
enum class DisplayMode {
    TIME,           ///< Время (ЧЧ:ММ)
    DATE,           ///< Дата (день недели, число, месяц, год)
    INDOOR_TEMP,    ///< Температура и влажность внутри
    OUTDOOR_TEMP,   ///< Погода на улице
    CUSTOM,         ///< Произвольное сообщение
    STARTUP         ///< Стартовое сообщение
};

/**
 * @class DisplayManager
 * @brief Управляет отображением информации на LED матрице
 *
 * Предоставляет неблокирующий интерфейс для отображения текста
 * с эффектами анимации. Использует state machine для управления
 * последовательностью отображения.
 *
 * @example
 * @code
 * DisplayManager display;
 *
 * void setup() {
 *     display.begin();
 *     display.showTime("12:30");
 * }
 *
 * void loop() {
 *     if (display.update()) {
 *         // Анимация завершена, можно показать следующее
 *         display.showDate("Понедельник 25 марта");
 *     }
 * }
 * @endcode
 */
class DisplayManager {
public:
    /**
     * @brief Конструктор
     */
    DisplayManager();

    /**
     * @brief Деструктор
     */
    ~DisplayManager();

    /**
     * @brief Инициализация дисплея
     *
     * Настраивает SPI соединение и загружает русский шрифт.
     * Должен вызываться в setup().
     */
    void begin();

    /**
     * @brief Обновление состояния дисплея
     *
     * Вызывается в каждом цикле loop() для неблокирующей
     * анимации текста.
     *
     * @return true если текущая анимация завершена
     */
    bool update();

    // ========== Методы отображения ==========

    /**
     * @brief Показать время
     *
     * @param timeStr Строка времени (например, "12:30")
     * @param randomEffect Использовать случайный эффект
     */
    void showTime(const char* timeStr, bool randomEffect = true);

    /**
     * @brief Показать дату
     *
     * @param dateStr Строка даты (например, "Понедельник 25 марта 2026")
     */
    void showDate(const char* dateStr);

    /**
     * @brief Показать температуру внутри
     *
     * @param temp Температура в градусах
     * @param humidity Влажность в процентах
     */
    void showIndoorTemp(float temp, float humidity);

    /**
     * @brief Показать погоду на улице
     *
     * @param temp Температура в градусах
     * @param description Описание погоды
     */
    void showOutdoorTemp(float temp, const char* description);

    /**
     * @brief Показать произвольное сообщение
     *
     * @param message Текст сообщения
     * @param useScroll Использовать прокрутку
     */
    void showCustomMessage(const char* message, bool useScroll = true);

    /**
     * @brief Показать стартовое сообщение
     */
    void showStartup();

    /**
     * @brief Установить режим отображения напрямую
     *
     * @param mode Режим отображения
     */
    void setMode(DisplayMode mode);

    /**
     * @brief Получить текущий режим
     *
     * @return DisplayMode текущий режим
     */
    DisplayMode getMode() const;

    // ========== Настройки дисплея ==========

    /**
     * @brief Установить яркость дисплея
     *
     * @param level Уровень яркости (0-15)
     */
    void setIntensity(uint8_t level);

    /**
     * @brief Получить текущую яркость
     *
     * @return uint8_t уровень яркости (0-15)
     */
    uint8_t getIntensity() const;

    /**
     * @brief Установить инверсию дисплея
     *
     * @param invert true для инверсии
     */
    void setInvert(bool invert);

    /**
     * @brief Очистить дисплей
     */
    void clear();

    /**
     * @brief Проверить, идет ли анимация
     *
     * @return true если анимация в процессе
     */
    bool isAnimating() const;

    /**
     * @brief Получить текущий отображаемый текст
     *
     * @return const char* указатель на буфер текста
     */
    const char* getCurrentText() const;

private:
    /**
     * @brief Установить текст с эффектом
     *
     * @param text Текст для отображения
     * @param effect Эффект анимации
     * @param speed Скорость анимации
     * @param pause Пауза после завершения
     */
    void setText(const char* text, textEffect_t effect,
                 uint16_t speed = 40, uint16_t pause = 4000);

    /**
     * @brief Получить случайный эффект
     *
     * @return textEffect_t случайный эффект из каталога
     */
    textEffect_t getRandomEffect() const;

    MD_Parola* _display;            ///< Объект Parola
    DisplayMode _currentMode;       ///< Текущий режим
    char _buffer[BUF_SIZE];         ///< Буфер для текста
    uint8_t _intensity;             ///< Текущая яркость
    bool _animationComplete;        ///< Флаг завершения анимации
};
