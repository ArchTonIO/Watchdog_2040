// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef GRAPHS_H
#define GRAPHS_H

#include <stdint.h>

#include "core/graphics/graphic_primitives.h"

typedef struct {
  char *title;
  uint8_t *values_buffer;
  uint8_t height;
  uint8_t width;
  uint8_t x_offset;
  uint8_t y_offset;
  uint16_t min_expected_value;
  uint16_t max_expected_value;
  char last_value_buf[5];
  line x_axis;
  line y_axis;
} graph;

graph *graph_init(char *title,
    uint8_t height,
    uint8_t width,
    uint8_t x_offset,
    uint8_t y_offset,
    uint16_t min_expected_value,
    uint16_t max_expected_value);
void graph_push_value(graph *g, uint16_t value);
void graph_update(graph *g);
void graph_free(graph *g);

#endif