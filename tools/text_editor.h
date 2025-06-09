#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"

#include "components/hw_manager.h"
#include "hardware_drivers/config.h"
#include "hardware_drivers/joystick.h"
#include "hardware_drivers/ssd1306.h"
#include "tools/virtual_keyboard.h"

#define MEMSET_FILL 149 /*ascii for bullet - not printable*/

#define MAX_VIDEO_ROWS 4
#define MAX_VIDEO_COLS 20
#define MAX_LOGIC_ROWS 400
#define MAX_LOGIC_COLS 20

#define SCROLL_CURSOR_MAX_HEIGHT 24
#define SCROLL_CURSOR_MIN_HEIGHT 2

typedef struct {
  bool show_cursor;
  int16_t scroll;
  uint8_t video_cursor_col;
  uint8_t video_cursor_row;
  uint8_t logic_cursor_col;
  uint8_t logic_cursor_row;
  bool placeholder_text_present;
  char video_buf[MAX_VIDEO_ROWS][MAX_VIDEO_COLS];
  char logic_buf[MAX_LOGIC_ROWS][MAX_LOGIC_COLS];
  virtual_keyboard *keyboard;
  bool debug;
} text_editor;

text_editor *text_editor_init(virtual_keyboard *keyboard, bool debug);
text_editor *text_editor_launch(char *text, bool is_text_placeholder);
void text_editor_kill(text_editor *editor);
char *text_editor_get_buf(text_editor *editor);
void reset_state(text_editor *editor);

#endif
