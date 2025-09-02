// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include "apps/virtual_keyboard/virtual_keyboard.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "core/components/hw_manager.h"
#include "core/data_structures/string_list.h"
#include "core/hardware_drivers/haptics.h"
#include "core/hardware_drivers/joystick.h"
#include "core/hardware_drivers/ssd1306.h"

/**
If you wish to modify the layout of the keyboard, you can do so by changing the
values of the uppercase_layout and lowercase_layout arrays. Changes will be
automatically reflected but make sure to keep the ROWS and COLS values
consistent with the new layout. and keep in mind that there are some special
characters:
- SPC: space
- BCK: backspace
- SHF: shift
- LFD: line feed (newline)
- UPP: go to uppercase
- LOW: go to lowercase
- NAV: navigate text
- END: save the buffer and exit
- NSK: not shown key
*/

// clang-format off
char uppercase_layout[ROWS][COLS] = {
    {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '|', SPC, LFD},
    {NSK, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', NSK, NSK, BCK},
    {NSK, NSK, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', NSK, NSK, '?', NSK, LOW, SHF},
    {'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', NSK, NAV, END}};

char lowercase_layout[ROWS][COLS] = {
    {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', NSK, SPC, LFD},
    {NSK, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', NSK, NSK, BCK},
    {NSK, NSK, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', NSK, UPP, SHF},
    {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', NSK, NAV, END}};

const uint8_t NEWLINE_PIXELS[] = {0x7c, 0x04, 0x24, 0x64, 0xcc, 0x60, 0x20, 0x00};
const uint8_t BACKSPACE_PIXELS[] = {0x00, 0x20, 0x60, 0xcc, 0x60, 0x20, 0x00, 0x00};
const uint8_t UPPERCASE_PIXELS[] = {0x78, 0x48, 0x78, 0x48, 0x48, 0x00, 0x84, 0xfc};
const uint8_t LOWERCASE_PIXELS[] = {0x78, 0x08, 0x78, 0x48, 0x78, 0x00, 0xfc, 0x84};
const uint8_t SHIFT_PIXELS[] = {0x20, 0x70, 0xd8, 0x00, 0x20, 0x20, 0x20, 0x20};
const uint8_t SPACE_PIXELS[] = {0x00, 0x00, 0x00, 0xfc, 0x84, 0x00, 0x00, 0x00};
const uint8_t NAVIGATE_PIXELS[] = {0x00, 0xcc, 0xb4, 0x78, 0x78, 0xb4, 0xcc, 0x00};
const uint8_t END_INPUT_PIXELS[] = {0xc0, 0xd0, 0xd8, 0xcc, 0xd8, 0xd0, 0xc0, 0xfc};

// clang-format on
void populate_keys(virtual_keyboard *keyboard);
void highlight_key(virtual_keyboard *keyboard, key *key, bool highlight);
void draw_keyboard(virtual_keyboard *keyboard);
void draw_uppercase(virtual_keyboard *keyboard);
void draw_lowercase(virtual_keyboard *keyboard);
void toggle_uppercase(virtual_keyboard *keyboard);
bool is_out_of_bounds(uint8_t row, uint8_t col);
key *select_key(virtual_keyboard *keyboard, uint8_t direction, key *last_key);

/**
 * @brief Initialize a new virtual keyboard instance
 *
 * @returns A new virtual keyboard instance
 */
virtual_keyboard *virtual_keyboard_init() {
  virtual_keyboard *new_keyboard = (virtual_keyboard *)malloc(
      sizeof(virtual_keyboard));
  populate_keys(new_keyboard);
  new_keyboard->shift = false;
  new_keyboard->caps_lock = false;
  new_keyboard->last_char = '\0';
  new_keyboard->last_key = &new_keyboard->keys[0][0];
  return new_keyboard;
}

/**
 * @brief Read the virtual keyboard, note this function MUST be called inside a
 * loop
 * @returns The character selected by the user
 */
char virtual_keyboard_read(virtual_keyboard *keyboard) {
  highlight_key(keyboard, keyboard->last_key, true);
  joystick_update(drivers->joystick);
  keyboard->target_key = select_key(keyboard,
      joystick_get_direction(drivers->joystick),
      keyboard->last_key);
  if (keyboard->target_key->label != NSK)
    keyboard->last_key = keyboard->target_key;
  if (!drivers->joystick->button_pressed) {
    sleep_ms(INPUT_TIMEOUT);
    return NOW;
  }
  haptic_short_pulse();
  keyboard->last_char = keyboard->last_key->label;

  /*display the lowercase layout if shift was previously enabled*/
  if (keyboard->shift) {
    keyboard->shift = false;
    toggle_uppercase(keyboard);
  }

  /*handle enter*/
  if (keyboard->last_char == LFD) {
    sleep_ms(DEBOUNCE_TIMEOUT);
    return '\n';
  }

  /*handle shift*/
  if (keyboard->last_char == SHF) {
    keyboard->shift = true;
    toggle_uppercase(keyboard);
    sleep_ms(DEBOUNCE_TIMEOUT);
    return NOW;
  }

  /*handle caps lock*/
  if (keyboard->last_char == UPP || keyboard->last_char == LOW) {
    if (keyboard->caps_lock)
      keyboard->caps_lock = false;
    else
      keyboard->caps_lock = true;
    toggle_uppercase(keyboard);
    sleep_ms(DEBOUNCE_TIMEOUT);
    return NOW;
  }

  /*handle space*/
  if (keyboard->last_char == SPC) {
    sleep_ms(DEBOUNCE_TIMEOUT);
    return ' ';
  }

  /*handle nav button*/
  if (keyboard->last_char == NAV) {
    sleep_ms(DEBOUNCE_TIMEOUT);
    keyboard->target_key = &keyboard->keys[0][0];
    highlight_key(keyboard, &keyboard->keys[3][13], true);
    return keyboard->last_char;
  }
  sleep_ms(DEBOUNCE_TIMEOUT);
  return keyboard->last_char;
}

void populate_keys(virtual_keyboard *keyboard) {
  for (uint8_t i = 0; i < ROWS; i++)
    for (uint8_t j = 0; j < COLS; j++) {
      keyboard->keys[i][j].label = lowercase_layout[i][j];
      keyboard->keys[i][j].row = i;
      keyboard->keys[i][j].col = j;
    }
}

void draw_custom_char(uint8_t row, uint8_t col, char label, bool reversed) {
  if (label == SPC) {
    ssd1306_draw_bitmap(drivers->oled_screen,
        col * CHAR_WIDTH + LEFT_PADDING,
        row * CHAR_HEIGHT + TOP_PADDING,
        SPACE_PIXELS,
        CHAR_WIDTH,
        CHAR_HEIGHT,
        reversed);
    return;
  }
  if (label == BCK) {
    ssd1306_draw_bitmap(drivers->oled_screen,
        col * CHAR_WIDTH + LEFT_PADDING,
        row * CHAR_HEIGHT + TOP_PADDING,
        BACKSPACE_PIXELS,
        CHAR_WIDTH,
        CHAR_HEIGHT,
        reversed);
    return;
  }
  if (label == SHF) {
    ssd1306_draw_bitmap(drivers->oled_screen,
        col * CHAR_WIDTH + LEFT_PADDING,
        row * CHAR_HEIGHT + TOP_PADDING,
        SHIFT_PIXELS,
        CHAR_WIDTH,
        CHAR_HEIGHT,
        reversed);
    return;
  }
  if (label == LFD) {
    ssd1306_draw_bitmap(drivers->oled_screen,
        col * CHAR_WIDTH + LEFT_PADDING,
        row * CHAR_HEIGHT + TOP_PADDING,
        NEWLINE_PIXELS,
        CHAR_WIDTH,
        CHAR_HEIGHT,
        reversed);
    return;
  }
  if (label == UPP) {
    ssd1306_draw_bitmap(drivers->oled_screen,
        col * CHAR_WIDTH + LEFT_PADDING,
        row * CHAR_HEIGHT + TOP_PADDING,
        UPPERCASE_PIXELS,
        CHAR_WIDTH,
        CHAR_HEIGHT,
        reversed);
    return;
  }
  if (label == LOW) {
    ssd1306_draw_bitmap(drivers->oled_screen,
        col * CHAR_WIDTH + LEFT_PADDING,
        row * CHAR_HEIGHT + TOP_PADDING,
        LOWERCASE_PIXELS,
        CHAR_WIDTH,
        CHAR_HEIGHT,
        reversed);
    return;
  }
  if (label == NAV) {
    ssd1306_draw_bitmap(drivers->oled_screen,
        col * CHAR_WIDTH + LEFT_PADDING,
        row * CHAR_HEIGHT + TOP_PADDING,
        NAVIGATE_PIXELS,
        CHAR_WIDTH,
        CHAR_HEIGHT,
        reversed);
    return;
  }
  if (label == END) {
    ssd1306_draw_bitmap(drivers->oled_screen,
        col * CHAR_WIDTH + LEFT_PADDING,
        row * CHAR_HEIGHT + TOP_PADDING,
        END_INPUT_PIXELS,
        CHAR_WIDTH,
        CHAR_HEIGHT,
        reversed);
    return;
  }
}

/**
 * @brief Draws the virtual keyboard on the oled screen provided by the drivers
 * manager
 */
void draw_keyboard(virtual_keyboard *keyboard) {
  for (uint8_t i = 0; i < ROWS; i++)
    for (uint8_t j = 0; j < COLS; j++) {
      if (keyboard->keys[i][j].label < ' ' ||
          keyboard->keys[i][j].label > 0x7F) {
        draw_custom_char(i, j, keyboard->keys[i][j].label, false);
        continue;
      }
      ssd1306_draw_letter_at(drivers->oled_screen,
          j * CHAR_WIDTH + LEFT_PADDING,
          i * CHAR_HEIGHT + TOP_PADDING,
          keyboard->keys[i][j].label,
          false);
    }
  ssd1306_show(drivers->oled_screen);
}

void draw_uppercase(virtual_keyboard *keyboard) {
  for (uint8_t i = 0; i < ROWS; i++)
    for (uint8_t j = 0; j < COLS; j++)
      keyboard->keys[i][j].label = uppercase_layout[i][j];
}

void draw_lowercase(virtual_keyboard *keyboard) {
  for (uint8_t i = 0; i < ROWS; i++)
    for (uint8_t j = 0; j < COLS; j++)
      keyboard->keys[i][j].label = lowercase_layout[i][j];
}

void toggle_uppercase(virtual_keyboard *keyboard) {
  if (keyboard->shift && !keyboard->caps_lock)
    draw_uppercase(keyboard);
  if (keyboard->shift && keyboard->caps_lock)
    draw_lowercase(keyboard);
  if (!keyboard->shift && !keyboard->caps_lock)
    draw_lowercase(keyboard);
  if (!keyboard->shift && keyboard->caps_lock)
    draw_uppercase(keyboard);
  draw_keyboard(keyboard);
}

void highlight_key(virtual_keyboard *keyboard, key *key, bool highlight) {
  if (key->label < ' ' || key->label > 0x7F) {
    draw_custom_char(key->row, key->col, key->label, highlight);
    return;
  }
  ssd1306_draw_letter_at(drivers->oled_screen,
      key->col * CHAR_WIDTH + LEFT_PADDING,
      key->row * CHAR_HEIGHT + TOP_PADDING,
      key->label,
      highlight);
}

bool is_out_of_bounds(uint8_t row, uint8_t col) {
  return row < 0 || row >= ROWS || col < 0 || col >= COLS;
}

key *select_key(virtual_keyboard *keyboard, uint8_t direction, key *last_key) {
  int8_t row_adder = 0;
  int8_t col_adder = 0;
  if (direction == E)
    col_adder = 1;
  if (direction == S)
    row_adder = 1;
  if (direction == N)
    row_adder = -1;
  if (direction == W)
    col_adder = -1;
  if (direction == NW) {
    row_adder = -1;
    col_adder = -1;
  }
  if (direction == NE) {
    row_adder = -1;
    col_adder = 1;
  }
  if (direction == SW) {
    row_adder = 1;
    col_adder = -1;
  }
  if (direction == SE) {
    row_adder = 1;
    col_adder = 1;
  }
  if (is_out_of_bounds(last_key->row + row_adder, last_key->col + col_adder))
    return last_key;
  key *target_key;
  target_key = &keyboard->keys[last_key->row + row_adder]
                              [last_key->col + col_adder];
  if (target_key->label != NSK) {
    highlight_key(keyboard, last_key, false);
    highlight_key(keyboard, target_key, true);
    return target_key;
  }
  for (uint8_t i = 1; i < 4; i++) {
    target_key = &keyboard->keys[last_key->row + row_adder * i]
                                [last_key->col + col_adder * i];
    if (target_key->label != NSK &&
        !is_out_of_bounds(last_key->row + row_adder * i,
            last_key->col + col_adder * i)) {
      highlight_key(keyboard, last_key, false);
      highlight_key(keyboard, target_key, true);
      return target_key;
    }
  }
  return last_key;
}
