// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "apps/time_submenus/set_time_submenu.h"

#include <pico/time.h>
#include <stdbool.h>
#include <stdlib.h>

#include "apps/time_submenus/time_utils.h"
#include "core/components/include/hw_manager.h"
#include "core/data_structures/include/string_list.h"
#include "core/graphics/include/bitmaps.h"
#include "core/hardware_drivers/include/ssd1306.h"

void save_time(time_digits *digits);
void set_time(time_digits *digits);

void enter_set_time_submenu() {
  sleep_ms(TIME_SUBMENUS_INPUT_TIMEOUT * 2);
  ssd1306_clear(&(drivers->ssd1306));
  time_digits *digits = time_digits_init();
  draw_symbols(set_timedate_incr,
      set_timedate_decr,
      set_timedate_leftmost,
      set_timedate_rigthmost);
  set_hours_tens(digits, save_time);
  save_time(digits);
  free(digits);
}

void save_time(time_digits *digits) {
  rtc_time_set_time(&(drivers->internal_rtc),
      drivers->internal_rtc.internal_datetime.year,
      drivers->internal_rtc.internal_datetime.month,
      drivers->internal_rtc.internal_datetime.day,
      drivers->internal_rtc.internal_datetime.dotw,
      digits->hour_tens * 10 + digits->hour_units,
      digits->minute_tens * 10 + digits->minute_units,
      digits->second_tens * 10 + digits->second_units);
}
