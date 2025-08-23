// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#ifndef STOPWATCH_SUBMENU_H
#define STOPWATCH_SUBMENU_H

#include <stdint.h>

typedef struct {
  uint8_t millisecond_tens;
  uint8_t millisecond_units;
} milliseconds_digits;

void enter_stopwatch_submenu();

#endif