#include <stdlib.h>
#include <string.h>

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
