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
#include "text_editor.h"

void push_video_buf_to_screen(text_editor *editor);
void write_char(uint8_t col, uint8_t row, char to_write);
void blink_cursor(text_editor *editor, uint8_t col, uint8_t row);
void reset_state(text_editor *editor);

text_editor *text_editor_init(virtual_keyboard *keyboard)
{
  text_editor *editor = (text_editor *)malloc(sizeof(text_editor));
  editor->keyboard = keyboard;
  editor->show_cursor = true;
  editor->cursors_col = 0;
  editor->cursors_row = 0;
  editor->buf_counter = 0;
  memset(editor->buf, '\0', MAX_INPUT_LENGTH + 1);
  memset(editor->video_buf, MEMSET_FILL, MAX_ROWS * MAX_CHARS_PER_ROW);
  return editor;
}

char *text_editor_start(text_editor *editor)
{
  char last_char = '\0';
  while (1)
  {
    ssd1306_show(drivers->oled_screen);
    blink_cursor(editor, editor->cursors_col, editor->cursors_row);
    last_char = virtual_keyboard_write(editor->keyboard);

    /*handle end input*/
    if (last_char == END_INPUT_CHAR)
      break;

    /*handle keyboard commands*/
    if (last_char == NO_WRITE_CHAR)
      continue;

    /*handle text wrapping*/
    if (editor->cursors_col == MAX_CHARS_PER_ROW)
    {
      editor->cursors_col = 0;
      editor->cursors_row++;
    }

    /*handle backspace*/
    if (last_char == BACKSPACE_CHAR)
    {
      if (editor->buf_counter == 0)
        continue;
      if (editor->cursors_col == 0)
      {
        editor->cursors_col = MAX_CHARS_PER_ROW - 1;
        editor->cursors_row--;
      }
      else
      {
        editor->cursors_col--;
        editor->buf_counter--;
      }
      editor->video_buf[editor->cursors_row][editor->cursors_col] = ' ';
      editor->buf[editor->buf_counter] = editor->buf[editor->buf_counter + 1];
    }

    /*handle newline character*/
    if (last_char == '\n')
    {
      editor->cursors_col = 0;
      editor->cursors_row++;
      editor->buf[editor->buf_counter] = '\n';
      editor->buf_counter++;
    }

    /*handle normal character*/
    if (last_char != BACKSPACE_CHAR && last_char != '\n')
    {
      editor->buf[editor->buf_counter] = last_char;
      editor->video_buf[editor->cursors_row][editor->cursors_col] = last_char;
      editor->cursors_col++;
      editor->buf_counter++;
    }
    push_video_buf_to_screen(editor);
  }
  editor->buf[editor->buf_counter] = '\0';
  char *res = (char *)malloc(editor->buf_counter);
  strcpy(res, editor->buf);
  reset_state(editor);
  return res;
}

void reset_state(text_editor *editor)
{
  editor->cursors_col = 0;
  editor->cursors_row = 0;
  editor->buf_counter = 0;
  memset(editor->buf, 0, MAX_INPUT_LENGTH + 1);
  memset(editor->video_buf, MEMSET_FILL, MAX_ROWS * MAX_CHARS_PER_ROW);
}

void push_video_buf_to_screen(text_editor *editor)
{
  for (uint8_t i = 0; i < MAX_ROWS; i++)
    for (uint8_t j = 0; j < MAX_CHARS_PER_ROW; j++)
      write_char(j, i, editor->video_buf[i][j]);
}

void write_char(
    uint8_t col,
    uint8_t row,
    char to_write)
{
  ssd1306_draw_letter_at(
      drivers->oled_screen,
      col * (CHAR_WIDTH - 2),
      row * CHAR_HEIGHT,
      to_write,
      false);
}

void blink_cursor(
    text_editor *editor,
    uint8_t col,
    uint8_t row)
{
  if (editor->show_cursor)
    ssd1306_draw_letter_at(
        drivers->oled_screen,
        col * (CHAR_WIDTH - 2),
        row * CHAR_HEIGHT,
        '_',
        false);
  else
    ssd1306_draw_letter_at(
        drivers->oled_screen,
        col * (CHAR_WIDTH - 2),
        row * CHAR_HEIGHT,
        ' ',
        false);
  editor->show_cursor = !editor->show_cursor;
}

// void show_scroll_cursor(uint8_t lenght, uint16_t y_pos)
// {
//   for (uint8_t i = 0; i < lenght; i++)
//     ssd1306_draw_pixel(
//         drivers->oled_screen,
//         SSD1306_WIDTH,
//         y_pos + i,
//         1);
// }