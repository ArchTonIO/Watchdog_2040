// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "apps/time_submenus/set_alarm_submenu.h"

#include <pico/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "apps/text_editor/text_editor.h"
#include "apps/time_submenus/time_utils.h"
#include "core/components/hw_manager.h"
#include "core/data_structures/string_list.h"
#include "core/graphics/bitmaps.h"
#include "core/hardware_drivers/haptics.h"
#include "core/hardware_drivers/joystick.h"
#include "core/hardware_drivers/rtc_time.h"
#include "core/hardware_drivers/ssd1306.h"
#include "core/utils/utils.h"

static char *alarm_message;

void alarm_callback();

void enter_set_alarm_submenu() {
  sleep_ms(TIME_SUBMENUS_INPUT_TIMEOUT * 2);
  ssd1306_clear(drivers->oled_screen);
  if (drivers->rtc->alarm_set) {
    print_usr_error("Alarm already set !\n\n"
                    "If you want to change\n"
                    "The alarm time or\n"
                    "message,\n"
                    "please unset it first");
    return;
  }
  time_digits *digits = time_digits_init();
  draw_symbols(set_timedate_incr,
      set_timedate_decr,
      set_timedate_leftmost,
      set_timedate_rigthmost);
  set_hours_tens(digits, NULL);
  text_editor *editor = text_editor_launch(
      "# Write the message to display when the alarm goes off\n",
      true);
  alarm_message = text_editor_get_buf(editor);
  text_editor_kill(editor);
  int8_t alarm_hour = digits->hour_tens * 10 + digits->hour_units;
  int8_t alarm_minute = digits->minute_tens * 10 + digits->minute_units;
  int8_t alarm_second = digits->second_tens * 10 + digits->second_units;
  rtc_time_add_alarm(drivers->rtc,
      alarm_hour,
      alarm_minute,
      alarm_second,
      alarm_callback);
  free(digits);
}

void alarm_callback() { drivers->rtc->alarm_triggered = true; }

void process_alarm() {
  if (!drivers->rtc->alarm_triggered)
    return;
  if (!ssd1306_was_mutex_support_enabled(drivers->oled_screen))
    return;
  ssd1306_get_mutex(drivers->oled_screen);
  ssd1306_clear(drivers->oled_screen);
  ssd1306_draw_bitmap(drivers->oled_screen,
      0,
      (SSD1306_HEIGHT - 28) / 2,
      malloc_with_both_eyes_saying_hi,
      26,
      28,
      false);
  ssd1306_print(drivers->oled_screen, alarm_message, 0, 0, false);
  ssd1306_print(drivers->oled_screen, "E' tempo !", 4, 5, false);
  ssd1306_show(drivers->oled_screen);
  joystick_update(drivers->joystick);
  haptics_switch_performing_core();
  while (joystick_get_direction(drivers->joystick) == C) {
    joystick_update(drivers->joystick);
    haptic_auto_pulse();
  }
  haptics_switch_performing_core();
  ssd1306_clear(drivers->oled_screen);
  ssd1306_show(drivers->oled_screen);
  ssd1306_release_mutex(drivers->oled_screen);
  drivers->rtc->alarm_triggered = false;
}

void unset_alarm() {
  rtc_time_remove_alarm(drivers->rtc);
  print_info("Alarm was disabled");
}