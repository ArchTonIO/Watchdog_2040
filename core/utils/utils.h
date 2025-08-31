// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/data_structures/string_list.h"

/*Testing functions*/
void wait_for_serial_input();

/*User input functions*/
bool request_password(const char *placeholder_text);

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
char *string_add(char *str1, char *str2);
bool is_string_numeric(char *str);
bool is_string_alphanumeric(char *str);
bool is_string_alpha(char *str);
str_list *string_split(char *str, char delimiter);
char *string_remove_linefeed(char *str);
char *string_replace(char *str, char old_char, char new_char);
char *
string_substring_replace(const char *str, const char *old, const char *new);
char *gen_random_string(size_t length);

#endif