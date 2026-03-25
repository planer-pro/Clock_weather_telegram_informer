/**
 * @file main.cpp
 * @brief Точка входа приложения ESP8266 Clock Informer
 *
 * Устройство отображает время, дату, температуру и погоду на LED матрице MAX7219.
 * Управление через Telegram бота.
 *
 * @author Clock Informer Team
 * @date 2026-03-25
 */

#include <Arduino.h>

// Модули проекта
#include "config.h"
#include "constants.h"

// Менеджеры
#include "network.h"
#include "display.h"
#include "sensors.h"
#include "weather.h"
#include "telegram_bot.h"
#include "async_beep.h"
#include "utils.h"

// ============================================================================
// Глобальные объекты
// ============================================================================

/// Менеджер сетевого подключения (WiFi + NTP)
NetworkManager network;

/// Менеджер дисплея MAX7219
DisplayManager display;

/// Менеджер датчиков (DHT22)
SensorsManager sensors;

/// Менеджер погодных данных
WeatherManager weather;

/// Менеджер Telegram бота
TelegramBotManager telegram;

/// Неблокирующий буззер
AsyncBeep beep;

// ============================================================================
// State Machine - Основные режимы работы
// ============================================================================

/**
 * @brief Состояние главного цикла
 *
 * State machine для неблокирующего переключения между экранами
 */
enum class MainState {
    /// Режим ожиданияWiFi подключения
    INITIALIZING,
    /// Режим подключения к WiFi
    CONNECTING_WIFI,
    /// Режим обновления данных
    UPDATING_DATA,
    /// Отображение времени
    SHOW_TIME,
    /// Отображение даты
    SHOW_DATE,
    /// Отображение температуры внутри
    SHOW_INDOOR_TEMP,
    /// Отображение погоды
    SHOW_WEATHER,
    /// Отображение сообщения из Telegram
    SHOW_MESSAGE,
    /// Режим работы (обычный цикл)
    NORMAL
};

/// Текущее состояние state machine
MainState currentState = MainState::INITIALIZING;

/// Следующее состояние для перехода
MainState nextState = MainState::SHOW_TIME;

/// Счётчик циклов (для последовательного отображения)
uint8_t cycleCounter = 0;

/// Состояние обновления погоды (для асинхронности)
bool weatherUpdatePending = false;

/// Сообщение из Telegram для отображения
String customMessage;

// ============================================================================
// Временные метки
// ============================================================================

/// Время последнего обновления NTP
uint32_t lastNtpUpdate = 0;

/// Время последнего чтения DHT
uint32_t lastDhtRead = 0;

/// Время последнего обновления погоды
uint32_t lastWeatherUpdate = 0;

// ============================================================================
// Функции обратного вызова
// ============================================================================

/**
 * @brief Callback для отображения сообщения из Telegram
 *
 * Вызывается когда пользователь отправляет текстовое сообщение (не команду).
 */
void onTelegramMessage(const String& message) {
    Serial.print(F("[Callback] Telegram message: "));
    Serial.println(message);

    // Сохранить сообщение для отображения
    customMessage = message;

    // Воспроизвести звуковой сигнал
    beep.play(BEEP_COUNT, BEEP_DURATION, BEEP_DURATION);

    // Переключить состояние
    currentState = MainState::SHOW_MESSAGE;
}

// ============================================================================
// Вспомогательные функции
// ============================================================================

/**
 * @brief Обновление всех периодических данных
 *
 * Вызывается каждый цикл для проверки необходимости обновления
 */
void updatePeriodicData() {
    uint32_t now = millis();

    // Обновление NTP (каждые NTP_PERIOD)
    if (now - lastNtpUpdate >= NTP_UPDATE_PERIOD) {
        network.updateNTP(true);
        lastNtpUpdate = now;
    }

    // Чтение датчика DHT (каждые DHT_READ_PERIOD)
    if (now - lastDhtRead >= DHT_READ_PERIOD) {
        sensors.update();
        lastDhtRead = now;
    }

    // Обновление погоды (каждые WEATHER_UPDATE_PERIOD)
    // Выполняется асинхронно - запрос отправляется, ответ обрабатывается
    if (now - lastWeatherUpdate >= WEATHER_UPDATE_PERIOD || !weather.hasValidData()) {
        weather.update();
        lastWeatherUpdate = now;
    }
}

/**
 * @brief Форматирование и отображение времени
 */
void showTimeDisplay() {
    char timeBuf[16];
    formatTime(network.getHours(), network.getMinutes(), timeBuf, sizeof(timeBuf));
    display.showTime(timeBuf, true);
}

/**
 * @brief Форматирование и отображение даты
 */
void showDateDisplay() {
    char dateBuf[32];
    formatDateRussian(network.getDay(), network.getMonth(), network.getYear(),
                    network.getDayOfWeek(), dateBuf, sizeof(dateBuf));
    display.showDate(dateBuf);
}

/**
 * @brief Отображение температуры внутри
 */
void showIndoorTempDisplay() {
    if (sensors.hasValidData()) {
        float t = sensors.getTemperature();
        float h = sensors.getHumidity();
        display.showIndoorTemp(t, h);
    } else {
        display.showCustomMessage(" Ошибка датчика DHT");
    }
}

/**
 * @brief Отображение погоды
 */
void showWeatherDisplay() {
    if (weather.hasValidData()) {
        float t = weather.getTemperature();
        const String& desc = weather.getDescription();
        display.showOutdoorTemp(t, desc.c_str());
    } else {
        display.showCustomMessage(" Погода недоступна");
    }
}

/**
 * @brief Отображение сообщения из Telegram
 */
void showMessageDisplay() {
    display.showCustomMessage(customMessage.c_str(), true);
}

// ============================================================================
// Основной цикл - State Machine
// ============================================================================

/**
 * @brief Обработка state machine
 *
 * Вызывается в каждом loop() для неблокирующего переключения состояний
 */
void processStateMachine() {
    // Состояние WAITING - ждём завершения анимации
    if (!display.update()) {
        return; // Анимация продолжается
    }

    // Анимация завершена - проверяем текущее состояние
    switch (currentState) {
    // ========== Инициализация и подключение ==========
    case MainState::INITIALIZING:
        display.showStartup();
        currentState = MainState::CONNECTING_WIFI;
        break;

    case MainState::CONNECTING_WIFI:
        network.connect(WIFI_SSID, WIFI_PASSWORD);
        currentState = MainState::UPDATING_DATA;
        break;

    case MainState::UPDATING_DATA:
        // Проверка подключения WiFi
        if (!network.isConnected()) {
            network.update();
            return; // Ожидаем подключения
        }

        // Подключение к Telegram
        if (!telegram.isConnected()) {
            telegram.begin();
        }

        // Переход к нормальному режиму
        currentState = MainState::NORMAL;
        break;

    // ========== Нормальный режим работы ==========
    case MainState::NORMAL:
        // Определяем следующий экран на основе счётчика
        switch (cycleCounter) {
        case 0:
            currentState = MainState::SHOW_TIME;
            break;
        case 1:
            currentState = MainState::SHOW_DATE;
            break;
        case 2:
            currentState = MainState::SHOW_INDOOR_TEMP;
            break;
        case 3:
            currentState = MainState::SHOW_WEATHER;
            break;
        }

        cycleCounter = (cycleCounter + 1) % 4;
        break;

    // ========== Отображение экранов ==========
    case MainState::SHOW_TIME:
        showTimeDisplay();
        // После завершения этой анимации перейдём к следующему экрану
        nextState = MainState::NORMAL;
        break;

    case MainState::SHOW_DATE:
        showDateDisplay();
        nextState = MainState::NORMAL;
        break;

    case MainState::SHOW_INDOOR_TEMP:
        showIndoorTempDisplay();
        nextState = MainState::NORMAL;
        break;

    case MainState::SHOW_WEATHER:
        showWeatherDisplay();
        nextState = MainState::NORMAL;
        break;

    case MainState::SHOW_MESSAGE:
        showMessageDisplay();
        // После сообщения вернёмся к нормальному циклу
        nextState = MainState::NORMAL;
        break;
    }
}

// ============================================================================
// Arduino Setup & Loop
// ============================================================================

/**
 * @brief Инициализация всех компонентов
 */
void setup() {
    // Инициализация Serial
    Serial.begin(115200);
    Serial.println(F("\n"));
    Serial.println(F("========================================"));
    Serial.println(F("Clock Informer v2.0"));
    Serial.println(F("ESP8266 Weather & Telegram Display"));
    Serial.println(F("========================================"));

    // Инициализация генератора случайных чисел
    randomSeed(analogRead(A0));

    // Инициализация модулей
    beep.begin();
    network.begin();
    display.begin();
    sensors.begin();
    weather.begin();

    // Настройка Telegram бота
    telegram.setModules(&display, &sensors, &weather);
    telegram.setMessageCallback(onTelegramMessage);

    // Начальная яркость
    display.setIntensity(DEFAULT_DISPLAY_INTENSITY);

    Serial.println(F("[System] Setup complete"));
}

/**
 * @brief Главный цикл приложения
 *
 * Неблокирующий цикл с state machine для отображения
 */
void loop() {
    // Обновление сети (WiFi + OTA + NTP)
    network.update();

    // Если WiFi не подключён - ничего больше не делаем
    if (!network.isConnected()) {
        return;
    }

    // Обновление периодических данных
    updatePeriodicData();

    // Обновление Telegram бота (приём сообщений)
    telegram.update();

    // Обновление буззера
    beep.update();

    // Обработка state machine для отображения
    processStateMachine();

    // Переход к следующему состоянию после завершения анимации
    if (display.isAnimating() == false && currentState != MainState::UPDATING_DATA) {
        if (nextState != MainState::NORMAL) {
            currentState = nextState;
        }
    }
}
