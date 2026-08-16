// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "apps/terminal/include/terminal_commands.h"

#include <device.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/reent.h>

#include "pico/bootrom.h"
#include "pico/stdio_usb.h"
#include "pico/time.h"

#include "apps/pwd_manager/include/pwd_manager_cli.h"
#include "apps/system_app/include/system_app.h"
#include "apps/terminal/include/bitmaps.h"
#include "apps/text_editor/include/text_editor.h"
#include "core/components/include/hw_manager.h"
#include "core/components/include/malloc_mascot.h"
#include "core/components/include/sys_paths_manager.h"
#include "core/data_structures/include/string_list.h"
#include "core/hardware_drivers/include/ssd1306.h"
#include "core/tools/include/sha_256.h"
#include "core/ulmp/include/ulmp.h"
#include "core/utils/include/path.h"
#include "core/utils/include/utils.h"
#include "device.h"
#include "hardware/watchdog.h"
#include "include/terminal.h"

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

/**
 * @brief Displays the help message with available commands.
 *
 * @param params The command parameters containing the terminal instance.
 * @return 0 on success, 1 on error, -1 to exit the terminal.
 */
int8_t __help__(command_params params) {
  terminal *term = params.term;
  if (term->on_serial) {
    for (size_t i = 0; i < term->commands_count; i++)
      printf("> %s: %s\n",
          term->commands[i].name,
          term->commands[i].description);
    return 0;
  }
  char *output = malloc(256);
  snprintf(output, 256, "Available commands:\n\n");
  for (size_t i = 0; i < term->commands_count; i++) {
    snprintf(output + strlen(output),
        256 - strlen(output),
        "> %s\n",
        term->commands[i].name);
  }
  strcpy(term->stdout_buf, output);
  terminal_display_stdout(term);
  free(output);
  return 0;
}

/**
 * @brief Prints the given text to the terminal.
 *
 * @param params The command parameters containing the terminal instance and
 * arguments.
 * @return 0 on success, 1 on error, -1 to exit the terminal.
 */
int8_t __echo__(command_params params) {
  if (params.args->len == 0) {
    strcpy(params.term->stderr_buf, "Usage: echo <text>\n");
    terminal_display_stderr(params.term);
    return 1;
  }
  char *text = str_list_get(params.args, 0);
  strcpy(params.term->stdout_buf, text);
  terminal_display_stdout(params.term);
  return 0;
}

/**
 * @brief Exits the terminal.
 *
 * @param params The command parameters containing the terminal instance.
 * @return -1 to exit the terminal.
 */
int8_t __esc__(command_params params) { return -1; }

/**
 * @brief Changes the current working directory.
 *
 * @param params The command parameters containing the terminal instance and
 * arguments.
 * @return 0 on success, 1 on error, -1 to exit the terminal.
 */
int8_t __cd__(command_params params) {
  if (params.args->len < 1) {
    path_free(params.term->current_path);
    params.term->current_path = path_init(
        sys_paths->dirs->user_path->abs_path);
    return 0;
  }
  char *dir = str_list_get(params.args, 0);
  if (strcmp(dir, "..") == 0) {
    if (strcmp(params.term->current_path->abs_path, HOME_DIR) == 0) {
      return 0;
    } else {
      path *old_path = params.term->current_path;
      params.term->current_path = path_init(
          params.term->current_path->parent->abs_path);
      path_free(old_path);
      return 0;
    }
  }
  if (strcmp(dir, ".") == 0)
    return 0;
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

/**
 * @brief Lists the files in the current directory or a specified directory.
 *
 * @param params The command parameters containing the terminal instance and
 * arguments.
 * @return 0 on success, 1 on error, -1 to exit the terminal.
 */
int8_t __ls__(command_params params) {
  path *target_path = params.term->current_path;
  bool show_hidden = false;
  char *target_dir = NULL;
  bool need_to_free_target = false;
  for (uinteger i = 0; i < params.args->len; i++) {
    char *arg = str_list_get(params.args, i);
    if (strcmp(arg, "-a") == 0)
      show_hidden = true;
    else if (arg[0] != '-')
      target_dir = arg;
  }
  if (target_dir != NULL) {
    if (target_dir[0] == '/') {
      target_path = path_init(target_dir);
    } else {
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
  str_list *to_show = str_list_init();
  for (uinteger i = 0; i < files->len; i++) {
    char *file = str_list_get(files, i);
    if (show_hidden || file[0] != '.')
      str_list_append(to_show, file);
  }
  char *content = str_list_concat(to_show, '\n');
  strcpy(params.term->stdout_buf, content);
  terminal_display_stdout(params.term);
  free(content);
  str_list_free(files);
  str_list_free(to_show);
  if (need_to_free_target)
    path_free(target_path);
  return 0;
}

/**
 * @brief Prints the current working directory.
 *
 * @param params The command parameters containing the terminal instance.
 * @return 0 on success, 1 on error, -1 to exit the terminal.
 */
int8_t __pwd__(command_params params) {
  strcpy(params.term->stdout_buf, params.term->current_path->abs_path);
  terminal_display_stdout(params.term);
  return 0;
}

/**
 * @brief Creates a new directory.
 *
 * @param params The command parameters containing the terminal instance and
 * arguments.
 * @return 0 on success, 1 on error, -1 to exit the terminal.
 */
int8_t __mkdir__(command_params params) {
  if (params.args->len < 1) {
    strcpy(params.term->stderr_buf, "Usage: mkdir <directory>\n");
    terminal_display_stderr(params.term);
    return 1;
  }
  char *dir = str_list_get(params.args, 0);
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

/**
 * @brief Creates a new file or updates the timestamp of an existing file.
 *
 * @param params The command parameters containing the terminal instance and
 * arguments.
 * @return 0 on success, 1 on error, -1 to exit the terminal.
 */
int8_t __touch__(command_params params) {
  if (params.args->len < 1) {
    strcpy(params.term->stderr_buf, "Usage: touch <file>\n");
    terminal_display_stderr(params.term);
    return 1;
  }
  char *file = str_list_get(params.args, 0);
  path *file_path = path_init(file);
  path *new_file = path_concat(params.term->current_path, file_path);
  path_free(file_path);
  if (path_ftouch(new_file)) {
    path_free(new_file);
    return 0;
  } else {
    strcpy(params.term->stderr_buf, "Failed to create file: ");
    strcat(params.term->stderr_buf, file);
    strcat(params.term->stderr_buf, " at path: ");
    strcat(params.term->stderr_buf, new_file->abs_path);
    terminal_display_stderr(params.term);
    path_free(new_file);
    return 1;
  }
}

/**
 * @brief Deletes a file.
 *
 * @param params The command parameters containing the terminal instance and
 * arguments.
 * @return 0 on success, 1 on error, -1 to exit the terminal.
 */
int8_t __rm__(command_params params) {
  bool recursive = false;
  if (params.args->len < 1) {
    strcpy(params.term->stderr_buf, "Usage: rm <file>\n");
    terminal_display_stderr(params.term);
    return 1;
  }
  if (params.args->len > 1 && strcmp(str_list_get(params.args, 0), "-r") == 0)
    recursive = true;
  char *file = str_list_get(params.args, recursive ? 1 : 0);
  path *file_path = path_init(file);
  path *full_path = path_concat(params.term->current_path, file_path);
  path_free(file_path);
  if (!path_exists(full_path)) {
    strcpy(params.term->stderr_buf, "File not found: ");
    strcat(params.term->stderr_buf, file);
    terminal_display_stderr(params.term);
    path_free(full_path);
    return 1;
  }
  if (recursive && full_path->is_dir) {
    if (path_rmtree(full_path)) {
      path_free(full_path);
      return 0;
    } else {
      strcpy(params.term->stderr_buf, "Failed to remove directory: ");
      strcat(params.term->stderr_buf, file);
      terminal_display_stderr(params.term);
      path_free(full_path);
      return 1;
    }
  }
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

/**
 * @brief Moves a file or directory to a new location.
 *
 * @param params The command parameters containing the terminal instance and
 * arguments.
 * @return 0 on success, 1 on error, -1 to exit the terminal.
 */
int8_t __mv__(command_params params) {
  if (params.args->len < 2) {
    strcpy(params.term->stderr_buf, "Usage: mv <source> <destination>\n");
    terminal_display_stderr(params.term);
    return 1;
  }
  char *src = str_list_get(params.args, 0);
  char *dest = str_list_get(params.args, 1);
  path *src_path = path_init(src);
  path *dest_path = path_init(dest);
  path *full_src_path = path_concat(params.term->current_path, src_path);
  path *full_dest_path = path_concat(params.term->current_path, dest_path);
  path_free(src_path);
  path_free(dest_path);
  if (!path_exists(full_src_path)) {
    strcpy(params.term->stderr_buf, "Source file not found: ");
    strcat(params.term->stderr_buf, src);
    terminal_display_stderr(params.term);
    path_free(full_src_path);
    path_free(full_dest_path);
    return 1;
  }
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

/**
 * @brief Copies the file content from source to destination.
 *
 * @param params The command parameters containing the terminal instance and
 * arguments.
 * @return 0 on success, 1 on error, -1 to exit the terminal.
 */
int8_t __cp__(command_params params) {
  if (params.args->len < 2) {
    strcpy(params.term->stderr_buf, "Usage: cp <source> <destination>\n");
    terminal_display_stderr(params.term);
    return 1;
  }
  char *src = str_list_get(params.args, 0);
  char *dest = str_list_get(params.args, 1);
  path *src_path = path_init(src);
  path *dest_path = path_init(dest);
  path *full_src_path = path_concat(params.term->current_path, src_path);
  path *full_dest_path = path_concat(params.term->current_path, dest_path);
  path_free(src_path);
  path_free(dest_path);
  if (!path_exists(full_src_path) || !path_exists(full_dest_path)) {
    strcpy(params.term->stderr_buf, "Source or dest file not found: ");
    strcat(params.term->stderr_buf, src);
    strcat(params.term->stderr_buf, " ");
    strcat(params.term->stderr_buf, dest);
    terminal_display_stderr(params.term);
    path_free(full_src_path);
    path_free(full_dest_path);
    return 1;
  }
  if (path_fcopy(full_src_path, full_dest_path)) {
    path_free(full_src_path);
    path_free(full_dest_path);
    return 0;
  } else {
    strcpy(params.term->stderr_buf, "Failed to copy file: ");
    strcat(params.term->stderr_buf, src);
    terminal_display_stderr(params.term);
    path_free(full_src_path);
    path_free(full_dest_path);
    return 1;
  }
}

/**
 * @brief Reads the content of a file and prints it to the terminal.
 *
 * @param params The command parameters containing the terminal instance and
 * arguments.
 * @return 0 on success, 1 on error, -1 to exit the terminal.
 */
int8_t __cat__(command_params params) {
  if (params.args->len < 1) {
    strcpy(params.term->stderr_buf, "Usage: cat <file>\n");
    terminal_display_stderr(params.term);
    return 1;
  }
  char *file = str_list_get(params.args, 0);
  if (params.args->len > 1)
    file = str_list_concat(params.args, ' ');

  path *file_path = path_init(file);
  path *full_path = path_concat(params.term->current_path, file_path);
  path_free(file_path);
  if (!path_exists(full_path)) {
    strcpy(params.term->stderr_buf, "File not found: ");
    strcat(params.term->stderr_buf, file);
    terminal_display_stderr(params.term);
    path_free(full_path);
    return 1;
  }
  str_list *lines = path_fread(full_path);
  if (lines == NULL) {
    strcpy(params.term->stderr_buf, "Failed to read file: ");
    strcat(params.term->stderr_buf, file);
    terminal_display_stderr(params.term);
    path_free(full_path);
    return 1;
  }
  char *content = str_list_concat(lines, NO_SEP);
  strcpy(params.term->stdout_buf, content);
  terminal_display_stdout(params.term);
  free(content);
  str_list_free(lines);
  path_free(full_path);
  return 0;
}

/**
 *@brief prints out the first n lines of a file (10 by default)
 *
 * @param params The command parameters containing the terminal instance and
 * arguments.
 * @return 0 on success, 1 on error, -1 to exit the terminal.
 */
int8_t __head__(command_params params) {
  if (params.args->len < 1) {
    strcpy(params.term->stderr_buf, "Usage: head <file> [n]\n");
    terminal_display_stderr(params.term);
    return 1;
  }
  char *file = str_list_get(params.args, 0);
  int n = 10;
  if (params.args->len > 1) {
    n = atoi(str_list_get(params.args, 1));
  }
  path *file_path = path_init(file);
  path *full_path = path_concat(params.term->current_path, file_path);
  path_free(file_path);
  if (!path_exists(full_path)) {
    strcpy(params.term->stderr_buf, "File not found: ");
    strcat(params.term->stderr_buf, file);
    terminal_display_stderr(params.term);
    path_free(full_path);
    return 1;
  }
  str_list *lines = path_fread(full_path);
  if (lines == NULL) {
    strcpy(params.term->stderr_buf, "Failed to read file: ");
    strcat(params.term->stderr_buf, file);
    terminal_display_stderr(params.term);
    path_free(full_path);
    return 1;
  }
  char *content = str_list_concat(lines, ' ');
  strcpy(params.term->stdout_buf, content);
  terminal_display_stdout(params.term);
  free(content);
  str_list_free(lines);
  path_free(full_path);
  return 0;
}

/**
 * @brief prints out the last n lines of a file (10 by default)
 *
 * @param params The command parameters containing the terminal instance and
 * arguments.
 * @return 0 on success, 1 on error, -1 to exit the terminal.
 */
int8_t __tail__(command_params params) {
  if (params.args->len < 1) {
    strcpy(params.term->stderr_buf, "Usage: tail <file> [n]\n");
    terminal_display_stderr(params.term);
    return 1;
  }
  char *file = str_list_get(params.args, 0);
  int n = 10;
  if (params.args->len > 1) {
    n = atoi(str_list_get(params.args, 1));
  }
  path *file_path = path_init(file);
  path *full_path = path_concat(params.term->current_path, file_path);
  path_free(file_path);
  if (!path_exists(full_path)) {
    strcpy(params.term->stderr_buf, "File not found: ");
    strcat(params.term->stderr_buf, file);
    terminal_display_stderr(params.term);
    path_free(full_path);
    return 1;
  }
  str_list *lines = path_fread(full_path);
  if (lines == NULL) {
    strcpy(params.term->stderr_buf, "Failed to read file: ");
    strcat(params.term->stderr_buf, file);
    terminal_display_stderr(params.term);
    path_free(full_path);
    return 1;
  }
  char *content = str_list_concat(lines, NO_SEP);
  strcpy(params.term->stdout_buf, content);
  terminal_display_stdout(params.term);
  free(content);
  str_list_free(lines);
  path_free(full_path);
  return 0;
}

/**
 * @brief Searches for a pattern in a file and prints the matching lines.
 *
 * @param params The command parameters containing the terminal instance and
 * arguments.
 * @return 0 on success, 1 on error, -1 to exit the terminal.
 */
int8_t __grep__(command_params params) {
  if (params.args->len < 2) {
    strcpy(params.term->stderr_buf, "Usage: grep <pattern> <file>\n");
    terminal_display_stderr(params.term);
    return 1;
  }
  char *pattern = str_list_get(params.args, 0);
  char *file = str_list_get(params.args, 1);
  path *file_path = path_init(file);
  path *full_path = path_concat(params.term->current_path, file_path);
  path_free(file_path);
  if (!path_exists(full_path)) {
    strcpy(params.term->stderr_buf, "File not found: ");
    strcat(params.term->stderr_buf, file);
    terminal_display_stderr(params.term);
    path_free(full_path);
    return 1;
  }
  str_list *lines = path_fread(full_path);
  if (lines == NULL) {
    strcpy(params.term->stderr_buf, "Failed to read file: ");
    strcat(params.term->stderr_buf, file);
    terminal_display_stderr(params.term);
    path_free(full_path);
    return 1;
  }
  str_list *matched_lines = str_list_init();
  for (uinteger i = 0; i < lines->len; i++) {
    char *line = str_list_get(lines, i);
    if (strstr(line, pattern) != NULL) {
      str_list_append(matched_lines, line);
    }
  }
  char *content = str_list_concat(matched_lines, '\n');
  strcpy(params.term->stdout_buf, content);
  terminal_display_stdout(params.term);
  free(content);
  str_list_free(lines);
  str_list_free(matched_lines);
  path_free(full_path);
  return 0;
}
/**
 * @brief Displays the current username.
 *
 * @param params The command parameters containing the terminal instance.
 * @return 0 on success, 1 on error, -1 to exit the terminal.
 */
int8_t __whoami__(command_params params) {
  strcpy(params.term->stdout_buf, malloc_memories_inst->username);
  terminal_display_stdout(params.term);
  return 0;
}

/**
 * @brief Pings an ULMP host to check its availability.
 *
 * @param params The command parameters containing the terminal instance and
 * arguments.
 * @return 0 on success, 1 on error, -1 to exit the terminal.
 */
int8_t __ping__(command_params params) {
  if (params.args->len < 1) {
    strcpy(params.term->stderr_buf, "Usage: ping <host>\n");
    terminal_display_stderr(params.term);
    return 1;
  }
  char *host = str_list_get(params.args, 0);
  uint16_t host_numeric = 0;
  if (sscanf(host, "%hu", &host_numeric) != 1) {
    strcpy(params.term->stderr_buf, "Invalid host address\n");
    terminal_display_stderr(params.term);
    return 1;
  }
  printf("Pinging host %hu...\n", host_numeric);
  if (lora_ping(host_numeric) != 0) {
    char *to_display = malloc(28);
    sprintf(to_display, "No pong received from %hu\n", host_numeric);
    to_display[27] = '\0';
    strcpy(params.term->stderr_buf, to_display);
    terminal_display_stderr(params.term);
    free(to_display);
    return 1;
  }
  char *to_display = malloc(25);
  sprintf(to_display, "Pong received from %hu\n", host_numeric);
  to_display[24] = '\0';
  strcpy(params.term->stdout_buf, to_display);
  terminal_display_stdout(params.term);
  free(to_display);
  return 0;
}

/**
 * @brief Opens the text editor for a file.
 *
 * @param params The command parameters containing the terminal instance and
 * arguments.
 * @return 0 on success, 1 on error, -1 to exit the terminal.
 */
int8_t __unano__(command_params params) {
  if (params.term->on_serial)
    printf("unano will now open up in the device display");
  if (params.args->len < 1) {
    strcpy(params.term->stderr_buf, "Usage: unano <file>\n");
    terminal_display_stderr(params.term);
    return 1;
  }
  path *file_path = path_init(str_list_get(params.args, 0));
  path *full_path = path_concat(params.term->current_path, file_path);
  path_free(file_path);
  if (!path_exists(full_path)) {
    text_editor *editor = text_editor_launch("", true);
    char *buf = text_editor_get_buf(editor);
    text_editor_kill(editor);
    path_fwrite(full_path, buf, 'w');
    free(buf);
    path_free(full_path);
    return 0;
  }
  str_list *lines = path_fread(full_path);
  char *concat_lines = str_list_concat(lines, NO_SEP);
  text_editor *editor = text_editor_launch(concat_lines, false);
  char *buf = text_editor_get_buf(editor);
  text_editor_kill(editor);
  path_fwrite(full_path, buf, 'w');
  free(concat_lines);
  str_list_free(lines);
  path_free(full_path);
  free(buf);
  return 0;
}

/**
 * @brief Displays the system information.
 *
 * @param params The command parameters containing the terminal instance.
 * @return 0 on success, 1 on error, -1 to exit the terminal.
 */
int8_t __info__(command_params params) {
  display_system_info(params.term->on_serial);
  return 0;
}

/**
 * @brief Displays the command history.
 *
 * @param params The command parameters containing the terminal instance.
 * @return 0 on success, 1 on error, -1 to exit the terminal.
 */
int8_t __history__(command_params params) {
  char *history_content = str_list_concat(params.term->history, '\n');
  strcpy(params.term->stdout_buf, history_content);
  terminal_display_stdout(params.term);
  free(history_content);
  return 0;
}

int8_t __pwd_man__(command_params params) {
  password_manager_launch_cli();
  return 0;
}

int8_t __ser__(command_params params) {
  bool pwd_required;
  pwd_required = false;

  if (params.term->on_serial) {
    strcpy(params.term->stdout_buf, "You are already using serial CLI");
    terminal_display_stdout(params.term);
    return 0;
  }

  if (strcmp(str_list_get(params.args, 0), "pwd_required") == 0)
    pwd_required = true;

  if (!pwd_required) {
    strcpy(params.term->stdout_buf, "Exit this message to enter serial CLI");
    terminal_display_stdout(params.term);
  }

  params.term->on_serial = true;
  ssd1306_draw_bitmap(&(drivers->ssd1306), 0, 0, serial_cli, 128, 64, false);
  ssd1306_show(&(drivers->ssd1306));
  while (!stdio_usb_connected()) {
    sleep_ms(100);
  }

  char buf[64];
  int8_t ret;
  setvbuf(stdin, buf, _IOFBF, sizeof(buf));
  printf("%s %s - serial CLI\n", DEVICE_NAME, FIRMWARE_VERSION);
  if (pwd_required) {
    while (true) {
      printf("Enter you password to continue - type \"esc\" to exit");
      fgets(buf, sizeof(buf), stdin);
      char *buf_no_lfd = string_remove_linefeed(buf);
      char *hashed = get_hash(buf_no_lfd);
      if (strcmp(hashed, malloc_memories_inst->user_password_hashed) == 0) {
        free(buf_no_lfd);
        free(hashed);
        break;
      }
      if (strcmp(buf_no_lfd, "esc") == 0) {
        free(buf_no_lfd);
        free(hashed);
        return -1;
      }
      printf("Wrong password, please retry");
      free(buf_no_lfd);
      free(hashed);
    }
  }

  printf("Type 'help' to see available commands.\n\n");
  while (1) {
    printf("%s", params.term->prefix);
    fgets(buf, sizeof(buf), stdin);
    char *buf_no_lfd = string_remove_linefeed(buf);
    char buf_w_prefix[100];
    snprintf(buf_w_prefix, 100, "%s%s", params.term->prefix, buf_no_lfd);
    ret = dispatch_command(params.term, buf_w_prefix);
    free(buf_no_lfd);
    terminal_update_prefix(params.term);
    if (ret == -1) {
      printf("Exiting serial CLI...\n");
      params.term->on_serial = false;
      return 0;
    }
  }
}

int8_t __reboot__(command_params params) {
  strcpy(params.term->stdout_buf, "Rebooting...");
  terminal_display_stdout(params.term);
  watchdog_reboot(0, 0, 0);
  return 0;
}

int8_t __bootsel__(command_params params) {
  strcpy(params.term->stdout_buf, "Rebooting to BOOTSEL mode...");
  terminal_display_stdout(params.term);
  reset_usb_boot(0, 0);
  return 0;
}