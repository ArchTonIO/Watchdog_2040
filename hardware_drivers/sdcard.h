#ifndef SDCARD_H
#define SDCARD_H
#include "sd_card.h"
#include "ff.h"
#include "data_structures/string_list.h"

typedef struct
{
  FRESULT fr;
  FATFS fs;
  FIL fil;
  char buf[100];
  char filename[100];
  int ret;
} sdcard;

sdcard *sdcard_init();
uint8_t sdcard_mount(sdcard *sd);
void sdcard_unmount(sdcard *sd);
void sdcard_write_file(sdcard *sd, char *filename, char *data, char mode);
str_list *sdcard_read_file(sdcard *sd, char *filename);
str_list *sdcard_list_files(sdcard *sd);
// void sdcard_delete_file(sdcard *sd, char *filename);
// void sdcard_format(sdcard *sd);

#endif