// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef BITMAPS_H
#define BITMAPS_H

#include <stdint.h>

// system bitmaps
extern const uint8_t no_battery[];
extern const uint8_t battery_low[];
extern const uint8_t charging[];
extern const uint8_t charging_complete[];
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
extern const uint8_t rxcontinuous_disabled[];
extern const uint8_t rxcontinuous_enabled[];
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
extern const uint8_t NO_ICON[8];
#define TOP_BAR_BITMAPS_H 16
#define TOP_BAR_BITMAPS_W 21
#define CLOCK_DIGIT_BITMAPS_H 17
#define CLOCK_DIGIT_BITMAPS_W 8
#define CLOCK_DOTS_BITMAPS_H 17
#define CLOCK_DOTS_BITMAPS_W 4

// unused
extern const uint8_t pong_icon[8];
extern const uint8_t malloc_icon[8];
extern const uint8_t haptic_icon[];
extern const uint8_t haptic_disabled[];

#endif