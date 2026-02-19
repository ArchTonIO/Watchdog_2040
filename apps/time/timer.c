// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "apps/time/include/timer.h"

#include <pico/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "apps/time/include/time_utils.h"
#include "core/components/include/hw_manager.h"
#include "core/data_structures/include/string_list.h"
#include "core/hardware_drivers/include/haptics.h"
#include "core/hardware_drivers/include/joystick.h"
#include "core/hardware_drivers/include/ssd1306.h"
#include "include/bitmaps.h"

void set_timer() {};
bool start_countdown(time_digits *digits);

void timer_launch() {
  sleep_ms(TIME_SUBMENUS_INPUT_TIMEOUT * 2);
  ssd1306_clear(&(drivers->ssd1306));
  time_digits *digits = time_digits_init();
  digits->hour_tens = 0;
  digits->hour_units = 0;
  digits->minute_tens = 0;
  digits->minute_units = 0;
  digits->second_tens = 0;
  digits->second_units = 0;
  draw_symbols(set_timedate_incr,
      set_timedate_decr,
      set_timedate_leftmost,
      set_timedate_rigthmost);
  time_digits_show(digits, 28, 23, 2);
  set_hours_tens(digits, set_timer);
  ssd1306_clear(&(drivers->ssd1306));
  ssd1306_print(&(drivers->ssd1306), "Press to start", 3, 0, false);
  time_digits_show(digits, 28, 23, 2);
  ssd1306_show(&(drivers->ssd1306));
  while (!(drivers->joystick).button_pressed)
    joystick_update(&(drivers->joystick));
  if (!start_countdown(digits)) {
    free(digits);
    return;
  }
  ssd1306_print(&(drivers->ssd1306), " Press to stop alarm ", 0, 0, false);
  ssd1306_show(&(drivers->ssd1306));
  joystick_update(&(drivers->joystick));
  while (!(drivers->joystick).button_pressed) {
    joystick_update(&(drivers->joystick));
    haptic_auto_pulse();
  }
  free(digits);
}

bool start_countdown(time_digits *digits) {
  ssd1306_clear(&(drivers->ssd1306));
  time_digits_show(digits, 28, 23, 2);
  ssd1306_print(&(drivers->ssd1306), "Left <- to exit timer", 0, 0, false);
  ssd1306_show(&(drivers->ssd1306));
  int32_t total_seconds = (digits->hour_tens * 10 + digits->hour_units) *
                              3600 +
                          (digits->minute_tens * 10 + digits->minute_units) *
                              60 +
                          (digits->second_tens * 10 + digits->second_units);
  while (total_seconds > 0) {
    joystick_update(&(drivers->joystick));
    if (joystick_get_direction(&(drivers->joystick)) == W)
      return false;
    sleep_ms(1000);
    total_seconds--;
    digits->second_units--;
    if (digits->second_units < 0) {
      digits->second_units = 9;
      digits->second_tens--;
      if (digits->second_tens < 0) {
        digits->second_tens = 5;
        digits->minute_units--;
        if (digits->minute_units < 0) {
          digits->minute_units = 9;
          digits->minute_tens--;
          if (digits->minute_tens < 0) {
            digits->minute_tens = 5;
            digits->hour_units--;
            if (digits->hour_units < 0) {
              digits->hour_units = 9;
              digits->hour_tens--;
              if (digits->hour_tens < 0) {
                break;
              }
            }
          }
        }
      }
    }
    time_digits_show(digits, 28, 23, 2);
    ssd1306_show(&(drivers->ssd1306));
  }
  return true;
}