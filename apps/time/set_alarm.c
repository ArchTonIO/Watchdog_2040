// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "apps/time/include/set_alarm.h"

#include <pico/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apps/time/include/time_utils.h"
#include "core/components/include/bitmaps.h"
#include "core/components/include/hw_manager.h"
#include "core/components/include/sys_paths_manager.h"
#include "core/data_structures/include/string_list.h"
#include "core/hardware_drivers/include/haptics.h"
#include "core/hardware_drivers/include/joystick.h"
#include "core/hardware_drivers/include/rtc_time.h"
#include "core/hardware_drivers/include/ssd1306.h"
#include "core/tools/include/crud_list.h"
#include "core/utils/include/path.h"
#include "core/utils/include/utils.h"
#include "include/bitmaps.h"

void alarm_callback();
void add_alarm(crud_list *clist);
void edit_alarm(crud_list *self, const char *alarm_data);
void set_alarm_time(time_digits *initial_time);
char *flag_alarm(char *alarm_data);
void set_rtc_alarm(time_digits *time);

static void at_change_callback(time_digits *digits) {}

void load_alarms() {
  str_list *alarms = path_listdir(sys_paths->dirs->alarms_path);
  for (size_t i = 0; i < alarms->len; i++) {
    char *alarm_data = str_list_get(alarms, i);
    if (strstr(alarm_data, ALARM_OFF) != NULL)
      continue;
    str_list *slices = string_split(alarm_data, ' ');
    char *time_str = str_list_get(slices, 1);
    time_digits *time = time_digits_init();
    time_digits_from_str(time, time_str);
    set_rtc_alarm(time);
    free(time);
    str_list_free(slices);
  }
  str_list_free(alarms);
}

void set_alarm_launch() {
  crud_list alarms;
  alarms.name = "alarms";
  alarms.items_category_name = "alarm";
  alarms.workdir = sys_paths->dirs->alarms_path;
  alarms.create_as_dir = false;
  alarms.create_callback = add_alarm;
  alarms.read_update_callback = edit_alarm;
  alarms.delete_callback = delete_item_basic;
  alarms.flag_callback = flag_alarm;
  alarms.flag_string = ALARM_ON;
  alarms.unflag_string = ALARM_OFF;
  launch_crud_list(&alarms);
}

void add_alarm(crud_list *clist) {
  time_digits *time = time_digits_init();
  set_alarm_time(time);
  char buf[9];
  time_digits_to_str(time, buf, 9);
  char flag_buf[16];
  snprintf(flag_buf, 16, "%s %s", ALARM_OFF, buf);
  create_or_overwrite_item(clist, flag_buf, "");
  free(time);
}

void edit_alarm(crud_list *clist, const char *alarm_data) {
  rtc_time_remove_alarm(&(drivers->internal_rtc));
  time_digits *time = time_digits_init();
  str_list *slices = string_split(alarm_data, ' ');
  char *alarm_flag = str_list_get(slices, 0);
  char *alarm_time = str_list_get(slices, 1);

  time_digits_from_str(time, alarm_time);
  set_alarm_time(time);
  char buf[15];
  time_digits_to_str(time, buf, 15);

  char flag_buf[16];
  snprintf(flag_buf, 16, "%s %s", alarm_flag, buf);
  delete_item_basic(clist, alarm_data);
  create_or_overwrite_item(clist, flag_buf, "");

  free(time);
  str_list_free(slices);
}

char *flag_alarm(char *alarm_data) {
  char *new_str;
  if (strstr(alarm_data, ALARM_OFF) != NULL) {
    if (drivers->internal_rtc.alarm_set) {
      print_usr_error("Disable an alarm to\nenable a new one!");
      ssd1306_clear(&(drivers->ssd1306));
      sleep_ms(200);
      return strdup(alarm_data);
    }
    new_str = string_substring_replace(alarm_data, ALARM_OFF, ALARM_ON);
    str_list *slices = string_split(alarm_data, ' ');
    char *alarm_flag = str_list_get(slices, 0);
    char *alarm_time = str_list_get(slices, 1);
    time_digits *time = time_digits_init();
    time_digits_from_str(time, alarm_time);
    set_rtc_alarm(time);
    str_list_free(slices);
  } else {
    new_str = string_substring_replace(alarm_data, ALARM_ON, ALARM_OFF);
    rtc_time_remove_alarm(&(drivers->internal_rtc));
  }
  return new_str;
}

/**
  @brief Let the user set the alarm with the GUI.

  @param initial_time An initialized instance of time_digits
 */
void set_alarm_time(time_digits *initial_time) {
  sleep_ms(TIME_SUBMENUS_INPUT_TIMEOUT * 2);
  ssd1306_clear(&(drivers->ssd1306));
  draw_symbols(set_timedate_incr,
      set_timedate_decr,
      set_timedate_leftmost,
      set_timedate_rigthmost);
  set_hours_tens(initial_time, at_change_callback);
}

/**
  @brief save the alarm to the rtc system

  @param time the time to set the alarm to
 */
void set_rtc_alarm(time_digits *time) {
  int8_t alarm_hour = time->hour_tens * 10 + time->hour_units;
  int8_t alarm_minute = time->minute_tens * 10 + time->minute_units;
  int8_t alarm_second = time->second_tens * 10 + time->second_units;
  rtc_time_add_alarm(&(drivers->internal_rtc),
      alarm_hour,
      alarm_minute,
      alarm_second,
      alarm_callback);
}

void alarm_callback() { drivers->internal_rtc.alarm_triggered = true; }

void process_alarm() {
  if (!drivers->internal_rtc.alarm_triggered)
    return;
  if (!ssd1306_was_mutex_support_enabled(&(drivers->ssd1306)))
    return;
  ssd1306_get_mutex(&(drivers->ssd1306));
  ssd1306_clear(&(drivers->ssd1306));
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      (SSD1306_HEIGHT - 28) / 2,
      malloc_with_both_eyes_saying_hi,
      26,
      28,
      false);
  ssd1306_print(&(drivers->ssd1306), "E' tempo !", 0, 0, false);
  ssd1306_show(&(drivers->ssd1306));
  joystick_update(&(drivers->joystick));
  haptics_switch_performing_core();
  while (joystick_get_direction(&(drivers->joystick)) == C) {
    joystick_update(&(drivers->joystick));
    haptic_auto_pulse();
  }
  haptics_switch_performing_core();
  ssd1306_clear(&(drivers->ssd1306));
  ssd1306_show(&(drivers->ssd1306));
  ssd1306_release_mutex(&(drivers->ssd1306));
  drivers->internal_rtc.alarm_triggered = false;
}
