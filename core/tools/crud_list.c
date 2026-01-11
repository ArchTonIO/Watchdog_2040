// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include "core/tools/crud_list.h"

#include <stdio.h>
#include <string.h>

#include "core/data_structures/string_list.h"
#include "core/tools/options_gen.h"
#include "core/utils/path.h"
#include "core/utils/utils.h"

void edit_or_delete_item(crud_list *list, const char *item_name);

/**
 * @brief Launches a CRUD list interface for managing items.
 *
 * @param list The CRUD list structure containing callbacks and configurations.
 */
void launch_crud_list(crud_list *list) {
  str_list *items = str_list_init();
  str_list *existing_items;
  str_list *options;
  options_page *items_page;

  size_t add_option_buf_len = strlen(list->items_category_name) + 3;
  char add_option_buf[add_option_buf_len];
  snprintf(add_option_buf,
      add_option_buf_len,
      "+ new %s",
      list->items_category_name);

  str_list_append(items, add_option_buf);
  while (true) {
    char print_buf[30];
    snprintf(print_buf, 30, "Loading %ss...", list->items_category_name);
    print_loading(print_buf);
    existing_items = path_listdir(list->workdir);
    options = str_list_extend(items, existing_items);
    items_page = options_page_init(list->name, options);
    char *buf = options_page_launch(items_page);
    if (strcmp(buf, add_option_buf) == 0)
      list->create_callback(list);
    else if (strcmp(buf, "") == 0)
      break;
    else
      edit_or_delete_item(list, buf);
    str_list_free(existing_items);
    options_page_free(items_page);
  }
  str_list_free(items);
  str_list_free(existing_items);
  options_page_free(items_page);
}

void edit_or_delete_item(crud_list *list, const char *item_name) {
  str_list *options = str_list_init();
  str_list_append(options, "view/edit");
  str_list_append(options, "delete");
  options_page *editordelete_page = options_page_init((char *)item_name,
      options);
  char *buf = options_page_launch(editordelete_page);
  if (strcmp(buf, "view/edit") == 0)
    list->read_update_callback(list, item_name);
  else if (strcmp(buf, "delete") == 0) {
    sleep_ms(200);
    str_list *yesno = str_list_init();
    str_list_append(yesno, "no");
    str_list_append(yesno, "yes");
    options_page *yesno_page = options_page_init("Are you sure?", yesno);
    char *yesno_buf = options_page_launch(yesno_page);
    if (strcmp(yesno_buf, "no") == 0 || strcmp(yesno_buf, "") == 0) {
      options_page_free(yesno_page);
      options_page_free(editordelete_page);
      return;
    }
    options_page_free(yesno_page);
    list->delete_callback(list, item_name);
  }
  options_page_free(editordelete_page);
}

/**
 * @brief Gets the full path of an item in the CRUD list.
 *
 * @param list The CRUD list structure.
 * @param item_name The name of the item.
 * @returns The full path of the item (points to the actual file in the
 * microSD).
 */
path *get_item_path(crud_list *list, const char *item_name) {
  path *item_path = path_init(item_name);
  path *full_item_path = path_concat(list->workdir, item_path);
  path_free(item_path);
  return full_item_path;
}

/**
 * @brief Checks if an item exists in the CRUD list.
 *
 * @param list The CRUD list structure.
 * @param item_name The name of the item.
 * @returns true if the item exists, false otherwise.
 */
bool item_exists(crud_list *list, const char *item_name) {
  str_list *existing_items = path_listdir(list->workdir);
  for (size_t i = 0; i < existing_items->len; i++) {
    if (strcmp(str_list_get(existing_items, i), item_name) == 0) {
      str_list_free(existing_items);
      return true;
    }
  }
  str_list_free(existing_items);
  return false;
}

/**
 * @brief Creates or overwrites an item in the CRUD list.
 *
 * @param list The CRUD list structure.
 * @param item_name The name of the item.
 * @param content The content to write to the item.
 */
void create_or_overwrite_item(crud_list *list,
    const char *item_name,
    char *content) {
  path *item_path = get_item_path(list, item_name);
  path_fwrite(item_path, content, 'w');
  path_free(item_path);
}

/**
 * @brief Deletes an item from the CRUD list.
 *
 * @param list The CRUD list structure.
 * @param item_name The name of the item to delete.
 */
void delete_item_basic(crud_list *list, const char *item_name) {
  char print_buf[50];
  snprintf(print_buf, 30, "Deleting\n%s...", item_name);
  print_loading(print_buf);
  path *item_path = get_item_path(list, item_name);
  path_fdelete(item_path);
  path_free(item_path);
}
