// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include "core/graphics/graphic_primitives.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "core/components/hw_manager.h"
#include "core/hardware_drivers/ssd1306.h"

/**
 * @brief Create a point with given x and y coordinates.
 * @param x The x coordinate of the point.
 * @param y The y coordinate of the
 * point.
 * @return A point structure with the specified coordinates.
 */
point create_point(uint8_t x, uint8_t y) {
  point p;
  p.x = x;
  p.y = y;
  return p;
}

/**
 * @brief Draws a point to the OLED screen memory.
 * @param p The point to draw.
 */
void draw_point(point p) {
  ssd1306_draw_pixel(drivers->oled_screen, p.x, p.y, 1);
}

/**
 * @brief Clears a point from the OLED screen memory.
 * @param p The point to clear.
 */
void clear_point(point p) {
  ssd1306_draw_pixel(drivers->oled_screen, p.x, p.y, 0);
}

/**
 * @brief Creates a line from two points.
 * @param start The starting point of the line.
 * @param end The ending point of the line.
 * @return A line structure representing the line between the two points.
 */
line create_line(point start, point end) {
  line l;
  l.start = start;
  l.end = end;
  return l;
}

/**
 * @brief Draws a line to the OLED screen memory.
 * @param l The line to draw.
 */
void draw_line(line l) {
  int16_t dx = l.end.x - l.start.x;
  int16_t dy = l.end.y - l.start.y;
  int16_t steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
  float x_inc = (float)dx / steps;
  float y_inc = (float)dy / steps;
  float x = l.start.x;
  float y = l.start.y;
  for (int i = 0; i <= steps; i++) {
    ssd1306_draw_pixel(drivers->oled_screen, (int)x, (int)y, 1);
    x += x_inc;
    y += y_inc;
  }
}

/**
 * @brief Clears a line from the OLED screen memory.
 * @param l The line to clear.
 */
void clear_line(line l) {
  for (int x = l.start.x; x <= l.end.x; x++) {
    ssd1306_draw_pixel(drivers->oled_screen, x, l.start.y, 0);
    ssd1306_draw_pixel(drivers->oled_screen, x, l.end.y, 0);
  }
  for (int y = l.start.y; y <= l.end.y; y++) {
    ssd1306_draw_pixel(drivers->oled_screen, l.start.x, y, 0);
    ssd1306_draw_pixel(drivers->oled_screen, l.end.x, y, 0);
  }
}

/**
 * @brief Creates a circle with a center point and radius.
 * @param center The center point of the circle.
 * @param radius The radius of the circle.
 * @return A circle structure representing the circle.
 */
circle create_circle(point center, uint8_t radius) {
  circle c;
  c.center = center;
  c.radius = radius;
  return c;
}

/**
 * @brief Draws a circle to the OLED screen memory.
 * @param c The circle to draw.
 */
void draw_circle(circle c) {
  int16_t x = c.radius;
  int16_t y = 0;
  int16_t err = 0;
  while (x >= y) {
    ssd1306_draw_pixel(drivers->oled_screen,
        c.center.x + x,
        c.center.y + y,
        1);
    ssd1306_draw_pixel(drivers->oled_screen,
        c.center.x + y,
        c.center.y + x,
        1);
    ssd1306_draw_pixel(drivers->oled_screen,
        c.center.x - y,
        c.center.y + x,
        1);
    ssd1306_draw_pixel(drivers->oled_screen,
        c.center.x - x,
        c.center.y + y,
        1);
    ssd1306_draw_pixel(drivers->oled_screen,
        c.center.x - x,
        c.center.y - y,
        1);
    ssd1306_draw_pixel(drivers->oled_screen,
        c.center.x - y,
        c.center.y - x,
        1);
    ssd1306_draw_pixel(drivers->oled_screen,
        c.center.x + y,
        c.center.y - x,
        1);
    ssd1306_draw_pixel(drivers->oled_screen,
        c.center.x + x,
        c.center.y - y,
        1);
    if (err <= 0) {
      y++;
      err += 2 * y + 1;
    }
    if (err > 0) {
      x--;
      err -= 2 * x + 1;
    }
  }
}

/**
 * @brief Clears a circle from the OLED screen memory.
 * @param c The circle to clear.
 */
void clear_circle(circle c) {
  int16_t x = c.radius;
  int16_t y = 0;
  int16_t err = 0;
  while (x >= y) {
    ssd1306_draw_pixel(drivers->oled_screen,
        c.center.x + x,
        c.center.y + y,
        0);
    ssd1306_draw_pixel(drivers->oled_screen,
        c.center.x + y,
        c.center.y + x,
        0);
    ssd1306_draw_pixel(drivers->oled_screen,
        c.center.x - y,
        c.center.y + x,
        0);
    ssd1306_draw_pixel(drivers->oled_screen,
        c.center.x - x,
        c.center.y + y,
        0);
    ssd1306_draw_pixel(drivers->oled_screen,
        c.center.x - x,
        c.center.y - y,
        0);
    ssd1306_draw_pixel(drivers->oled_screen,
        c.center.x - y,
        c.center.y - x,
        0);
    ssd1306_draw_pixel(drivers->oled_screen,
        c.center.x + y,
        c.center.y - x,
        0);
    ssd1306_draw_pixel(drivers->oled_screen,
        c.center.x + x,
        c.center.y - y,
        0);
    if (err <= 0) {
      y++;
      err += 2 * y + 1;
    }
    if (err > 0) {
      x--;
      err -= 2 * x + 1;
    }
  }
}

/**
 * @brief Creates a rectangle with top-left and bottom-right points and a
 * radius.
 * @param top_left The top-left point of the rectangle.
 * @param bottom_right The bottom-right point of the rectangle.
 * @param radius The radius of the corners.
 * @return A rectangle structure representing the rectangle.
 */
rectangle
create_rectangle(point top_left, point bottom_right, uint8_t radius) {
  rectangle r;
  r.top_left = top_left;
  r.bottom_right = bottom_right;
  r.radius = radius;
  return r;
}

/**
 * @brief Draws a rectangle to the OLED screen memory.
 * @param r The rectangle to draw.
 */
void draw_rectangle(rectangle r) {
  for (uint8_t x = r.top_left.x; x <= r.bottom_right.x; x++) {
    ssd1306_draw_pixel(drivers->oled_screen, x, r.top_left.y, 1);
    ssd1306_draw_pixel(drivers->oled_screen, x, r.bottom_right.y, 1);
  }
  for (uint8_t y = r.top_left.y; y <= r.bottom_right.y; y++) {
    ssd1306_draw_pixel(drivers->oled_screen, r.top_left.x, y, 1);
    ssd1306_draw_pixel(drivers->oled_screen, r.bottom_right.x, y, 1);
  }
}

/**
 * @brief Clears a rectangle from the OLED screen memory.
 * @param r The rectangle to clear.
 */
void clear_rectangle(rectangle r) {
  for (uint8_t x = r.top_left.x; x <= r.bottom_right.x; x++) {
    ssd1306_draw_pixel(drivers->oled_screen, x, r.top_left.y, 0);
    ssd1306_draw_pixel(drivers->oled_screen, x, r.bottom_right.y, 0);
  }
  for (uint8_t y = r.top_left.y; y <= r.bottom_right.y; y++) {
    ssd1306_draw_pixel(drivers->oled_screen, r.top_left.x, y, 0);
    ssd1306_draw_pixel(drivers->oled_screen, r.bottom_right.x, y, 0);
  }
}

/**
 * @brief Creates a polyline from an array of points.
 * @param points An array of points representing the polyline.
 * @return A polyline structure representing the polyline.
 */
polyline create_polyline(point *points) {
  polyline pl;
  pl.points = points;
  return pl;
}

/**
 * @brief Draws a polyline to the OLED screen memory.
 * @param pl The polyline to draw.
 */
void draw_polyline(polyline pl) {
  for (int i = 0; pl.points[i + 1].x != 0 || pl.points[i + 1].y != 0; i++) {
    line l = create_line(pl.points[i], pl.points[i + 1]);
    draw_line(l);
  }
}

/**
 * @brief Clears a polyline from the OLED screen memory.
 * @param pl The polyline to clear.
 */
void clear_polyline(polyline pl) {
  for (int i = 0; pl.points[i + 1].x != 0 || pl.points[i + 1].y != 0; i++) {
    line l = create_line(pl.points[i], pl.points[i + 1]);
    clear_line(l);
  }
}

/**
 * @brief Initializes a layout structure.
 * @return A pointer to the initialized layout structure.
 */
layout *layout_init() {
  layout *ly = malloc(sizeof(layout));
  ly->lines_count = 0;
  ly->circles_count = 0;
  ly->rectangles_count = 0;
  ly->polylines_count = 0;
  ly->bitmap_defs_count = 0;
  ly->text_areas_count = 0;
  return ly;
}

/**
 * @brief Adds a line to the layout.
 * @param ly The layout to which the line will be added.
 * @param ln The line to add.
 */
void layout_add_line(layout *ly, line ln) {
  ly->lines[ly->lines_count] = ln;
  ly->lines_count++;
}

/**
 * @brief Adds a circle to the layout.
 * @param ly The layout to which the circle will be added.
 * @param c The circle to add.
 */
void layout_add_circle(layout *ly, circle c) {
  ly->circles[ly->circles_count] = c;
  ly->circles_count++;
}

/**
 * @brief Adds a rectangle to the layout.
 * @param ly The layout to which the rectangle will be added.
 * @param r The rectangle to add.
 */
void layout_add_rectangle(layout *ly, rectangle r) {
  ly->rectangles_count++;
  ly->rectangles[ly->rectangles_count] = r;
}

/**
 * @brief Adds a polyline to the layout.
 * @param ly The layout to which the polyline will be added.
 * @param pl The polyline to add.
 */
void layout_add_polyline(layout *ly, polyline pl) {
  ly->polylines[ly->polylines_count] = pl;
  ly->polylines_count++;
}

/**
 * @brief Adds a bitmap definition to the layout.
 * @param ly The layout to which the bitmap definition will be added.
 * @param bd The bitmap definition to add.
 */
void layout_add_bitmap_definition(layout *ly, bitmap_definition bd) {
  ly->bitmap_defs[ly->bitmap_defs_count] = bd;
  ly->bitmap_defs_count++;
}

/**
 * @brief Adds a text area to the layout.
 * @param ly The layout to which the text area will be added.
 * @param ta The text area to add.
 */
void layout_add_text_area(layout *ly, text_area ta) {
  ly->text_areas[ly->text_areas_count] = ta;
  ly->text_areas_count++;
}

/**
 * @brief Draws all elements in the layout to the OLED screen memory.
 * @param ly The layout to draw.
 */
void layout_draw(layout *ly) {
  for (uint8_t i = 0; i < ly->lines_count; i++) {
    draw_line(ly->lines[i]);
  }
  for (uint8_t i = 0; i < ly->circles_count; i++) {
    draw_circle(ly->circles[i]);
  }
  for (uint8_t i = 0; i < ly->rectangles_count; i++) {
    draw_rectangle(ly->rectangles[i]);
  }
  for (uint8_t i = 0; i < ly->polylines_count; i++) {
    draw_polyline(ly->polylines[i]);
  }
  for (uint8_t i = 0; i < ly->bitmap_defs_count; i++) {
    ssd1306_draw_bitmap(drivers->oled_screen,
        ly->bitmap_defs[i].posx,
        ly->bitmap_defs[i].posy,
        ly->bitmap_defs[i].bitmap,
        ly->bitmap_defs[i].width,
        ly->bitmap_defs[i].height,
        ly->bitmap_defs[i].is_inverted);
  }
  for (uint8_t i = 0; i < ly->text_areas_count; i++) {
    ssd1306_print(drivers->oled_screen,
        ly->text_areas[i].text,
        ly->text_areas[i].posx,
        ly->text_areas[i].posy,
        ly->text_areas[i].is_inverted);
  }
}

/**
 * @brief Clears all elements in the layout from the OLED screen memory.
 * @param ly The layout to clear.
 */
void layout_clear(layout *ly) {
  for (uint8_t i = 0; i < ly->lines_count; i++) {
    clear_line(ly->lines[i]);
  }
  for (uint8_t i = 0; i < ly->circles_count; i++) {
    clear_circle(ly->circles[i]);
  }
  for (uint8_t i = 0; i < ly->rectangles_count; i++) {
    clear_rectangle(ly->rectangles[i]);
  }
  for (uint8_t i = 0; i < ly->polylines_count; i++) {
    clear_polyline(ly->polylines[i]);
  }
  for (uint8_t i = 0; i < ly->bitmap_defs_count; i++) {
    ssd1306_draw_bitmap(drivers->oled_screen,
        ly->bitmap_defs[i].posx,
        ly->bitmap_defs[i].posy,
        ly->bitmap_defs[i].bitmap,
        ly->bitmap_defs[i].width,
        ly->bitmap_defs[i].height,
        true);
  }
  for (uint8_t i = 0; i < ly->text_areas_count; i++) {
    ssd1306_print(drivers->oled_screen,
        ly->text_areas[i].text,
        ly->text_areas[i].posx,
        ly->text_areas[i].posy,
        true);
  }
}

/**
 * @brief Deletes all the lines in the layout.
 * @param ly The layout to delete the lines from.
 */
void layout_flush_lines(layout *ly) { ly->lines_count = 0; }

/**
 * @brief Deletes all the circles in the layout.
 * @param ly The layout to delete the circles from.
 */
void layout_flush_circles(layout *ly) { ly->circles_count = 0; }

/**
 * @brief Deletes all the rectangles in the layout.
 * @param ly The layout to delete the rectangles from.
 */
void layout_flush_rectangles(layout *ly) { ly->rectangles_count = 0; }

/**
 * @brief Deletes all the polylines in the layout.
 * @param ly The layout to delete the polylines from.
 */
void layout_flush_polylines(layout *ly) { ly->polylines_count = 0; }

/**
 * @brief Deletes all the bitmap definitions in the layout.
 * @param ly The layout to delete the bitmap definitions from.
 */
void layout_flush_bitmap_definitions(layout *ly) { ly->bitmap_defs_count = 0; }

/**
 * @brief Deletes all the text areas in the layout.
 * @param ly The layout to delete the text areas from.
 */
void layout_flush_text_areas(layout *ly) { ly->text_areas_count = 0; }

/**
 * @brief Deletes all entities from a layout.
 * @param ly The layout to flush.
 */
void layout_flush_all(layout *ly) {
  ly->lines_count = 0;
  ly->circles_count = 0;
  ly->rectangles_count = 0;
  ly->polylines_count = 0;
  ly->bitmap_defs_count = 0;
  ly->text_areas_count = 0;
}

/**
 * @brief Frees the memory allocated for a layout.
 * @param ly The layout to free.
 */
void layout_free(layout *ly) {
  if (ly != NULL) {
    free(ly);
  }
}