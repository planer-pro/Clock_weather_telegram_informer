/**
 * @file constants.h
 * @brief Константы устройства: пины, тайминги, размеры буферов
 */

#pragma once

#include <MD_Parola.h>

// ============================================================================
// Hardware Pin Definitions
// ============================================================================

/// Тип контроллера матрицы
constexpr auto HARDWARE_TYPE = MD_MAX72XX::FC16_HW;

/// Количество модулей MAX7219
constexpr uint8_t MAX_DEVICES = 4;

/// Пин SPI Clock (CLK)
constexpr uint8_t PIN_CLK = 14;

/// Пин SPI Data (MOSI)
constexpr uint8_t PIN_DATA = 13;

/// Пин SPI Chip Select (CS)
constexpr uint8_t PIN_CS = 12;

/// Пин буззера
constexpr uint8_t PIN_BEEP = 5;

/// Пин датчика DHT22
constexpr uint8_t PIN_DHT = 2;

// ============================================================================
// Timing Constants (milliseconds)
// ============================================================================

/// Период опроса Telegram (мс)
constexpr uint32_t TELEGRAM_POLL_PERIOD = 500;

/// Период обновления NTP времени (мс)
constexpr uint32_t NTP_UPDATE_PERIOD = 30000;

/// Период чтения датчика DHT (мс)
constexpr uint32_t DHT_READ_PERIOD = 10000;

/// Период обновления погоды (мс)
constexpr uint32_t WEATHER_UPDATE_PERIOD = 600000; // 10 минут

/// Длительность одного звукового сигнала (мс)
constexpr uint32_t BEEP_DURATION = 80;

/// Количество звуковых сигналов
constexpr uint8_t BEEP_COUNT = 2;

/// Время отображения каждого экрана (мс)
constexpr uint16_t DISPLAY_PAUSE_TIME = 4000;

// ============================================================================
// Buffer Sizes
// ============================================================================

/// Размер буфера для отображаемого текста
/// 4 модуля x 8 пикселей = 32 символа максимум с небольшим запасом
constexpr uint16_t BUF_SIZE = 64;

// ============================================================================
// Display Effects
// ============================================================================

/// Начальная яркость дисплея (0-15)
constexpr uint8_t DEFAULT_DISPLAY_INTENSITY = 2;

/**
 * @brief Структура для хранения параметров эффекта отображения
 */
struct EffectCatalog {
    textEffect_t effect;    ///< Тип эффекта
    const char* name;       ///< Название эффекта (для отладки)
    uint16_t speed;         ///< Скорость анимации
    uint16_t pause;         ///< Пауза после завершения
};

/// Каталог эффектов для отображения времени
constexpr EffectCatalog EFFECTS[] = {
    {PA_PRINT,          "PRINT",           40,  3000},
    {PA_SCROLL_UP,      "SCROLL_UP",       40,  3000},
    {PA_SCROLL_DOWN,    "SCROLL_DOWN",     40,  3000},
    {PA_SCROLL_LEFT,    "SCROLL_LEFT",     40,  3000},
    {PA_SCROLL_RIGHT,   "SCROLL_RIGHT",    40,  3000},
    {PA_SPRITE,         "SPRITE",          40,  3000},
    {PA_SLICE,          "SLICE",           8,   3000},
    {PA_MESH,           "MESH",            80,  3000},
    {PA_FADE,           "FADE",            125, 3000},
    {PA_DISSOLVE,       "DISSOLVE",        250, 3000},
    {PA_BLINDS,         "BLINDS",          60,  3000},
    {PA_RANDOM,         "RANDOM",          25,  3000},
    {PA_WIPE,           "WIPE",            40,  3000},
    {PA_WIPE_CURSOR,    "WIPE_CURSOR",     40,  3000},
    {PA_SCAN_HORIZ,     "SCAN_HORIZ",      40,  3000},
    {PA_SCAN_HORIZX,    "SCAN_HORIZX",     40,  3000},
    {PA_SCAN_VERT,      "SCAN_VERT",       40,  3000},
    {PA_SCAN_VERTX,     "SCAN_VERTX",      40,  3000},
    {PA_OPENING,        "OPENING",         40,  3000},
    {PA_OPENING_CURSOR, "OPENING_CURSOR",  40,  3000},
    {PA_CLOSING,        "CLOSING",         40,  3000},
    {PA_CLOSING_CURSOR, "CLOSING_CURSOR",  40,  3000},
    {PA_SCROLL_UP_LEFT, "SCROLL_UP_LEFT",  40,  3000},
    {PA_SCROLL_UP_RIGHT,"SCROLL_UP_RIGHT", 40,  3000},
    {PA_SCROLL_DOWN_LEFT, "SCROLL_DOWN_LEFT", 40, 3000},
    {PA_SCROLL_DOWN_RIGHT, "SCROLL_DOWN_RIGHT", 40, 3000},
    {PA_GROW_UP,        "GROW_UP",         40,  3000},
    {PA_GROW_DOWN,      "GROW_DOWN",       40,  3000},
};

/// Количество эффектов в каталоге
constexpr uint8_t EFFECTS_COUNT = sizeof(EFFECTS) / sizeof(EffectCatalog);

/// Индекс первого эффекта для случайного выбора (пропускаем простые)
constexpr uint8_t RANDOM_EFFECT_MIN = 6;

/// Индекс последнего эффекта для случайного выбора
constexpr uint8_t RANDOM_EFFECT_MAX = EFFECTS_COUNT - 1;

// ============================================================================
// Russian Localization
// ============================================================================

/// Дни недели на русском (начиная с воскресенья)
constexpr const char* WEEKDAYS_RU[] = {
    " Воскресенье",
    " Понедельник",
    " Вторник",
    " Среда",
    " Четверг",
    " Пятница",
    " Суббота"
};

/// Месяцы на русском (в родительном падеже)
constexpr const char* MONTHS_RU[] = {
    "января",
    "февраля",
    "марта",
    "апреля",
    "мая",
    "июня",
    "июля",
    "августа",
    "сентября",
    "октября",
    "ноября",
    "декабря"
};

/// Стартовое сообщение при включении
constexpr const char* STARTUP_MESSAGE = "Home Technologies";
