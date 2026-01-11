// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <stdint.h>

#include "core/graphics/graphic_primitives.h"

#define TIME_SUBMENUS_INPUT_TIMEOUT 100

typedef struct {
  int8_t hour_tens;
  int8_t hour_units;
  int8_t minute_tens;
  int8_t minute_units;
  int8_t second_tens;
  int8_t second_units;
} time_digits;

time_digits *time_digits_init();
void time_digits_show(time_digits *digits,
    uint8_t start_x,
    uint8_t start_y,
    uint8_t spacing);
void blink_lines(line underline, line upperline, uint8_t interval);
void draw_symbols(const uint8_t *top_symbol,
    const uint8_t *bottom_symbol,
    const uint8_t *left_symbol,
    const uint8_t *right_symbol);
char *from_dotw_to_weekday(int8_t dotw);
void set_hours_tens(time_digits *digits,
    void (*at_change_callback)(time_digits *digits));
void set_hours_units(time_digits *digits,
    void (*at_change_callback)(time_digits *digits));
void set_minutes_tens(time_digits *digits,
    void (*at_change_callback)(time_digits *digits));
void set_minutes_units(time_digits *digits,
    void (*at_change_callback)(time_digits *digits));
void set_seconds_tens(time_digits *digits,
    void (*at_change_callback)(time_digits *digits));
void set_seconds_units(time_digits *digits,
    void(at_change_callback)(time_digits *digits));

#endif