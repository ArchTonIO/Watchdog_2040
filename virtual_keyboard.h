#ifndef VIRTUAL_KEYBOARD_H
#define VIRTUAL_KEYBOARD_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "hardware_drivers/joystick.h"
#include "hardware_drivers/ssd1306.h"
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
#define MAX_CHARS_PER_ROW 20
#define MAX_ROWS 4
#define MAX_INPUT_LENGTH (MAX_CHARS_PER_ROW * MAX_ROWS)

typedef struct
{
  char label;
  uint8_t row;
  uint8_t col;
} key;

typedef struct
{
  key keys[ROWS][COLS];
  bool show_cursor;
  bool uppercase;
} virtual_keyboard;

virtual_keyboard *virtual_keyboard_init();
void draw_keyboard(virtual_keyboard *keyboard);
char *virtual_keyboard_write(virtual_keyboard *keyboard);

#endif