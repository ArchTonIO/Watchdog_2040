// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "rtc_time.h"

#include <stdbool.h>
#include <stdlib.h>

#include "pico/time.h"
#include "pico/util/datetime.h"

#include "hardware/rtc.h"

rtc_time *rtc_time_init(int16_t year,
    int8_t month,
    int8_t day,
    int8_t weekday,
    int8_t hour,
    int8_t minute,
    int8_t second) {
  rtc_time *new_rtc = (rtc_time *)malloc(sizeof(rtc_time));
  new_rtc->year = year;
  new_rtc->month = month;
  new_rtc->day = day;
  new_rtc->weekday = weekday;
  new_rtc->hour = hour;
  new_rtc->minute = minute;
  new_rtc->second = second;
  char datetime_buf[256];
  char *datetime_str = &datetime_buf[0];
  datetime_t t = {.year = year,
      .month = month,
      .day = day,
      .dotw = weekday,
      .hour = hour,
      .min = minute,
      .sec = second};
  new_rtc->internal_datetime = t;
  new_rtc->alarm_set = false;
  new_rtc->alarm_triggered = false;
  rtc_init();
  rtc_set_datetime(&new_rtc->internal_datetime);
  sleep_us(64);
  return new_rtc;
}

/**
  * @brief Sets the RTC time to the specified values.
  * @param rtc Pointer to the rtc_time instance.
  * @param year The year to set (e.g., 2025).
  * @param month The month to set (1-12).
  * @param day The day of the month to set (1-31).
  * @param weekday The day of the week to set (0-6, where
  0 = Sunday).
  * @param hour The hour to set (0-23).
  * @param minute The minute to set (0-59).
  * @param second The second to set (0-59).
 */
void rtc_time_set_time(rtc_time *rtc,
    int16_t year,
    int8_t month,
    int8_t day,
    int8_t weekday,
    int8_t hour,
    int8_t minute,
    int8_t second) {
  char datetime_buf[256];
  char *datetime_str = &datetime_buf[0];
  datetime_t t = {.year = year,
      .month = month,
      .day = day,
      .dotw = weekday,
      .hour = hour,
      .min = minute,
      .sec = second};
  rtc->internal_datetime = t;
  rtc_set_datetime(&rtc->internal_datetime);
  sleep_us(64);
}

void update_time(rtc_time *rtc) { rtc_get_datetime(&rtc->internal_datetime); }

/**
 * @brief Gets the current RTC time as a formatted string.
 * @param rtc Pointer to the rtc_time instance.
 * @return A string representation of the current RTC time.
 */
char *rtc_time_now(rtc_time *rtc) {
  update_time(rtc);
  datetime_to_str(rtc->internal_datetime_buf,
      sizeof(rtc->internal_datetime_buf),
      &rtc->internal_datetime);
  return rtc->internal_datetime_buf;
}

/**
 * @brief Adds an alarm to the RTC time.
 * @param rtc Pointer to the rtc_time instance.
 * @param hour The hour for the alarm (0-23).
 * @param minute The minute for the alarm (0-59).
 * @param second The second for the alarm (0-59).
 * @param callback The callback function to be called when the alarm triggers.
 */
void rtc_time_add_alarm(rtc_time *rtc,
    int8_t hour,
    int8_t minute,
    int8_t second,
    rtc_callback_t callback) {
  datetime_t target_time = {.year = rtc->year,
      .month = rtc->month,
      .day = rtc->day,
      .dotw = rtc->weekday,
      .hour = hour,
      .min = minute,
      .sec = second};
  rtc->alarm_set = true;
  rtc_set_alarm(&target_time, callback);
  rtc_enable_alarm();
}

/**
 * @brief Checks if the RTC alarm is triggered.
 * @param rtc Pointer to the rtc_time instance.
 * @return True if the alarm is triggered, false otherwise.
 */
void rtc_time_remove_alarm(rtc_time *rtc) {
  rtc_disable_alarm();
  rtc->alarm_set = false;
}