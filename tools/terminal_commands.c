#include "tools/terminal_commands.h"

#include <device.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/reent.h>

#include "pico/stdlib.h"

#include "components/hw_manager.h"
#include "components/malloc_mascot.h"
#include "data_structures/string_list.h"
#include "hardware_drivers/joystick.h"
#include "hardware_drivers/ssd1306.h"
#include "terminal.h"
#include "tools/text_editor.h"
#include "tools/virtual_keyboard.h"
#include "utils/path.h"
#include "utils/utils.h"

command create_command(char *name,
    char *description,
    int8_t (*callback)(command_params)) {
  command cmd;
  strncpy(cmd.name, name, sizeof(cmd.name) - 1);
  strncpy(cmd.description, description, sizeof(cmd.description) - 1);
  cmd.name[sizeof(cmd.name) - 1] = '\0';
  cmd.description[sizeof(cmd.description) - 1] = '\0';
  cmd.callback = callback;
  return cmd;
}

int8_t __help__(command_params params) {
  terminal *term = params.term;
  char *output = malloc(256);
  snprintf(output, 256, "Available commands:\n");
  for (size_t i = 0; i < term->commands_count; i++) {
    snprintf(output + strlen(output),
        256 - strlen(output),
        "%s\n",
        term->commands[i].name);
  }
  strcpy(term->stdout_buf, output);
  terminal_display_stdout(term);
  free(output);
  return 0;
}

int8_t __echo__(command_params params) { return 0; }
int8_t __exit__(command_params params) { return -1; }
int8_t __cd__(command_params params) {
  if (params.args->len < 1) {
    strcpy(params.term->stderr_buf, "Usage: cd <directory>\n");
    terminal_display_stderr(params.term);
    return 1;
  }
  char *dir = get(params.args, 0);
  if (strcmp(dir, "..") == 0)
    dir = params.term->current_path->parent->abs_path;
  path *dir_path = path_init(dir);
  path *new_path = path_concat(params.term->current_path, dir_path);
  path_free(dir_path);

  if (path_exists(new_path)) {
    path_free(params.term->current_path);
    params.term->current_path = new_path;
    return 0;
  } else {
    strcpy(params.term->stderr_buf, "Directory not found: ");
    strcat(params.term->stderr_buf, dir);
    terminal_display_stderr(params.term);
    path_free(new_path);
    return 1;
  }
}

int8_t __ls__(command_params params) {
  path *target_path = params.term->current_path;
  bool show_hidden = false;
  char *target_dir = NULL;
  bool need_to_free_target = false;

  for (uinteger i = 0; i < params.args->len; i++) {
    char *arg = get(params.args, i);
    if (strcmp(arg, "-a") == 0)
      show_hidden = true;
    else if (arg[0] != '-')
      target_dir = arg;
  }

  if (target_dir != NULL) {
    if (target_dir[0] == '/') {
      // Absolute path
      target_path = path_init(target_dir);
    } else {
      // Relative path - use path_concat
      path *dir_path = path_init(target_dir);
      target_path = path_concat(params.term->current_path, dir_path);
      path_free(dir_path);
    }
    need_to_free_target = true;

    if (!path_exists(target_path)) {
      strcpy(params.term->stderr_buf, "Directory not found: ");
      strcat(params.term->stderr_buf, target_dir);
      terminal_display_stderr(params.term);
      path_free(target_path);
      return 1;
    }
  }

  str_list *files = path_listdir(target_path);
  str_list *to_show = list_init();
  for (uinteger i = 0; i < files->len; i++) {
    char *file = get(files, i);
    if (show_hidden || file[0] != '.')
      list_append(to_show, file);
  }
  char *content = list_concat(to_show, '\n');
  strcpy(params.term->stdout_buf, content);
  terminal_display_stdout(params.term);

  free(content);
  list_free(files);
  list_free(to_show);
  if (need_to_free_target)
    path_free(target_path);
  return 0;
}

int8_t __pwd__(command_params params) {
  strcpy(params.term->stdout_buf, params.term->current_path->abs_path);
  terminal_display_stdout(params.term);
  return 0;
}

int8_t __mkdir__(command_params params) {
  if (params.args->len < 1) {
    strcpy(params.term->stderr_buf, "Usage: mkdir <directory>\n");
    terminal_display_stderr(params.term);
    return 1;
  }
  char *dir = get(params.args, 0);
  path *dirpath = path_init(dir);
  path *new_dir = path_concat(params.term->current_path, dirpath);
  path_free(dirpath);
  if (path_mkdir(new_dir)) {
    path_free(new_dir);
    return 0;
  } else {
    strcpy(params.term->stderr_buf, "Failed to create directory: ");
    strcat(params.term->stderr_buf, dir);
    terminal_display_stderr(params.term);
    path_free(new_dir);
    return 1;
  }
}

int8_t __touch__(command_params params) {
  if (params.args->len < 1) {
    strcpy(params.term->stderr_buf, "Usage: touch <file>\n");
    terminal_display_stderr(params.term);
    return 1;
  }
  char *file = get(params.args, 0);
  path *file_path = path_init(file);
  path *new_file = path_concat(params.term->current_path, file_path);
  path_free(file_path);
  if (path_ftouch(new_file)) {
    path_free(new_file);
    return 0;
  } else {
    strcpy(params.term->stderr_buf, "Failed to create file: ");
    strcat(params.term->stderr_buf, file);
    terminal_display_stderr(params.term);
    path_free(new_file);
    return 1;
  }
}

int8_t __rm__(command_params params) {
  if (params.args->len < 1) {
    strcpy(params.term->stderr_buf, "Usage: rm <file>\n");
    terminal_display_stderr(params.term);
    return 1;
  }
  char *file = get(params.args, 0);
  path *file_path = path_init(file);
  path *full_path = path_concat(params.term->current_path, file_path);
  path_free(file_path);

  if (path_fdelete(full_path)) {
    path_free(full_path);
    return 0;
  } else {
    strcpy(params.term->stderr_buf, "Failed to delete file: ");
    strcat(params.term->stderr_buf, file);
    terminal_display_stderr(params.term);
    path_free(full_path);
    return 1;
  }
}

int8_t __mv__(command_params params) {
  if (params.args->len < 2) {
    strcpy(params.term->stderr_buf, "Usage: mv <source> <destination>\n");
    terminal_display_stderr(params.term);
    return 1;
  }
  char *src = get(params.args, 0);
  char *dest = get(params.args, 1);

  path *src_path = path_init(src);
  path *dest_path = path_init(dest);
  path *full_src_path = path_concat(params.term->current_path, src_path);
  path *full_dest_path = path_concat(params.term->current_path, dest_path);
  path_free(src_path);
  path_free(dest_path);

  if (path_rename(full_src_path, full_dest_path)) {
    path_free(full_src_path);
    path_free(full_dest_path);
    return 0;
  } else {
    strcpy(params.term->stderr_buf, "Failed to move file: ");
    strcat(params.term->stderr_buf, src);
    terminal_display_stderr(params.term);
    path_free(full_src_path);
    path_free(full_dest_path);
    return 1;
  }
}

int8_t __cp__(command_params params) { printf("__cp__ called"); }
int8_t __cat__(command_params params) { printf("__cat__ called"); }
int8_t __head__(command_params params) { printf("__head__ called"); }
int8_t __tail__(command_params params) { printf("__tail__ called"); }
int8_t __grep__(command_params params) { printf("__grep__ called"); }
int8_t __top__(command_params params) { printf("__top__ called"); }
int8_t __whoami__(command_params params) { printf("__whoami__ called"); }
int8_t __ping__(command_params params) { printf("__ping__ called"); }
int8_t __nano__(command_params params) { printf("__nano__ called"); }
int8_t __sensors__(command_params params) { printf("__sensors__ called"); }
int8_t __history__(command_params params) { printf("__history__ called"); }