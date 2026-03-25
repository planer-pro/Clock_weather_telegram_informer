/**
 * @file utils.cpp
 * @brief Реализация вспомогательных функций
 */

#include "utils.h"
#include "constants.h"

/**
 * @brief Форматирование времени в формате HH:MM
 *
 * @param hour Час (0-23)
 * @param minute Минута (0-59)
 * @param buffer Буфер для результата (минимум 6 байт)
 * @param size Размер буфера
 */
void formatTime(uint8_t hour, uint8_t minute, char* buffer, size_t size) {
    if (buffer == nullptr || size < 6) {
        return;
    }

    snprintf(buffer, size, "%02d:%02d", hour, minute);
}

/**
 * @brief Форматирование даты на русском языке
 *
 * @param day День (1-31)
 * @param month Месяц (1-12)
 * @param year Год
 * @param dayOfWeek День недели (0=воскресенье, 6=суббота)
 * @param buffer Буфер для результата
 * @param size Размер буфера
 */
void formatDateRussian(uint8_t day, uint8_t month, int year, uint8_t dayOfWeek,
                      char* buffer, size_t size) {
    if (buffer == nullptr || size < 32) {
        return;
    }

    // Проверка диапазонов
    if (month < 1 || month > 12) {
        month = 1;
    }
    if (dayOfWeek > 6) {
        dayOfWeek = 0;
    }

    snprintf(buffer, size, "%s %d %s %d года",
             WEEKDAYS_RU[dayOfWeek], day, MONTHS_RU[month - 1], year);
}

/**
 * @brief Безопасное преобразование строки в число
 *
 * @param str Строка для преобразования
 * @param defaultValue Значение по умолчанию при ошибке
 * @return int Результат преобразования
 */
int safeStringToInt(const String& str, int defaultValue) {
    if (str.length() == 0) {
        return defaultValue;
    }

    for (size_t i = 0; i < str.length(); i++) {
        if (!isDigit(str[i]) && str[i] != '-' && str[i] != '+') {
            return defaultValue;
        }
    }

    return str.toInt();
}

/**
 * @brief Безопасная проверка, что строка содержит только цифры
 *
 * @param str Строка для проверки
 * @return true если только цифры, false иначе
 */
bool isDigitsOnly(const String& str) {
    if (str.length() == 0) {
        return false;
    }

    for (size_t i = 0; i < str.length(); i++) {
        if (!isDigit(str[i])) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Безопасное копирование строки с ограничением длины
 *
 * @param dest Буфер назначения
 * @param destSize Размер буфера назначения
 * @param src Исходная строка
 */
void safeStrCopy(char* dest, size_t destSize, const char* src) {
    if (dest == nullptr || destSize == 0 || src == nullptr) {
        return;
    }

    strncpy(dest, src, destSize - 1);
    dest[destSize - 1] = '\0';
}

/**
 * @brief Форматирование температуры с 1 знаком после запятой
 *
 * @param temp Температура
 * @param buffer Буфер для результата (минимум 16 байт)
 * @param size Размер буфера
 */
void formatTemperature(float temp, char* buffer, size_t size) {
    if (buffer == nullptr || size < 16) {
        return;
    }

    int tempInt = (int)temp;
    int tempFrac = abs((int)((temp - tempInt) * 10)) % 10;
    snprintf(buffer, size, "%d.%dC", tempInt, tempFrac);
}

/**
 * @brief Получение случайного эффекта из каталога
 *
 * @return textEffect_t Случайный эффект
 */
textEffect_t getRandomEffect() {
    uint8_t idx = random(RANDOM_EFFECT_MIN, RANDOM_EFFECT_MAX + 1);
    return EFFECTS[idx].effect;
}

/**
 * @brief Очистка строки от лишних символов
 *
 * @param str Строка для очистки
 */
void trimString(String& str) {
    str.trim();
}
