// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#ifndef HOME_PAGE_H
#define HOME_PAGE_H

#include <stdbool.h>
#include <stdint.h>

#include "core/data_structures/string_list.h"
#include "core/graphics/graphic_primitives.h"

typedef struct {
  const uint8_t *hour_tens_bitmap;
  const uint8_t *hour_units_bitmap;
  const uint8_t *minute_tens_bitmap;
  const uint8_t *minute_units_bitmap;
  const uint8_t *second_tens_bitmap;
  const uint8_t *second_units_bitmap;
} clock_bitmaps;

typedef struct {
  char *timedate;
  uint8_t battery_level;
  bool sd_status;
  bool sx1278_status;
  bool en160_status;
  bool alarm_set;
  clock_bitmaps *clock_bmp;
  uint8_t alarm_time;
  uint8_t aqi;
  uint8_t bpm;
  uint8_t spo2;
  uint16_t notifications;
  layout *ly;
} home_page;

home_page *home_page_init();
void check_pheripherals();
void process_system_state();
void display_home_page();

#endif
