#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "hardware_drivers/joystick.h"
#include "hardware_drivers/ssd1306.h"
#include "hardware_drivers/config.h"
#include "data_structures/string_list.h"
#include "utils.h"
#include "pico/stdlib.h"
#include "hw_manager.h"
#include "virtual_keyboard.h"

/*
If you wish to modify the layout of the keyboard, you can do so by changing the values
of the uppercase_layout and lowercase_layout arrays. Changes will be automatically reflected
but make sure to keep the ROWS and COLS values consistent with the new layout.
and keep in mind that there are some special characters:
- '_' is used to represent a space
- '^' is used to switch from uppercase to lowercase and vice versa
- '<' is used to represent the backspace key
- '>' is used to represent the enter key, that will return the input
- '~' is used to represent the caps lock key
- '`' is used to represent the end input key
- NSK (not shown key) is used to represent a key that will not be displayed
No matter where you put this buttons, they will always have the same functionality.
*/
char uppercase_layout[ROWS][COLS] = {
    {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '|', '`', '_'},
    {'~', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', NSK, NSK, '<'},
    {NSK, NSK, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', NSK, NSK, '?', NSK, NSK, '^'},
    {'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', NSK, '+', NSK, NSK, '>'}};

char lowercase_layout[ROWS][COLS] = {
    {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', NSK, '`', '_'},
    {'~', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', NSK, NSK, '<'},
    {NSK, NSK, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', NSK, NSK, '^'},
    {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', NSK, NSK, '>'}};

void populate_keys(virtual_keyboard *keyboard);
void highlight_key(virtual_keyboard *keyboard, key *key, bool highlight);
void draw_keyboard(virtual_keyboard *keyboard);
void draw_uppercase(virtual_keyboard *keyboard);
void draw_lowercase(virtual_keyboard *keyboard);
void toggle_uppercase(virtual_keyboard *keyboard);
bool is_out_of_bounds(uint8_t row, uint8_t col);
key *select_key(virtual_keyboard *keyboard, uint8_t direction, key *last_key);

virtual_keyboard *virtual_keyboard_init()
{
  virtual_keyboard *new_keyboard = (virtual_keyboard *)malloc(sizeof(virtual_keyboard));
  populate_keys(new_keyboard);
  new_keyboard->shift = false;
  new_keyboard->caps_lock = false;
  new_keyboard->last_char = '\0';
  new_keyboard->last_key = &new_keyboard->keys[0][0];
  return new_keyboard;
}

char virtual_keyboard_write(virtual_keyboard *keyboard)
{
  highlight_key(keyboard, keyboard->last_key, true);
  joystick_update(drivers->joystick);
  keyboard->target_key = select_key(keyboard, joystick_get_direction(drivers->joystick), keyboard->last_key);
  if (keyboard->target_key->label != NSK)
    keyboard->last_key = keyboard->target_key;
  if (!drivers->joystick->button_pressed)
  {
    sleep_ms(INPUT_TIMEOUT);
    return NO_WRITE_CHAR;
  }
  keyboard->last_char = keyboard->last_key->label;

  /*display the lowercase layout if shift was previously enabled*/
  if (keyboard->shift)
  {
    keyboard->shift = false;
    toggle_uppercase(keyboard);
  }

  /*handle enter*/
  if (keyboard->last_char == NEWLINE_CHAR)
  {
    sleep_ms(DEBOUNCE_TIMEOUT);
    return '\n';
  }

  /*handle shift*/
  if (keyboard->last_char == SHIFT_CHAR)
  {
    keyboard->shift = true;
    toggle_uppercase(keyboard);
    sleep_ms(DEBOUNCE_TIMEOUT);
    return NO_WRITE_CHAR;
  }

  /*handle caps lock*/
  if (keyboard->last_char == CAPS_LOCK_CHAR)
  {
    if (keyboard->caps_lock)
      keyboard->caps_lock = false;
    else
      keyboard->caps_lock = true;
    toggle_uppercase(keyboard);
    sleep_ms(DEBOUNCE_TIMEOUT);
    return NO_WRITE_CHAR;
  }

  /*handle space*/
  if (keyboard->last_char == SPACE_CHAR)
  {
    sleep_ms(DEBOUNCE_TIMEOUT);
    return ' ';
  }
  sleep_ms(DEBOUNCE_TIMEOUT);
  return keyboard->last_char;
}

void populate_keys(virtual_keyboard *keyboard)
{
  for (uint8_t i = 0; i < ROWS; i++)
    for (uint8_t j = 0; j < COLS; j++)
    {
      keyboard->keys[i][j].label = lowercase_layout[i][j];
      keyboard->keys[i][j].row = i;
      keyboard->keys[i][j].col = j;
    }
}

void draw_keyboard(virtual_keyboard *keyboard)
{
  for (uint8_t i = 0; i < ROWS; i++)
    for (uint8_t j = 0; j < COLS; j++)
      ssd1306_draw_letter_at(
          drivers->oled_screen,
          j * CHAR_WIDTH + LEFT_PADDING,
          i * CHAR_HEIGHT + TOP_PADDING,
          keyboard->keys[i][j].label,
          false);
  ssd1306_show(drivers->oled_screen);
}

void draw_uppercase(virtual_keyboard *keyboard)
{
  for (uint8_t i = 0; i < ROWS; i++)
    for (uint8_t j = 0; j < COLS; j++)
      keyboard->keys[i][j].label = uppercase_layout[i][j];
}

void draw_lowercase(virtual_keyboard *keyboard)
{
  for (uint8_t i = 0; i < ROWS; i++)
    for (uint8_t j = 0; j < COLS; j++)
      keyboard->keys[i][j].label = lowercase_layout[i][j];
}

void toggle_uppercase(virtual_keyboard *keyboard)
{
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

void highlight_key(
    virtual_keyboard *keyboard,
    key *key,
    bool highlight)
{
  ssd1306_draw_letter_at(
      drivers->oled_screen,
      key->col * CHAR_WIDTH + LEFT_PADDING,
      key->row * CHAR_HEIGHT + TOP_PADDING,
      key->label, highlight);
}

bool is_out_of_bounds(uint8_t row, uint8_t col)
{
  return row < 0 || row >= ROWS || col < 0 || col >= COLS;
}

key *select_key(virtual_keyboard *keyboard, uint8_t direction, key *last_key)
{
  int8_t row_adder = 0;
  int8_t col_adder = 0;
  if (direction == W)
    col_adder = 1;
  if (direction == S)
    row_adder = 1;
  if (direction == N)
    row_adder = -1;
  if (direction == E)
    col_adder = -1;
  if (direction == NE)
  {
    row_adder = -1;
    col_adder = -1;
  }
  if (direction == NW)
  {
    row_adder = -1;
    col_adder = 1;
  }
  if (direction == SE)
  {
    row_adder = 1;
    col_adder = -1;
  }
  if (direction == SW)
  {
    row_adder = 1;
    col_adder = 1;
  }
  if (is_out_of_bounds(last_key->row + row_adder, last_key->col + col_adder))
    return last_key;
  key *target_key;
  target_key = &keyboard->keys[last_key->row + row_adder][last_key->col + col_adder];
  if (target_key->label != NSK)
  {
    highlight_key(keyboard, last_key, false);
    highlight_key(keyboard, target_key, true);
    return target_key;
  }
  for (uint8_t i = 1; i < 4; i++)
  {
    target_key = &keyboard->keys[last_key->row + row_adder * i][last_key->col + col_adder * i];
    if (target_key->label != NSK && !is_out_of_bounds(last_key->row + row_adder * i, last_key->col + col_adder * i))
    {
      highlight_key(keyboard, last_key, false);
      highlight_key(keyboard, target_key, true);
      return target_key;
    }
  }
  return last_key;
}
