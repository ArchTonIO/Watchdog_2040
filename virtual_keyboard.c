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

char uppercase_layout[ROWS][COLS] = {
    {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '[', ']', '_'},
    {'!', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', '"', '@', '<'},
    {'(', ')', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '#', '$', '/', '?', '&', '^'},
    {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '.', '=', '+', '>'}};

char lowercase_layout[ROWS][COLS] = {
    {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', '[', ']', '_'},
    {'!', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', ':', '"', '@', '<'},
    {'(', ')', 'z', 'x', 'c', 'v', 'b', 'n', 'm', '#', '$', '/', '?', '&', '^'},
    {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '.', '=', '+', '>'}};

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
      printf("ITERATING\n");
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
        false
      );
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
  bool highlight
){
  ssd1306_draw_letter_at(
    drivers->oled_screen,
    key->col * CHAR_WIDTH + LEFT_PADDING,
    key->row * CHAR_HEIGHT + TOP_PADDING,
    key->label, highlight
  );
}

bool is_out_of_bounds(uint8_t row, uint8_t col)
{
  return row < 0 || row >= ROWS || col < 0 || col >= COLS;
}

void do_backspace(uint8_t last_char_col, uint8_t last_char_row){
  ssd1306_draw_letter_at(
    drivers->oled_screen,
    last_char_col * (CHAR_WIDTH - 2) - (CHAR_WIDTH - 2),
    last_char_row * CHAR_HEIGHT,
    ' ',
    false
  );
}

void write_char(
  uint8_t last_char_col,
  uint8_t last_char_row, 
  char last_char
){
  ssd1306_draw_letter_at(
    drivers->oled_screen,
    last_char_col * (CHAR_WIDTH - 2),
    last_char_row * CHAR_HEIGHT,
    last_char,
    false
  );
}

void toggle_cursor(
  virtual_keyboard *keyboard,
  uint8_t last_char_col,
  uint8_t last_char_row
){
  if (keyboard->show_cursor)
    ssd1306_draw_letter_at(
      drivers->oled_screen,
      last_char_col * (CHAR_WIDTH - 2),
      last_char_row * CHAR_HEIGHT,
      '_',
      false
    );
  else
    ssd1306_draw_letter_at(
      drivers->oled_screen,
      last_char_col * (CHAR_WIDTH - 2),
      last_char_row * CHAR_HEIGHT,
      ' ',
      false
    );
  keyboard->show_cursor = !keyboard->show_cursor;
}

char *virtual_keyboard_write(virtual_keyboard *keyboard)
{
  char buf[MAX_INPUT_LENGTH + 1];
  char last_char = '\0';
  key *last_key;
  last_key = &keyboard->keys[0][0];
  uint8_t last_char_col = 0;
  uint8_t last_char_row = 0;
  while (last_char != '>')
  {
    toggle_cursor(keyboard, last_char_col, last_char_row);
    highlight_key(keyboard, last_key, true);
    joystick_update(drivers->joystick);
    if (
      joystick_get_direction(drivers->joystick) == "W"
      && !is_out_of_bounds(last_key->row, last_key->col + 1)
    ){
      highlight_key(keyboard, last_key, false);
      highlight_key(keyboard, &keyboard->keys[last_key->row][last_key->col + 1], true);
      last_key = &keyboard->keys[last_key->row][last_key->col + 1];
    }
    else if (
      joystick_get_direction(drivers->joystick) == "S"
      && !is_out_of_bounds(last_key->row + 1, last_key->col)
    ){
      highlight_key(keyboard, last_key, false);
      highlight_key(keyboard, &keyboard->keys[last_key->row + 1][last_key->col], true);
      last_key = &keyboard->keys[last_key->row + 1][last_key->col];
    }
    else if (
      joystick_get_direction(drivers->joystick) == "N"
      && !is_out_of_bounds(last_key->row - 1, last_key->col)
    )
    {
      highlight_key(keyboard, last_key, false);
      highlight_key(keyboard, &keyboard->keys[last_key->row - 1][last_key->col], true);
      last_key = &keyboard->keys[last_key->row - 1][last_key->col];
    }
    else if (
      joystick_get_direction(drivers->joystick) == "E"
      && !is_out_of_bounds(last_key->row, last_key->col - 1)
    ){
      highlight_key(keyboard, last_key, false);
      highlight_key(keyboard, &keyboard->keys[last_key->row][last_key->col - 1], true);
      last_key = &keyboard->keys[last_key->row][last_key->col - 1];
    }
    if (drivers->joystick->button_pressed)
    {
      last_char = last_key->label;
      if (
        last_char_row == MAX_ROWS
        && last_char_col == MAX_CHARS_PER_ROW
      )
        continue;
      if (last_char_col == MAX_CHARS_PER_ROW){
        last_char_col = 0;
        last_char_row++;
      }
      if (last_char == '<'){
        if (last_char_col == 0)
          continue;
        do_backspace(last_char_col, last_char_row);
        last_char_col--;
        buf[last_char_col] = '\0';
        sleep_ms(DEBOUNCE_TIMEOUT);
        continue;
      }
      if (last_char == '^'){
        toggle_uppercase(keyboard);
        sleep_ms(DEBOUNCE_TIMEOUT);
        continue;
      }
      if(last_char == '_')
        last_char = ' ';
      buf[last_char_col] = last_char;
      write_char(last_char_col, last_char_row, last_char);
      last_char_col++;
      sleep_ms(DEBOUNCE_TIMEOUT);
    }
    ssd1306_show(drivers->oled_screen);
    sleep_ms(INPUT_TIMEOUT);
  }
  buf[last_char_col] = '\0';
  char *result = (char *)malloc(sizeof(char) * ((last_char_col * (last_char_row + 1)) + 1));
  strcpy(result, buf);
  return result;
}