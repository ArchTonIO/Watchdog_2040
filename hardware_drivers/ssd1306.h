#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>

#include "config.h"
#include "hardware/i2c.h"

extern const uint8_t ssd1306_font6x8[];
#define SET_CONTRAST 0x81
#define SET_ENTIRE_ON 0xA4
#define SET_NORM_INV 0xA6
#define SET_DISP 0xAE
#define SET_MEM_ADDR 0x20
#define SET_COL_ADDR 0x21
#define SET_PAGE_ADDR 0x22
#define SET_DISP_START_LINE 0x40
#define SET_SEG_REMAP 0xA0
#define SET_MUX_RATIO 0xA8
#define SET_COM_OUT_DIR 0xC0
#define SET_DISP_OFFSET 0xD3
#define SET_COM_PIN_CFG 0xDA
#define SET_DISP_CLK_DIV 0xD5
#define SET_PRECHARGE 0xD9
#define SET_VCOM_DESEL 0xDB
#define SET_CHARGE_PUMP 0x8D

#define CHAR_HEIGHT 8 // char height in pixels
#define CHAR_WIDTH 8  // char width in pixels
#define I2C_PORT i2c1

#define MAX_X_CHARS 21
#define MAX_Y_CHARS 8

typedef struct {
  pin sda;
  pin sck;
  i2c_inst_t *i2c_port;
  uint32_t baudrate;
  uint8_t width;
  uint8_t height;
  uint8_t SID;
  uint8_t scr[1025];
  volatile bool animation_timer_fired;
  int cursorx;
  int cursory;
} ssd1306;

ssd1306 *ssd1306_init(pin sda,
    pin sck,
    i2c_inst_t *i2c_port,
    uint32_t baudrate,
    uint8_t width,
    uint8_t height,
    uint8_t SID);
void ssd1306_draw_pixel(ssd1306 *screen, int16_t x, int16_t y, int color);
void ssd1306_draw_letter_at(
    ssd1306 *screen, uint8_t x, uint8_t y, char c, bool reversed);
void ssd1306_print(
    ssd1306 *screen, const char *str, uint8_t x, uint8_t y, bool reversed);
void ssd1306_print_gradually(
    ssd1306 *screen, const char *str, uint8_t x, uint8_t y, bool reversed);
void ssd1306_draw_bitmap(ssd1306 *display,
    uint8_t x,
    uint8_t y,
    const uint8_t bitmap[],
    int16_t width,
    int16_t height,
    bool reversed);
void ssd1306_set_cursor(ssd1306 *screen, uint8_t x, uint8_t y);
void ssd1306_invert(ssd1306 *display, uint8_t invert);
void ssd1306_show(ssd1306 *screen);
void ssd1306_clear(ssd1306 *screen);

#endif