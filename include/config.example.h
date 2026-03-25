/**
 * @file config.example.h
 * @brief Шаблон конфигурации устройства
 *
 * Инструкция:
 * 1. Скопируйте этот файл и переименуйте в config.h
 * 2. Заполните все поля своими данными
 * 3. config.h НЕ будет добавлен в git (добавлен в .gitignore)
 */

#pragma once

// ============================================================================
// WiFi Configuration
// ============================================================================

/// Имя WiFi сети (SSID)
constexpr const char* WIFI_SSID = "YOUR_WIFI_SSID";

/// Пароль WiFi сети
constexpr const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

/// Таймаут подключения к WiFi (миллисекунды)
constexpr uint32_t WIFI_CONNECT_TIMEOUT = 30000;

// ============================================================================
// Telegram Bot Configuration
// ============================================================================

/// Токен Telegram бота (получить у @BotFather)
/// Пример: "1234567890:ABCdefGHIjklMNOpqrsTUVwxyz"
constexpr const char* TELEGRAM_BOT_TOKEN = "YOUR_BOT_TOKEN";

// ============================================================================
// OpenWeatherMap Configuration
// ============================================================================

/// API ключ OpenWeatherMap (получить на https://openweathermap.org/api)
constexpr const char* OPENWEATHER_API_KEY = "YOUR_API_KEY";

/// Язык для погодных данных (ru, en, de, etc.)
constexpr const char* OPENWEATHER_LANGUAGE = "ru";

/// Использовать метрическую систему (true) или имперскую (false)
constexpr bool OPENWEATHER_METRIC = true;

/// Широта местоположения (найти на maps.google.com)
constexpr float LOCATION_LATITUDE = 0.0f;

/// Долгота местоположения
constexpr float LOCATION_LONGITUDE = 0.0f;

// ============================================================================
// NTP Configuration
// ============================================================================

/// Смещение времени в секундах для вашего часового пояса
/// Москва (UTC+3): 10800
/// Киев (UTC+2): 7200
/// Лондон (UTC+0): 0
/// Нью-Йорк (UTC-5): -18000
constexpr int32_t NTP_TIME_OFFSET = 10800;

// ============================================================================
// OTA Configuration
// ============================================================================

/// Имя устройства для OTA обновлений
constexpr const char* OTA_HOSTNAME = "clock-informer";

/// OTA порт (обычно не нужно менять)
constexpr uint16_t OTA_PORT = 8266;
