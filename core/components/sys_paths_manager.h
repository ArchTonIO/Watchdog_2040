// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#ifndef DIRECTORIES_MANAGER_H
#define DIRECTORIES_MANAGER_H

#include <stdbool.h>

#include "core/utils/path.h"

#define FIRST_BOOT_FILE "/.booted"
#define USER_FILE "/.user"

#define ROOT_DIR "/"
#define HOME_DIR "home"
#define MALLOC_MASCOT_DIR "malloc"
#define MESSAGES_DIR "messages"
#define CONTACTS_DIR "contacts"
#define LOGS_DIR "logs"
#define CONFIG_DIR "config"
#define NOTES_DIR "notes"
#define TODO_DIR "todo"

#define MALLOC_MEMORIES_FILE "malloc_memories"
#define CONTACTS_ADDR_FILE "contacts"
#define CONTACTS_NAMES_FILE "contact-names"
#define LOG_FILE "log"
#define CONFIG_FILE "config"

typedef struct {
  path *root_path;
  path *home_path;
  path *user_path;
  path *malloc_mascot_path;
  path *messages_path;
  path *contacts_path;
  path *logs_path;
  path *config_path;
  path *notes_path;
  path *todo_path;
} sys_dirs;

typedef struct {
  path *first_boot_file;
  path *user_file;
  path *malloc_memories_file;
  path *log_file;
  path *config_file;
} sys_files;

typedef struct {
  sys_dirs *dirs;
  sys_files *files;
  bool load_executed;
} sys_paths_manager;

extern sys_paths_manager *sys_paths;

sys_paths_manager *sys_paths_manager_init();
void sys_paths_manager_load();
void sys_paths_manager_make();

#endif