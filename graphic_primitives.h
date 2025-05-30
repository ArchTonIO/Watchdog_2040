#ifndef GRAPHIC_PRIMITIVES_H
#define GRAPHIC_PRIMITIVES_H

#include <stdint.h>
#include <stdbool.h>
#include "hardware_drivers/ssd1306.h"
#include "hw_manager.h"

typedef struct
{
  uint8_t x;
  uint8_t y;
} point;

typedef struct
{
  point start;
  point end;
} line;

typedef struct
{
  point center;
  uint8_t radius;
} circle;

typedef struct
{
  point top_left;
  point bottom_right;
  uint8_t radius;
} rectangle;

typedef struct
{
  point *points;
} polyline;

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

#endif