#include "graphics/graphic_primitives.h"

#include <stdbool.h>
#include <stdint.h>

#include "components/hw_manager.h"
#include "hardware_drivers/ssd1306.h"

point create_point(uint8_t x, uint8_t y) {
  point p;
  p.x = x;
  p.y = y;
  return p;
}

void draw_point(point p) {
  ssd1306_draw_pixel(drivers->oled_screen, p.x, p.y, 1);
}

void clear_point(point p) {
  ssd1306_draw_pixel(drivers->oled_screen, p.x, p.y, 0);
}

line create_line(point start, point end) {
  line l;
  l.start = start;
  l.end = end;
  return l;
}

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

void clear_line(line l) {
  ssd1306_draw_pixel(drivers->oled_screen, l.start.x, l.start.y, 0);
  ssd1306_draw_pixel(drivers->oled_screen, l.end.x, l.end.y, 0);
}

circle create_circle(point center, uint8_t radius) {
  circle c;
  c.center = center;
  c.radius = radius;
  return c;
}

void draw_circle(circle c) {
  int16_t x = c.radius;
  int16_t y = 0;
  int16_t err = 0;
  while (x >= y) {
    ssd1306_draw_pixel(drivers->oled_screen, c.center.x + x, c.center.y + y, 1);
    ssd1306_draw_pixel(drivers->oled_screen, c.center.x + y, c.center.y + x, 1);
    ssd1306_draw_pixel(drivers->oled_screen, c.center.x - y, c.center.y + x, 1);
    ssd1306_draw_pixel(drivers->oled_screen, c.center.x - x, c.center.y + y, 1);
    ssd1306_draw_pixel(drivers->oled_screen, c.center.x - x, c.center.y - y, 1);
    ssd1306_draw_pixel(drivers->oled_screen, c.center.x - y, c.center.y - x, 1);
    ssd1306_draw_pixel(drivers->oled_screen, c.center.x + y, c.center.y - x, 1);
    ssd1306_draw_pixel(drivers->oled_screen, c.center.x + x, c.center.y - y, 1);
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

void clear_circle(circle c) {
  int16_t x = c.radius;
  int16_t y = 0;
  int16_t err = 0;
  while (x >= y) {
    ssd1306_draw_pixel(drivers->oled_screen, c.center.x + x, c.center.y + y, 0);
    ssd1306_draw_pixel(drivers->oled_screen, c.center.x + y, c.center.y + x, 0);
    ssd1306_draw_pixel(drivers->oled_screen, c.center.x - y, c.center.y + x, 0);
    ssd1306_draw_pixel(drivers->oled_screen, c.center.x - x, c.center.y + y, 0);
    ssd1306_draw_pixel(drivers->oled_screen, c.center.x - x, c.center.y - y, 0);
    ssd1306_draw_pixel(drivers->oled_screen, c.center.x - y, c.center.y - x, 0);
    ssd1306_draw_pixel(drivers->oled_screen, c.center.x + y, c.center.y - x, 0);
    ssd1306_draw_pixel(drivers->oled_screen, c.center.x + x, c.center.y - y, 0);
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

rectangle create_rectangle(point top_left, point bottom_right, uint8_t radius) {
  rectangle r;
  r.top_left = top_left;
  r.bottom_right = bottom_right;
  r.radius = radius;
  return r;
}

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

polyline create_polyline(point *points) {
  polyline pl;
  pl.points = points;
  return pl;
}

void draw_polyline(polyline pl) {
  for (int i = 0; pl.points[i + 1].x != 0 || pl.points[i + 1].y != 0; i++) {
    line l = create_line(pl.points[i], pl.points[i + 1]);
    draw_line(l);
  }
}

void clear_polyline(polyline pl) {
  for (int i = 0; pl.points[i + 1].x != 0 || pl.points[i + 1].y != 0; i++) {
    line l = create_line(pl.points[i], pl.points[i + 1]);
    clear_line(l);
  }
}
