#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "data_structures/string_list.h"

char *string_add(char *str1, char *str2);
uint16_t array_find_max(uint16_t *array, size_t len);
uint16_t array_find_min(uint16_t *array, size_t len);
bool is_string_numeric(char *str);
bool is_string_alphanumeric(char *str);
bool is_string_alpha(char *str);
void wait_for_user_input();
str_list *string_split(char *str, char delimiter);
char *string_remove_linefeed(char *str);
char *string_replace(char *str, char old_char, char new_char);
char *
string_substring_replace(const char *str, const char *old, const char *new);
char *gen_random_string(size_t length);

#endif