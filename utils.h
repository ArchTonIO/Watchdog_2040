#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

char *string_add(char *str1, char *str2);
uint16_t array_find_max(uint16_t *array, size_t len);
uint16_t array_find_min(uint16_t *array, size_t len);
bool is_string_numeric(char *str);
bool is_string_alphanumeric(char *str);
bool is_string_alpha(char *str);

#endif