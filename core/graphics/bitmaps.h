// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#ifndef BITMAPS_H
#define BITMAPS_H

#include <stdint.h>

#include "core/hardware_drivers/battery.h"

extern uint8_t battery_empty[];
extern uint8_t battery_1_bar[];
extern uint8_t battery_2_bars[];
extern uint8_t battery_3_bars[];
extern uint8_t battery_4_bars[];
extern uint8_t microsd_working[];
extern uint8_t microsd_not_working[];
extern uint8_t lora_working[];
extern uint8_t lora_not_working[];
extern uint8_t ens160_working[];
extern uint8_t ens160_not_working[];
extern uint8_t *num_msg_bitmaps[12];
extern uint8_t alarm_enabled[];
extern uint8_t alarm_disabled[];
extern uint8_t *clock_digits[10];
extern uint8_t clock_dots[];
extern uint8_t message_received[];
extern uint8_t message_received_open[];
extern uint8_t malloc_the_mascot[];
extern uint8_t *startup_animation[16];
extern uint8_t malloc_saying_hi[];
extern uint8_t malloc_with_glasses[];
extern uint8_t malloc_with_pointy_eyes[];
extern uint8_t malloc_with_both_eyes[];
extern uint8_t malloc_with_both_eyes_saying_hi[];
extern uint8_t easyarrow[];
extern uint8_t stopwatch_go[];
extern uint8_t stopwatch_stop[];
extern uint8_t stopwatch_exit[];
extern uint8_t stopwatch_zero[];
extern uint8_t set_timedate_incr[];
extern uint8_t set_timedate_decr[];
extern uint8_t set_timedate_leftmost[];
extern uint8_t set_timedate_rigthmost[];

#define TOP_BAR_BITMAPS_H 16
#define TOP_BAR_BITMAPS_W 21

#define CLOCK_DIGIT_BITMAPS_H 17
#define CLOCK_DIGIT_BITMAPS_W 8
#define CLOCK_DOTS_BITMAPS_H 17
#define CLOCK_DOTS_BITMAPS_W 4
#define TIME_NAV_SYMBOLS_L 11

#endif