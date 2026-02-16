// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef DATE_SUBMENU_H
#define DATE_SUBMENU_H

#include <stdint.h>

typedef struct {
  int8_t dotw;
  char *week_day;
  int8_t day;
  int8_t month;
  int16_t year;
} date;

void enter_set_date_submenu();

#endif