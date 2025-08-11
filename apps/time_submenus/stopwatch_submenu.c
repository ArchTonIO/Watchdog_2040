#include "apps/time_submenus/stopwatch_submenu.h"

#include <pico/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "apps/time_submenus/time_utils.h"
#include "core/components/hw_manager.h"
#include "core/data_structures/string_list.h"
#include "core/graphics/bitmaps.h"
#include "core/hardware_drivers/haptics.h"
#include "core/hardware_drivers/joystick.h"
#include "core/hardware_drivers/ssd1306.h"

void start_stopwatch(time_digits *digits,
    milliseconds_digits *stopwatch_milliseconds_digits);
void stop_stopwatch(time_digits *digits);
void reset_stopwatch(time_digits *digits,
    milliseconds_digits *stopwatch_milliseconds_digits);
void save_stopwatch_time(time_digits *digits);

void milliseconds_show(milliseconds_digits *digits,
    uint8_t start_x,
    uint8_t start_y,
    uint8_t spacing) {
  start_x += spacing;
  ssd1306_draw_bitmap(drivers->oled_screen,
      start_x,
      start_y,
      clock_dots,
      CLOCK_DOTS_BITMAPS_W,
      CLOCK_DOTS_BITMAPS_H,
      false);
  start_x += CLOCK_DOTS_BITMAPS_W + spacing;
  ssd1306_draw_bitmap(drivers->oled_screen,
      start_x,
      start_y,
      clock_digits[digits->millisecond_tens],
      CLOCK_DIGIT_BITMAPS_W,
      CLOCK_DIGIT_BITMAPS_H,
      false);
  start_x += CLOCK_DIGIT_BITMAPS_W + spacing;
  ssd1306_draw_bitmap(drivers->oled_screen,
      start_x,
      start_y,
      clock_digits[digits->millisecond_units],
      CLOCK_DIGIT_BITMAPS_W,
      CLOCK_DIGIT_BITMAPS_H,
      false);
}

void enter_stopwatch_submenu() {
  sleep_ms(TIME_SUBMENUS_INPUT_TIMEOUT * 2);
  ssd1306_clear(drivers->oled_screen);
  time_digits *stopwatch_digits = time_digits_init();
  stopwatch_digits->hour_tens = 0;
  stopwatch_digits->hour_units = 0;
  stopwatch_digits->minute_tens = 0;
  stopwatch_digits->minute_units = 0;
  stopwatch_digits->second_tens = 0;
  stopwatch_digits->second_units = 0;
  milliseconds_digits *stopwatch_milliseconds_digits = malloc(
      sizeof(milliseconds_digits));
  stopwatch_milliseconds_digits->millisecond_tens = 0;
  stopwatch_milliseconds_digits->millisecond_units = 0;
  draw_symbols(stopwatch_go, stopwatch_stop, stopwatch_exit, stopwatch_zero);
  while (true) {
    joystick_update(drivers->joystick);
    uint8_t direction = joystick_get_direction(drivers->joystick);
    time_digits_show(stopwatch_digits, 16, 23, 2);
    milliseconds_show(stopwatch_milliseconds_digits, 86, 23, 2);
    ssd1306_show(drivers->oled_screen);
    if (direction == N) {
      haptic_short_pulse();
      start_stopwatch(stopwatch_digits, stopwatch_milliseconds_digits);
    }
    if (direction == E) {
      haptic_short_pulse();
      reset_stopwatch(stopwatch_digits, stopwatch_milliseconds_digits);
    }
    if (direction == W) {
      haptic_short_pulse();
      break;
    }
  }
  free(stopwatch_digits);
  free(stopwatch_milliseconds_digits);
}

void reset_stopwatch(time_digits *digits,
    milliseconds_digits *stopwatch_milliseconds_digits) {
  digits->hour_tens = 0;
  digits->hour_units = 0;
  digits->minute_tens = 0;
  digits->minute_units = 0;
  digits->second_tens = 0;
  digits->second_units = 0;
  stopwatch_milliseconds_digits->millisecond_tens = 0;
  stopwatch_milliseconds_digits->millisecond_units = 0;
}

void start_stopwatch(time_digits *digits,
    milliseconds_digits *stopwatch_milliseconds_digits) {
  absolute_time_t start_time = get_absolute_time();
  while (joystick_get_direction(drivers->joystick) != S) {
    absolute_time_t current_time = get_absolute_time();
    uint64_t elapsed_ms = absolute_time_diff_us(start_time, current_time) /
                          1000;
    digits->second_units = (elapsed_ms / 1000) % 10;
    digits->second_tens = (elapsed_ms / 10000) % 6;
    digits->minute_units = (elapsed_ms / 60000) % 10;
    digits->minute_tens = (elapsed_ms / 600000) % 6;
    digits->hour_units = (elapsed_ms / 3600000) % 10;
    digits->hour_tens = (elapsed_ms / 36000000) % 3;
    stopwatch_milliseconds_digits->millisecond_units = 0;
    stopwatch_milliseconds_digits->millisecond_tens = (elapsed_ms / 100) % 10;
    time_digits_show(digits, 16, 23, 2);
    milliseconds_show(stopwatch_milliseconds_digits, 86, 23, 2);
    ssd1306_show(drivers->oled_screen);
    joystick_update(drivers->joystick);
  }
  haptic_short_pulse();
}