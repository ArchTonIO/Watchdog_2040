// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "include/string_list.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct lnode {
  char *value;
  struct lnode *prev;
  struct lnode *next;
};

/**
 * @brief Create a simple doubly linked list for strings.
 *
 * @return str_list * A pointer to the doubly linked list.
 */
str_list *str_list_init() {
  str_list *newlist = (str_list *)malloc(sizeof(str_list));
  newlist->len = 0;
  newlist->head = NULL;
  newlist->tail = NULL;
  return newlist;
}

/**
 * @brief Append a string constant to the list.
 *
 * @param *list The list to append to.
 * @param *value The value to append.
 */
void str_list_append(str_list *list, const char *value) {
  if (list->len >= MAX_LIST_LEN) {
    printf("%s\n", MAX_LEN_REACHED_STR);
    return;
  }
  if (strlen(value) > MAX_STR_LEN) {
    printf("%s\n", MAX_STRLEN_REACHED_STR);
    return;
  }
  list->len++;
  struct lnode *newnode = (struct lnode *)malloc(sizeof(struct lnode));
  if (list->tail != NULL) {
    list->tail->next = newnode;
  }
  char *valuecp = (char *)malloc(strlen(value) + 1);
  if (valuecp == NULL) {
    printf("%s\n", MEMORY_ERR_STR);
    return;
  }
  strcpy(valuecp, value);
  newnode->value = valuecp;
  newnode->next = NULL;
  newnode->prev = list->tail;
  list->tail = newnode;
  if (list->head == NULL) {
    list->head = newnode;
  }
}

/**
 * @brief Get the index of a certain string constant (value)
 *stored in the list.
 *
 * @param *list The list to look in.
 * @param *value The value you are looking the index for.
 * @return integer The index of that value in the list.
 */
integer str_list_index_of(str_list *list, const char *value) {
  struct lnode *cursor = list->head;
  uinteger index = 0;
  while (cursor != NULL) {
    if (strcmp(cursor->value, value) == 0) {
      return index;
    }
    index++;
    cursor = cursor->next;
  }
  return -1;
}

/**
 * @brief Get a value in the list by its index.
 *
 * @param *list The list to look in.
 * @param index The index of the element, supports negative indexing.
 * @return char * The string at that index.
 */
char *str_list_get(str_list *list, integer index) {
  struct lnode *cursor = list->head;
  if (index < 0) {
    index = list->len + index;
  }
  if (index >= list->len) {
    printf("%s: %d\n", INDEX_ERR_STR, index);
    return INDEX_ERR_STR;
  }
  for (uinteger i = 0; i < index && cursor != NULL; i++) {
    cursor = cursor->next;
  }
  return cursor->value;
}

/**
 * @brief Pop a value in the list by its index.
 *
 * @param *list The list to look in.
 * @param index The index of the element.
 * @return the value that was removed
 */
char *str_list_pop(str_list *list, integer index) {
  char *value = str_list_get(list, index);
  struct lnode *cursor = list->head;
  while (cursor != NULL) {
    if (strcmp(cursor->value, value) == 0) {
      if (cursor == list->head) {
        if (list->len > 1) {
          list->head = cursor->next;
          list->head->prev = NULL;
        } else {
          list->head = NULL;
        }
        free(cursor);
        list->len--;
        return value;
      }
      if (cursor == list->tail) {
        list->tail = cursor->prev;
        list->tail->next = NULL;
        free(cursor);
        list->len--;
        return value;
      }
      if (cursor != list->head && cursor != list->tail) {
        cursor->prev->next = cursor->next;
        cursor->next->prev = cursor->prev;
        free(cursor);
        list->len--;
        return value;
      }
    }
    cursor = cursor->next;
  }
  return NULL;
}

/**
 * @brief Print list's info and values.
 *
 * @param *list The list to print.
 */
void str_list_print(str_list *list) {
  struct lnode *cursor = list->head;
  uinteger i = 0;
  printf("length: %d\n", list->len);
  while (cursor != NULL) {
    printf("  > %d: %s\n", i, cursor->value);
    cursor = cursor->next;
    i++;
  }
}

/**
 * @brief Clear all the list content.
 *
 * @param *list The list to clear.
 */
void str_list_clear(str_list *list) {
  uinteger len = list->len;
  for (integer i = len - 1; i > -1; i--) {
    str_list_pop(list, i);
  }
}

/**
 * @brief Returns the length of the list.
 *
 * @param *list The list.
 */
uinteger str_list_len(str_list *list) { return list->len; }

/**
 * @brief Compare two lists of strings.
 *
 * @param *list1, the first list of strings.
 * @param *list2, the second list of strings.
 * @return 0 if the list are identical, 1 otherwise
 */
uinteger str_list_compare(str_list *list1, str_list *list2) {
  if (list1->len != list2->len) {
    return 1;
  }
  uinteger matches = 0;
  for (uinteger i = 0; i < list1->len; i++) {
    for (uinteger j = 0; j < list2->len; j++) {
      if (strcmp(str_list_get(list1, i), str_list_get(list2, j)) == 0) {
        matches++;
      }
    }
  }
  if (matches == list1->len) {
    return 0;
  }
  return 1;
}

/**
 * @brief Concatenate all the strings in the list.
 *
 * @param *list The list to concatenate.
 * @param char separator The separator to use between strings.
 * @return char * The concatenated string.
 */
char *str_list_concat(str_list *list, char separator) {
  if (list->len == 0) {
    char *empty = (char *)malloc(1);
    if (!empty)
      return NULL;
    empty[0] = '\0';
    return empty;
  }
  size_t total_length = 0;
  for (int i = 0; i < list->len; i++) {
    total_length += strlen(str_list_get(list, i));
  }
  total_length += (list->len - 1) + 1;
  char *total_payload = (char *)malloc(total_length);
  if (!total_payload)
    return NULL;
  char *ptr = total_payload;
  for (int i = 0; i < list->len; i++) {
    char *payload = str_list_get(list, i);
    size_t len = strlen(payload);
    memcpy(ptr, payload, len);
    ptr += len;
    if (i < list->len - 1) {
      *ptr = separator;
      ptr++;
    }
  }
  *ptr = '\0';
  return total_payload;
}

/**
 * @brief Extend a list with another list.
 *
 * @param *list1 The first list.
 * @param *list2 The second list.
 * @return str_list * The new list.
 */
str_list *str_list_extend(str_list *list1, str_list *list2) {
  str_list *newlist = str_list_init();
  for (uinteger i = 0; i < list1->len; i++) {
    str_list_append(newlist, str_list_get(list1, i));
  }
  for (uinteger i = 0; i < list2->len; i++) {
    str_list_append(newlist, str_list_get(list2, i));
  }
  return newlist;
}

/**
 * @brief Copy a list.
 *
 * @param *to_copy The list to copy.
 * @return str_list * The new list.
 */
str_list *str_list_copy(str_list *to_copy) {
  str_list *newlist = str_list_init();
  for (uinteger i = 0; i < to_copy->len; i++) {
    str_list_append(newlist, str_list_get(to_copy, i));
  }
  return newlist;
}

/**
 * @brief Reverse a list.
 * @param *list The list to reverse.
 * @return str_list * The new reversed list.
 */
str_list *str_list_reverse(str_list *list) {
  str_list *newlist = str_list_init();
  for (integer i = list->len - 1; i >= 0; i--) {
    str_list_append(newlist, str_list_get(list, i));
  }
  return newlist;
}

/**
 * @brief Apply a function to every string of the list.
 * @param list The input list.
 * @param func The pointer to the callback function.
 * @param must_free specify if the return string of the callback function
 * should be freed
 */
str_list *str_list_apply_func(str_list *list,
    char *(func)(const char *input),
    bool must_free) {
  str_list *new_list = str_list_init();
  char *out;
  for (size_t i = 0; i < list->len; i++) {
    out = func(str_list_get(list, i));
    str_list_append(new_list, out);
    if (must_free)
      free(out);
  }
  str_list_free(list);
  return new_list;
}

/**
 * @brief Delete the list.
 *
 * @param *list The list to delete.
 */
void str_list_free(str_list *list) {
  struct lnode *cursor = list->head;
  while (cursor != NULL) {
    struct lnode *next = cursor->next;
    free(cursor->value);
    free(cursor);
    cursor = next;
  }
  free(list);
}
