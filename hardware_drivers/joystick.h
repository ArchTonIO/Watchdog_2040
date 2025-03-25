#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "config.h"

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
} joystick;

typedef struct
{
  float l;
  float theta_deg;
} polar_coords;

joystick *joystick_init(pin x_pin, pin y_pin, uint8_t x_channel, uint8_t y_channel, pin button_pin);
char *joystick_get_direction(joystick *joystick);
bool joystick_read_button(joystick *joystick);
void joystick_print(joystick *joystick);
void joystick_update(joystick *joystick);

#endif