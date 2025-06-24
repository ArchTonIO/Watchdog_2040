#ifndef PATH_H
#define PATH_H

#include <stdbool.h>

#include "data_structures/string_list.h"

typedef struct path path;

struct path {
  char *abs_path;  /**< The full path of this file or directory */
  char *full_name; /**< The full name of this file or directory, including
                      extension (for files) */
  char *
      name; /**< The name of this file or directory, excluding the extension */
  char *ext;      /**< The extension (just for files) */
  path *parent;   /**< The pointer to the parent directory */
  bool is_dir;    /**< Tells you if this path is a directory */
  bool is_hidden; /**< Tells you if this path is hidden */
};

path *path_init(const char *abs_path);
bool path_fwrite(path *file, const char *data, char mode);
str_list *path_fread(path *file);
bool path_ftouch(path *file);
bool path_fdelete(path *path);
bool path_mkdir(path *dir);
bool path_rmdir(path *dir);
str_list *path_listdir(path *path);
bool path_key_value_dump(path *file,
    char mode,
    const char *key,
    const char *value);
char *path_key_value_get(path *file, const char *key);
bool path_replace_value_at_key(path *file, const char *key, const char *value);
bool path_exists(path *path);
void path_print(const path *path);
void path_free(path *path);

#endif