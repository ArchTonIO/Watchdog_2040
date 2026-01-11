// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef CRUD_LIST_H
#define CRUD_LIST_H

#include "core/utils/path.h"

typedef struct crud_list crud_list;

typedef struct crud_list {
  char *name;
  char *items_category_name;
  path *workdir;
  void (*create_callback)(crud_list *self);
  void (*read_update_callback)(crud_list *self, const char *item);
  void (*delete_callback)(crud_list *self, const char *item);
} crud_list;

void launch_crud_list(crud_list *list);
void create_or_overwrite_item(crud_list *list,
    const char *item_name,
    char *content);
void delete_item_basic(crud_list *list, const char *item_name);
bool item_exists(crud_list *list, const char *item_name);
path *get_item_path(crud_list *list, const char *item_name);

#endif
