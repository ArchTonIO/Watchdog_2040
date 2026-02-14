// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "rtc_time.h"

#include <stdbool.h>
#include <stdio.h>

#include "pico/time.h"
#include "pico/util/datetime.h"

#include "core/hardware_drivers/ds3231.h"
#include "hardware/rtc.h"

void internal_rtc_init(internal_rtc_t *rtc,
    ds3231_rtc_t external_rtc,
    int16_t year,
    int8_t month,
    int8_t day,
    int8_t weekday,
    int8_t hour,
    int8_t minute,
    int8_t second) {
  rtc->external_rtc = external_rtc;
  rtc->year = year;
  rtc->month = month;
  rtc->day = day;
  rtc->weekday = weekday;
  rtc->hour = hour;
  rtc->minute = minute;
  rtc->second = second;
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
  rtc->alarm_set = false;
  rtc->alarm_triggered = false;
  rtc_init();
  rtc_set_datetime(&rtc->internal_datetime);
  sleep_us(64);
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
void rtc_time_set_time(internal_rtc_t *rtc,
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
  ds3231_datetime_t external_rtc_datetime = {
      .hour = hour,
      .minutes = minute,
      .seconds = second,
      .day = day,
      .dotw = weekday,
      .month = month,
      .year = year,
  };
  ds3231_set_datetime(&external_rtc_datetime, &(rtc->external_rtc));
  rtc_set_datetime(&rtc->internal_datetime);
  sleep_us(64);
}

void rtc_time_load_time_from_external_rtc(internal_rtc_t *rtc,
    ds3231_rtc_t *external_rtc) {
  ds3231_datetime_t ds3231_time;
  ds3231_get_datetime(&ds3231_time, external_rtc);
  printf("LOADING TIME FROM EXTERNAL_RTC\n");
  printf("%d", ds3231_time.hour);
  datetime_t t = {.year = ds3231_time.year,
      .month = ds3231_time.month,
      .day = ds3231_time.day,
      .dotw = ds3231_time.dotw,
      .hour = ds3231_time.hour,
      .min = ds3231_time.minutes,
      .sec = ds3231_time.seconds};
  rtc->internal_datetime = t;
  rtc_set_datetime(&rtc->internal_datetime);
  sleep_us(64);
}

void update_time(internal_rtc_t *rtc) {
  rtc_get_datetime(&rtc->internal_datetime);
}

/**
 * @brief Gets the current RTC time as a formatted string.
 * @param rtc Pointer to the rtc_time instance.
 * @return A string representation of the current RTC time.
 */
char *rtc_time_now(internal_rtc_t *rtc) {
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
void rtc_time_add_alarm(internal_rtc_t *rtc,
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
void rtc_time_remove_alarm(internal_rtc_t *rtc) {
  rtc_disable_alarm();
  rtc->alarm_set = false;
}