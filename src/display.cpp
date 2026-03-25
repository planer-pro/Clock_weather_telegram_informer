/**
 * @file display.cpp
 * @brief Реализация менеджера дисплея
 */

#include "display.h"
#include "constants.h"
#include "6bite_rus.h"
#include <Arduino.h>

DisplayManager::DisplayManager()
    : _display(nullptr)
    , _currentMode(DisplayMode::STARTUP)
    , _intensity(0)
    , _animationComplete(true)
{
    memset(_buffer, 0, BUF_SIZE);
}

DisplayManager::~DisplayManager() {
    if (_display != nullptr) {
        delete _display;
        _display = nullptr;
    }
}

void DisplayManager::begin() {
    // Создание объекта дисплея
    _display = new MD_Parola(HARDWARE_TYPE, PIN_CS, MAX_DEVICES);

    _display->begin();
    _display->setInvert(false);
    _display->setIntensity(_intensity);
    _display->setFont(0, _6bite_rus);

    // Показать стартовое сообщение
    strncpy(_buffer, STARTUP_MESSAGE, BUF_SIZE - 1);
    _buffer[BUF_SIZE - 1] = '\0';

    _display->displayText(_buffer, PA_CENTER, 25, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

    Serial.println(F("[Display] Initialized"));
}

bool DisplayManager::update() {
    if (_display == nullptr) {
        return true;
    }

    // Вызываем анимацию один раз
    _animationComplete = _display->displayAnimate();

    return _animationComplete;
}

void DisplayManager::showTime(const char* timeStr, bool randomEffect) {
    if (_display == nullptr || timeStr == nullptr) {
        return;
    }

    strncpy(_buffer, timeStr, BUF_SIZE - 1);
    _buffer[BUF_SIZE - 1] = '\0';
    _currentMode = DisplayMode::TIME;

    if (randomEffect) {
        textEffect_t effect = getRandomEffect();
        _display->displayText(_buffer, PA_CENTER, 40, DISPLAY_PAUSE_TIME,
                              effect, effect);
    } else {
        _display->displayText(_buffer, PA_CENTER, 40, DISPLAY_PAUSE_TIME,
                              PA_PRINT, PA_PRINT);
    }

    _animationComplete = false;
}

void DisplayManager::showDate(const char* dateStr) {
    if (_display == nullptr || dateStr == nullptr) {
        return;
    }

    strncpy(_buffer, dateStr, BUF_SIZE - 1);
    _buffer[BUF_SIZE - 1] = '\0';
    _currentMode = DisplayMode::DATE;

    _display->displayText(_buffer, PA_CENTER, 25, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    _animationComplete = false;
}

void DisplayManager::showIndoorTemp(float temp, float humidity) {
    if (_display == nullptr) {
        return;
    }

    // Форматирование: " Температура в доме: XX.XC, Влажность: XX%"
    // Усекаем до 1 знака после запятой
    int tempInt = (int)temp;
    int tempFrac = abs((int)((temp - tempInt) * 10)) % 10;
    int humInt = (int)humidity;
    int humFrac = abs((int)((humidity - humInt) * 10)) % 10;

    snprintf(_buffer, BUF_SIZE, " Температура: %d.%dC, Влажность: %d.%d%%",
             tempInt, tempFrac, humInt, humFrac);

    _currentMode = DisplayMode::INDOOR_TEMP;

    _display->displayText(_buffer, PA_CENTER, 25, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    _animationComplete = false;
}

void DisplayManager::showOutdoorTemp(float temp, const char* description) {
    if (_display == nullptr || description == nullptr) {
        return;
    }

    // Форматирование: " На улице: XX.XC, описание"
    int tempInt = (int)temp;
    int tempFrac = abs((int)((temp - tempInt) * 10)) % 10;

    snprintf(_buffer, BUF_SIZE, " На улице: %d.%dC, %s",
             tempInt, tempFrac, description);

    _currentMode = DisplayMode::OUTDOOR_TEMP;

    _display->displayText(_buffer, PA_CENTER, 25, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    _animationComplete = false;
}

void DisplayManager::showCustomMessage(const char* message, bool useScroll) {
    if (_display == nullptr || message == nullptr) {
        return;
    }

    strncpy(_buffer, message, BUF_SIZE - 1);
    _buffer[BUF_SIZE - 1] = '\0';
    _currentMode = DisplayMode::CUSTOM;

    if (useScroll) {
        _display->displayText(_buffer, PA_CENTER, 25, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    } else {
        _display->displayText(_buffer, PA_CENTER, 25, DISPLAY_PAUSE_TIME, PA_PRINT, PA_PRINT);
    }

    _animationComplete = false;
}

void DisplayManager::showStartup() {
    if (_display == nullptr) {
        return;
    }

    strncpy(_buffer, STARTUP_MESSAGE, BUF_SIZE - 1);
    _buffer[BUF_SIZE - 1] = '\0';
    _currentMode = DisplayMode::STARTUP;

    _display->displayText(_buffer, PA_CENTER, 25, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    _animationComplete = false;
}

void DisplayManager::setMode(DisplayMode mode) {
    _currentMode = mode;
}

DisplayMode DisplayManager::getMode() const {
    return _currentMode;
}

void DisplayManager::setIntensity(uint8_t level) {
    if (level > 15) {
        level = 15;
    }

    _intensity = level;

    if (_display != nullptr) {
        _display->setIntensity(level);
    }

    Serial.print(F("[Display] Intensity set to "));
    Serial.println(level);
}

uint8_t DisplayManager::getIntensity() const {
    return _intensity;
}

void DisplayManager::setInvert(bool invert) {
    if (_display != nullptr) {
        _display->setInvert(invert);
    }
}

void DisplayManager::clear() {
    if (_display != nullptr) {
        _display->displayClear();
    }
    memset(_buffer, 0, BUF_SIZE);
}

bool DisplayManager::isAnimating() const {
    return !_animationComplete;
}

const char* DisplayManager::getCurrentText() const {
    return _buffer;
}

void DisplayManager::setText(const char* text, textEffect_t effect,
                             uint16_t speed, uint16_t pause) {
    if (_display == nullptr || text == nullptr) {
        return;
    }

    strncpy(_buffer, text, BUF_SIZE - 1);
    _buffer[BUF_SIZE - 1] = '\0';

    _display->displayText(_buffer, PA_CENTER, speed, pause, effect, effect);
    _animationComplete = false;
}

textEffect_t DisplayManager::getRandomEffect() const {
    uint8_t idx = random(RANDOM_EFFECT_MIN, RANDOM_EFFECT_MAX + 1);
    return EFFECTS[idx].effect;
}
