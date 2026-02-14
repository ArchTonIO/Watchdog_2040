// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef SDCARD_H
#define SDCARD_H
#include <stdbool.h>

#include "core/data_structures/include/string_list.h"
#include "core/utils/include/path.h"
#include "ff.h"

typedef struct {
  FRESULT fr;
  FATFS fs;
  FIL fil;
  char buf[100];
  char filename[100];
  int ret;
  bool is_working;
} sdcard_t;

void sdcard_init(sdcard_t *sd);
bool sdcard_mount(sdcard_t *sd);
void sdcard_unmount(sdcard_t *sd);
bool sdcard_write_file(sdcard_t *sd, path *file, const char *data, char mode);
str_list *sdcard_read_file(sdcard_t *sd, path *file);
str_list *sdcard_list_files(sdcard_t *sd, path *directory);
bool sdcard_write_key_value_to_file(sdcard_t *sd,
    path *file,
    char mode,
    const char *key,
    const char *value);
char *sdcard_read_value_from_file(sdcard_t *sd, path *file, const char *key);
bool sdcard_replace_value_at_key(sdcard_t *sd,
    path *file,
    const char *key,
    const char *value);
bool sdcard_file_exists(sdcard_t *sd, path *file);
bool sdcard_delete_file(sdcard_t *sd, path *file);
bool sdcard_rename(sdcard_t *sd, path *src, path *dest);
bool sdcard_touch_file(sdcard_t *sd, path *file);
bool sdcard_mkdir(sdcard_t *sd, path *dir);
bool sdcard_rmdir(sdcard_t *sd, path *dir);
bool sdcard_rmtree(sdcard_t *sd, path *dir);
bool sdcard_path_is_dir(sdcard_t *sd, path *path);

#endif