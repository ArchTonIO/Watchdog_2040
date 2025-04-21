#ifndef OPTIONS_GEN_H
#define OPTIONS_GEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hw_manager.h"
#include "data_structures/string_list.h"
#include "hardware_drivers/ssd1306.h"
#include "hardware_drivers/joystick.h"

#define MAX_OPTIONS 20

typedef struct
{
  char *name;
  bool selected;
  void (*callback)(void);
} option;

typedef struct
{
  option options[MAX_OPTIONS];
  uint8_t num_options;
  uint8_t selected_option;
  char *title;
} options_page;

options_page *options_page_init(char *title, str_list *options);
void attach_callback_to_option(
    options_page *page,
    uint8_t option_index,
    void (*callback)(void));
char *options_page_launch(options_page *page);

#endif