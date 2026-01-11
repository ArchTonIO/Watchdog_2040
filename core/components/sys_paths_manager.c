// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include "core/components/sys_paths_manager.h"

#include <stdlib.h>

#include "core/components/hw_manager.h"
#include "core/components/malloc_mascot.h"
#include "core/hardware_drivers/ssd1306.h"
#include "core/utils/path.h"

sys_paths_manager *sys_paths;

void sys_paths_manager_load_dirs();
void sys_paths_manager_load_files();
void sys_paths_manager_mkdirs();
void sys_paths_manager_ftouch();

/**
 * @brief Load all files and directories paths into the sys_paths structure
 */
void sys_paths_manager_load() {
  if (!sys_paths->load_executed) {
    sys_paths_manager_load_dirs();
    sys_paths_manager_load_files();
    sys_paths->load_executed = true;
  }
}

/**
 * @brief Create all necessary system directories and files
 */
void sys_paths_manager_make() {
  sys_paths_manager_mkdirs();
  sys_paths_manager_ftouch();
}

sys_paths_manager *sys_paths_manager_init() {
  sys_paths_manager *manager = malloc(sizeof(sys_paths_manager));
  manager->dirs = malloc(sizeof(sys_dirs));
  manager->files = malloc(sizeof(sys_files));
  manager->load_executed = false;
  sys_paths = manager;
  return manager;
}

/**
 * @brief Load the system directories into the sys_paths structure.
 */
void sys_paths_manager_load_dirs() {
  path *home_dir = path_init(HOME_DIR);
  path *user_dir = path_init(malloc_memories_inst->username);
  path *malloc_mascot_dir = path_init(MALLOC_MASCOT_DIR);
  path *messages_dir = path_init(MESSAGES_DIR);
  path *contacts_dir = path_init(CONTACTS_DIR);
  path *logs_dir = path_init(LOGS_DIR);
  path *config_dir = path_init(CONFIG_DIR);
  path *notes_dir = path_init(NOTES_DIR);
  path *todo_dir = path_init(TODO_DIR);
  path *pwd_manager_dir = path_init(PWD_MANAGER_DIR);
  path *temp_paths[] = {home_dir,
      user_dir,
      malloc_mascot_dir,
      messages_dir,
      contacts_dir,
      logs_dir,
      config_dir,
      notes_dir,
      todo_dir,
      pwd_manager_dir};
  sys_paths->dirs->root_path = path_init(ROOT_DIR);
  sys_paths->dirs->home_path = path_init(home_dir->abs_path);
  sys_paths->dirs->user_path = path_concat(sys_paths->dirs->home_path,
      user_dir);
  sys_paths->dirs->malloc_mascot_path = path_concat(sys_paths->dirs->user_path,
      malloc_mascot_dir);
  sys_paths->dirs->messages_path = path_concat(sys_paths->dirs->user_path,
      messages_dir);
  sys_paths->dirs->contacts_path = path_concat(sys_paths->dirs->user_path,
      contacts_dir);
  sys_paths->dirs->logs_path = path_concat(sys_paths->dirs->user_path,
      logs_dir);
  sys_paths->dirs->config_path = path_concat(sys_paths->dirs->user_path,
      config_dir);
  sys_paths->dirs->notes_path = path_concat(sys_paths->dirs->user_path,
      notes_dir);
  sys_paths->dirs->todo_path = path_concat(sys_paths->dirs->user_path,
      todo_dir);
  sys_paths->dirs->pwd_manager_path = path_concat(sys_paths->dirs->user_path,
      pwd_manager_dir);
  for (uint8_t i = 0; i < sizeof(temp_paths) / sizeof(temp_paths[0]); i++)
    path_free(temp_paths[i]);
}

/**
 * @brief Create the system directories loaded in sys_paths.
 */
void sys_paths_manager_mkdirs() {
  path *paths[] = {
      sys_paths->dirs->home_path,
      sys_paths->dirs->user_path,
      sys_paths->dirs->malloc_mascot_path,
      sys_paths->dirs->messages_path,
      sys_paths->dirs->contacts_path,
      sys_paths->dirs->logs_path,
      sys_paths->dirs->config_path,
      sys_paths->dirs->notes_path,
      sys_paths->dirs->todo_path,
      sys_paths->dirs->pwd_manager_path,
  };
  ssd1306_clear(drivers->oled_screen);
  ssd1306_print(drivers->oled_screen, "Creating sys dir tree", 0, 0, false);
  ssd1306_show(drivers->oled_screen);
  for (uint8_t i = 0; i < sizeof(paths) / sizeof(paths[0]); i++) {
    if (path_mkdir(paths[i])) {
      ssd1306_print(drivers->oled_screen, "[OK] ", 0, 1 + i, false);
      ssd1306_print(drivers->oled_screen, paths[i]->abs_path, 4, 1 + i, false);
    } else {
      ssd1306_print(drivers->oled_screen, "[ERR] ", 0, 1 + i, false);
      ssd1306_print(drivers->oled_screen, paths[i]->abs_path, 5, 1 + i, false);
    }
    ssd1306_show(drivers->oled_screen);
  }
}

void sys_paths_manager_load_files() {
  path *malloc_memories_file = path_init(MALLOC_MEMORIES_FILE);
  path *log_file = path_init(LOG_FILE);
  path *config_file = path_init(CONFIG_FILE);
  path *temp_files[] = {
      malloc_memories_file,
      log_file,
      config_file,
  };
  sys_paths->files->first_boot_file = path_init(FIRST_BOOT_FILE);
  sys_paths->files->user_file = path_init(USER_FILE);
  sys_paths->files->malloc_memories_file = path_concat(
      sys_paths->dirs->malloc_mascot_path,
      malloc_memories_file);
  sys_paths->files->log_file = path_concat(sys_paths->dirs->logs_path,
      log_file);
  sys_paths->files->config_file = path_concat(sys_paths->dirs->config_path,
      config_file);
  for (uint8_t i = 0; i < sizeof(temp_files) / sizeof(temp_files[0]); i++)
    path_free(temp_files[i]);
}

void sys_paths_manager_ftouch() {
  path *files[] = {
      sys_paths->files->first_boot_file,
      sys_paths->files->user_file,
      sys_paths->files->malloc_memories_file,
      sys_paths->files->log_file,
      sys_paths->files->config_file,
  };
  ssd1306_clear(drivers->oled_screen);
  ssd1306_print(drivers->oled_screen, "Creating sys files", 0, 0, false);
  ssd1306_show(drivers->oled_screen);
  for (uint8_t i = 0; i < sizeof(files) / sizeof(files[0]); i++) {
    if (path_ftouch(files[i])) {
      ssd1306_print(drivers->oled_screen, "[OK] ", 0, 1 + i, false);
      ssd1306_print(drivers->oled_screen, files[i]->abs_path, 4, 1 + i, false);
    } else {
      ssd1306_print(drivers->oled_screen, "[ERR] ", 0, 1 + i, false);
      ssd1306_print(drivers->oled_screen, files[i]->abs_path, 5, 1 + i, false);
    }
    ssd1306_show(drivers->oled_screen);
  }
  ssd1306_clear(drivers->oled_screen);
  ssd1306_show(drivers->oled_screen);
}
