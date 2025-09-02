// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include "joystick.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "pico/time.h"

#include "config.h"
#include "core/hardware_drivers/haptics.h"
#include "core/utils/utils.h"
#include "hardware/adc.h"

void auto_calibrate(joystick *stick);
bool joystick_is_working(joystick *stick);

/**
 * @brief Initializes a joystick instance.
 *
 * @param x_pin The pin where the x axis of the joystick is connected.
 * @param y_pin The pin where the y axis of the joystick is connected.
 * @param x_channel The adc channel where the x axis of the joystick is
 * connected.
 * @param y_channel The adc channel where the y axis of the joystick is
 * connected.
 * @param button_pin The pin where the button of the joystick is connected.
 * @return joystick* A pointer to the joystick instance.
 */
joystick *joystick_init(pin x_pin,
    pin y_pin,
    uint8_t x_channel,
    uint8_t y_channel,
    pin button_pin,
    float sensitivity,
    int16_t axis_rotation) {
  joystick *new_joystick = (joystick *)malloc(sizeof(joystick));
  new_joystick->x_pin = x_pin;
  new_joystick->y_pin = y_pin;
  new_joystick->button_pin = button_pin;
  new_joystick->x_channel = x_channel;
  new_joystick->y_channel = y_channel;
  new_joystick->x_deadzone_min = JOYSTICK_DEAFULT_DEADZONE_MIN;
  new_joystick->x_deadzone_max = JOYSTICK_DEFAULT_DEADZONE_MAX;
  new_joystick->x_center = JOYSTICK_DEFAULT_CENTER;
  new_joystick->y_deadzone_min = JOYSTICK_DEAFULT_DEADZONE_MIN;
  new_joystick->y_deadzone_max = JOYSTICK_DEFAULT_DEADZONE_MAX;
  new_joystick->y_center = JOYSTICK_DEFAULT_CENTER;
  new_joystick->max_l = JOYSTICK_DEFAULT_MAX_L;
  new_joystick->x_value = JOYSTICK_DEFAULT_CENTER;
  new_joystick->y_value = JOYSTICK_DEFAULT_CENTER;
  new_joystick->button_pressed = false;
  new_joystick->sensitivity = sensitivity;
  new_joystick->axis_rotation = axis_rotation;
  adc_gpio_init(x_pin);
  adc_gpio_init(y_pin);
  gpio_init(button_pin);
  gpio_pull_up(button_pin);
  gpio_set_dir(button_pin, GPIO_IN);
  auto_calibrate(new_joystick);
  new_joystick->is_working = joystick_is_working(new_joystick);
  return new_joystick;
}

/**
 * @brief Updates the joystick values.
 *
 * @param stick The joystick instance.
 */
void joystick_update(joystick *stick) {
  adc_select_input(stick->x_channel);
  stick->x_value = adc_read();
  adc_select_input(stick->y_channel);
  stick->y_value = adc_read();
  stick->button_pressed = !gpio_get(stick->button_pin);
}

void auto_calibrate(joystick *stick) {
  size_t samples = JOYSTICK_AUTO_CALIBRATION_SAMPLES;
  uint16_t x_samples[samples];
  uint16_t y_samples[samples];
  for (uint8_t i = 0; i < samples; i++) {
    joystick_update(stick);
    x_samples[i] = stick->x_value;
    y_samples[i] = stick->y_value;
    sleep_ms(JOYSTICK_AUTO_CALIBRATION_INTERVAL_MS);
  }
  stick->x_deadzone_min = array_find_min(x_samples, samples);
  stick->x_deadzone_max = array_find_max(x_samples, samples);
  stick->y_deadzone_min = array_find_min(y_samples, samples);
  stick->y_deadzone_max = array_find_max(y_samples, samples);
  stick->x_center = (uint16_t)stick->x_deadzone_min +
                    (stick->x_deadzone_max - stick->x_deadzone_min) / 2;
  stick->y_center = (uint16_t)stick->y_deadzone_min +
                    (stick->y_deadzone_max - stick->y_deadzone_min) / 2;
}

/**
 * @brief Gets the polar coordinates of the joystick.
 *
 * @param stick The joystick instance.
 * @return polar_coords The polar coordinates of the joystick, in coords.l the
 * magnitude of the vector and in coords.theta_deg the angle in degrees.
 */
polar_coords joystick_get_polar(joystick *stick) {
  float raw_x = (float)stick->x_value;
  float raw_y = (float)stick->y_value;
  float x = (raw_x >= stick->x_deadzone_min && raw_x <= stick->x_deadzone_max)
                ? 0.0f
                : raw_x - stick->x_center;
  float y = (raw_y >= stick->y_deadzone_min && raw_y <= stick->y_deadzone_max)
                ? 0.0f
                : raw_y - stick->y_center;
  polar_coords result;
  result.l = sqrtf(x * x + y * y) / stick->max_l;
  float angle = -atan2f(y, x) * (180.0f / M_PI);
  if (angle < 0.0f)
    angle += 360.0f;
  result.theta_deg = angle + stick->axis_rotation;
  if (result.theta_deg >= 360.0f)
    result.theta_deg -= 360.0f;
  if (result.theta_deg < 0.0f)
    result.theta_deg += 360.0f;
  return result;
}

/**
 * @brief Gets the direction of the joystick.
 *
 * @param stick The joystick instance.
 * @retval `C`: Center.
 * @retval `N`: North.
 * @retval `S`: South.
 * @retval `E`: East.
 * @retval `W`: West.
 * @retval `NE`: North-East.
 * @retval `NW`: North-West.
 * @retval `SE`: South-East.
 * @retval `SW`: South-West.
 */
uint8_t joystick_get_direction(joystick *stick) {
  polar_coords polar = joystick_get_polar(stick);
  if (polar.l < stick->sensitivity)
    return C;
  if (polar.theta_deg >= N_DEG - ZONE_SIZE &&
      polar.theta_deg < N_DEG + ZONE_SIZE)
    return N;
  if (polar.theta_deg >= S_DEG - ZONE_SIZE &&
      polar.theta_deg < S_DEG + ZONE_SIZE)
    return S;
  if (polar.theta_deg >= E_DEG - ZONE_SIZE &&
      polar.theta_deg < E_DEG + ZONE_SIZE)
    return E;
  if (polar.theta_deg >= W_DEG - ZONE_SIZE &&
      polar.theta_deg < W_DEG + ZONE_SIZE)
    return W;
  if (polar.theta_deg >= NE_DEG - ZONE_SIZE &&
      polar.theta_deg < NE_DEG + ZONE_SIZE)
    return NE;
  if (polar.theta_deg >= NW_DEG - ZONE_SIZE &&
      polar.theta_deg < NW_DEG + ZONE_SIZE)
    return NW;
  if (polar.theta_deg >= SE_DEG - ZONE_SIZE &&
      polar.theta_deg < SE_DEG + ZONE_SIZE)
    return SE;
  if (polar.theta_deg >= SW_DEG - ZONE_SIZE &&
      polar.theta_deg < SW_DEG + ZONE_SIZE)
    return SW;
  return C;
}

/**
 * @brief: Check if a long press is being performed by the user.
 *
 * @param stick The joystick instance.
 * @param interval_ms The interval to wait to newly check if the joystick is
 * pressed, in milliseconds.
 * @retval true if the long press was performed.
 * @retval flase if the long press was not performed.
 */
bool joystick_check_long_press(joystick *stick, uint16_t interval_ms) {
  joystick_update(stick);
  if (stick->button_pressed) {
    haptic_short_pulse();
    sleep_ms(interval_ms);
    joystick_update(stick);
    if (stick->button_pressed) {
      haptic_short_pulse();
      return true;
    }
  }
  return false;
}

/**
 * @brief Prints the joytick status.
 *
 * @param stick The joystick instance.
 */
void joystick_print(joystick *stick) {
  joystick_update(stick);
  polar_coords polar = joystick_get_polar(stick);
  uint8_t direction = joystick_get_direction(stick);
  printf("JOYSTICK -> X: %d  Y: %d  S: %d\n",
      stick->x_value,
      stick->y_value,
      stick->button_pressed);
  printf("JOYSTICK -> L: %f  THETA: %f\n", polar.l, polar.theta_deg);
  printf("JOYSTICK -> DIRECTION: %d\n", direction);
}

bool joystick_is_working(joystick *stick) {
  bool x_ok = (stick->x_value >= stick->x_deadzone_min &&
               stick->x_value <= stick->x_deadzone_max);
  bool y_ok = (stick->y_value >= stick->y_deadzone_min &&
               stick->y_value <= stick->y_deadzone_max);
  bool button_ok = (stick->button_pressed == false);
  if (x_ok && y_ok && button_ok)
    return true;
  return false;
}