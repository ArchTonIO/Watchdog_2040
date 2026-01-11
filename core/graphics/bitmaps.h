// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef BITMAPS_H
#define BITMAPS_H

#include <stdint.h>

extern const uint8_t battery_empty[];
extern const uint8_t battery_1_bar[];
extern const uint8_t battery_2_bars[];
extern const uint8_t battery_3_bars[];
extern const uint8_t battery_4_bars[];
extern const uint8_t microsd_working[];
extern const uint8_t microsd_not_working[];
extern const uint8_t lora_working[];
extern const uint8_t lora_not_working[];
extern const uint8_t ens160_working[];
extern const uint8_t ens160_not_working[];
extern const uint8_t *num_msg_bitmaps[12];
extern const uint8_t alarm_enabled[];
extern const uint8_t alarm_disabled[];
extern const uint8_t *clock_digits[10];
extern const uint8_t clock_dots[];
extern const uint8_t message_received[];
extern const uint8_t message_received_open[];
extern const uint8_t malloc_the_mascot[];
extern const uint8_t *startup_animation[16];
extern const uint8_t malloc_saying_hi[];
extern const uint8_t malloc_with_glasses[];
extern const uint8_t malloc_with_pointy_eyes[];
extern const uint8_t malloc_with_both_eyes[];
extern const uint8_t malloc_with_both_eyes_saying_hi[];
extern const uint8_t easyarrow[];
extern const uint8_t stopwatch_go[];
extern const uint8_t stopwatch_stop[];
extern const uint8_t stopwatch_exit[];
extern const uint8_t stopwatch_zero[];
extern const uint8_t set_timedate_incr[];
extern const uint8_t set_timedate_decr[];
extern const uint8_t set_timedate_leftmost[];
extern const uint8_t set_timedate_rigthmost[];
extern const uint8_t clock_icon[8];
extern const uint8_t connections_icon[8];
extern const uint8_t qfn_package_icon[8];
extern const uint8_t pong_icon[8];
extern const uint8_t malloc_icon[8];
extern const uint8_t cli_icon[8];
extern const uint8_t flashlight_icon[8];
extern const uint8_t AQI_icon[8];
extern const uint8_t ens160_icon[8];
extern const uint8_t ens160_disabled_icon[8];
extern const uint8_t notes_icon[8];
extern const uint8_t contacts_icon[8];
extern const uint8_t read_messages_icon[8];
extern const uint8_t send_msg_icon[8];
extern const uint8_t scan_contacts_icon[8];
extern const uint8_t notifications_icon[8];
extern const uint8_t enable_notifications_icon[8];
extern const uint8_t disable_notifications_icon[8];
extern const uint8_t sx1278_icon[8];
extern const uint8_t sx1278_disabled_icon[8];
extern const uint8_t system_info_icon[8];
extern const uint8_t battery_status_icon[8];
extern const uint8_t check_joystick_icon[8];
extern const uint8_t reset_icon[8];
extern const uint8_t set_time_icon[8];
extern const uint8_t set_date_icon[8];
extern const uint8_t set_alarm_icon[8];
extern const uint8_t unset_alarm_icon[8];
extern const uint8_t stopwatch_icon[8];
extern const uint8_t timer_icon[8];
extern const uint8_t NO_ICON[8];

#define TOP_BAR_BITMAPS_H 16
#define TOP_BAR_BITMAPS_W 21

#define CLOCK_DIGIT_BITMAPS_H 17
#define CLOCK_DIGIT_BITMAPS_W 8
#define CLOCK_DOTS_BITMAPS_H 17
#define CLOCK_DOTS_BITMAPS_W 4
#define TIME_NAV_SYMBOLS_L 11

#endif