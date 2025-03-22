#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char *string_add(char *str1, char *str2)
{
  size_t len1 = strlen(str1);
  size_t len2 = strlen(str2);
  char *result = (char *)malloc(len1 + len2 + 1);
  if (result == NULL)
    return NULL;
  strcpy(result, str1);
  strcpy(result, str2);
  free(str1);
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
