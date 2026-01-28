// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "apps/todo/todo.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "apps/text_editor/text_editor.h"
#include "core/components/sys_paths_manager.h"
#include "core/data_structures/string_list.h"
#include "core/tools/crud_list.h"
#include "core/utils/path.h"
#include "core/utils/utils.h"

void create_task_list(crud_list *list);
void edit_task_list(crud_list *list, const char *list_name);
void register_task_list(crud_list *list);

void create_task(crud_list *task_list);
void edit_task_name(crud_list *task_list, const char *task_name);
void delete_task(crud_list *task_list, const char *task_name);
char *flag_task(char *input);

void todo_launch() {
  crud_list list;
  list.name = "Todo";
  list.items_category_name = "task list";
  list.workdir = sys_paths->dirs->todo_path;
  list.create_as_dir = true;
  list.create_callback = create_task_list;
  list.read_update_callback = edit_task_list;
  list.delete_callback = delete_item_basic;
  list.flag_callback = NULL;
  list.flag_string = NULL;
  list.unflag_string = NULL;
  launch_crud_list(&list);
}

void create_task_list(crud_list *list) {
  text_editor *name_editor = text_editor_launch("# Type in the task list name",
      true);
  char *name_buf = text_editor_get_buf(name_editor);
  text_editor_kill(name_editor);
  if (item_exists(list, name_buf)) {
    free(name_buf);
    print_usr_error("A task list with this\nname already exists!");
    return;
  }
  print_loading("Saving task list...");
  create_or_overwrite_item(list, name_buf, "");
  free(name_buf);
}

void edit_task_list(crud_list *list, const char *list_name) {
  path *workdir = get_item_path(list, list_name);
  crud_list task_list;
  task_list.name = (char *)list_name;
  task_list.items_category_name = "task";
  task_list.workdir = workdir;
  task_list.create_as_dir = false;
  task_list.create_callback = create_task;
  task_list.read_update_callback = edit_task_name;
  task_list.delete_callback = delete_item_basic;
  task_list.flag_callback = flag_task;
  task_list.flag_string = DONE;
  task_list.unflag_string = NOTDONE;
  launch_crud_list(&task_list);
  path_free(workdir);
}

void create_task(crud_list *task_list) {
  text_editor *task_name_editor = text_editor_launch("# Type in the task name",
      true);
  char *task_name_buf = text_editor_get_buf(task_name_editor);
  text_editor_kill(task_name_editor);
  if (item_exists(task_list, task_name_buf)) {
    print_usr_error("A task with this\nname already exists");
    free(task_name_buf);
    return;
  }
  size_t size = strlen(task_name_buf) + 5;
  char complete_name[size];
  snprintf(complete_name, size, "%s %s", NOTDONE, task_name_buf);
  create_or_overwrite_item(task_list, complete_name, "");
  free(task_name_buf);
  return;
}

char *flag_task(char *input) {
  char *new_str;
  if (strstr(input, DONE) != NULL)
    new_str = string_substring_replace(input, DONE, NOTDONE);
  else
    new_str = string_substring_replace(input, NOTDONE, DONE);
  return new_str;
}

void edit_task_name(crud_list *tasks, const char *task_name) {
  path *task_path = get_item_path(tasks, task_name);

  char rep[10];
  char *text;
  bool was_done = false;
  if (strstr(task_name, DONE) != NULL) {
    snprintf(rep, 10, "%s %s", DONE, "");
    was_done = true;
  } else if (strstr(task_name, NOTDONE) != NULL)
    snprintf(rep, 10, "%s %s", NOTDONE, "");
  text = string_substring_replace(task_name, rep, "");

  text_editor *task_name_editor = text_editor_launch(text, false);
  char *file_new_buf = text_editor_get_buf(task_name_editor);
  text_editor_kill(task_name_editor);

  if (strcmp(file_new_buf, text) != 0) {
    delete_item_basic(tasks, task_name);
    char new_buf[30];
    char *flag = (was_done) ? DONE : NOTDONE;
    snprintf(new_buf, 30, "%s %s", flag, file_new_buf);
    create_or_overwrite_item(tasks, new_buf, "");
  }
  path_free(task_path);
  free(file_new_buf);
  free(text);
}
