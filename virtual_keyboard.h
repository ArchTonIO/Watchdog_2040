#ifndef VIRTUAL_KEYBOARD_H
#define VIRTUAL_KEYBOARD_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "hardware_drivers/joystick.h"
#include "hardware_drivers/config.h"
#include "data_structures/string_list.h"
#include "utils.h"
#include "pico/stdlib.h"
#include "hw_manager.h"

#define ROWS 4
#define COLS 15
#define LEFT_PADDING (uint16_t)(SSD1306_WIDTH - (COLS * CHAR_WIDTH)) / 2
#define TOP_PADDING (uint16_t)(SSD1306_HEIGHT - (ROWS * CHAR_HEIGHT))
#define DEBOUNCE_TIMEOUT 100
#define INPUT_TIMEOUT 50

#define CURSOR_CHAR '_'
#define BACKSPACE_CHAR '<'
#define NEWLINE_CHAR '>'
#define CAPS_LOCK_CHAR '~'
#define SHIFT_CHAR '^'
#define SPACE_CHAR '_'
#define NSK ' '
#define NO_WRITE_CHAR '\0'
#define END_INPUT_CHAR '`'

typedef struct
{
  char label;
  uint8_t row;
  uint8_t col;
} key;

typedef struct
{
  key keys[ROWS][COLS];
  bool shift;
  bool caps_lock;
  char last_char;
  key *last_key;
  key *target_key;
} virtual_keyboard;

virtual_keyboard *virtual_keyboard_init();
void draw_keyboard(virtual_keyboard *keyboard);
char virtual_keyboard_write(virtual_keyboard *keyboard);

#endif