#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdbool.h>

#include "components/hw_manager.h"
#include "data_structures/string_list.h"
#include "tools/virtual_keyboard.h"

typedef struct {
  char name[20];
  void (*callback)(void);
} command;

typedef struct {
  str_list *history;
  char current_command[20];
  command commands[100];
  virtual_keyboard *keyboard;
} terminal;

terminal *terminal_init();
void terminal_launch(terminal *term);
void terminal_kill(terminal *term);

#endif