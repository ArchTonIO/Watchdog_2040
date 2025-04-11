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

#define MEMSET_FILL 149 /*ascii for bullet - not printable*/

#define MAX_VIDEO_ROWS 4
#define MAX_VIDEO_COLS 20
#define MAX_LOGIC_ROWS 400
#define MAX_LOGIC_COLS 20

#define SCROLL_CURSOR_MAX_HEIGHT 24
#define SCROLL_CURSOR_MIN_HEIGHT 2

typedef struct
{
  bool show_cursor;
  int16_t scroll;
  uint8_t video_cursor_col;
  uint8_t video_cursor_row;
  uint8_t logic_cursor_col;
  uint8_t logic_cursor_row;
  char video_buf[MAX_VIDEO_ROWS][MAX_VIDEO_COLS];
  char logic_buf[MAX_LOGIC_ROWS][MAX_LOGIC_COLS];
  virtual_keyboard *keyboard;
  bool debug;
} text_editor;

text_editor *text_editor_init(virtual_keyboard *keyboard, bool debug);
char *text_editor_start(text_editor *editor);
void reset_state(text_editor *editor);
void save_buffer_as_file(text_editor *editor, char *filename);
void load_file_to_buffer(text_editor *editor, char *filename);

#endif
