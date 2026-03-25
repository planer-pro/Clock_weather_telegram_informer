/**
 * @file async_beep.h
 * @brief Асинхронный менеджер звуковых сигналов
 * @author Clock Informer Team
 *
 * Обеспечивает неблокирующее воспроизведение звуковых сигналов
 * через буззер.
 */

#pragma once

#include <Arduino.h>

/**
 * @enum BeepState
 * @brief Состояния звукового сигнала
 */
enum class BeepState {
    IDLE,       ///< Ожидание
    BEEPING,    ///< Воспроизведение
    PAUSING     ///< Пауза между сигналами
};

/**
 * @class AsyncBeep
 * @brief Управляет неблокирующими звуковыми сигналами
 *
 * Заменяет блокирующий delay() на state machine с millis().
 *
 * @example
 * @code
 * AsyncBeep beep;
 *
 * void setup() {
 *     beep.begin();
 * }
 *
 * void loop() {
 *     beep.update();
 *
 *     // Запуск звукового сигнала
 *     beep.play(2, 80);  // 2 сигнала по 80мс
 * }
 * @endcode
 */
class AsyncBeep {
public:
    /**
     * @brief Конструктор
     */
    AsyncBeep();

    /**
     * @brief Конструктор с параметрами
     *
     * @param pin Номер пина буззера
     * @param activeHigh true если активный уровень высокий
     */
    AsyncBeep(uint8_t pin, bool activeHigh = true);

    /**
     * @brief Инициализация буззера
     */
    void begin();

    /**
     * @brief Установить пин буззера
     *
     * @param pin Номер пина
     * @param activeHigh true если активный уровень высокий
     */
    void setPin(uint8_t pin, bool activeHigh = true);

    /**
     * @brief Обновление состояния (вызывать в loop())
     */
    void update();

    /**
     * @brief Запустить звуковой сигнал
     *
     * @param count Количество сигналов
     * @param duration Длительность одного сигнала (мс)
     * @param pause Пауза между сигналами (мс), по умолчанию = duration
     */
    void play(uint8_t count = 2, uint32_t duration = 80, uint32_t pause = 0);

    /**
     * @brief Запустить непрерывный сигнал
     *
     * @param duration Длительность (мс), 0 = бесконечно
     */
    void playContinuous(uint32_t duration = 0);

    /**
     * @brief Остановить сигнал
     */
    void stop();

    /**
     * @brief Проверить, воспроизводится ли сигнал
     *
     * @return true если сигнал воспроизводится
     */
    bool isPlaying() const;

    /**
     * @brief Проверить, свободен ли буззер
     *
     * @return true если буззер не занят
     */
    bool isIdle() const;

    /**
     * @brief Получить оставшееся количество сигналов
     *
     * @return uint8_t количество оставшихся сигналов
     */
    uint8_t getRemainingBeeps() const;

private:
    /**
     * @brief Включить буззер
     */
    void beepOn();

    /**
     * @brief Выключить буззер
     */
    void beepOff();

    uint8_t _pin;               ///< Пин буззера
    bool _activeHigh;           ///< Активный уровень
    BeepState _state;           ///< Текущее состояние
    uint8_t _count;             ///< Количество сигналов
    uint8_t _currentCount;      ///< Текущий номер сигнала
    uint32_t _duration;         ///< Длительность сигнала
    uint32_t _pause;            ///< Пауза между сигналами
    uint32_t _startTime;        ///< Время начала текущей фазы
    bool _continuous;           ///< Режим непрерывного сигнала
};
