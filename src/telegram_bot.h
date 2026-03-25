/**
 * @file telegram_bot.h
 * @brief Telegram бот для управления устройством
 * @author Clock Informer Team
 *
 * Обеспечивает взаимодействие с Telegram API, обработку команд
 * и отправку сообщений.
 */

#pragma once

#include <Arduino.h>
#include <CTBot.h>
#include <functional>
#include <map>

// Форвардные объявления
class DisplayManager;
class SensorsManager;
class WeatherManager;

/**
 * @brief Тип обработчика команды
 *
 * @param params Параметры команды (без имени команды)
 * @param message Исходное сообщение
 * @return String ответ пользователю
 */
using CommandHandler = std::function<String(const String& params, const TBMessage& message)>;

/**
 * @struct BotCommand
 * @brief Информация о команде бота
 */
struct BotCommand {
    String name;            ///< Имя команды (например, ".DBR")
    String description;     ///< Описание команды
    CommandHandler handler; ///< Обработчик команды
};

/**
 * @class TelegramBotManager
 * @brief Управляет Telegram ботом
 *
 * Предоставляет расширяемую систему команд и обработку сообщений.
 *
 * @example
 * @code
 * TelegramBotManager bot;
 *
 * void setup() {
 *     bot.begin();
 *
 *     // Регистрация команды
 *     bot.registerCommand(".DBR", [](const String& params, const TBMessage& msg) {
 *         int brightness = params.toInt();
 *         display.setIntensity(brightness);
 *         return "Яркость установлена";
 *     }, "Установить яркость (0-15)");
 * }
 *
 * void loop() {
 *     bot.update();
 * }
 * @endcode
 */
class TelegramBotManager {
public:
    /**
     * @brief Конструктор
     */
    TelegramBotManager();

    /**
     * @brief Деструктор
     */
    ~TelegramBotManager();

    /**
     * @brief Инициализация бота
     *
     * Настраивает подключение к Telegram API.
     *
     * @return true если подключение успешно
     */
    bool begin();

    /**
     * @brief Установка ссылок на другие модули
     *
     * @param display Указатель на менеджер дисплея
     * @param sensors Указатель на менеджер датчиков
     * @param weather Указатель на менеджер погоды
     */
    void setModules(DisplayManager* display, SensorsManager* sensors, WeatherManager* weather);

    /**
     * @brief Обновление бота (вызывать в loop())
     *
     * Проверяет новые сообщения и обрабатывает команды.
     */
    void update();

    /**
     * @brief Проверка подключения к Telegram
     *
     * @return true если подключено
     */
    bool isConnected() const;

    // ========== Система команд ==========

    /**
     * @brief Регистрация новой команды
     *
     * @param name Имя команды (например, ".DBR")
     * @param handler Функция-обработчик
     * @param description Описание команды
     */
    void registerCommand(const String& name, CommandHandler handler, const String& description);

    /**
     * @brief Удалить команду
     *
     * @param name Имя команды
     */
    void unregisterCommand(const String& name);

    /**
     * @brief Получить список команд
     *
     * @return String список команд с описаниями
     */
    String getCommandsList() const;

    // ========== Отправка сообщений ==========

    /**
     * @brief Отправить сообщение
     *
     * @param chatId ID чата
     * @param message Текст сообщения
     * @return true если отправлено успешно
     */
    bool sendMessage(int64_t chatId, const String& message);

    /**
     * @brief Ответить на сообщение
     *
     * @param message Исходное сообщение
     * @param reply Текст ответа
     * @return true если отправлено успешно
     */
    bool replyTo(const TBMessage& message, const String& reply);

    /**
     * @brief Установить callback для новых сообщений
     *
     * @param callback Функция, вызываемая при получении сообщения
     */
    void setMessageCallback(std::function<void(const String& message)> callback);

private:
    /**
     * @brief Обработка входящего сообщения
     *
     * @param message Сообщение от Telegram
     */
    void handleMessage(TBMessage& message);

    /**
     * @brief Обработка команды
     *
     * @param text Текст сообщения
     * @param message Сообщение от Telegram
     */
    void handleCommand(const String& text, TBMessage& message);

    /**
     * @brief Инициализация стандартных команд
     */
    void initDefaultCommands();

    CTBot _bot;                                 ///< Клиент Telegram
    std::map<String, BotCommand> _commands;     ///< Карта команд
    DisplayManager* _display;                   ///< Ссылка на дисплей
    SensorsManager* _sensors;                   ///< Ссылка на датчики
    WeatherManager* _weather;                   ///< Ссылка на погоду
    std::function<void(const String&)> _messageCallback;  ///< Callback для сообщений
    uint32_t _lastPollTime;                     ///< Время последнего опроса
    bool _connected;                            ///< Флаг подключения
};
