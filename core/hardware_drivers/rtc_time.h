// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef RTC_TIME_H
#define RTC_TIME_H

#include <stdbool.h>
#include <stdint.h>

#include "core/hardware_drivers/ds3231.h"
#include "hardware/rtc.h"

typedef struct {
  char internal_datetime_buf[256];
  char *internal_datetime_str;
  datetime_t internal_datetime;
  ds3231_rtc_t external_rtc;
  bool alarm_set;
  bool alarm_triggered;
  int16_t year;
  int8_t month;
  int8_t day;
  int8_t weekday;
  int8_t hour;
  int8_t minute;
  int8_t second;
} rtc_time;

rtc_time *rtc_time_init(ds3231_rtc_t external_rtc,
    int16_t year,
    int8_t month,
    int8_t day,
    int8_t weekday,
    int8_t hour,
    int8_t minute,
    int8_t second);
void rtc_time_set_time(rtc_time *rtc,
    int16_t year,
    int8_t month,
    int8_t day,
    int8_t weekday,
    int8_t hour,
    int8_t minute,
    int8_t second);
char *rtc_time_now(rtc_time *rtc);
void rtc_time_add_alarm(rtc_time *rtc,
    int8_t hour,
    int8_t minute,
    int8_t second,
    rtc_callback_t callback);
void rtc_time_remove_alarm(rtc_time *rtc);
void update_time(rtc_time *rtc);
void rtc_time_load_time_from_external_rtc(rtc_time *rtc,
    ds3231_rtc_t *external_rtc);

#endif