/**
 * @file telegram_bot.cpp
 * @brief Реализация Telegram бота
 */

#include "telegram_bot.h"
#include "display.h"
#include "sensors.h"
#include "weather.h"
#include "config.h"
#include "constants.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>

TelegramBotManager::TelegramBotManager()
    : _display(nullptr)
    , _sensors(nullptr)
    , _weather(nullptr)
    , _messageCallback(nullptr)
    , _lastPollTime(0)
    , _connected(false)
{
}

TelegramBotManager::~TelegramBotManager() {
    _commands.clear();
}

bool TelegramBotManager::begin() {
    // Подключение к Telegram
    _bot.setTelegramToken(TELEGRAM_BOT_TOKEN);

    // Проверка соединения
    _connected = _bot.testConnection();

    if (_connected) {
        Serial.println(F("[Telegram] Connected!"));
        initDefaultCommands();
    } else {
        Serial.println(F("[Telegram] Connection failed!"));
    }

    return _connected;
}

void TelegramBotManager::setModules(DisplayManager* display,
                                     SensorsManager* sensors,
                                     WeatherManager* weather) {
    _display = display;
    _sensors = sensors;
    _weather = weather;
}

void TelegramBotManager::update() {
    uint32_t now = millis();

    // Проверка периода опроса
    if (now - _lastPollTime < TELEGRAM_POLL_PERIOD) {
        return;
    }

    _lastPollTime = now;

    // Проверка новых сообщений
    TBMessage message;

    CTBotMessageType msgType = _bot.getNewMessage(message);

    if (msgType == CTBotMessageText) {
        handleMessage(message);
    }
}

bool TelegramBotManager::isConnected() const {
    return _connected;
}

// ========== Система команд ==========

void TelegramBotManager::registerCommand(const String& name,
                                          CommandHandler handler,
                                          const String& description) {
    BotCommand cmd;
    cmd.name = name;
    cmd.description = description;
    cmd.handler = handler;

    _commands[name] = cmd;

    Serial.print(F("[Telegram] Command registered: "));
    Serial.println(name);
}

void TelegramBotManager::unregisterCommand(const String& name) {
    _commands.erase(name);
}

String TelegramBotManager::getCommandsList() const {
    String list = "Доступные команды:\n";

    for (const auto& pair : _commands) {
        list += pair.first + " - " + pair.second.description + "\n";
    }

    return list;
}

// ========== Отправка сообщений ==========

bool TelegramBotManager::sendMessage(int64_t chatId, const String& message) {
    return _bot.sendMessage(chatId, message);
}

bool TelegramBotManager::replyTo(const TBMessage& message, const String& reply) {
    return _bot.sendMessage(message.sender.id, reply);
}

void TelegramBotManager::setMessageCallback(std::function<void(const String&)> callback) {
    _messageCallback = callback;
}

// ========== Приватные методы ==========

void TelegramBotManager::handleMessage(TBMessage& message) {
    String text = message.text;
    text.trim();

    Serial.print(F("[Telegram] Message: "));
    Serial.println(text);

    // Проверка на команду
    if (text.length() > 0 && text[0] == '.') {
        handleCommand(text, message);
    } else {
        // Обычное сообщение - показать на дисплее
        replyTo(message, "Сообщение принято");

        // Вызов callback для отображения
        if (_messageCallback) {
            _messageCallback(text);
        }
    }
}

void TelegramBotManager::handleCommand(const String& text, TBMessage& message) {
    // Поиск пробела (разделитель команды и параметров)
    int spaceIdx = text.indexOf(' ');

    String cmdName;
    String params;

    if (spaceIdx > 0) {
        cmdName = text.substring(0, spaceIdx);
        params = text.substring(spaceIdx + 1);
        params.trim();
    } else {
        cmdName = text;
    }

    // Поиск команды
    auto it = _commands.find(cmdName);

    if (it != _commands.end()) {
        // Выполнение обработчика
        String response = it->second.handler(params, message);
        replyTo(message, response);
    } else {
        replyTo(message, "Неизвестная команда. Отправьте .help для списка команд.");
    }
}

void TelegramBotManager::initDefaultCommands() {
    // Команда .help - список команд
    registerCommand(".help", [this](const String& params, const TBMessage& msg) {
        return getCommandsList();
    }, "Показать список команд");

    // Команда .DBR - яркость дисплея
    registerCommand(".DBR", [this](const String& params, const TBMessage& msg) {
        // Проверка параметров
        for (size_t i = 0; i < params.length(); i++) {
            if (!isDigit(params[i]) && params[i] != ' ') {
                return String("Неверный параметр. Введите число от 0 до 15.");
            }
        }

        int brightness = params.toInt();

        if (brightness < 0 || brightness > 15) {
            return String("Параметр выходит за пределы. Введите число от 0 до 15.");
        }

        if (_display != nullptr) {
            _display->setIntensity(brightness);
            return String("Яркость установлена: ") + String(brightness);
        }

        return String("Ошибка: дисплей не инициализирован");
    }, "Установить яркость дисплея (0-15)");

    // Команда .TEMP - температура внутри
    registerCommand(".TEMP", [this](const String& params, const TBMessage& msg) {
        if (_sensors != nullptr && _sensors->hasValidData()) {
            float t = _sensors->getTemperature();
            float h = _sensors->getHumidity();
            return String("Температура: ") + String(t, 1) + "C, Влажность: " + String(h, 1) + "%";
        }
        return String("Ошибка чтения датчика");
    }, "Показать температуру и влажность");

    // Команда .WEATHER - погода на улице
    registerCommand(".WEATHER", [this](const String& params, const TBMessage& msg) {
        if (_weather != nullptr && _weather->hasValidData()) {
            float t = _weather->getTemperature();
            const String& desc = _weather->getDescription();
            return String("На улице: ") + String(t, 1) + "C, " + desc;
        }
        return String("Данные о погоде недоступны");
    }, "Показать погоду");

    // Команда .STATUS - статус устройства
    registerCommand(".STATUS", [this](const String& params, const TBMessage& msg) {
        String status = "Статус устройства:\n";

        // Память
        status += "Свободная память: " + String(ESP.getFreeHeap()) + " байт\n";

        // WiFi
        if (WiFi.isConnected()) {
            status += "WiFi: подключен\n";
            status += "IP: " + WiFi.localIP().toString() + "\n";
            status += "RSSI: " + String(WiFi.RSSI()) + " dBm\n";
        } else {
            status += "WiFi: отключен\n";
        }

        return status;
    }, "Показать статус устройства");

    // Команда .TIME - показать время на дисплее
    registerCommand(".TIME", [this](const String& params, const TBMessage& msg) {
        if (_display != nullptr) {
            _display->setMode(DisplayMode::TIME);
            return String("Режим отображения: время");
        }
        return String("Ошибка");
    }, "Показать время на дисплее");
}
