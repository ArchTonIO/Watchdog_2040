#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware_drivers/joystick.h"
#include "hardware_drivers/ssd1306.h"
#include "hw_manager.h"
#include "virtual_keyboard.h"
#include "data_structures/string_list.h"
#include "utils.h"

typedef struct
{
  char name[20];
  void (*callback)(void);
} command;

typedef struct
{
  str_list *history;
  char current_command[20];
  command commands[100];
  virtual_keyboard *keyboard;
} terminal;

terminal *terminal_init();
void terminal_launch(terminal *term);
void terminal_kill(terminal *term);

#endif