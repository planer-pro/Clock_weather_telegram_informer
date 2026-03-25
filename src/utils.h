/**
 * @file utils.h
 * @brief Вспомогательные функции
 */

#pragma once

#include <Arduino.h>
#include <MD_Parola.h>

// ============================================================================
// Типы
// ============================================================================

/**
 * @brief Форматирование времени в формате HH:MM
 *
 * @param hour Час (0-23)
 * @param minute Минута (0-59)
 * @param buffer Буфер для результата (минимум 6 байт)
 * @param size Размер буфера
 */
void formatTime(uint8_t hour, uint8_t minute, char* buffer, size_t size);

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
                      char* buffer, size_t size);

/**
 * @brief Безопасное преобразование строки в число
 *
 * @param str Строка для преобразования
 * @param defaultValue Значение по умолчанию при ошибке
 * @return int Результат преобразования
 */
int safeStringToInt(const String& str, int defaultValue = 0);

/**
 * @brief Безопасная проверка, что строка содержит только цифры
 *
 * @param str Строка для проверки
 * @return true если только цифры, false иначе
 */
bool isDigitsOnly(const String& str);

/**
 * @brief Безопасное копирование строки с ограничением длины
 *
 * @param dest Буфер назначения
 * @param destSize Размер буфера назначения
 * @param src Исходная строка
 */
void safeStrCopy(char* dest, size_t destSize, const char* src);

/**
 * @brief Форматирование температуры с 1 знаком после запятой
 *
 * @param temp Температура
 * @param buffer Буфер для результата (минимум 16 байт)
 * @param size Размер буфера
 */
void formatTemperature(float temp, char* buffer, size_t size);

/**
 * @brief Получение случайного эффекта из каталога
 *
 * @return textEffect_t Случайный эффект
 */
textEffect_t getRandomEffect();

/**
 * @brief Очистка строки от лишних символов
 *
 * @param str Строка для очистки
 */
void trimString(String& str);
