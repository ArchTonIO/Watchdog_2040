// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdbool.h>
#include <stdint.h>

#include "config.h"

#define C 0
#define N 1
#define S 2
#define E 3
#define W 4
#define NE 5
#define NW 6
#define SE 7
#define SW 8

#define N_DEG 90.0f
#define S_DEG 270.0f
#define E_DEG 0.0f
#define W_DEG 180.0f
#define NE_DEG 45.0f
#define NW_DEG 135.0f
#define SE_DEG 315.0f
#define SW_DEG 225.0f

#define ZONE_SIZE 22.5f

typedef struct {
  pin x_pin;
  pin y_pin;
  pin button_pin;
  uint8_t x_channel;
  uint8_t y_channel;
  uint16_t x_deadzone_min;
  uint16_t x_deadzone_max;
  uint16_t x_center;
  uint16_t y_deadzone_min;
  uint16_t y_deadzone_max;
  uint16_t y_center;
  uint16_t max_l;
  uint16_t x_value;
  uint16_t y_value;
  bool button_pressed;
  float sensitivity;
  int16_t axis_rotation;
  bool is_working;
} joystick_t;

typedef struct {
  float l;
  float theta_deg;
} polar_coords;

void joystick_init(joystick_t *joystick,
    pin x_pin,
    pin y_pin,
    uint8_t x_channel,
    uint8_t y_channel,
    pin button_pin,
    float sensitivity,
    int16_t axis_rotation);
uint8_t joystick_get_direction(joystick_t *joystick);
polar_coords joystick_get_polar(joystick_t *joystick);
bool joystick_check_long_press(joystick_t *joystick, uint16_t interval_ms);
void joystick_print(joystick_t *joystick);
void joystick_update(joystick_t *joystick);

#endif