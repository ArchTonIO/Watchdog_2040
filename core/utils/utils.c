// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "core/utils/utils.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/rand.h"
#include "pico/stdio_usb.h"

#include "apps/text_editor/text_editor.h"
#include "core/components/hw_manager.h"
#include "core/components/malloc_mascot.h"
#include "core/data_structures/string_list.h"
#include "core/hardware_drivers/ssd1306.h"
#include "core/tools/sha_256.h"

bool request_password() {
  text_editor *pwd_editor = text_editor_launch("#Enter your password", true);
  char *buf = text_editor_get_buf(pwd_editor);
  text_editor_kill(pwd_editor);
  char *hashed = get_hash(buf);
  if (strcmp(hashed, malloc_memories_inst->user_password_hashed) != 0) {
    free(hashed);
    free(buf);
    print_usr_error("Wrong password");
    return false;
  }
  free(hashed);
  free(buf);
  return true;
}

bool request_and_get_password(char return_buf[]) {
  text_editor *pwd_editor = text_editor_launch("#Enter your password", true);
  char *buf = text_editor_get_buf(pwd_editor);
  strcpy(return_buf, buf);
  text_editor_kill(pwd_editor);
  char *hashed = get_hash(buf);
  if (strcmp(hashed, malloc_memories_inst->user_password_hashed) != 0) {
    free(hashed);
    free(buf);
    print_usr_error("Wrong password");
    return false;
  }
  free(hashed);
  free(buf);
  return true;
}

void print_log(const char *message, uint16_t persistency, bool autoclear) {
  ssd1306_clear(drivers->oled_screen);
  ssd1306_print(drivers->oled_screen, message, 0, 0, false);
  ssd1306_show(drivers->oled_screen);
  sleep_ms(persistency);
  if (autoclear) {
    ssd1306_clear(drivers->oled_screen);
    ssd1306_show(drivers->oled_screen);
  }
}

/**
 * @brief Prints an info message to the OLED display.
 *
 * @param message The message to print.
 */
void print_info(const char *message) {
  char *to_print = string_add("[INFO]\n_______\n", message);
  print_log(to_print, 2000, true);
  free(to_print);
}

/**
 * @brief Prints a user error message to the OLED display.
 *
 * @param message The message to print.
 */
void print_usr_error(const char *message) {
  char *to_print = string_add("[USER ERROR]\n_______\n", message);
  print_log(to_print, 2000, true);
  free(to_print);
}

/**
 * @brief Prints a system error message to the OLED display.
 *
 * @param message The message to print.
 */
void print_sys_error(const char *message) {
  char *to_print = string_add("[SYSTEM ERROR]\n_______\n", message);
  print_log(to_print, 2000, true);
  free(to_print);
}

/**
 * @brief Prints a debug message to the OLED display.
 *
 * @param message The message to print.
 */
void print_debug(const char *message) {
  char *to_print = string_add("[DEBUG]\n_______\n", message);
  print_log(to_print, 2000, true);
  free(to_print);
}

/**
 * @brief Prints a loading message to the OLED display.
 *
 * @param message The message to print.
 */
void print_loading(const char *message) {
  char *to_print = string_add("[LOADING]\n_______\n", message);
  print_log(to_print, 0, false);
  free(to_print);
}

/**
 * @brief Concatenates two strings.
 *
 * @param str1 The first string.
 * @param str2 The second string.
 * @return A new string that is the concatenation of str1 and str2, or NULL on
 * failure.
 */
char *string_add(const char *str1, const char *str2) {
  size_t len1 = strlen(str1);
  size_t len2 = strlen(str2);
  char *result = malloc(len1 + len2 + 1);
  if (!result)
    return NULL;
  for (size_t i = 0; i < len1; ++i)
    result[i] = str1[i];
  for (size_t i = 0; i < len2; ++i)
    result[len1 + i] = str2[i];
  result[len1 + len2] = '\0';
  return result;
}

uint16_t array_find_max(uint16_t *array, size_t len) {
  uint16_t max = 0;
  for (uint8_t i = 0; i < len; i++)
    if (array[i] > max)
      max = array[i];
  return max;
}

uint16_t array_find_min(uint16_t *array, size_t len) {
  uint16_t min = array[0];
  for (uint8_t i = 0; i < len; i++)
    if (array[i] < min)
      min = array[i];
  return min;
}

bool is_string_numeric(const char *str) {
  for (uint8_t i = 0; i < strlen(str); i++)
    if (str[i] < '0' || str[i] > '9')
      return false;
  return true;
}

bool is_string_alphanumeric(const char *str) {
  for (uint8_t i = 0; i < strlen(str); i++)
    if ((str[i] < '0' || str[i] > '9') && (str[i] < 'A' || str[i] > 'Z') &&
        (str[i] < 'a' || str[i] > 'z'))
      return false;
  return true;
}

bool is_string_alpha(const char *str) {
  for (uint8_t i = 0; i < strlen(str); i++)
    if ((str[i] < 'A' || str[i] > 'Z') && (str[i] < 'a' || str[i] > 'z'))
      return false;
  return true;
}

void wait_for_serial_input() {
  while (!stdio_usb_connected()) {
    sleep_ms(100);
  }
  printf("Watchdog_2040 tester, send any key to continue...\n");
  while (1) {
    getchar();
    break;
  }
}

str_list *string_split(const char *str, const char delimiter) {
  str_list *result = str_list_init();
  char *copy = strdup(str);
  char delim[2] = {delimiter, '\0'};
  char *token = strtok(copy, delim);
  while (token != NULL) {
    str_list_append(result, token);
    token = strtok(NULL, delim);
  }
  free(copy);
  return result;
}

char *string_remove_linefeed(const char *str) {
  char *newstr = strdup(str);
  size_t len = strlen(newstr);
  if (len > 0 && newstr[len - 1] == '\n')
    newstr[len - 1] = '\0';
  return newstr;
}

char *string_replace(const char *str, char old_char, char new_char) {
  char *newstr = strdup(str);
  for (size_t i = 0; i < strlen(newstr); i++)
    if (newstr[i] == old_char)
      newstr[i] = new_char;
  return newstr;
}

char *gen_random_string(size_t length) {
  char *s = malloc(length + 1);
  for (uint8_t i = 0; i < length; i++) {
    s[i] = 'A' + get_rand_32() % 26;
  }
  s[length] = '\0';
  return s;
}

char *
string_substring_replace(const char *str, const char *old, const char *new) {
  size_t str_len = strlen(str);
  size_t old_len = strlen(old);
  size_t new_len = strlen(new);
  if (old_len == 0)
    return NULL;
  size_t count = 0;
  const char *p = str;
  while ((p = strstr(p, old)) != NULL) {
    count++;
    p += old_len;
  }
  size_t new_str_len = str_len + count * (new_len - old_len) + 1;
  char *result = malloc(new_str_len);
  if (!result)
    return NULL;
  char *dst = result;
  p = str;
  const char *match;
  while ((match = strstr(p, old)) != NULL) {
    size_t bytes_before = match - p;
    memcpy(dst, p, bytes_before);
    dst += bytes_before;
    memcpy(dst, new, new_len);
    dst += new_len;
    p = match + old_len;
  }
  strcpy(dst, p);
  return result;
}
