#include "todo.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/components/sys_paths_manager.h"
#include "core/data_structures/string_list.h"
#include "core/tools/options_gen.h"
#include "core/utils/path.h"
#include "core/utils/utils.h"

todo_app_lists *todo_app_init() {
  todo_app_lists *todo_lists = (todo_app_lists *)malloc(
      sizeof(todo_app_lists));
  todo_lists->lists_count = 0;
  return todo_lists;
}

todo_app_lists *load_existing_lists(todo_app_lists *todo_lists) {
  str_list *lists_names = path_listdir(sys_paths->dirs->todo_path);
  for (uint8_t i; i < lists_names->len; i++) {
    todo_lists->lists[todo_lists->lists_count]->name = str_list_get(
        lists_names,
        i);
    path *todos_path = path_init(str_list_get(lists_names, i));
    path *list_full_path = path_concat(sys_paths->dirs->todo_path, todos_path);
    str_list *todos = path_fread(list_full_path);
    path_free(todos_path);
    path_free(list_full_path);
    for (uint8_t j = 0; j < todos->len; j++) {
      str_list *split_line = string_split(str_list_get(todos, j), '~');
      char *description = str_list_get(split_line, 0);
      char *status = str_list_get(split_line, 1);
      todo_item *item = (todo_item *)malloc(sizeof(todo_item));
      item->description = description;
      item->completed = (strcmp(status, "1") == 0) ? true : false;
      todo_lists->lists[todo_lists->lists_count]->items
          [todo_lists->lists[todo_lists->lists_count]->items_count] = item;
      todo_lists->lists[todo_lists->lists_count]->items_count++;
      str_list_free(split_line);
    }
    str_list_free(todos);
  }
  str_list_free(lists_names);
  return todo_lists;
}

uint8_t list_index_by_name(todo_app_lists *todo_lists, char *name) {
  for (uint8_t i = 0; i < todo_lists->lists_count; i++) {
    if (strcmp(todo_lists->lists[i]->name, name) == 0)
      return i;
  }
  return 255;
}

void display_todo_list(path *list_full_path) {}

void todo_app_launch() {
  todo_app_lists *todo_lists = todo_app_init();
  todo_lists = load_existing_lists(todo_lists);
  str_list *lists_names = str_list_init();
  str_list_append(lists_names, " + Create new list");
  for (uint8_t i = 0; i < todo_lists->lists_count; i++)
    str_list_append(lists_names, todo_lists->lists[i]->name);
  options_page *page = options_page_init("Todo lists", lists_names);
  char *selected_list = options_page_launch(page);
  if (strcmp(selected_list, " + Create new list") == 0) {
    printf("Create new list selected\n");
  } else {
    todo_list *selected_todo_list = todo_lists
                                        ->lists[list_index_by_name(todo_lists,
                                            selected_list)]
                                        ->items;
  }
  options_page_free(page);
  str_list_free(lists_names);
}