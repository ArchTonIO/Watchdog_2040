// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include "core/graphics/graphs.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/components/hw_manager.h"
#include "core/graphics/graphic_primitives.h"
#include "core/hardware_drivers/ssd1306.h"

uint8_t normalize(graph *g, int16_t val, int16_t min, int16_t max);
void shift_and_add(graph *g, uint8_t new_val);

/**
 * @brief Initializes a graph structure with the given parameters.
 * @param title The title of the graph.
 * @param height The height of the graph in pixels.
 * @param width The width of the graph in pixels.
 * @param x_offset The x offset for the graph position on the screen.
 * @param y_offset The y offset for the graph position on the screen.
 * @param min_expected_value The minimum expected value for normalization.
 * @param max_expected_value The maximum expected value for normalization.
 * @return A pointer to the initialized graph structure.
 */
graph *graph_init(char *title,
    uint8_t height,
    uint8_t width,
    uint8_t x_offset,
    uint8_t y_offset,
    uint16_t min_expected_value,
    uint16_t max_expected_value) {
  graph *g = (graph *)malloc(sizeof(graph));
  g->title = title;
  g->height = height;
  g->width = width;
  g->x_offset = x_offset;
  g->y_offset = y_offset;
  g->values_buffer = malloc(width * sizeof(uint8_t));
  g->min_expected_value = 0;
  memset(g->values_buffer, 0, width * sizeof(uint8_t));
  g->min_expected_value = min_expected_value;
  g->max_expected_value = max_expected_value;
  g->x_axis = create_line(create_point(x_offset, y_offset + height - 1),
      create_point(x_offset + width - 1, y_offset + height - 1));
  g->y_axis = create_line(create_point(x_offset, y_offset),
      create_point(x_offset, y_offset + height - 1));
  return g;
}

/**
 * @brief Pushes a new value onto the graph, normalizing it and updating the
 * buffer.
 * @param g Pointer to the graph structure.
 * @param value Pointer to the value to be pushed onto the graph.
 */
void graph_push_value(graph *g, uint16_t value) {
  uint8_t normalized = normalize(g,
      value,
      g->min_expected_value,
      g->max_expected_value);
  shift_and_add(g, normalized);
  snprintf(g->last_value_buf, sizeof(g->last_value_buf), "%d", value);
}

/**
 * @brief Updates the graph display on the OLED screen.
 * @param g Pointer to the graph structure.
 */
void graph_update(graph *g) {
  ssd1306_print(drivers->oled_screen,
      g->title,
      g->x_offset / 7,
      g->y_offset / 8 - 2,
      false);
  ssd1306_print(drivers->oled_screen,
      g->last_value_buf,
      g->x_offset / 7,
      g->y_offset / 8 - 1,
      false);
  point last_point;
  for (int i = 0; i < g->width; i++) {
    uint8_t x = g->x_offset + i;
    uint8_t y = g->y_offset + g->values_buffer[i];
    if (y == g->y_offset)
      y = g->y_offset + g->height - 1;
    if (i > 0) {
      line l = create_line(last_point, create_point(x, y));
      draw_line(l);
    }
    last_point = create_point(x, y);
  }
  draw_line(g->x_axis);
  draw_line(g->y_axis);
}

uint8_t normalize(graph *g, int16_t val, int16_t min, int16_t max) {
  if (val < min)
    val = min;
  if (val > max)
    val = max;
  return (uint8_t)((g->height - 1) -
                   ((val - min) * (g->height - 1)) / (max - min));
}

void shift_and_add(graph *g, uint8_t new_val) {
  memmove(&g->values_buffer[0], &g->values_buffer[1], g->width - 1);
  g->values_buffer[g->width - 1] = new_val;
}

void graph_free(graph *g) {
  if (g) {
    if (g->values_buffer)
      free(g->values_buffer);
    free(g);
  }
}