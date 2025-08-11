#include "ssd1306.h"

#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"

#include "hardware/i2c.h"

// clang-format off

const uint8_t ssd1306_font6x8[] =
		{
				0x00, 0x06, 0x08, 0x20,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // sp
				0x00, 0x00, 0x00, 0x2f, 0x00, 0x00, // !
				0x00, 0x00, 0x07, 0x00, 0x07, 0x00, // "
				0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14, // #
				0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12, // $
				0x00, 0x23, 0x13, 0x08, 0x64, 0x62, // %
				0x00, 0x36, 0x49, 0x55, 0x22, 0x50, // &
				0x00, 0x00, 0x05, 0x03, 0x00, 0x00, // '
				0x00, 0x00, 0x1c, 0x22, 0x41, 0x00, // (
				0x00, 0x00, 0x41, 0x22, 0x1c, 0x00, // )
				0x00, 0x14, 0x08, 0x3E, 0x08, 0x14, // *
				0x00, 0x08, 0x08, 0x3E, 0x08, 0x08, // +
				0x00, 0x00, 0x00, 0xA0, 0x60, 0x00, // ,
				0x00, 0x08, 0x08, 0x08, 0x08, 0x08, // -
				0x00, 0x00, 0x60, 0x60, 0x00, 0x00, // .
				0x00, 0x20, 0x10, 0x08, 0x04, 0x02, // /
				0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
				0x00, 0x00, 0x42, 0x7F, 0x40, 0x00, // 1
				0x00, 0x42, 0x61, 0x51, 0x49, 0x46, // 2
				0x00, 0x21, 0x41, 0x45, 0x4B, 0x31, // 3
				0x00, 0x18, 0x14, 0x12, 0x7F, 0x10, // 4
				0x00, 0x27, 0x45, 0x45, 0x45, 0x39, // 5
				0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30, // 6
				0x00, 0x01, 0x71, 0x09, 0x05, 0x03, // 7
				0x00, 0x36, 0x49, 0x49, 0x49, 0x36, // 8
				0x00, 0x06, 0x49, 0x49, 0x29, 0x1E, // 9
				0x00, 0x00, 0x36, 0x36, 0x00, 0x00, // :
				0x00, 0x00, 0x56, 0x36, 0x00, 0x00, // ;
				0x00, 0x08, 0x14, 0x22, 0x41, 0x00, // <
				0x00, 0x14, 0x14, 0x14, 0x14, 0x14, // =
				0x00, 0x00, 0x41, 0x22, 0x14, 0x08, // >
				0x00, 0x02, 0x01, 0x51, 0x09, 0x06, // ?
				0x00, 0x32, 0x49, 0x59, 0x51, 0x3E, // @
				0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C, // A
				0x00, 0x7F, 0x49, 0x49, 0x49, 0x36, // B
				0x00, 0x3E, 0x41, 0x41, 0x41, 0x22, // C
				0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C, // D
				0x00, 0x7F, 0x49, 0x49, 0x49, 0x41, // E
				0x00, 0x7F, 0x09, 0x09, 0x09, 0x01, // F
				0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A, // G
				0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F, // H
				0x00, 0x00, 0x41, 0x7F, 0x41, 0x00, // I
				0x00, 0x20, 0x40, 0x41, 0x3F, 0x01, // J
				0x00, 0x7F, 0x08, 0x14, 0x22, 0x41, // K
				0x00, 0x7F, 0x40, 0x40, 0x40, 0x40, // L
				0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F, // M
				0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F, // N
				0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E, // O
				0x00, 0x7F, 0x09, 0x09, 0x09, 0x06, // P
				0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
				0x00, 0x7F, 0x09, 0x19, 0x29, 0x46, // R
				0x00, 0x46, 0x49, 0x49, 0x49, 0x31, // S
				0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, // T
				0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F, // U
				0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F, // V
				0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F, // W
				0x00, 0x63, 0x14, 0x08, 0x14, 0x63, // X
				0x00, 0x07, 0x08, 0x70, 0x08, 0x07, // Y
				0x00, 0x61, 0x51, 0x49, 0x45, 0x43, // Z
				0x00, 0x00, 0x7F, 0x41, 0x41, 0x00, // [
				0x00, 0x55, 0x2A, 0x55, 0x2A, 0x55, // 55
				0x00, 0x00, 0x41, 0x41, 0x7F, 0x00, // ]
				0x00, 0x04, 0x02, 0x01, 0x02, 0x04, // ^
				0x00, 0x40, 0x40, 0x40, 0x40, 0x40, // _
				0x00, 0x00, 0x01, 0x02, 0x04, 0x00, // '
				0x00, 0x20, 0x54, 0x54, 0x54, 0x78, // a
				0x00, 0x7F, 0x48, 0x44, 0x44, 0x38, // b
				0x00, 0x38, 0x44, 0x44, 0x44, 0x20, // c
				0x00, 0x38, 0x44, 0x44, 0x48, 0x7F, // d
				0x00, 0x38, 0x54, 0x54, 0x54, 0x18, // e
				0x00, 0x08, 0x7E, 0x09, 0x01, 0x02, // f
				0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C, // g
				0x00, 0x7F, 0x08, 0x04, 0x04, 0x78, // h
				0x00, 0x00, 0x44, 0x7D, 0x40, 0x00, // i
				0x00, 0x40, 0x80, 0x84, 0x7D, 0x00, // j
				0x00, 0x7F, 0x10, 0x28, 0x44, 0x00, // k
				0x00, 0x00, 0x41, 0x7F, 0x40, 0x00, // l
				0x00, 0x7C, 0x04, 0x18, 0x04, 0x78, // m
				0x00, 0x7C, 0x08, 0x04, 0x04, 0x78, // n
				0x00, 0x38, 0x44, 0x44, 0x44, 0x38, // o
				0x00, 0xFC, 0x24, 0x24, 0x24, 0x18, // p
				0x00, 0x18, 0x24, 0x24, 0x18, 0xFC, // q
				0x00, 0x7C, 0x08, 0x04, 0x04, 0x08, // r
				0x00, 0x48, 0x54, 0x54, 0x54, 0x20, // s
				0x00, 0x04, 0x3F, 0x44, 0x40, 0x20, // t
				0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C, // u
				0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C, // v
				0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C, // w
				0x00, 0x44, 0x28, 0x10, 0x28, 0x44, // x
				0x00, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C, // y
				0x00, 0x44, 0x64, 0x54, 0x4C, 0x44, // z
				0x00, 0x00, 0x08, 0x77, 0x00, 0x00, // {
				0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, // |
				0x00, 0x00, 0x77, 0x08, 0x00, 0x00, // }
				0x00, 0x10, 0x08, 0x10, 0x08, 0x00, // ~
				0x14, 0x14, 0x14, 0x14, 0x14, 0x14, // horiz lines // DEL
				0x00																/* This byte is required for italic type of font */
};

// clang-format on

void init_i2c(ssd1306 *display);
void write_cmd(ssd1306 *display, uint8_t cmd);
void send2(ssd1306 *display, uint8_t v1, uint8_t v2);
void send_data(ssd1306 *display, uint8_t *data, int nbytes);
int pages(ssd1306 *display);
void ssd1306_invert(ssd1306 *display, uint8_t invert) {
  write_cmd(display, SET_NORM_INV | (invert & 1));
}

/*
 * @brief Initialize the display
 * @param sda: the sda pin
 * @param sck: the sck pin
 * @param i2c_port: the i2c port to use
 * @param baudrate: the baudrate of the i2c port
 * @param width: the width of the display
 * @param height: the height of the display
 * @param SID: the slave address of the display
 * @return the display
 */
ssd1306 *ssd1306_init(pin sda,
    pin sck,
    i2c_inst_t *i2c_port,
    uint32_t baudrate,
    uint8_t width,
    uint8_t height,
    uint8_t SID) {
  ssd1306 *new_display = (ssd1306 *)malloc(sizeof(ssd1306));
  new_display->sda = sda;
  new_display->sck = sck;
  new_display->i2c_port = i2c_port;
  new_display->baudrate = baudrate;
  new_display->width = width;
  new_display->height = height;
  new_display->SID = SID;
  new_display->cursorx = 0;
  new_display->cursory = 0;
  new_display->animation_timer_fired = false;
  int buf_size = (height / 8) * width + 1;
  new_display->scr = (uint8_t *)malloc(buf_size);
  if (!new_display->scr) {
    free(new_display);
    return NULL;
  }
  memset(new_display->scr, 0, buf_size);
  init_i2c(new_display);
  static const uint8_t cmds[] = {SET_DISP | 0x00,
      SET_MEM_ADDR,
      0x00,
      SET_DISP_START_LINE | 0x00,
      SET_SEG_REMAP | 0x01,
      SET_MUX_RATIO,
      0x3F,
      SET_COM_OUT_DIR | 0x08,
      SET_DISP_OFFSET,
      0x00,
      SET_COM_PIN_CFG,
      0x12,
      SET_DISP_CLK_DIV,
      0x80,
      SET_PRECHARGE,
      0xF1,
      SET_VCOM_DESEL,
      0x40,
      SET_CONTRAST,
      0xFF,
      SET_ENTIRE_ON,
      SET_NORM_INV,
      SET_CHARGE_PUMP,
      0x14,
      SET_DISP | 0x01};
  for (int i = 0; i < sizeof(cmds); i++)
    write_cmd(new_display, cmds[i]);
  ssd1306_clear(new_display);
  ssd1306_show(new_display);
  return new_display;
}

/*
 * @brief Draw a pixel at the specified position
 * The origin is the top left corner of the display (0, 0),
 * x and y progress to the right and down respectively
 * @param display: the display to draw to
 * @param x: the x position of the pixel
 * @param y: the y position of the pixel
 * @param color: the color of the pixel (1 for white, 0 for black, -1 for
 * invert)
 */
void ssd1306_draw_pixel(ssd1306 *display, int16_t x, int16_t y, int color) {
  if (x < 0 || x >= display->width || y < 0 || y >= display->height)
    return;
  int page = y / 8;
  int bit = 1 << (y % 8);
  int xincr = 8;
  xincr = display->height / 8;
  uint8_t *ptr = display->scr + x * xincr + page + 1;
  switch (color) {
  case 1:
    *ptr |= bit;
    break;
  case 0:
    *ptr &= ~bit;
    break;
  case -1:
    *ptr ^= bit;
    break;
  }
}

/*
 * @brief Draw a character at the specified position,
 * every character is 6 pixels wide and 8 pixels tall
 *
 * @param display: the display to draw to
 * @param x: the x position of the first character
 * @param y: the y position of the first character
 * @param c: the character to draw
 * @param reversed: whether to draw the character in reversed colors (thus
 * making it black fg in white bg)
 */
void ssd1306_draw_letter_at(ssd1306 *display,
    uint8_t x,
    uint8_t y,
    char c,
    bool reversed) {
  if (c < ' ' || c > 0x7F)
    c = '?';
  int offset = 4 + (c - ' ') * 6;
  for (int col = 0; col < 6; col++) {
    uint8_t line = reversed ? ~ssd1306_font6x8[offset + col]
                            : ssd1306_font6x8[offset + col];
    for (int row = 0; row < 8; row++) {
      ssd1306_draw_pixel(display, x + col, y + row, line & 1);
      line >>= 1;
    }
  }
  for (int row = 0; row < 8; row++) {
    ssd1306_draw_pixel(display, x + 6, y + row, 0);
    ssd1306_draw_pixel(display, x + 7, y + row, 0);
  }
}

/*
 * @brief Print a string to the display at the specified position,
 * every character is 6 pixels wide and 8 pixels tall

 * @param display: the display to print to
 * @param str: the string to print
 * @param x: the x position of the first character (CHAR_WIDTH will be
 multiplied by this value)
 * @param y: the y position of the first character (CHAR_HEIGHT will be
 multiplied by this value)
 * @param reversed: whether to print the string in reversed colors (thus making
 it black fg in white bg)
 */
void ssd1306_print(ssd1306 *display,
    const char *str,
    uint8_t x,
    uint8_t y,
    bool reversed) {
  display->cursorx = x * CHAR_WIDTH;
  display->cursory = y * CHAR_HEIGHT;
  char c;
  while (c = *str) {
    str++;
    if (c == '\n') {
      display->cursorx = 0;
      display->cursory += CHAR_HEIGHT;
      continue;
    }
    ssd1306_draw_letter_at(display,
        display->cursorx,
        display->cursory,
        c,
        reversed);
    display->cursorx += CHAR_WIDTH - 2;
  }
}

void ssd1306_print_gradually(ssd1306 *display,
    const char *str,
    uint8_t x,
    uint8_t y,
    bool reversed) {
  display->cursorx = x * CHAR_WIDTH;
  display->cursory = y * CHAR_HEIGHT;
  char c;
  while (c = *str) {
    str++;
    if (c == '\n') {
      display->cursorx = x * CHAR_WIDTH;
      display->cursory += CHAR_HEIGHT;
      continue;
    }
    ssd1306_draw_letter_at(display,
        display->cursorx,
        display->cursory,
        c,
        reversed);
    display->cursorx += CHAR_WIDTH - 2;
    ssd1306_show(display);
  }
}

/*
 * @brief Draw a bitmap at the specified position
 * The bitmap is a 1bpp monochrome bitmap, the width and height are in pixels
 *
 * @param display: the display to draw to
 * @param x: the x position of the first pixel
 * @param y: the y position of the first pixel
 * @param bitmap: the bitmap to draw
 * @param width: the width of the bitmap in pixels
 * @param height: the height of the bitmap in pixels
 * @param color: the color of the bitmap (1 for white, 0 for black)
 */
void ssd1306_draw_bitmap(ssd1306 *display,
    uint8_t x,
    uint8_t y,
    const uint8_t bitmap[],
    int16_t width,
    int16_t height,
    bool reversed) {
  int16_t byte_width = (width + 7) / 8;
  for (int16_t j = 0; j < height; j++, y++) {
    for (int16_t i = 0; i < width; i++) {
      uint8_t byte = bitmap[j * byte_width + (i / 8)];
      if (reversed)
        byte = ~byte;
      ssd1306_draw_pixel(display,
          x + i,
          y,
          (byte & (0x80 >> (i % 8))) ? 1 : 0);
    }
  }
}

/*
 * @brief Show the display buffer on the screen,
 * (display buffer is the buffer that holds the pixel data)
 *
 * @param display: the display to show
 */
void ssd1306_show(ssd1306 *display) {

  write_cmd(display, SET_MEM_ADDR);
  write_cmd(display, 0b01);
  write_cmd(display, SET_COL_ADDR);
  write_cmd(display, 0);
  write_cmd(display, 127);
  write_cmd(display, SET_PAGE_ADDR);
  write_cmd(display, 0);
  write_cmd(display, pages(display) - 1);
  display->scr[0] = 0x40;
  int size = (display->height / 8) * display->width + 1;
  send_data(display, display->scr, size);
}

void init_i2c(ssd1306 *display) {
  i2c_init(display->i2c_port, display->baudrate);
  gpio_set_function(display->sda, GPIO_FUNC_I2C);
  gpio_set_function(display->sck, GPIO_FUNC_I2C);
  gpio_pull_up(display->sda);
  gpio_pull_up(display->sck);
}

/*
 * @brief Clear the display buffer
 *
 * @param display: the display to clear
 */
void ssd1306_clear(ssd1306 *display) {
  int buf_size = (display->height / 8) * display->width + 1;
  if (display->scr)
    memset(display->scr, 0, buf_size);
}

/*
 * @brief Set the cursor position
 * @param display: the display to set the cursor position for
 * @param x: the x position of the cursor in characters coordinates (CHAR_WIDTH
 * will be multiplied by this value)
 * @param y: the y position of the cursor in characters coordinates
 * (CHAR_HEIGHT will be multiplied by this value)
 */
void ssd1306_set_cursor(ssd1306 *display, uint8_t x, uint8_t y) {
  display->cursorx = CHAR_WIDTH * x;
  display->cursory = CHAR_HEIGHT * y;
}

void write_cmd(ssd1306 *display, uint8_t cmd) { send2(display, 0x80, cmd); }

void send2(ssd1306 *display, uint8_t v1, uint8_t v2) {
  uint8_t buf[2];
  buf[0] = v1;
  buf[1] = v2;
  send_data(display, buf, 2);
}

void send_data(ssd1306 *display, uint8_t *data, int nbytes) {
  i2c_write_blocking(display->i2c_port, display->SID, data, nbytes, false);
}

int pages(ssd1306 *display) { return display->height / 8; }