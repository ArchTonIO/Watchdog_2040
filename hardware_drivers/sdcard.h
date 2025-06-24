#ifndef SDCARD_H
#define SDCARD_H
#include <stdbool.h>

#include "data_structures/string_list.h"
#include "ff.h"
#include "utils/path.h"

typedef struct {
  FRESULT fr;
  FATFS fs;
  FIL fil;
  char buf[100];
  char filename[100];
  int ret;
  bool is_working;
} sdcard;

sdcard *sdcard_init();
bool sdcard_mount(sdcard *sd);
void sdcard_unmount(sdcard *sd);
bool sdcard_write_file(sdcard *sd, path *file, const char *data, char mode);
str_list *sdcard_read_file(sdcard *sd, path *file);
str_list *sdcard_list_files(sdcard *sd, path *directory);
bool sdcard_write_key_value_to_file(sdcard *sd,
    path *file,
    char mode,
    const char *key,
    const char *value);
char *sdcard_read_value_from_file(sdcard *sd, path *file, const char *key);
bool sdcard_replace_value_at_key(sdcard *sd,
    path *file,
    const char *key,
    const char *value);
bool sdcard_file_exists(sdcard *sd, path *file);
bool sdcard_delete_file(sdcard *sd, path *file);
bool sdcard_touch_file(sdcard *sd, path *file);
bool sdcard_mkdir(sdcard *sd, path *dir);
bool sdcard_rmdir(sdcard *sd, path *dir);
bool sdcard_path_is_dir(sdcard *sd, path *path);
// void sdcard_format(sdcard *sd);

#endif