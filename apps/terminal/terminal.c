// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include "apps/terminal/terminal.h"

#include <device.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apps/terminal/terminal_commands.h"
#include "apps/text_editor/text_editor.h"
#include "core/components/malloc_mascot.h"
#include "core/components/sys_paths_manager.h"
#include "core/data_structures/string_list.h"
#include "core/utils/path.h"
#include "core/utils/utils.h"

int8_t dispatch_command(terminal *term, const char *command);

void terminal_bind_std_commands(terminal *term) {
  terminal_add_command(term,
      create_command("help", "Show available commands.", __help__));
  terminal_add_command(term,
      create_command("echo", "Print text to the terminal.", __echo__));
  terminal_add_command(term,
      create_command("esc", "Exit the terminal.", __esc__));
  terminal_add_command(term,
      create_command("cd", "Change current directory.", __cd__));
  terminal_add_command(term,
      create_command("ls", "List directory contents.", __ls__));
  terminal_add_command(term,
      create_command("pwd", "Print working directory.", __pwd__));
  terminal_add_command(term,
      create_command("mkdir", "Create a new directory.", __mkdir__));
  terminal_add_command(term,
      create_command("touch", "Create an empty file.", __touch__));
  terminal_add_command(term,
      create_command("rm", "Remove files or directories.", __rm__));
  terminal_add_command(term,
      create_command("mv", "Move or rename files.", __mv__));
  terminal_add_command(term,
      create_command("cp", "Copy files or directories.", __cp__));
  terminal_add_command(term,
      create_command("cat", "Show file contents.", __cat__));
  terminal_add_command(term,
      create_command("head", "Show first lines of a file.", __head__));
  terminal_add_command(term,
      create_command("tail", "Show last lines of a file.", __tail__));
  terminal_add_command(term,
      create_command("grep", "Search text in files.", __grep__));
  terminal_add_command(term,
      create_command("whoami", "Show current user.", __whoami__));
  terminal_add_command(term,
      create_command("ping", "Ping another ULMP device.", __ping__));
  terminal_add_command(term,
      create_command("unano", "Open unano text editor.", __unano__));
  terminal_add_command(term,
      create_command("info", "Show hardware sensor data.", __info__));
  terminal_add_command(term,
      create_command("history", "Show command history.", __history__));
  terminal_add_command(term,
      create_command("ser", "Launch terminal over serial port.", __ser__));
  terminal_add_command(term,
      create_command("reboot", "Reboot the device.", __reboot__));
  terminal_add_command(term,
      create_command("bootsel", "Reboot to BOOTSEL mode.", __bootsel__));
}

void terminal_add_command(terminal *term, command cmd) {
  if (term->commands_count < MAX_COMMANDS) {
    term->commands[term->commands_count] = cmd;
    term->commands_count++;
  } else {
    printf(
        "[TERMINAL](ERR)Command limit reached. Cannot add more commands.\n");
  }
}

terminal *terminal_init() {
  terminal *term = (terminal *)malloc(sizeof(terminal));
  term->history = str_list_init();
  term->commands_count = 0;
  term->current_path = path_init(sys_paths->dirs->user_path->abs_path);
  term->prefix = (char *)malloc(1);
  term->on_serial = false;
  strcpy(term->current_command, "");
  return term;
}

void terminal_kill(terminal *term) {
  if (term) {
    str_list_free(term->history);
    path_free(term->current_path);
    free(term->prefix);
    free(term);
  }
}

void terminal_clear_buffers(terminal *term) {
  term->stdout_buf[0] = '\0';
  term->stderr_buf[0] = '\0';
}

void terminal_display_stdout(terminal *term) {
  if (term->on_serial) {
    printf("%s\n", term->stdout_buf);
    terminal_clear_buffers(term);
    return;
  }
  text_editor *editor = text_editor_launch(term->stdout_buf, false);
  char *buf = text_editor_get_buf(editor);
  text_editor_kill(editor);
  free(buf);
  terminal_clear_buffers(term);
}

void terminal_display_stderr(terminal *term) {
  if (term->on_serial) {
    printf("%s\n", term->stderr_buf);
    terminal_clear_buffers(term);
    return;
  }
  text_editor *editor = text_editor_launch(term->stderr_buf, false);
  char *buf = text_editor_get_buf(editor);
  text_editor_kill(editor);
  free(buf);
  terminal_clear_buffers(term);
}

void terminal_update_prefix(terminal *term) {

  size_t prefix_len = 0;
  const char *relative_path = NULL;
  if (strcmp(term->current_path->abs_path,
          sys_paths->dirs->user_path->abs_path) == 0) {
    prefix_len = strlen(malloc_memories_inst->username) + strlen(HOSTNAME) +
                 5 + 1;
    term->prefix = (char *)realloc(term->prefix, prefix_len);
    snprintf(term->prefix,
        prefix_len,
        "%s@%s:~$ ",
        malloc_memories_inst->username,
        HOSTNAME);
  } else if (strncmp(term->current_path->abs_path,
                 sys_paths->dirs->user_path->abs_path,
                 strlen(sys_paths->dirs->user_path->abs_path)) == 0) {
    relative_path = term->current_path->abs_path +
                    strlen(sys_paths->dirs->user_path->abs_path);
    if (relative_path[0] == '/')
      relative_path++;
    prefix_len = strlen(malloc_memories_inst->username) + strlen(HOSTNAME) +
                 strlen(relative_path) + 6 + 1;
    term->prefix = (char *)realloc(term->prefix, prefix_len);
    snprintf(term->prefix,
        prefix_len,
        "%s@%s:~/%s$ ",
        malloc_memories_inst->username,
        HOSTNAME,
        relative_path);
  } else {
    prefix_len = strlen(malloc_memories_inst->username) + strlen(HOSTNAME) +
                 strlen(term->current_path->abs_path) + 5 + 1;
    term->prefix = (char *)realloc(term->prefix, prefix_len);
    snprintf(term->prefix,
        prefix_len,
        "%s@%s:%s$ ",
        malloc_memories_inst->username,
        HOSTNAME,
        term->current_path->abs_path);
  }
}

void terminal_launch() {
  if (!request_password())
    return;
  terminal *term = terminal_init();
  terminal_bind_std_commands(term);
  while (true) {
    terminal_update_prefix(term);
    text_editor *editor = text_editor_launch(term->prefix, false);
    char *command = text_editor_get_buf(editor);
    int8_t ret = dispatch_command(term, command);
    free(command);
    text_editor_kill(editor);
    if (ret == -1) {
      terminal_kill(term);
      break;
    }
  }
}

int8_t dispatch_command(terminal *term, const char *command) {
  str_list *slices = string_split(command, ' ');
  char *prefix = str_list_get(slices, 0);
  char *cmd = str_list_get(slices, 1);
  str_list *args = str_list_init();
  if (cmd == NULL || strlen(cmd) == 0) {
    str_list_free(args);
    str_list_free(slices);
    return 1;
  }
  for (size_t i = 2; i < slices->len; i++) {
    str_list_append(args, str_list_get(slices, i));
  }
  str_list_append(term->history, command);
  for (size_t i = 0; i < term->commands_count; i++) {
    if (strcmp(term->commands[i].name, cmd) == 0) {
      command_params params = {term, args};
      int8_t ret = term->commands[i].callback(params);
      str_list_free(args);
      str_list_free(slices);
      return ret;
    }
  }
  printf("[TERMINAL](ERR)Command not found: %s\n", cmd);
  str_list_free(args);
  str_list_free(slices);
  return 1;
}