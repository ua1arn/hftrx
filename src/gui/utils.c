/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Touch GUI от RA4ASN

#include "utils.h"

/* Безопасное сравнение строк */
int safe_strcmp(const char * s1, const char * s2)
{
    if (s1 == NULL && s2 == NULL)
        return 0;

    if (s1 == NULL)
        return -1;

    if (s2 == NULL)
        return 1;

    return strcmp(s1, s2);
}

/* Проверка времени и даты на корректность */
int is_valid_datetime(int year, int month, int day, int hour, int minute, int second)
{
    if (month < 1 || month > 12)
        return 0;

    if (day < 1 || day > 31)
        return 0;

    if (hour < 0 || minute < 0 || second < 0)
        return 0;

    if (hour > 23 || minute > 59 || second > 59)
        return 0;

    uint8_t days_in_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // Корректировка количества дней в феврале високосного года
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
    	days_in_month[1] = 29;

    if (day > days_in_month[month - 1])
        return 0;

    return 1;
}

/* Удаление пробелов в конце строки */
void remove_end_line_spaces(char * str)
{
	size_t i = strlen(str);
	if (i == 0)
		return;

	for (; -- i > 0;)
	{
		if (str[i] != ' ')
			break;
	}
	str[i + 1] = '\0';
}

/* Удаление пробелов в начале строки */
const char * remove_start_line_spaces(const char * str)
{
	size_t len = strlen(str);
	uint_fast8_t i = 0;

	if (len == 0)
		return NULL;

	for (; i < len; i ++)
	{
		if (str[i] != ' ')
			break;
	}

	if (i >= len)
		return NULL;
	else
		return str + i;
}
