// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include "core/utils/path.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/components/hw_manager.h"
#include "core/data_structures/string_list.h"
#include "core/hardware_drivers/sdcard.h"
#include "core/utils/utils.h"

void generate_parent(path *path);

/**
 * @brief Initializes a new path structure with the given absolute path.
 *
 * @param abs_path The absolute path of the file or directory.
 * @return A pointer to the initialized path structure.
 */
path *path_init(const char *abs_path) {
  path *new_path = malloc(sizeof(path));
  if (!new_path)
    return NULL;
  new_path->abs_path = strdup(abs_path);
  char *last_slash = strrchr(abs_path, '/');
  const char *full_name = last_slash ? last_slash + 1 : abs_path;
  new_path->full_name = strdup(full_name);
  new_path->name = strdup(full_name);
  char *dot = strrchr(new_path->name, '.');
  if (dot && dot != new_path->name) {
    new_path->ext = strdup(dot + 1);
    *dot = '\0';
  } else
    new_path->ext = NULL;
  new_path->is_dir = (sdcard_path_is_dir(drivers->sd_card, new_path));
  new_path->is_hidden = (new_path->full_name[0] == '.');
  generate_parent(new_path);
  return new_path;
}

/**
 * @brief Writes data to a file at the specified path.
 *
 * @param file The path structure representing the file.
 * @param data The data to write to the file.
 * @param mode The mode to open the file in ('w' for write, 'a' for append).
 * @return true if the write was successful, false otherwise.
 */
bool path_fwrite(path *file, const char *data, char mode) {
  sdcard_write_file(drivers->sd_card, file, data, mode);
}

/**
 * @brief Reads the contents of a file at the specified path.
 *
 * @param file The path structure representing the file.
 * @return A pointer to a string list containing the file's contents, or NULL
 * if an error occurred.
 */
str_list *path_fread(path *file) {
  return sdcard_read_file(drivers->sd_card, file);
}

/**
 * @brief Touches a file at the specified path, creating it if it does not
 * exist.
 *
 * @param file The path structure representing the file.
 * @return true if the touch operation was successful, false otherwise.
 */
bool path_ftouch(path *file) {
  return sdcard_touch_file(drivers->sd_card, file);
}

/**
 * @brief Deletes a file at the specified path.
 *
 * @param path The path structure representing the file to delete.
 * @return true if the deletion was successful, false otherwise.
 */
bool path_fdelete(path *path) {
  return sdcard_delete_file(drivers->sd_card, path);
}

/**
 * @brief Creates a directory at the specified path.
 *
 * @param dir The path structure representing the directory to create.
 * @return true if the directory was created successfully, false otherwise.
 */
bool path_mkdir(path *dir) { return sdcard_mkdir(drivers->sd_card, dir); }

/**
 * @brief Removes a directory at the specified path.
 *
 * @param dir The path structure representing the directory to remove.
 * @return true if the directory was removed successfully, false otherwise.
 */
bool path_rmdir(path *dir) { return sdcard_rmdir(drivers->sd_card, dir); }

/**
 * @brief Removes all of the childs (either files or directories) of a
 * directory, and the directory itself
 *
 * @param dir The path structure representing the directory to remove.
 * @return true if the directory and its contents were removed successfully,
 * false otherwise.
 */
bool path_rmtree(path *dir) { return sdcard_rmtree(drivers->sd_card, dir); }

/**
 * @brief Renames a file or directory from the source path to the destination
 * path.
 *
 * @param src The source path structure representing the file or directory to
 * rename.
 * @param dest The destination path structure representing the new name.
 * @return true if the rename operation was successful, false otherwise.
 */
bool path_rename(path *src, path *dest) {
  return sdcard_rename(drivers->sd_card, src, dest);
}

/**
 * @brief Copies the contents of a file from the source path to the destination
 * path.
 *
 * @param src The source path structure representing the file to copy.
 * @param dest The destination path structure representing the new file.
 * @return true if the copy operation was successful, false otherwise.
 */
bool path_fcopy(path *src, path *dest) {
  str_list *src_content = sdcard_read_file(drivers->sd_card, src);
  char *concat_content = str_list_concat(src_content, '\n');
  bool ret = sdcard_write_file(drivers->sd_card, dest, concat_content, 'w');
  free(concat_content);
  str_list_free(src_content);
  return ret;
}

/**
 * @brief Lists the files in a directory at the specified path.
 *
 * @param path The path structure representing the directory.
 * @return A pointer to a string list containing the names of the files in
 * the directory.
 */
str_list *path_listdir(path *path) {
  return sdcard_list_files(drivers->sd_card, path);
}

/**
 * @brief Dumps a key-value pair to a file at the specified path.
 *
 * @param file The path structure representing the file.
 * @param mode The mode to open the file in ('w' for write, 'a' for append).
 * @param key The key to write.
 * @param value The value to write.
 * @return true if the dump was successful, false otherwise.
 */
bool path_key_value_dump(path *file,
    char mode,
    const char *key,
    const char *value) {
  return sdcard_write_key_value_to_file(drivers->sd_card,
      file,
      mode,
      key,
      value);
}

bool path_replace_value_at_key(path *file,
    const char *key,
    const char *value) {
  return sdcard_replace_value_at_key(drivers->sd_card, file, key, value);
}

/**
 * @brief Retrieves the value associated with a key from a file at the
 * specified path.
 *
 * @param file The path structure representing the file.
 * @param key The key to look for.
 * @return true if the value was found, false otherwise.
 */
char *path_key_value_get(path *file, const char *key) {
  return sdcard_read_value_from_file(drivers->sd_card, file, key);
}

/**
 * @brief Checks if a file or directory exists at the specified path.
 *
 * @param path The path structure representing the file or directory.
 * @return true if the file or directory exists, false otherwise.
 */
bool path_exists(path *path) {
  return sdcard_file_exists(drivers->sd_card, path);
}

path *path_concat(path *path_1, path *path_2) {
  char *path_1_slash = string_add(path_1->abs_path, "/");
  char *new_abs_path = string_add(path_1_slash, path_2->abs_path);
  path *new_path = path_init(new_abs_path);
  free(new_abs_path);
  free(path_1_slash);
  return new_path;
}

/**
 * @brief Prints the details of a path structure.
 *
 * @param path The path structure to print.
 */
void path_print(const path *path) {
  printf("Absolute Path: %s\n", path->abs_path);
  printf("Full Name: %s\n", path->full_name);
  printf("Name: %s\n", path->name);
  printf("Extension: %s\n", path->ext ? path->ext : "None");
  printf("Is Directory: %s\n", path->is_dir ? "Yes" : "No");
  printf("Is Hidden: %s\n", path->is_hidden ? "Yes" : "No");
  if (path->parent)
    printf("Parent Path: %s\n", path->parent->abs_path);
  else
    printf("Parent Path: None\n");
}

/**
 * @brief Frees the memory allocated for a path structure.
 *
 * @param path The path structure to free.
 */
void path_free(path *path) {
  free(path->abs_path);
  free(path->name);
  free(path->ext);
  free(path->full_name);
  if (path->parent != NULL)
    path_free(path->parent);
  free(path);
}

void generate_parent(path *path) {
  const char *abs_path = path->abs_path;
  if (strcmp(abs_path, "/") == 0) {
    path->parent = NULL;
    return;
  }
  char *last_slash = strrchr(abs_path, '/');
  if (!last_slash) {
    path->parent = NULL;
    return;
  }
  if (last_slash == abs_path) {
    path->parent = path_init("/");
    return;
  }
  size_t parent_len = last_slash - abs_path;
  char *parent_path = malloc(parent_len + 1);
  if (!parent_path) {
    path->parent = NULL;
    return;
  }
  strncpy(parent_path, abs_path, parent_len);
  parent_path[parent_len] = '\0';
  path->parent = path_init(parent_path);
  free(parent_path);
}