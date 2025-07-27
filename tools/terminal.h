#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdbool.h>
#include <stddef.h>

#include "components/hw_manager.h"
#include "data_structures/string_list.h"
#include "terminal_commands.h"
#include "tools/virtual_keyboard.h"
#include "utils/path.h"
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
};

terminal *terminal_init();
void terminal_launch();
void terminal_kill(terminal *term);
void terminal_clear_buffers(terminal *term);
void terminal_display_stdout(terminal *term);
void terminal_display_stderr(terminal *term);
void terminal_add_command(terminal *term, command cmd);

#endif