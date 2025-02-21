#include "string_list.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
A straigtforward implementation of a list of strings.
Supports basic happend(), pop(), get(), index_of() and has some
useful features such as:
- yells at you when trying to use a index > lenght of the list.
- supports negative indexing (I like it).
- print_list() prints you the whole list info and content.
 */

#define index_err_str "[INDEX_ERROR]"
#define max_len_reached_str "[MAX_LEN_REACHED_ERROR]"

struct lnode
{
  char *value;
  struct lnode *prev;
  struct lnode *next;
};

/*
 * @brief Create a simple doubly linked list for strings.
 *
 * @return str_list * A pointer to the doubly linked list.
 */
str_list *list()
{
  str_list *newlist = (str_list *)malloc(sizeof(str_list));
  newlist->len = 0;
  newlist->head = NULL;
  newlist->tail = NULL;
  return newlist;
}

/*
 * @brief Appends a string constant to the list.
 *
 * @param *list The list to append to.
 * @param *value The value to append.
 */
void lstappend(str_list *list, char *value)
{
  if (list->len == sizeof(uinteger) * 255)
  {
    printf(
        "%s because of the uinteger definition in list.h the list reached max "
        "lengt (%d), adding items will cause overflow, pop items or change "
        "uinteger definition to a bigger one",
        max_len_reached_str, sizeof(uinteger) * 255);
    return;
  }
  list->len++;
  struct lnode *newnode = (struct lnode *)malloc(sizeof(struct lnode));
  if (list->tail != NULL)
  {
    list->tail->next = newnode;
  }
  newnode->value = value;
  newnode->next = NULL;
  newnode->prev = list->tail;
  list->tail = newnode;
  if (list->head == NULL)
  {
    list->head = newnode;
  }
}

/*
 * @brief Get the index of a certain string constant (value)
 *stored in the list.
 *
 * @param *list The list to look in.
 * @param *value The value you are looking the index for.
 * @return integer The index of that value in the list.
 */
integer index_of(str_list *list, char *value)
{
  struct lnode *cursor = list->head;
  uinteger index = 0;
  while (cursor != NULL)
  {
    if (strcmp(cursor->value, value) == 0)
    {
      return index;
    }
    index++;
    cursor = cursor->next;
  }
  return -1;
}

/*
 * @brief Get a value in the list by its index.
 *
 * @param *list The list to look in.
 * @param index The index of the element, supports negative indexing.
 * @return char * The string at that index.
 */
char *lstget(str_list *list, integer index)
{
  struct lnode *cursor = list->head;
  if (index < 0)
  {
    index = list->len - (index * -1);
  }
  if (index > list->len - 1)
  {
    printf("%s: %d\n", index_err_str, index);
    return index_err_str;
  }
  for (uinteger i = 0; i < index; i++)
  {
    cursor = cursor->next;
  }
  return cursor->value;
}

/*
 * @brief Pop a value in the list by its index.
 *
 * @param *list The list to look in.
 * @param index The index of the element.
 * @return the value that was removed
 */
char *lstpop(str_list *list, integer index)
{
  char *value = lstget(list, index);
  struct lnode *cursor = list->head;
  while (cursor != NULL)
  {
    if (strcmp(cursor->value, value) == 0)
    {
      if (cursor == list->head)
      {
        if (list->len > 1)
        {
          list->head = cursor->next;
          list->head->prev = NULL;
        }
        else
        {
          list->head = NULL;
        }
        free(cursor);
        list->len--;
        return value;
      }
      if (cursor == list->tail)
      {
        list->tail = cursor->prev;
        list->tail->next = NULL;
        free(cursor);
        list->len--;
        return value;
      }
      if (cursor != list->head && cursor != list->tail)
      {
        cursor->prev->next = cursor->next;
        cursor->next->prev = cursor->prev;
        free(cursor);
        list->len--;
        return value;
      }
    }
    cursor = cursor->next;
  }
}

/*
 * @brief print lists info and values.
 *
 * @param *list The list to print.
 */
void lstprint(str_list *list)
{
  struct lnode *cursor = list->head;
  uinteger i = 0;
  printf("lenght: %d\n", list->len);
  while (cursor != NULL)
  {
    printf("  > %d: %s\n", i, cursor->value);
    cursor = cursor->next;
    i++;
  }
}

/*
 * @brief clear all the list content.
 *
 * @param *list The list to clear.
 */
void lstclear(str_list *list)
{
  uinteger len = list->len;
  for (integer i = len - 1; i > -1; i--)
  {
    lstpop(list, i);
  }
}

/*
 * @brief returns the lenght of the list.
 *
 * @param *list The list.
 */
uinteger lstlen(str_list *list) { return list->len; }

/*
 * @brief compare two lists of strings.
 *
 * @param *list1, the first list of strings.
 * @param *list2, the second list of strings.
 * @return 0 if the list are identical, 1 otherwise
 */
uinteger lstcmp(str_list *list1, str_list *list2)
{
  if (list1->len != list2->len)
  {
    return 1;
  }
  uinteger matches = 0;
  for (uinteger i = 0; i < list1->len; i++)
  {
    for (uinteger j = 0; j < list2->len; j++)
    {
      if (strcmp(lstget(list1, i), lstget(list2, j)) == 0)
      {
        matches++;
      }
    }
  }
  if (matches == list1->len)
  {
    return 0;
  }
  return 1;
}

/*
 * @brief Delete the list.
 *
 * @param *list The list to delete.
 */
void lstdel(str_list *list)
{
  struct lnode *cursor = list->head;
  while (cursor != NULL)
  {
    if (cursor != list->head)
    {
      free(cursor->prev);
    }
    if (cursor == list->tail)
    {
      free(cursor);
      return;
    }
    cursor = cursor->next;
  }
  free(list);
}
