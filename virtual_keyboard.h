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

#define SPC 0x1D /*space*/
#define BCK 0x08 /*backspace*/
#define SHF 0x1C /*shift*/
#define LFD 0x0A /*line feed (newline)*/
#define UPP 0x1B /*uppercase*/
#define LOW 0x1A /*lowercase*/
#define NAV 0x1E /*navigate*/
#define END 0x03 /*end of input*/
#define NSK 0x20 /*not shown key*/
#define NOW 0x00 /*no write*/

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
char virtual_keyboard_read(virtual_keyboard *keyboard);

#endif