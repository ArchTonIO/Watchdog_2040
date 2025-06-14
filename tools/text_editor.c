#include "tools/text_editor.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"

#include "components/hw_manager.h"
#include "data_structures/string_list.h"
#include "hardware_drivers/config.h"
#include "hardware_drivers/joystick.h"
#include "hardware_drivers/ssd1306.h"
#include "tools/virtual_keyboard.h"

void push_video_buf_to_screen(text_editor *editor);
void write_char(uint8_t col, uint8_t row, char to_write);
void blink_cursor(text_editor *editor, uint8_t col, uint8_t row);
void reset_state(text_editor *editor);
void show_scroll_cursor(uint8_t lenght, uint8_t y_pos);
uint8_t calculate_scroll_cursor_height(int16_t num_extra_lines);
void scroll_view_up(text_editor *editor);
void scroll_view_down(text_editor *editor);
void handle_backspace(text_editor *editor);
void handle_newline(text_editor *editor);
void handle_normal_char(text_editor *editor, char last_char);
void handle_keyboard_commands(text_editor *editor, char last_char);
void handle_text_wrapping(text_editor *editor);
void print_logic_buf(text_editor *editor);
void populate_video_buffer(text_editor *editor);
char *stringify_logic_buffer(text_editor *editor);
void navigate_text(text_editor *editor);
void insert_text(text_editor *editor, char *text, bool is_text_placeholder);

/**
 * @brief Launch a new instance of the text editor so that it appears
 * on the oled screen with all needed components loaded
 */
text_editor *text_editor_launch(char *text, bool is_text_placeholder) {
  virtual_keyboard *keyboard = virtual_keyboard_init();
  ssd1306_clear(drivers->oled_screen);
  text_editor *editor = text_editor_init(keyboard, false);
  draw_keyboard(keyboard);
  if (strcmp(text, "") != 0) {
    insert_text(editor, text, is_text_placeholder);
    if (is_text_placeholder)
      editor->placeholder_text_present = true;
  }
  return editor;
}

void text_editor_kill(text_editor *editor) {
  ssd1306_clear(drivers->oled_screen);
  ssd1306_show(drivers->oled_screen);
  free(editor->keyboard);
  free(editor);
}

/**
 * @brief Initialize a new text editor instance
 * @returns a new text editor instance
 */
text_editor *text_editor_init(virtual_keyboard *keyboard, bool debug) {
  text_editor *editor = (text_editor *)malloc(sizeof(text_editor));
  editor->show_cursor = true;
  editor->scroll = 0;
  editor->video_cursor_col = 0;
  editor->video_cursor_row = 0;
  editor->logic_cursor_col = 0;
  editor->logic_cursor_row = 0;
  editor->placeholder_text_present = false;
  memset(editor->logic_buf, MEMSET_FILL, MAX_LOGIC_ROWS * MAX_LOGIC_COLS);
  memset(editor->video_buf, MEMSET_FILL, MAX_VIDEO_ROWS * MAX_VIDEO_COLS);
  editor->keyboard = keyboard;
  editor->debug = debug;
  return editor;
}

void insert_text(text_editor *editor, char *text, bool is_text_placeholder) {
  for (uint32_t i = 0; i < strlen(text); i++) {
    if (text[i] == '\n') {
      handle_newline(editor);
      continue;
    }
    editor->logic_buf[editor->logic_cursor_row]
                     [editor->logic_cursor_col] = text[i];
    editor->logic_cursor_col++;
    editor->video_cursor_col++;
  }
  editor->logic_cursor_row++;
  editor->video_cursor_row++;
  handle_text_wrapping(editor);
  if (!is_text_placeholder)
    return;
  editor->video_cursor_col = 0;
  editor->logic_cursor_col = 0;
  editor->video_cursor_row = 0;
  editor->logic_cursor_row = 0;
}

/**
 * @brief Start the text editor loop to write a file
 * using the virtual keyboard and the oled screen
 * provided by the drivers manager
 *
 * @returns The user input buffer as string in
 * the moment an END is received from keyboard or
 * MAX_LOGIC_ROWS is reached
 */
char *text_editor_get_buf(text_editor *editor) {
  char last_input_char = '\0';
  while (1) {
    blink_cursor(editor, editor->video_cursor_col, editor->video_cursor_row);
    ssd1306_show(drivers->oled_screen);
    last_input_char = virtual_keyboard_read(editor->keyboard);
    if (last_input_char == END || editor->logic_cursor_row == MAX_LOGIC_ROWS)
      break;
    handle_text_wrapping(editor);
    handle_keyboard_commands(editor, last_input_char);
    populate_video_buffer(editor);
    push_video_buf_to_screen(editor);
    show_scroll_cursor(calculate_scroll_cursor_height(
                           (editor->logic_cursor_row - (MAX_VIDEO_ROWS - 1))),
        0);
  }
  return stringify_logic_buffer(editor);
}

void navigate_text(text_editor *editor) {
  while (1) {
    if (joystick_check_long_press(drivers->joystick, 500)) {
      sleep_ms(1000);
      break;
    }
    blink_cursor(editor, editor->video_cursor_col, editor->video_cursor_row);
    ssd1306_show(drivers->oled_screen);
    joystick_update(drivers->joystick);
    if (joystick_get_direction(drivers->joystick) == N) {
      if (editor->video_cursor_row > 0)
        editor->video_cursor_row--;
      if (editor->logic_cursor_row > 0)
        editor->logic_cursor_row--;
      if (editor->video_cursor_row == 0 &&
          editor->logic_cursor_row > editor->video_cursor_row + 1)
        scroll_view_up(editor);
    }
    if (joystick_get_direction(drivers->joystick) == S) {
      if (editor->video_cursor_row < MAX_VIDEO_ROWS - 1)
        editor->video_cursor_row++;
      if (editor->logic_cursor_row < MAX_LOGIC_ROWS - 1)
        editor->logic_cursor_row++;
      if (editor->video_cursor_row == MAX_VIDEO_ROWS - 1 &&
          editor->logic_cursor_row > editor->video_cursor_row)
        scroll_view_down(editor);
    }
    if (joystick_get_direction(drivers->joystick) == W &&
        editor->video_cursor_col > 0) {
      editor->video_cursor_col--;
      editor->logic_cursor_col--;
    }
    if (joystick_get_direction(drivers->joystick) == E &&
        editor->video_cursor_col < MAX_VIDEO_COLS) {
      editor->video_cursor_col++;
      editor->logic_cursor_col++;
    }
    sleep_ms(INPUT_TIMEOUT);
    populate_video_buffer(editor);
    push_video_buf_to_screen(editor);
    show_scroll_cursor(calculate_scroll_cursor_height(
                           (editor->logic_cursor_row - (MAX_VIDEO_ROWS - 1))),
        0);
  }
}

void reset_state(text_editor *editor) {
  editor->scroll = 0;
  editor->video_cursor_col = 0;
  editor->video_cursor_row = 0;
  editor->logic_cursor_col = 0;
  editor->logic_cursor_row = 0;
  memset(editor->logic_buf, MEMSET_FILL, MAX_LOGIC_ROWS * MAX_LOGIC_COLS);
  memset(editor->video_buf, MEMSET_FILL, MAX_VIDEO_ROWS * MAX_VIDEO_COLS);
}

void handle_keyboard_commands(text_editor *editor, char last_char) {
  if (last_char == NOW)
    return;
  if (editor->placeholder_text_present) {
    reset_state(editor);
    editor->placeholder_text_present = false;
  }
  if (last_char == NAV) {
    navigate_text(editor);
    return;
  }
  if (last_char == BCK) {
    handle_backspace(editor);
    editor->debug ? print_logic_buf(editor) : NULL;
    return;
  }
  if (last_char == '\n') {
    handle_newline(editor);
    editor->debug ? print_logic_buf(editor) : NULL;
    return;
  }
  handle_normal_char(editor, last_char);
  editor->debug ? print_logic_buf(editor) : NULL;
}

void handle_text_wrapping(text_editor *editor) {
  if (!(editor->logic_cursor_col == MAX_LOGIC_COLS))
    return;
  editor->logic_cursor_col = 0;
  editor->video_cursor_col = 0;
  editor->logic_cursor_row++;
  editor->video_cursor_row++;
}

void handle_normal_char(text_editor *editor, char last_char) {
  editor->logic_buf[editor->logic_cursor_row]
                   [editor->logic_cursor_col] = last_char;
  editor->logic_cursor_col++;
  editor->video_cursor_col++;
}

void handle_newline(text_editor *editor) {
  editor->logic_buf[editor->logic_cursor_row][editor->logic_cursor_col] = '\n';
  editor->logic_cursor_col = 0;
  editor->video_cursor_col = 0;
  editor->logic_cursor_row++;
  editor->video_cursor_row++;
}

void handle_backspace(text_editor *editor) {
  if (editor->logic_cursor_col == 0 && editor->logic_cursor_row == 0)
    return;
  if (editor->logic_cursor_col > 0) {
    char previous = MEMSET_FILL;
    char following;
    while (previous == MEMSET_FILL || previous == '\n') {
      previous = editor->logic_buf[editor->logic_cursor_row]
                                  [editor->logic_cursor_col - 1];
      following = editor->logic_buf[editor->logic_cursor_row]
                                   [editor->logic_cursor_col + 1];
      editor->logic_cursor_col--;
      editor->video_cursor_col--;
      editor->logic_buf[editor->logic_cursor_row]
                       [editor->logic_cursor_col] = following;
    }
  } else {
    char following = editor->logic_buf[editor->logic_cursor_row]
                                      [editor->logic_cursor_col + 1];
    editor->logic_cursor_col = MAX_LOGIC_COLS;
    editor->video_cursor_col = MAX_VIDEO_COLS;
    editor->logic_cursor_row--;
    if (editor->video_cursor_row > 0)
      editor->video_cursor_row--;
    if (editor->logic_cursor_row >= MAX_VIDEO_ROWS - 1)
      scroll_view_up(editor);
    while (editor->logic_buf[editor->logic_cursor_row]
                            [editor->logic_cursor_col] == MEMSET_FILL ||
           editor->logic_buf[editor->logic_cursor_row]
                            [editor->logic_cursor_col] == '\n') {
      if (editor->logic_cursor_col == 0) {
        if (editor->logic_cursor_row > MAX_VIDEO_ROWS - 1)
          scroll_view_up(editor);
        editor->logic_cursor_row--;
        if (editor->video_cursor_row > 0)
          editor->video_cursor_row--;
        editor->logic_cursor_col = MAX_LOGIC_COLS;
        editor->video_cursor_col = MAX_VIDEO_COLS;
      } else {
        editor->logic_cursor_col--;
        editor->video_cursor_col--;
      }
    }
    editor->logic_buf[editor->logic_cursor_row]
                     [editor->logic_cursor_col] = following;
  }
}

void print_logic_buf(text_editor *editor) {
  printf("\n");
  char to_print = '\0';
  for (uint8_t i = 0; i < MAX_LOGIC_ROWS; i++) {
    for (uint8_t j = 0; j < MAX_LOGIC_COLS; j++) {
      if (editor->logic_buf[i][j] == '\n')
        to_print = MEMSET_FILL;
      else
        to_print = editor->logic_buf[i][j];
      printf("%c", to_print);
    }
    printf("\n");
  }
  printf("VIDEO CURSOR ROW: %d, VIDEO CURSOR COL: %d\nLOGIC CURSOR ROW: %d, "
         "LOGIC CURSOR COL: %d, SCROLL: %d\n",
      editor->video_cursor_row,
      editor->video_cursor_col,
      editor->logic_cursor_row,
      editor->logic_cursor_col,
      editor->scroll);
}

char *stringify_logic_buffer(text_editor *editor) {
  char *res = (char *)malloc(sizeof(char));
  res[0] = '\0';
  for (uint16_t i = 0; i < MAX_LOGIC_ROWS; i++) {
    for (uint8_t j = 0; j < MAX_LOGIC_COLS; j++) {
      if (editor->logic_buf[i][j] == MEMSET_FILL)
        continue;
      size_t len = strlen(res);
      res = realloc(res, sizeof(char) * (len + 2));
      res[len] = editor->logic_buf[i][j];
      res[len + 1] = '\0';
    }
  }
  return res;
}

void populate_video_buffer(text_editor *editor) {
  for (uint8_t i = 0; i < MAX_VIDEO_ROWS; i++)
    for (uint8_t j = 0; j < MAX_VIDEO_COLS; j++)
      if (editor->logic_buf[i + editor->scroll][j] != MEMSET_FILL &&
          editor->logic_buf[i + editor->scroll][j] != '\n')
        editor->video_buf[i][j] = editor->logic_buf[i + editor->scroll][j];
      else
        editor->video_buf[i][j] = ' ';
  if (editor->video_cursor_row > MAX_VIDEO_ROWS - 1) {
    scroll_view_down(editor);
    return;
  }
}

void scroll_view_down(text_editor *editor) {
  for (uint8_t i = 0; i < MAX_VIDEO_ROWS - 1; i++)
    for (uint8_t j = 0; j < MAX_VIDEO_COLS; j++)
      editor->video_buf[i][j] = editor->video_buf[i + 1][j];
  for (uint8_t j = 0; j < MAX_VIDEO_COLS; j++)
    editor->video_buf[MAX_VIDEO_ROWS - 1][j] = ' ';
  editor->video_cursor_row--;
  editor->scroll++;
}

void scroll_view_up(text_editor *editor) {
  for (uint8_t i = MAX_VIDEO_ROWS - 1; i > 0; i--)
    for (uint8_t j = 0; j < MAX_VIDEO_COLS; j++)
      editor->video_buf[i][j] = editor->video_buf[i - 1][j];
  editor->video_cursor_row++;
  editor->scroll--;
}

void push_video_buf_to_screen(text_editor *editor) {
  for (uint8_t i = 0; i < MAX_VIDEO_ROWS; i++)
    for (uint8_t j = 0; j < MAX_VIDEO_COLS; j++)
      write_char(j, i, editor->video_buf[i][j]);
}

void write_char(uint8_t col, uint8_t row, char to_write) {
  ssd1306_draw_letter_at(drivers->oled_screen,
      col * (CHAR_WIDTH - 2),
      row * CHAR_HEIGHT,
      to_write,
      false);
}

void blink_cursor(text_editor *editor, uint8_t col, uint8_t row) {
  if (col >= MAX_VIDEO_COLS)
    col = MAX_VIDEO_COLS - 1;
  if (editor->show_cursor)
    ssd1306_draw_letter_at(drivers->oled_screen,
        col * (CHAR_WIDTH - 2),
        row * CHAR_HEIGHT,
        '_',
        false);
  else
    ssd1306_draw_letter_at(drivers->oled_screen,
        col * (CHAR_WIDTH - 2),
        row * CHAR_HEIGHT,
        ' ',
        false);
  editor->show_cursor = !editor->show_cursor;
}

void show_scroll_cursor(uint8_t lenght, uint8_t y_pos) {
  for (uint8_t i = 0; i < (MAX_VIDEO_ROWS * CHAR_HEIGHT); i++) {
    ssd1306_draw_pixel(drivers->oled_screen, SSD1306_WIDTH - 2, i, 0);
    ssd1306_draw_pixel(drivers->oled_screen, SSD1306_WIDTH - 1, i, 0);
  }
  if (lenght == 0)
    return;
  for (uint8_t i = 0; i < lenght; i++) {
    ssd1306_draw_pixel(drivers->oled_screen, SSD1306_WIDTH - 2, y_pos + i, 1);
    ssd1306_draw_pixel(drivers->oled_screen, SSD1306_WIDTH - 1, y_pos + i, 1);
  }
}

uint8_t calculate_scroll_cursor_height(int16_t num_extra_lines) {
  if (num_extra_lines <= 0)
    return 0;
  float ratio = (float)MAX_VIDEO_ROWS / (MAX_VIDEO_ROWS + num_extra_lines);
  uint8_t height = (uint8_t)(ratio * MAX_VIDEO_ROWS * CHAR_HEIGHT);
  if (height < SCROLL_CURSOR_MIN_HEIGHT)
    return SCROLL_CURSOR_MIN_HEIGHT;
  return height;
}
