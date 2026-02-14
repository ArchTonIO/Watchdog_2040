// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef SSD1306_H
#define SSD1306_H

#include <pico/mutex.h>
#include <stdbool.h>
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

#define MAX_BRIGHTNESS 0xFF
#define MIN_BRIGHTNESS 0X01

#define MAX_PHOTORESISTOR_ADC_VALUE 2000
#define MIN_PHOTORESISTOR_ADC_VALUE 15
/*the width of the photoresistor adc step that triggers the upper or lower
 * brightness level*/
#define BRIGHTNESS_STEP_WIDTH                                                 \
  (MAX_PHOTORESISTOR_ADC_VALUE - MIN_PHOTORESISTOR_ADC_VALUE) / MAX_BRIGHTNESS

typedef struct {
  pin photo_pin;
  uint8_t photo_channel;
} photoresistor_t;

void photoresistor_init(pin analog_pin,
    uint8_t channel,
    photoresistor_t *photoresistor);

uint16_t photoresistor_read(photoresistor_t *photoresistor);
uint8_t photoresistor_get_screen_level(photoresistor_t *photoresistor);

typedef struct {
  pin sda;
  pin sck;
  i2c_inst_t *i2c_port;
  uint32_t baudrate;
  uint8_t width;
  uint8_t height;
  uint8_t SID;
  uint8_t scr[SSD1306_BUF_SIZE];
  int cursorx;
  int cursory;
  bool mutex_support_enabled;
  mutex_t mutex;
  photoresistor_t photoresistor;
  bool auto_brightness;
} ssd1306_t;

void ssd1306_init(ssd1306_t *display,
    pin sda,
    pin sck,
    i2c_inst_t *i2c_port,
    uint32_t baudrate,
    uint8_t width,
    uint8_t height,
    uint8_t SID);
void ssd1306_set_brightness(ssd1306_t *display, uint8_t level);
void ssd1306_auto_adjust_brightness(ssd1306_t *display);
void ssd1306_draw_pixel(ssd1306_t *display, int16_t x, int16_t y, int color);
void ssd1306_draw_letter_at(ssd1306_t *display,
    uint8_t x,
    uint8_t y,
    char c,
    bool reversed);
void ssd1306_print(ssd1306_t *display,
    const char *str,
    uint8_t x,
    uint8_t y,
    bool reversed);
void ssd1306_print_gradually(ssd1306_t *display,
    const char *str,
    uint8_t x,
    uint8_t y,
    bool reversed);
void ssd1306_draw_bitmap(ssd1306_t *display,
    uint8_t x,
    uint8_t y,
    const uint8_t bitmap[],
    int16_t width,
    int16_t height,
    bool reversed);
void ssd1306_set_cursor(ssd1306_t *display, uint8_t x, uint8_t y);
void ssd1306_invert(ssd1306_t *display, uint8_t invert);
void ssd1306_show(ssd1306_t *display);
void ssd1306_clear(ssd1306_t *display);
uint8_t *ssd1306_take_screenshot(ssd1306_t *display);
inline void ssd1306_get_mutex(ssd1306_t *display) {
  mutex_enter_blocking(&display->mutex);
}
inline void ssd1306_release_mutex(ssd1306_t *display) {
  mutex_exit(&display->mutex);
}
inline void ssd1306_enable_mutex_support(ssd1306_t *display) {
  display->mutex_support_enabled = true;
}
inline void ssd1306_disable_mutex_support(ssd1306_t *display) {
  display->mutex_support_enabled = false;
}

/**
 * @brief Informs if mutex support was enabled
 * by display-involving running code.
 * Usually needed when code on core0 is using the display
 * and core1 needs to know if this code is using mutex or not to
 * access the display (often for some screen real time notification system)
 * @retval true if mutex support was enabled
 * @retval false if mutex support was not enabled
 */
inline bool ssd1306_was_mutex_support_enabled(ssd1306_t *display) {
  return display->mutex_support_enabled;
}

#endif