// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include <stdint.h>

#ifndef STRING_LIST_H
#define STRING_LIST_H

#ifndef uinteger
#define uinteger uint16_t
#endif
#ifndef integer
#define integer int32_t
#endif

#define MAX_STR_LEN 65535
#define MAX_LIST_LEN 65535

#define INDEX_ERR_STR "[INDEX_ERROR]"
#define MAX_LEN_REACHED_STR "[MAX_LEN_REACHED_ERROR]"
#define MEMORY_ERR_STR "[MEMORY_ERROR]"
#define MAX_STRLEN_REACHED_STR "[MAX_STR_LEN_REACHED]"

typedef struct {
  uinteger len;
  struct lnode *head;
  struct lnode *tail;
} str_list;

str_list *str_list_init();
void str_list_append(str_list *list, const char *value);
integer str_list_index_of(str_list *list, const char *value);
char *str_list_get(str_list *list, integer index);
char *str_list_pop(str_list *list, integer index);
void str_list_print(str_list *list);
void str_list_clear(str_list *list);
uinteger str_list_len(str_list *list);
uinteger str_list_compare(str_list *list1, str_list *list2);
char *str_list_concat(str_list *list, char separator);
str_list *str_list_extend(str_list *list1, str_list *list2);
str_list *str_list_copy(str_list *to_copy);
str_list *str_list_reverse(str_list *list);
void str_list_free(str_list *list);

#endif