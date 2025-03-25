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

char keyboard_layout[ROWS][COLS] = {
    {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '[', ']', '*'},
    {'!', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', '"', '@', '<'},
    {'(', ')', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '#', '$', '/', '?', '&', '^'},
    {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '_', '=', '+', '>'}};

void populate_keys(virtual_keyboard *keyboard);
void highlight_key(virtual_keyboard *keyboard, key *key, bool highlight);

virtual_keyboard *virtual_keyboard_init()
{
  virtual_keyboard *new_keyboard = (virtual_keyboard *)malloc(sizeof(virtual_keyboard));
  populate_keys(new_keyboard);
  return new_keyboard;
}

void populate_keys(virtual_keyboard *keyboard)
{
  for (uint8_t i = 0; i < ROWS; i++)
    for (uint8_t j = 0; j < COLS; j++)
    {
      printf("ITERATING\n");
      keyboard->keys[i][j].label = keyboard_layout[i][j];
      keyboard->keys[i][j].row = i;
      keyboard->keys[i][j].col = j;
    }
}

void draw_keyboard(virtual_keyboard *keyboard)
{
  ssd1306_clear(drivers->oled_screen);
  for (uint8_t i = 0; i < ROWS; i++)
    for (uint8_t j = 0; j < COLS; j++)
      ssd1306_draw_letter_at(drivers->oled_screen, j * CHAR_WIDTH + LEFT_PADDING, i * CHAR_HEIGHT + TOP_PADDING, keyboard->keys[i][j].label, false);
  ssd1306_show(drivers->oled_screen);
  highlight_key(keyboard, &keyboard->keys[0][0], true);
}

void highlight_key(virtual_keyboard *keyboard, key *key, bool highlight)
{
  ssd1306_draw_letter_at(drivers->oled_screen, key->col * CHAR_WIDTH + LEFT_PADDING, key->row * CHAR_HEIGHT + TOP_PADDING, key->label, highlight);
  ssd1306_show(drivers->oled_screen);
}

char get_selected_char(virtual_keyboard *keyboard)
{
}

bool is_out_of_bounds(uint8_t row, uint8_t col)
{
  return row < 0 || row >= ROWS || col < 0 || col >= COLS;
}

char *virtual_keyboard_write(virtual_keyboard *keyboard)
{
  //! add secure bounds
  char buf[100];
  char last_char = '\0';
  key *last_key;
  last_key = &keyboard->keys[0][0];
  sleep_ms(1000);
  uint8_t last_char_pos = 0;
  while (last_char != '>')
  {
    joystick_update(drivers->joystick);
    if (joystick_get_direction(drivers->joystick) == "W" && !is_out_of_bounds(last_key->row, last_key->col + 1))
    {
      highlight_key(keyboard, last_key, false);
      highlight_key(keyboard, &keyboard->keys[last_key->row][last_key->col + 1], true);
      last_key = &keyboard->keys[last_key->row][last_key->col + 1];
    }
    else if (joystick_get_direction(drivers->joystick) == "S" && !is_out_of_bounds(last_key->row + 1, last_key->col))
    {
      highlight_key(keyboard, last_key, false);
      highlight_key(keyboard, &keyboard->keys[last_key->row + 1][last_key->col], true);
      last_key = &keyboard->keys[last_key->row + 1][last_key->col];
    }
    else if (joystick_get_direction(drivers->joystick) == "N" && !is_out_of_bounds(last_key->row - 1, last_key->col))
    {
      highlight_key(keyboard, last_key, false);
      highlight_key(keyboard, &keyboard->keys[last_key->row - 1][last_key->col], true);
      last_key = &keyboard->keys[last_key->row - 1][last_key->col];
    }
    else if (joystick_get_direction(drivers->joystick) == "E" && !is_out_of_bounds(last_key->row, last_key->col - 1))
    {
      highlight_key(keyboard, last_key, false);
      highlight_key(keyboard, &keyboard->keys[last_key->row][last_key->col - 1], true);
      last_key = &keyboard->keys[last_key->row][last_key->col - 1];
    }
    if (drivers->joystick->button_pressed)
    {
      last_char = last_key->label;
      buf[last_char_pos] = last_char;
      last_char_pos++;
      sleep_ms(100);
    }
    sleep_ms(50);
  }
  buf[last_char_pos] = '\0';
  char *result = (char *)malloc(sizeof(char) * (last_char_pos + 1));
  strcpy(result, buf);
  return result;
}