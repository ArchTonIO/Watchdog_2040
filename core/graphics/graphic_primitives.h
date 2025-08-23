// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#ifndef GRAPHIC_PRIMITIVES_H
#define GRAPHIC_PRIMITIVES_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_ENTITIES_BY_TYPE 100

typedef struct {
  uint8_t x;
  uint8_t y;
} point;

typedef struct {
  point start;
  point end;
} line;

typedef struct {
  point center;
  uint8_t radius;
} circle;

typedef struct {
  point top_left;
  point bottom_right;
  uint8_t radius;
} rectangle;

typedef struct {
  point *points;
} polyline;

typedef struct {
  uint8_t *bitmap;
  uint8_t width;
  uint8_t height;
  uint8_t posx;
  uint8_t posy;
  bool is_inverted;
} bitmap_definition;

typedef struct {
  char *text;
  uint8_t posx;
  uint8_t posy;
  bool is_inverted;
} text_area;

typedef struct {
  line lines[MAX_ENTITIES_BY_TYPE];
  circle circles[MAX_ENTITIES_BY_TYPE];
  rectangle rectangles[MAX_ENTITIES_BY_TYPE];
  polyline polylines[MAX_ENTITIES_BY_TYPE];
  bitmap_definition bitmap_defs[MAX_ENTITIES_BY_TYPE];
  text_area text_areas[MAX_ENTITIES_BY_TYPE];
  uint8_t lines_count;
  uint8_t circles_count;
  uint8_t rectanlges_count;
  uint8_t polylines_count;
  uint8_t bitmap_defs_count;
  uint8_t text_areas_count;
} layout;

point create_point(uint8_t x, uint8_t y);
void draw_point(point p);
void clear_point(point p);

line create_line(point start, point end);
void clear_line(line l);
void draw_line(line l);

circle create_circle(point center, uint8_t radius);
void clear_circle(circle c);
void draw_circle(circle c);

rectangle create_rectangle(point top_left, point bottom_right, uint8_t radius);
void clear_rectangle(rectangle r);
void draw_rectangle(rectangle r);

polyline create_polyline(point *points);
void clear_polyline(polyline pl);
void draw_polyline(polyline pl);

layout *layout_init();
void layout_add_line(layout *ly, line ln);
void layout_add_circle(layout *ly, circle c);
void layout_add_rectangle(layout *ly, rectangle r);
void layout_add_polyline(layout *ly, polyline p);
void layout_add_bitmap_definition(layout *ly, bitmap_definition bd);
void layout_add_text_area(layout *ly, text_area ta);
void layout_draw(layout *ly);
void layout_clear(layout *ly);
void layout_flush_lines(layout *ly);
void layout_flush_circles(layout *ly);
void layout_flush_rectangles(layout *ly);
void layout_flush_polylines(layout *ly);
void layout_flush_bitmap_definitions(layout *ly);
void layout_flush_text_areas(layout *ly);
void layout_flush_all(layout *ly);
void layout_free(layout *ly);

#endif