#include "sdcard.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/components/sys_paths_manager.h"
#include "core/data_structures/string_list.h"
#include "core/utils/path.h"
#include "device.h"
#include "ff.h"
#include "sd_card.h"

sdcard *sdcard_init() {
  if (!sd_init_driver()) {
    printf("ERROR: Could not initialize SD card\r\n");
  }
  sdcard *sd = (sdcard *)malloc(sizeof(sdcard));
  sd->is_working = true;
  return sd;
}

bool sdcard_mount(sdcard *sd) {
  sd->fr = f_mount(&sd->fs, ROOT_DIR, 1);
  if (sd->fr != FR_OK) {
    printf("ERROR: Could not mount filesystem (%d)\r\n", sd->fr);
    sd->is_working = false;
    return false;
  }
  return true;
}

void sdcard_unmount(sdcard *sd) { f_unmount("0:"); }

/**
 * @brief Write content to a file on the SD card.
 * @param sd The sdcard instance
 * @param file The path to the file to write to
 * @param content The content to write to the file
 * @param mode The mode to open the file in ('w' for write, 'a' for append)
 * @return true if the write was successful, false otherwise
 */
bool sdcard_write_file(sdcard *sd,
    path *file,
    const char *content,
    char mode) {
  if (file->is_dir) {
    printf("ERROR: Cannot write to a directory (%s)\r\n", file->abs_path);
    return false;
  }
  if (mode == 'w')
    sd->fr = f_open(&sd->fil, file->abs_path, FA_WRITE | FA_CREATE_ALWAYS);
  else if (mode == 'a')
    sd->fr = f_open(&sd->fil, file->abs_path, FA_WRITE | FA_OPEN_APPEND);
  if (sd->fr != FR_OK) {
    printf("ERROR: Could not open file (%d)\r\n", sd->fr);
    return false;
  }
  sd->ret = f_printf(&sd->fil, content);
  if (sd->ret < 0) {
    printf("ERROR: Could not write to file (%d)\r\n", sd->ret);
    f_close(&sd->fil);
    return false;
  }
  sd->fr = f_close(&sd->fil);
  if (sd->fr != FR_OK) {
    printf("ERROR: Could not close file (%d)\r\n", sd->fr);
    return false;
  }
  return true;
}

/**
 * @brief Read the content of a file on the SD card line by line.
 * @param sd The sdcard instance
 * @param file The path to the file to read
 * @return A list of strings, each representing a line in the file
 */
str_list *sdcard_read_file(sdcard *sd, path *file) {
  str_list *lines = list_init();
  sd->fr = f_open(&sd->fil, file->abs_path, FA_READ);
  if (sd->fr != FR_OK) {
    printf("ERROR: Could not open file (%d)\r\n", sd->fr);
    return lines;
  }
  while (f_gets(sd->buf, sizeof(sd->buf), &sd->fil)) {
    char *line = (char *)malloc(strlen(sd->buf) + 1);
    strcpy(line, sd->buf);
    list_append(lines, line);
    free(line);
  }
  sd->fr = f_close(&sd->fil);
  if (sd->fr != FR_OK) {
    printf("ERROR: Could not close file (%d)\r\n", sd->fr);
    return lines;
  }
  return lines;
}

/**
 * @brief List all files in a directory on the SD card.
 * @param sd The sdcard instance
 * @param directory The path to the directory to list files from
 * @return A list of strings, each representing a file name in the directory
 */
str_list *sdcard_list_files(sdcard *sd, path *directory) {

  str_list *files = list_init();
  DIR dir;
  FILINFO fno;
  sd->fr = f_opendir(&dir, directory->abs_path);
  if (sd->fr != FR_OK) {
    printf("ERROR: Could not open directory (%d)\r\n", sd->fr);
    return files;
  }
  for (;;) {
    sd->fr = f_readdir(&dir, &fno);
    if (sd->fr != FR_OK || fno.fname[0] == 0)
      break;
    char *file = (char *)malloc(strlen(fno.fname) + 1);
    strcpy(file, fno.fname);
    list_append(files, file);
    free(file);
  }
  sd->fr = f_closedir(&dir);
  if (sd->fr != FR_OK) {
    printf("ERROR: Could not close directory (%d)\r\n", sd->fr);
    return files;
  }
  return files;
}

/**
 * @brief Write a key-value pair to a file in the format "key~value\n"
 *
 * @param sd The sdcard instance
 * @param filename The name of the file to write to
 * @param mode The mode to open the file in ('w' for write, 'a' for
 * list_append)
 * @param key The key to write
 * @param value The value to write
 * @return true if the write was successful, false otherwise
 *
 * @note it is assumed that the key and value do not contain the separator
 * character '~'
 */
bool sdcard_write_key_value_to_file(sdcard *sd,
    path *file,
    char mode,
    const char *key,
    const char *value) {
  char sep = '~';
  size_t total_len = strlen(key) + strlen(value) + 3;
  char *content = (char *)malloc(total_len);
  snprintf(content, total_len, "%s%c%s\n", key, sep, value);
  bool res = sdcard_write_file(sd, file, content, mode);
  free(content);
  return res;
}

/**
 * @brief Replace the value at a given key in a file with a new value.
 *
 * @param sd The sdcard instance
 * @param file The path to the file to modify
 * @param key The key to search for
 * @param value The new value to set
 * @return true if the key was found and replaced, false otherwise
 *
 * @note it is assumed that the key and value do not contain the separator
 * character '~'
 */
bool sdcard_replace_value_at_key(sdcard *sd,
    path *file,
    const char *key,
    const char *value) {
  bool found = false;
  str_list *lines = sdcard_read_file(sd, file);
  str_list *new_lines = list_init();
  for (uint16_t i = 0; i < lines->len; i++) {
    char *line = strdup(get(lines, i));
    char *delimiter = strchr(line, '~');
    if (delimiter != NULL) {
      *delimiter = '\0';
      if (strcmp(line, key) == 0) {
        size_t new_len = strlen(key) + strlen(value) + 3;
        char *new_line = (char *)malloc(new_len);
        snprintf(new_line, new_len, "%s~%s\n", key, value);
        list_append(new_lines, new_line);
        free(new_line);
        found = true;
      } else {
        list_append(new_lines, get(lines, i));
      }
      free(line);
    }
  }
  char *new_file_content = (char *)malloc(1);
  new_file_content[0] = '\0';
  for (uint16_t i = 0; i < new_lines->len; i++) {
    char *line = get(new_lines, i);
    size_t new_len = strlen(new_file_content) + strlen(line) + 1;
    new_file_content = (char *)realloc(new_file_content, new_len);
    strcat(new_file_content, line);
  }
  sdcard_write_file(sd, file, new_file_content, 'w');
  free(new_file_content);
  list_free(lines);
  list_free(new_lines);
  return found;
}

/**
 * @brief Read a value from a file given a key in the format "key~value\n"
 *
 * @param sd The sdcard instance
 * @param filename The name of the file to read from
 * @param key The key to search for
 * @return char* The value associated with the key, or NULL if not found
 *
 * @note it is assumed that the key and value do not contain the separator
 * character '~'
 */
char *sdcard_read_value_from_file(sdcard *sd, path *file, const char *key) {
  char sep = '~';
  str_list *lines = sdcard_read_file(sd, file);
  for (uint16_t i = 0; i < lines->len; i++) {
    char *line = get(lines, i);
    char *delimiter = strchr(line, sep);
    *delimiter = '\0';
    if (strcmp(line, key) == 0) {
      char *value = (char *)malloc(strlen(delimiter + 1) + 1);
      strcpy(value, delimiter + 1);
      list_free(lines);
      return value;
    }
  }
  list_free(lines);
  return NULL;
}

/**
 * @brief Check if a file exists on the SD card.
 *
 * @param sd The sdcard instance
 * @param file The path to the file to check
 * @return true if the file exists, false otherwise
 */
bool sdcard_file_exists(sdcard *sd, path *file) {
  str_list *files = sdcard_list_files(sd, file->parent);
  bool found = false;
  if (list_index_of(files, file->full_name) == -1)
    found = false;
  else
    found = true;
  list_free(files);
  return found;
}

/**
 * @brief Delete a file from the SD card.
 *
 * @param sd The sdcard instance
 * @param file The path to the file to delete
 * @return true if the file was successfully deleted, false otherwise
 */
bool sdcard_delete_file(sdcard *sd, path *file) {
  if (file->is_dir == true) {
    printf("ERROR: The path '%s' is a directory, not a file.\r\n",
        file->abs_path);
    return false;
  }
  sd->fr = f_unlink(file->abs_path);
  if (sd->fr != FR_OK) {
    printf("ERROR: Could not delete file (%d)\r\n", sd->fr);
    return false;
  }
  return true;
}

bool sdcard_rename(sdcard *sd, path *src, path *dest) {
  if (src->is_dir) {
    printf("ERROR: Cannot rename a directory (%s)\r\n", src->abs_path);
    return false;
  }
  sd->fr = f_rename(src->abs_path, dest->abs_path);
  if (sd->fr != FR_OK) {
    printf("ERROR: Could not rename file (%d)\r\n", sd->fr);
    return false;
  }
  return true;
}

/**
 * @brief Create an empty file on the SD card if it does not exist.
 *
 * @param sd The sdcard instance
 * @param file The path to the file to create
 * @return true if the file was created or already exists, false otherwise
 */
bool sdcard_touch_file(sdcard *sd, path *file) {
  if (!sdcard_file_exists(sd, file))
    return sdcard_write_file(sd, file, "", 'w');
}

/**
 * @brief Create a directory on the SD card.
 *
 * @param sd The sdcard instance
 * @param dir The path to the directory to create
 * @return true if the directory was successfully created, false otherwise
 */
bool sdcard_mkdir(sdcard *sd, path *dir) {
  sd->fr = f_mkdir(dir->abs_path);
  if (sd->fr != FR_OK) {
    printf("ERROR: Could not create directory (%d)\r\n", sd->fr);
    return false;
  }
  return true;
}

/**
 * @brief Remove a directory from the SD card.
 *
 * @param sd The sdcard instance
 * @param dir The path to the directory to remove
 * @return true if the directory was successfully removed, false otherwise
 */
bool sdcard_rmdir(sdcard *sd, path *dir) {
  if (dir->is_dir == false) {
    printf("ERROR: The path '%s' is not a directory.\r\n", dir->abs_path);
    return false;
  }
  sd->fr = f_unlink(dir->abs_path);
  if (sd->fr != FR_OK) {
    printf("ERROR: Could not remove directory (%d)\r\n", sd->fr);
    return false;
  }
  return true;
}

/**
 * @brief Check if a path is a directory.
 *
 * @param sd The sdcard instance
 * @param path The path to check
 * @return true if the path is a directory, false otherwise
 */
bool sdcard_path_is_dir(sdcard *sd, path *path) {
  FILINFO fno;
  FRESULT res = f_stat(path->abs_path, &fno);
  if (res == FR_OK)
    return (fno.fattrib & AM_DIR) != 0;
  else
    return false;
}