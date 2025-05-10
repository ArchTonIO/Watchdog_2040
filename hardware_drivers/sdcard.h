#ifndef SDCARD_H
#define SDCARD_H
#include "sd_card.h"
#include "ff.h"
#include "data_structures/string_list.h"
#include <stdbool.h>

typedef struct
{
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
bool sdcard_write_file(sdcard *sd, char *filename, char *data, char mode);
str_list *sdcard_read_file(sdcard *sd, char *filename);
str_list *sdcard_list_files(sdcard *sd);
bool sdcard_write_key_value_to_file(sdcard *sd, char *filename, char mode, char *key, char *value);
char *sdcard_read_value_from_file(sdcard *sd, char *filename, char *key);
bool sdcard_file_exists(sdcard *sd, char *filename);
// void sdcard_delete_file(sdcard *sd, char *filename);
// void sdcard_format(sdcard *sd);

#endif