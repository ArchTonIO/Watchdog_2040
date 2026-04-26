// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "apps/time/include/time_utils.h"

#include <pico/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/components/include/bitmaps.h"
#include "core/components/include/hw_manager.h"
#include "core/data_structures/include/string_list.h"
#include "core/graphics/include/graphic_primitives.h"
#include "core/hardware_drivers/include/haptics.h"
#include "core/hardware_drivers/include/ssd1306.h"
#include "core/utils/include/utils.h"
#include "include/bitmaps.h"

time_digits *time_digits_init() {
  update_time(&(drivers->internal_rtc));
  int8_t hour = drivers->internal_rtc.internal_datetime.hour;
  int8_t minute = drivers->internal_rtc.internal_datetime.min;
  int8_t second = drivers->internal_rtc.internal_datetime.sec;
  time_digits *digits = malloc(sizeof(time_digits));
  digits->hour_tens = hour / 10;
  digits->hour_units = hour % 10;
  digits->minute_tens = minute / 10;
  digits->minute_units = minute % 10;
  digits->second_tens = second / 10;
  digits->second_units = second % 10;
  return digits;
}

void draw_symbols(const uint8_t *top_symbol,
    const uint8_t *bottom_symbol,
    const uint8_t *left_symbol,
    const uint8_t *right_symbol) {
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      (SSD1306_WIDTH - TIME_NAV_SYMBOLS_L) / 2,
      0,
      top_symbol,
      TIME_NAV_SYMBOLS_L,
      TIME_NAV_SYMBOLS_L,
      false);
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      (SSD1306_WIDTH - TIME_NAV_SYMBOLS_L) / 2,
      SSD1306_HEIGHT - TIME_NAV_SYMBOLS_L,
      bottom_symbol,
      TIME_NAV_SYMBOLS_L,
      TIME_NAV_SYMBOLS_L,
      false);
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      (SSD1306_HEIGHT - TIME_NAV_SYMBOLS_L) / 2,
      left_symbol,
      TIME_NAV_SYMBOLS_L,
      TIME_NAV_SYMBOLS_L,
      false);
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      SSD1306_WIDTH - TIME_NAV_SYMBOLS_L,
      (SSD1306_HEIGHT - TIME_NAV_SYMBOLS_L) / 2,
      right_symbol,
      TIME_NAV_SYMBOLS_L,
      TIME_NAV_SYMBOLS_L,
      false);
}

void time_digits_show(time_digits *digits,
    uint8_t start_x,
    uint8_t start_y,
    uint8_t spacing) {
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      start_x,
      start_y,
      clock_digits[digits->hour_tens],
      CLOCK_DIGIT_BITMAPS_W,
      CLOCK_DIGIT_BITMAPS_H,
      false);
  start_x += CLOCK_DIGIT_BITMAPS_W + spacing;
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      start_x,
      start_y,
      clock_digits[digits->hour_units],
      CLOCK_DIGIT_BITMAPS_W,
      CLOCK_DIGIT_BITMAPS_H,
      false);
  start_x += CLOCK_DIGIT_BITMAPS_W + spacing;
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      start_x,
      start_y,
      clock_dots,
      CLOCK_DOTS_BITMAPS_W,
      CLOCK_DOTS_BITMAPS_H,
      false);
  start_x += CLOCK_DOTS_BITMAPS_W + spacing;
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      start_x,
      start_y,
      clock_digits[digits->minute_tens],
      CLOCK_DIGIT_BITMAPS_W,
      CLOCK_DIGIT_BITMAPS_H,
      false);
  start_x += CLOCK_DIGIT_BITMAPS_W + spacing;
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      start_x,
      start_y,
      clock_digits[digits->minute_units],
      CLOCK_DIGIT_BITMAPS_W,
      CLOCK_DIGIT_BITMAPS_H,
      false);
  start_x += CLOCK_DIGIT_BITMAPS_W + spacing;
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      start_x,
      start_y,
      clock_dots,
      CLOCK_DOTS_BITMAPS_W,
      CLOCK_DOTS_BITMAPS_H,
      false);
  start_x += CLOCK_DOTS_BITMAPS_W + spacing;
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      start_x,
      start_y,
      clock_digits[digits->second_tens],
      CLOCK_DIGIT_BITMAPS_W,
      CLOCK_DIGIT_BITMAPS_H,
      false);
  start_x += CLOCK_DIGIT_BITMAPS_W + spacing;
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      start_x,
      start_y,
      clock_digits[digits->second_units],
      CLOCK_DIGIT_BITMAPS_W,
      CLOCK_DIGIT_BITMAPS_H,
      false);
}

void blink_lines(line underline, line upperline, uint8_t interval) {
  draw_line(underline);
  draw_line(upperline);
  ssd1306_show(&(drivers->ssd1306));
  sleep_ms(interval / 2);
  clear_line(underline);
  clear_line(upperline);
}

void set_hours_tens(time_digits *digits,
    void (*at_change_callback)(time_digits *digits)) {
  uint8_t start_x = 28;
  uint8_t lower_y = 42;
  uint8_t upper_y = 20;
  line underline = create_line(create_point(start_x, lower_y),
      create_point(start_x + CLOCK_DIGIT_BITMAPS_W - 1, lower_y));
  line upperline = create_line(create_point(start_x, upper_y),
      create_point(start_x + CLOCK_DIGIT_BITMAPS_W - 1, upper_y));
  while (true) {
    blink_lines(underline, upperline, TIME_SUBMENUS_INPUT_TIMEOUT / 2);
    joystick_update(&(drivers->joystick));
    uint8_t direction = joystick_get_direction(&(drivers->joystick));
    switch (direction) {
    case N:
      haptic_auto_pulse();
      digits->hour_tens++;
      if (digits->hour_tens > 2)
        digits->hour_tens = 0;
      break;
    case S:
      haptic_auto_pulse();
      digits->hour_tens--;
      if (digits->hour_tens < 0)
        digits->hour_tens = 2;
      break;
    case E:
      haptic_auto_pulse();
      set_hours_units(digits, at_change_callback);
      break;
    case W:
      haptic_auto_pulse();
      return;
    default:
      break;
    }
    time_digits_show(digits, 28, 23, 2);
    ssd1306_show(&(drivers->ssd1306));
  }
}

void set_hours_units(time_digits *digits,
    void (*at_change_callback)(time_digits *digits)) {
  uint8_t start_x = 38;
  uint8_t lower_y = 42;
  uint8_t upper_y = 20;
  line underline = create_line(create_point(start_x, lower_y),
      create_point(start_x + CLOCK_DIGIT_BITMAPS_W - 1, lower_y));
  line upperline = create_line(create_point(start_x, upper_y),
      create_point(start_x + CLOCK_DIGIT_BITMAPS_W - 1, upper_y));
  while (true) {
    blink_lines(underline, upperline, TIME_SUBMENUS_INPUT_TIMEOUT / 2);
    joystick_update(&(drivers->joystick));
    uint8_t direction = joystick_get_direction(&(drivers->joystick));
    switch (direction) {
    case N:
      haptic_auto_pulse();
      digits->hour_units++;
      if (digits->hour_units > 9 ||
          (digits->hour_tens == 2 && digits->hour_units > 4))
        digits->hour_units = 0;
      break;
    case S:
      haptic_auto_pulse();
      digits->hour_units--;
      if (digits->hour_units < 0)
        digits->hour_units = 9;
      break;
    case E:
      haptic_auto_pulse();
      set_minutes_tens(digits, at_change_callback);
      break;
    case W:
      haptic_auto_pulse();
      return;
    default:
      break;
    }
    time_digits_show(digits, 28, 23, 2);
    ssd1306_show(&(drivers->ssd1306));
  }
}

void set_minutes_tens(time_digits *digits,
    void (*at_change_callback)(time_digits *digits)) {
  uint8_t start_x = 54;
  uint8_t lower_y = 42;
  uint8_t upper_y = 20;
  line underline = create_line(create_point(start_x, lower_y),
      create_point(start_x + CLOCK_DIGIT_BITMAPS_W - 1, lower_y));
  line upperline = create_line(create_point(start_x, upper_y),
      create_point(start_x + CLOCK_DIGIT_BITMAPS_W - 1, upper_y));

  while (true) {
    blink_lines(underline, upperline, TIME_SUBMENUS_INPUT_TIMEOUT / 2);
    joystick_update(&(drivers->joystick));
    uint8_t direction = joystick_get_direction(&(drivers->joystick));
    switch (direction) {
    case N:
      haptic_auto_pulse();
      digits->minute_tens++;
      if (digits->minute_tens > 5)
        digits->minute_tens = 0;
      break;
    case S:
      haptic_auto_pulse();
      digits->minute_tens--;
      if (digits->minute_tens < 0)
        digits->minute_tens = 5;
      break;
    case E:
      haptic_auto_pulse();
      set_minutes_units(digits, at_change_callback);
      break;
    case W:
      haptic_auto_pulse();
      return;
    default:
      break;
    }
    time_digits_show(digits, 28, 23, 2);
    ssd1306_show(&(drivers->ssd1306));
  }
}

void set_minutes_units(time_digits *digits,
    void (*at_change_callback)(time_digits *digits)) {
  uint8_t start_x = 64;
  uint8_t lower_y = 42;
  uint8_t upper_y = 20;
  line underline = create_line(create_point(start_x, lower_y),
      create_point(start_x + CLOCK_DIGIT_BITMAPS_W - 1, lower_y));
  line upperline = create_line(create_point(start_x, upper_y),
      create_point(start_x + CLOCK_DIGIT_BITMAPS_W - 1, upper_y));
  while (true) {
    blink_lines(underline, upperline, TIME_SUBMENUS_INPUT_TIMEOUT / 2);
    joystick_update(&(drivers->joystick));
    uint8_t direction = joystick_get_direction(&(drivers->joystick));
    switch (direction) {
    case N:
      haptic_auto_pulse();
      digits->minute_units++;
      if (digits->minute_units > 9)
        digits->minute_units = 0;
      break;
    case S:
      haptic_auto_pulse();
      digits->minute_units--;
      if (digits->minute_units < 0)
        digits->minute_units = 9;
      break;
    case E:
      haptic_auto_pulse();
      set_seconds_tens(digits, at_change_callback);
      break;
    case W:
      haptic_auto_pulse();
      return;
    default:
      break;
    }
    time_digits_show(digits, 28, 23, 2);
    ssd1306_show(&(drivers->ssd1306));
  }
}

void set_seconds_tens(time_digits *digits,
    void (*at_change_callback)(time_digits *digits)) {
  uint8_t start_x = 80;
  uint8_t lower_y = 42;
  uint8_t upper_y = 20;
  line underline = create_line(create_point(start_x, lower_y),
      create_point(start_x + CLOCK_DIGIT_BITMAPS_W - 1, lower_y));
  line upperline = create_line(create_point(start_x, upper_y),
      create_point(start_x + CLOCK_DIGIT_BITMAPS_W - 1, upper_y));
  while (true) {
    blink_lines(underline, upperline, TIME_SUBMENUS_INPUT_TIMEOUT / 2);
    joystick_update(&(drivers->joystick));
    uint8_t direction = joystick_get_direction(&(drivers->joystick));
    switch (direction) {
    case N:
      haptic_auto_pulse();
      digits->second_tens++;
      if (digits->second_tens > 5)
        digits->second_tens = 0;
      break;
    case S:
      haptic_auto_pulse();
      digits->second_tens--;
      if (digits->second_tens < 0)
        digits->second_tens = 5;
      break;
    case E:
      haptic_auto_pulse();
      set_seconds_units(digits, at_change_callback);
      break;
    case W:
      haptic_auto_pulse();
      return;
    default:
      break;
    }
    time_digits_show(digits, 28, 23, 2);
    ssd1306_show(&(drivers->ssd1306));
  }
}

void set_seconds_units(time_digits *digits,
    void (*at_change_callback)(time_digits *digits)) {
  uint8_t start_x = 90;
  uint8_t lower_y = 42;
  uint8_t upper_y = 20;
  line underline = create_line(create_point(start_x, lower_y),
      create_point(start_x + CLOCK_DIGIT_BITMAPS_W - 1, lower_y));
  line upperline = create_line(create_point(start_x, upper_y),
      create_point(start_x + CLOCK_DIGIT_BITMAPS_W - 1, upper_y));
  while (true) {
    blink_lines(underline, upperline, TIME_SUBMENUS_INPUT_TIMEOUT / 2);
    joystick_update(&(drivers->joystick));
    uint8_t direction = joystick_get_direction(&(drivers->joystick));
    switch (direction) {
    case N:
      haptic_auto_pulse();
      digits->second_units++;
      if (digits->second_units > 9)
        digits->second_units = 0;
      at_change_callback(digits);
      break;
    case S:
      haptic_auto_pulse();
      digits->second_units--;
      if (digits->second_units < 0)
        digits->second_units = 9;
      at_change_callback(digits);
      break;
    case E:
      haptic_auto_pulse();
      break;
    case W:
      haptic_auto_pulse();
      return;
    default:
      break;
    }
    time_digits_show(digits, 28, 23, 2);
    ssd1306_show(&(drivers->ssd1306));
  }
}

char *from_dotw_to_weekday(int8_t dotw) {
  switch (dotw) {
  case 0:
    return "Sun";
  case 1:
    return "Mon";
  case 2:
    return "Tue";
  case 3:
    return "Wed";
  case 4:
    return "Thu";
  case 5:
    return "Fri";
  case 6:
    return "Sat";
  default:
    return "Err";
  }
}

void time_digits_to_str(time_digits *time, char *buf, size_t buf_size) {
  snprintf(buf,
      buf_size,
      "%d%d_%d%d_%d%d",
      time->hour_tens,
      time->hour_units,
      time->minute_tens,
      time->minute_units,
      time->second_tens,
      time->second_units);
}

void time_digits_from_str(time_digits *time, char *buf) {
  str_list *slices = string_split(buf, '_');
  time->hour_tens = (int8_t)((str_list_get(slices, 0)[0]) - '0');
  time->hour_units = (int8_t)((str_list_get(slices, 0)[1]) - '0');
  time->minute_tens = (int8_t)((str_list_get(slices, 1)[0]) - '0');
  time->minute_units = (int8_t)((str_list_get(slices, 1)[1]) - '0');
  time->second_tens = (int8_t)((str_list_get(slices, 2)[0]) - '0');
  time->second_units = (int8_t)((str_list_get(slices, 2)[1]) - '0');
  str_list_free(slices);
}