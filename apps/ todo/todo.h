#ifndef TODO_H
#define TODO_H
#include <stdbool.h>
#include <stdint.h>

#define MAX_TODO_LISTS 50
#define MAX_ITEMS_PER_LIST 100

typedef struct {
  char *description;
  bool completed;
} todo_item;

typedef struct {
  char *name;
  todo_item *items[MAX_ITEMS_PER_LIST];
  uint8_t items_count;
} todo_list;

typedef struct {
  todo_list *lists[MAX_TODO_LISTS];
  uint8_t lists_count;
} todo_app_lists;

todo_app_lists *todo_app_init();
void todo_app_launch();

#endif