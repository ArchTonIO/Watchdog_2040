#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "hardware_drivers/joystick.h"
#include "hardware_drivers/ssd1306.h"
#include "hardware_drivers/config.h"
#include "pico/stdlib.h"
#include "hw_manager.h"
#include "virtual_keyboard.h"

#define MEMSET_FILL 32

#define MAX_CHARS_PER_ROW 20
#define MAX_ROWS 4
#define MAX_PAGES 100
#define MAX_INPUT_LENGTH (MAX_CHARS_PER_ROW * MAX_ROWS) * MAX_PAGES
#define SCROLL_CURSOR_MAX_HEIGHT 24
#define SCROLL_CURSOR_MIN_HEIGHT 2

typedef struct
{
  bool show_cursor;
  uint8_t num_extra_lines;
  uint8_t cursors_col;
  uint8_t cursors_row;
  char buf[MAX_INPUT_LENGTH + 1];
  uint16_t buf_counter;
  char video_buf[MAX_ROWS][MAX_CHARS_PER_ROW];
  virtual_keyboard *keyboard;
} text_editor;

text_editor *text_editor_init();
char *text_editor_start(text_editor *editor);
void save_buffer_as_file(text_editor *editor, char *filename);
void load_file_to_buffer(text_editor *editor, char *filename);

#endif

// uint8_t calculate_scroll_cursor_height(uint8_t num_extra_lines)
// {
//   switch (num_extra_lines)
//   {
//   case 1:
//     return SCROLL_CURSOR_MAX_HEIGHT;
//   case 2:
//     return 16;
//   case 3:
//     return 8;
//   case 4:
//     return 4;
//   case 5:
//     return 2;
//   default:
//     return SCROLL_CURSOR_MIN_HEIGHT;
//   }
// }