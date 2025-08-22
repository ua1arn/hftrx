#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "hardware.h"
#include "formats.h"

int safe_strcmp(const char * s1, const char * s2);
int is_valid_datetime(int year, int month, int day, int hour, int minute, int second);
void remove_end_line_spaces(char * str);
const char * remove_start_line_spaces(const char * str);

#endif /* UTILS_H_INCLUDED */
