#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "pico/stdlib.h"

char *string_add(char *str1, char *str2)
{
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

uint16_t array_find_max(uint16_t *array, size_t len)
{
  uint16_t max = 0;
  for (uint8_t i = 0; i < len; i++)
    if (array[i] > max)
      max = array[i];
  return max;
}

uint16_t array_find_min(uint16_t *array, size_t len)
{
  uint16_t min = array[0];
  for (uint8_t i = 0; i < len; i++)
    if (array[i] < min)
      min = array[i];
  return min;
}

bool is_string_numeric(char *str)
{
  for (uint8_t i = 0; i < strlen(str); i++)
    if (str[i] < '0' || str[i] > '9')
      return false;
  return true;
}

bool is_string_alphanumeric(char *str)
{
  for (uint8_t i = 0; i < strlen(str); i++)
    if ((str[i] < '0' || str[i] > '9') && (str[i] < 'A' || str[i] > 'Z') && (str[i] < 'a' || str[i] > 'z'))
      return false;
  return true;
}

bool is_string_alpha(char *str)
{
  for (uint8_t i = 0; i < strlen(str); i++)
    if ((str[i] < 'A' || str[i] > 'Z') && (str[i] < 'a' || str[i] > 'z'))
      return false;
  return true;
}

void wait_for_user_input()
{
  while (!stdio_usb_connected())
  {
    sleep_ms(100);
  }
  printf("Watchdog_2040 tester, send any key to continue...\n");
  while (1)
  {
    getchar();
    break;
  }
}
