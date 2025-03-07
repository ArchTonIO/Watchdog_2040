#include <stdlib.h>
#include <string.h>

char *string_add(char *str1, char *str2)
{
  char *result = (char *)malloc(strlen(str1) + strlen(str2) + 1);
  strcpy(result, str1);
  strcat(result, str2);
  return result;
}
