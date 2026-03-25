/**
 * @file async_beep.cpp
 * @brief Реализация асинхронного менеджера звуковых сигналов
 */

#include "async_beep.h"
#include "constants.h"

AsyncBeep::AsyncBeep()
    : _pin(PIN_BEEP)
    , _activeHigh(true)
    , _state(BeepState::IDLE)
    , _count(0)
    , _currentCount(0)
    , _duration(BEEP_DURATION)
    , _pause(BEEP_DURATION)
    , _startTime(0)
    , _continuous(false)
{
}

AsyncBeep::AsyncBeep(uint8_t pin, bool activeHigh)
    : _pin(pin)
    , _activeHigh(activeHigh)
    , _state(BeepState::IDLE)
    , _count(0)
    , _currentCount(0)
    , _duration(BEEP_DURATION)
    , _pause(BEEP_DURATION)
    , _startTime(0)
    , _continuous(false)
{
}

void AsyncBeep::begin() {
    pinMode(_pin, OUTPUT);
    beepOff();

    Serial.println(F("[Beep] Initialized"));
}

void AsyncBeep::setPin(uint8_t pin, bool activeHigh) {
    _pin = pin;
    _activeHigh = activeHigh;
    pinMode(_pin, OUTPUT);
    beepOff();
}

void AsyncBeep::update() {
    if (_state == BeepState::IDLE) {
        return;
    }

    uint32_t now = millis();
    uint32_t elapsed = now - _startTime;

    switch (_state) {
    case BeepState::BEEPING:
        // Проверка окончания сигнала
        if (_continuous) {
            // Непрерывный режим с длительностью
            if (_duration > 0 && elapsed >= _duration) {
                stop();
            }
            // Если duration == 0, звучит бесконечно до stop()
        } else {
            // Обычный режим
            if (elapsed >= _duration) {
                beepOff();

                // Переход к паузе или завершение
                _currentCount++;

                if (_currentCount >= _count) {
                    // Все сигналы воспроизведены
                    _state = BeepState::IDLE;
                } else {
                    // Пауза перед следующим сигналом
                    _state = BeepState::PAUSING;
                    _startTime = now;
                }
            }
        }
        break;

    case BeepState::PAUSING:
        // Проверка окончания паузы
        if (elapsed >= _pause) {
            // Начало следующего сигнала
            _state = BeepState::BEEPING;
            _startTime = now;
            beepOn();
        }
        break;

    case BeepState::IDLE:
    default:
        break;
    }
}

void AsyncBeep::play(uint8_t count, uint32_t duration, uint32_t pause) {
    if (count == 0) {
        return;
    }

    // Если уже воспроизводится - останавливаем
    if (_state != BeepState::IDLE) {
        stop();
    }

    _count = count;
    _currentCount = 0;
    _duration = duration;
    _pause = (pause > 0) ? pause : duration;
    _continuous = false;
    _state = BeepState::BEEPING;
    _startTime = millis();

    beepOn();
}

void AsyncBeep::playContinuous(uint32_t duration) {
    if (_state != BeepState::IDLE) {
        stop();
    }

    _duration = duration;
    _continuous = true;
    _state = BeepState::BEEPING;
    _startTime = millis();

    beepOn();
}

void AsyncBeep::stop() {
    beepOff();
    _state = BeepState::IDLE;
    _continuous = false;
}

bool AsyncBeep::isPlaying() const {
    return _state != BeepState::IDLE;
}

bool AsyncBeep::isIdle() const {
    return _state == BeepState::IDLE;
}

uint8_t AsyncBeep::getRemainingBeeps() const {
    if (_continuous) {
        return 0;
    }
    return _count - _currentCount;
}

void AsyncBeep::beepOn() {
    digitalWrite(_pin, _activeHigh ? HIGH : LOW);
}

void AsyncBeep::beepOff() {
    digitalWrite(_pin, _activeHigh ? LOW : HIGH);
}
