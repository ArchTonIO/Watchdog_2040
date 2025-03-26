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
No matter where you put this buttons, they will always have the same functionality.
*/
char uppercase_layout[ROWS][COLS] = {
    {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '|', ' ', '_'},
    {' ', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', ' ', ' ', '<'},
    {' ', ' ', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ' ', ' ', '?', ' ', ' ', '^'},
    {'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', ' ', '+', ' ', ' ', '>'}};

char lowercase_layout[ROWS][COLS] = {
    {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', ' ', ' ', '_'},
    {' ', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', ' ', ' ', '<'},
    {' ', ' ', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', ' ', ' ', '^'},
    {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', ' ', ' ', '>'}};

void populate_keys(virtual_keyboard *keyboard);
void highlight_key(virtual_keyboard *keyboard, key *key, bool highlight);

virtual_keyboard *virtual_keyboard_init()
{
  virtual_keyboard *new_keyboard = (virtual_keyboard *)malloc(sizeof(virtual_keyboard));
  populate_keys(new_keyboard);
  new_keyboard->show_cursor = true;
  new_keyboard->uppercase = true;
  return new_keyboard;
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

void toggle_uppercase(virtual_keyboard *keyboard)
{
  if (keyboard->uppercase)
    for (uint8_t i = 0; i < ROWS; i++)
      for (uint8_t j = 0; j < COLS; j++)
        keyboard->keys[i][j].label = uppercase_layout[i][j];
  else
    for (uint8_t i = 0; i < ROWS; i++)
      for (uint8_t j = 0; j < COLS; j++)
        keyboard->keys[i][j].label = lowercase_layout[i][j];
  keyboard->uppercase = !keyboard->uppercase;
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

void do_backspace(uint8_t last_char_col, uint8_t last_char_row)
{
  ssd1306_draw_letter_at(
      drivers->oled_screen,
      last_char_col * (CHAR_WIDTH - 2),
      last_char_row * CHAR_HEIGHT,
      ' ',
      false);
}

void write_char(
    uint8_t last_char_col,
    uint8_t last_char_row,
    char last_char)
{
  ssd1306_draw_letter_at(
      drivers->oled_screen,
      last_char_col * (CHAR_WIDTH - 2),
      last_char_row * CHAR_HEIGHT,
      last_char,
      false);
}

void toggle_cursor(
    virtual_keyboard *keyboard,
    uint8_t last_char_col,
    uint8_t last_char_row)
{
  if (keyboard->show_cursor)
    ssd1306_draw_letter_at(
        drivers->oled_screen,
        last_char_col * (CHAR_WIDTH - 2),
        last_char_row * CHAR_HEIGHT,
        '_',
        false);
  else
    ssd1306_draw_letter_at(
        drivers->oled_screen,
        last_char_col * (CHAR_WIDTH - 2),
        last_char_row * CHAR_HEIGHT,
        ' ',
        false);
  keyboard->show_cursor = !keyboard->show_cursor;
}

key *select_key(virtual_keyboard *keyboard, char *direction, key *last_key)
{
  int8_t row_adder = 0;
  int8_t col_adder = 0;
  if (strcmp(direction, "W") == 0)
    col_adder = 1;
  if (strcmp(direction, "S") == 0)
    row_adder = 1;
  if (strcmp(direction, "N") == 0)
    row_adder = -1;
  if (strcmp(direction, "E") == 0)
    col_adder = -1;
  if (strcmp(direction, "NE") == 0)
  {
    row_adder = -1;
    col_adder = -1;
  }
  if (strcmp(direction, "NW") == 0)
  {
    row_adder = -1;
    col_adder = 1;
  }
  if (strcmp(direction, "SE") == 0)
  {
    row_adder = 1;
    col_adder = -1;
  }
  if (strcmp(direction, "SW") == 0)
  {
    row_adder = 1;
    col_adder = 1;
  }
  if (is_out_of_bounds(last_key->row + row_adder, last_key->col + col_adder))
    return last_key;
  key *target_key;
  target_key = &keyboard->keys[last_key->row + row_adder][last_key->col + col_adder];
  if (target_key->label != ' ')
  {
    highlight_key(keyboard, last_key, false);
    highlight_key(keyboard, target_key, true);
    return target_key;
  }
  for (uint8_t i = 1; i < 4; i++)
  {
    target_key = &keyboard->keys[last_key->row + row_adder * i][last_key->col + col_adder * i];
    if (target_key->label != ' ' && !is_out_of_bounds(last_key->row + row_adder * i, last_key->col + col_adder * i))
    {
      highlight_key(keyboard, last_key, false);
      highlight_key(keyboard, target_key, true);
      return target_key;
    }
  }
  return last_key;
}

char *virtual_keyboard_write(virtual_keyboard *keyboard)
{
  char buf[MAX_INPUT_LENGTH + 1];
  char last_char = '\0';
  key *last_key;
  key *target_key;
  last_key = &keyboard->keys[0][0];
  uint8_t last_char_col = 0;
  uint8_t last_char_row = 0;
  uint8_t buf_counter = 0;
  while (true)
  {
    toggle_cursor(keyboard, last_char_col, last_char_row);
    highlight_key(keyboard, last_key, true);
    joystick_update(drivers->joystick);
    target_key = select_key(keyboard, joystick_get_direction(drivers->joystick), last_key);
    if (target_key->label != ' ')
      last_key = target_key;
    if (drivers->joystick->button_pressed)
    {
      last_char = last_key->label;

      /*handle enter*/
      if (last_char == '>')
        break;

      /*handle backspace*/
      if (last_char == '<')
      {
        if (buf_counter == 0)
          continue;
        do_backspace(last_char_col, last_char_row);
        last_char_col--;
        buf_counter--;
        sleep_ms(DEBOUNCE_TIMEOUT * 2);
        continue;
      }

      /*handle case toggling*/
      if (last_char == '^')
      {
        toggle_uppercase(keyboard);
        sleep_ms(DEBOUNCE_TIMEOUT);
        continue;
      }

      /*handle space*/
      if (last_char == '_')
        last_char = ' ';

      /*handle no space left*/
      if (last_char_row == MAX_ROWS - 1 && last_char_col == MAX_CHARS_PER_ROW)
        continue;

      /*handle line full*/
      if (last_char_col == MAX_CHARS_PER_ROW)
      {
        last_char_col = 0;
        last_char_row++;
      }

      /*handle write character*/
      buf[buf_counter] = last_char;
      write_char(last_char_col, last_char_row, last_char);
      last_char_col++;
      buf_counter++;
      sleep_ms(DEBOUNCE_TIMEOUT);
    }
    ssd1306_show(drivers->oled_screen);
    sleep_ms(INPUT_TIMEOUT);
  }
  buf[buf_counter] = '\0';
  char *result = (char *)malloc(buf_counter);
  strcpy(result, buf);
  return result;
}