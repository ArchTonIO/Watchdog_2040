// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "apps/time_submenus/set_date_submenu.h"

#include <pico/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "apps/time_submenus/time_utils.h"
#include "core/components/hw_manager.h"
#include "core/data_structures/string_list.h"
#include "core/graphics/bitmaps.h"
#include "core/graphics/graphic_primitives.h"
#include "core/hardware_drivers/haptics.h"
#include "core/hardware_drivers/joystick.h"
#include "core/hardware_drivers/ssd1306.h"

void set_weekday(date *d);
void save_date(date *d);
void set_day(date *d);
void set_month(date *d);
void set_year(date *d);

date *date_init(int8_t dotw, int8_t day, int8_t month, int16_t year) {
  date *new_date = malloc(sizeof(date));
  new_date->dotw = dotw;
  new_date->day = day;
  new_date->month = month;
  new_date->year = year;
  new_date->week_day = from_dotw_to_weekday(dotw);
  return new_date;
}

void enter_set_date_submenu() {
  sleep_ms(TIME_SUBMENUS_INPUT_TIMEOUT * 2);
  ssd1306_clear(&(drivers->ssd1306));
  date *actual_date = date_init(drivers->rtc->internal_datetime.dotw,
      drivers->rtc->internal_datetime.day,
      drivers->rtc->internal_datetime.month,
      drivers->rtc->internal_datetime.year);
  draw_symbols(set_timedate_incr,
      set_timedate_decr,
      set_timedate_leftmost,
      set_timedate_rigthmost);
  set_weekday(actual_date);
  save_date(actual_date);
  free(actual_date);
}

void show_set_date(date *d) {
  uint8_t start_pix_w = 28;
  uint8_t start_pix_h = 28;
  char *week_day = from_dotw_to_weekday(d->dotw);
  char *date_str = malloc(15 * sizeof(char));
  snprintf(date_str,
      13,
      "%s %02d/%02d/%02d",
      week_day,
      d->day,
      d->month,
      d->year - 2000);
  ssd1306_print(&(drivers->ssd1306),
      date_str,
      (start_pix_w / 6) - 1,
      (start_pix_h / 8) + 1,
      false);
  free(date_str);
  ssd1306_show(&(drivers->ssd1306));
}

void save_date(date *d) {
  drivers->rtc->internal_datetime.dotw = d->dotw;
  drivers->rtc->internal_datetime.day = d->day;
  drivers->rtc->internal_datetime.month = d->month;
  drivers->rtc->internal_datetime.year = d->year;
  rtc_time_set_time(drivers->rtc,
      d->year,
      d->month,
      d->day,
      d->dotw,
      drivers->rtc->internal_datetime.hour,
      drivers->rtc->internal_datetime.min,
      drivers->rtc->internal_datetime.sec);
  ssd1306_clear(&(drivers->ssd1306));
}

void set_weekday(date *d) {
  uint8_t start_x = 25;
  uint8_t lower_y = 42;
  uint8_t upper_y = 28;
  line underline = create_line(create_point(start_x, lower_y),
      create_point(start_x + (CHAR_WIDTH - 2) * 3, lower_y));
  line upperline = create_line(create_point(start_x, upper_y),
      create_point(start_x + (CHAR_WIDTH - 2) * 3, upper_y));
  while (true) {
    blink_lines(underline, upperline, TIME_SUBMENUS_INPUT_TIMEOUT / 2);
    joystick_update(&(drivers->joystick));
    uint8_t direction = joystick_get_direction(&(drivers->joystick));
    switch (direction) {
    case N:
      haptic_auto_pulse();
      d->dotw++;
      if (d->dotw > 6)
        d->dotw = 0;
      break;
    case S:
      haptic_auto_pulse();
      d->dotw--;
      if (d->dotw < 0)
        d->dotw = 6;
      break;
    case E:
      haptic_auto_pulse();
      set_day(d);
      break;
    case W:
      haptic_auto_pulse();
      return;
    default:
      break;
    }
    show_set_date(d);
  }
}

void set_day(date *d) {
  uint8_t start_x = 25 + (CHAR_WIDTH - 2) * 4;
  uint8_t lower_y = 42;
  uint8_t upper_y = 28;
  line underline = create_line(create_point(start_x, lower_y),
      create_point(start_x + (CHAR_WIDTH - 2) * 2 - 2, lower_y));
  line upperline = create_line(create_point(start_x, upper_y),
      create_point(start_x + (CHAR_WIDTH - 2) * 2 - 2, upper_y));
  while (true) {
    blink_lines(underline, upperline, TIME_SUBMENUS_INPUT_TIMEOUT / 2);
    joystick_update(&(drivers->joystick));
    uint8_t direction = joystick_get_direction(&(drivers->joystick));
    switch (direction) {
    case N:
      haptic_auto_pulse();
      d->day++;
      if (d->day > 31)
        d->day = 1;
      break;
    case S:
      haptic_auto_pulse();
      d->day--;
      if (d->day < 1)
        d->day = 31;
      break;
    case E:
      haptic_auto_pulse();
      set_month(d);
      break;
    case W:
      haptic_auto_pulse();
      return;
    default:
      break;
    }
    show_set_date(d);
  }
}

void set_month(date *d) {
  uint8_t start_x = 25 + (CHAR_WIDTH - 2) * 4 + (CHAR_WIDTH - 2) * 3;
  uint8_t lower_y = 42;
  uint8_t upper_y = 28;
  line underline = create_line(create_point(start_x, lower_y),
      create_point(start_x + (CHAR_WIDTH - 2) * 2 - 2, lower_y));
  line upperline = create_line(create_point(start_x, upper_y),
      create_point(start_x + (CHAR_WIDTH - 2) * 2 - 2, upper_y));
  while (true) {
    blink_lines(underline, upperline, TIME_SUBMENUS_INPUT_TIMEOUT / 2);
    joystick_update(&(drivers->joystick));
    uint8_t direction = joystick_get_direction(&(drivers->joystick));
    switch (direction) {
    case N:
      haptic_auto_pulse();
      d->month++;
      if (d->month > 12)
        d->month = 1;
      break;
    case S:
      haptic_auto_pulse();
      d->month--;
      if (d->month < 1)
        d->month = 12;
      break;
    case E:
      haptic_auto_pulse();
      set_year(d);
      break;
    case W:
      haptic_auto_pulse();
      return;
    default:
      break;
    }
    show_set_date(d);
  }
}

void set_year(date *d) {
  uint8_t start_x = 25 + (CHAR_WIDTH - 2) * 4 + (CHAR_WIDTH - 2) * 3 +
                    (CHAR_WIDTH - 2) * 3;
  uint8_t lower_y = 42;
  uint8_t upper_y = 28;
  line underline = create_line(create_point(start_x, lower_y),
      create_point(start_x + (CHAR_WIDTH - 2) * 2 - 2, lower_y));
  line upperline = create_line(create_point(start_x, upper_y),
      create_point(start_x + (CHAR_WIDTH - 2) * 2 - 2, upper_y));
  while (true) {
    blink_lines(underline, upperline, TIME_SUBMENUS_INPUT_TIMEOUT / 2);
    joystick_update(&(drivers->joystick));
    uint8_t direction = joystick_get_direction(&(drivers->joystick));
    switch (direction) {
    case N:
      haptic_auto_pulse();
      d->year++;
      break;
    case S:
      haptic_auto_pulse();
      d->year--;
      break;
    case E:
      haptic_auto_pulse();
      return;
    case W:
      haptic_auto_pulse();
      return;
    default:
      break;
    }
    show_set_date(d);
  }
}
