#include "tools/submenus/set_time_submenu.h"

#include <pico/time.h>
#include <stdbool.h>
#include <stdlib.h>

#include "components/hw_manager.h"
#include "data_structures/string_list.h"
#include "graphics/bitmaps.h"
#include "hardware_drivers/ssd1306.h"
#include "tools/submenus/time_utils.h"

void save_time(time_digits *digits);
void set_time(time_digits *digits);

void enter_set_time_submenu() {
  sleep_ms(TIME_SUBMENUS_INPUT_TIMEOUT * 2);
  ssd1306_clear(drivers->oled_screen);
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
  rtc_time_set_time(drivers->rtc,
      drivers->rtc->internal_datetime.year,
      drivers->rtc->internal_datetime.month,
      drivers->rtc->internal_datetime.day,
      drivers->rtc->internal_datetime.dotw,
      digits->hour_tens * 10 + digits->hour_units,
      digits->minute_tens * 10 + digits->minute_units,
      digits->second_tens * 10 + digits->second_units);
}
