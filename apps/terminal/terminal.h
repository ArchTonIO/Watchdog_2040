// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdbool.h>
#include <stddef.h>

#include "apps/terminal/terminal_commands.h"
#include "core/data_structures/string_list.h"
#include "core/utils/path.h"
#define MAX_COMMANDS 100

struct terminal {
  str_list *history;
  char current_command[20];
  command commands[MAX_COMMANDS];
  size_t commands_count;
  path *current_path;
  char *prefix;
  char stdout_buf[256];
  char stderr_buf[256];
  bool on_serial;
};

terminal *terminal_init();
void terminal_launch();
void terminal_kill(terminal *term);
void terminal_clear_buffers(terminal *term);
void terminal_display_stdout(terminal *term);
void terminal_display_stderr(terminal *term);
void terminal_add_command(terminal *term, command cmd);
int8_t dispatch_command(terminal *term, const char *command);
void terminal_update_prefix(terminal *term);

#endif