#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "pico/stdlib.h"
#include "hardware/adc.h"
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

#define N_MIN 67.5f
#define N_MAX 112.5f
#define S_MIN -112.5f
#define S_MAX -67.5f
#define E_MIN -22.5f
#define E_MAX 22.5f
#define W_MIN 157.5f
#define W_MAX -157.5f
#define NE_MIN 22.5f
#define NE_MAX 67.5f
#define NW_MIN 112.5f
#define NW_MAX 157.5f
#define SE_MIN -67.5f
#define SE_MAX -22.5f
#define SW_MIN -157.5f
#define SW_MAX -112.5f

typedef struct
{
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
} joystick;

typedef struct
{
  float l;
  float theta_deg;
} polar_coords;

joystick *joystick_init(pin x_pin, pin y_pin, uint8_t x_channel, uint8_t y_channel, pin button_pin, float sensitivity, int16_t axis_rotation);
uint8_t joystick_get_direction(joystick *joystick);
bool joystick_check_long_press(joystick *joystick, uint16_t interval_ms);
void joystick_print(joystick *joystick);
void joystick_update(joystick *joystick);

#endif