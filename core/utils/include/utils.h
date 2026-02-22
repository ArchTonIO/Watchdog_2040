// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "core/data_structures/include/string_list.h"

/*Testing functions*/
void wait_for_serial_input();

/*User input functions*/
bool request_password();
bool request_and_get_password(char return_buf[]);

/*Logging functions*/
void print_info(const char *message);
void print_usr_error(const char *message);
void print_sys_error(const char *message);
void print_debug(const char *message);
void print_loading(const char *message);

/*Array helpers*/
uint16_t array_find_max(uint16_t *array, size_t len);
uint16_t array_find_min(uint16_t *array, size_t len);

/*Strings helpers*/
bool is_string_numeric(const char *str);
bool is_string_alphanumeric(const char *str);
bool is_string_alpha(const char *str);
str_list *string_split(const char *str, const char delimiter);
char *string_remove_linefeed(const char *str);
char *string_replace(const char *str, char old_char, char new_char);
char *
string_substring_replace(const char *str, const char *old, const char *new);
char *gen_random_string(size_t length);

#endif