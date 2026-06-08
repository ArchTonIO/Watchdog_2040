// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "include/joystick.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "pico/time.h"

#include "core/utils/include/utils.h"
#include "hardware/adc.h"
#include "include/config.h"
#include "include/haptics.h"

void auto_calibrate(joystick_t *joystick);
bool joystick_is_working(joystick_t *joystick);

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
void joystick_init(joystick_t *joystick,
    pin x_pin,
    pin y_pin,
    uint8_t x_channel,
    uint8_t y_channel,
    pin button_pin,
    float sensitivity,
    int16_t axis_rotation) {
  joystick->x_pin = x_pin;
  joystick->y_pin = y_pin;
  joystick->button_pin = button_pin;
  joystick->x_channel = x_channel;
  joystick->y_channel = y_channel;
  joystick->x_deadzone_min = JOYSTICK_DEFAULT_DEADZONE_MIN;
  joystick->x_deadzone_max = JOYSTICK_DEFAULT_DEADZONE_MAX;
  joystick->x_center = JOYSTICK_DEFAULT_CENTER;
  joystick->y_deadzone_min = JOYSTICK_DEFAULT_DEADZONE_MIN;
  joystick->y_deadzone_max = JOYSTICK_DEFAULT_DEADZONE_MAX;
  joystick->y_center = JOYSTICK_DEFAULT_CENTER;
  joystick->max_l = JOYSTICK_DEFAULT_MAX_L;
  joystick->x_value = JOYSTICK_DEFAULT_CENTER;
  joystick->y_value = JOYSTICK_DEFAULT_CENTER;
  joystick->button_pressed = false;
  joystick->sensitivity = sensitivity;
  joystick->axis_rotation = axis_rotation;
  adc_gpio_init(x_pin);
  adc_gpio_init(y_pin);
  gpio_init(button_pin);
  gpio_pull_up(button_pin);
  gpio_set_dir(button_pin, GPIO_IN);
  auto_calibrate(joystick);
  joystick->is_working = joystick_is_working(joystick);
}

/**
 * @brief Updates the joystick values.
 *
 * @param stick The joystick instance.
 */
void joystick_update(joystick_t *joystick) {
  adc_select_input(joystick->x_channel);
  joystick->x_value = adc_read();
  adc_select_input(joystick->y_channel);
  joystick->y_value = adc_read();
  joystick->button_pressed = !gpio_get(joystick->button_pin);
}

void auto_calibrate(joystick_t *joystick) {
  size_t samples = JOYSTICK_AUTO_CALIBRATION_SAMPLES;
  uint16_t x_samples[samples];
  uint16_t y_samples[samples];
  for (uint8_t i = 0; i < samples; i++) {
    joystick_update(joystick);
    x_samples[i] = joystick->x_value;
    y_samples[i] = joystick->y_value;
    sleep_ms(JOYSTICK_AUTO_CALIBRATION_INTERVAL_MS);
  }
  joystick->x_deadzone_min = array_find_min(x_samples, samples);
  joystick->x_deadzone_max = array_find_max(x_samples, samples);
  joystick->y_deadzone_min = array_find_min(y_samples, samples);
  joystick->y_deadzone_max = array_find_max(y_samples, samples);
  joystick->x_center = (uint16_t)joystick->x_deadzone_min +
                       (joystick->x_deadzone_max - joystick->x_deadzone_min) /
                           2;
  joystick->y_center = (uint16_t)joystick->y_deadzone_min +
                       (joystick->y_deadzone_max - joystick->y_deadzone_min) /
                           2;
}

/**
 * @brief Gets the polar coordinates of the joystick.
 *
 * @param stick The joystick instance.
 * @return polar_coords The polar coordinates of the joystick, in coords.l the
 * magnitude of the vector and in coords.theta_deg the angle in degrees.
 */
polar_coords joystick_get_polar(joystick_t *joystick) {
  float raw_x = (float)joystick->x_value;
  float raw_y = (float)joystick->y_value;
  float x = (raw_x >= joystick->x_deadzone_min &&
                raw_x <= joystick->x_deadzone_max)
                ? 0.0f
                : raw_x - joystick->x_center;
  float y = (raw_y >= joystick->y_deadzone_min &&
                raw_y <= joystick->y_deadzone_max)
                ? 0.0f
                : raw_y - joystick->y_center;
  polar_coords result;
  result.l = sqrtf(x * x + y * y) / joystick->max_l;
  float angle = -atan2f(y, x) * (180.0f / M_PI);
  if (angle < 0.0f)
    angle += 360.0f;
  result.theta_deg = angle + joystick->axis_rotation;
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
uint8_t joystick_get_direction(joystick_t *joystick) {
  polar_coords polar = joystick_get_polar(joystick);
  if (polar.l < joystick->sensitivity)
    return C;
  if (polar.theta_deg >= N_DEG - ZONE_SIZE &&
      polar.theta_deg < N_DEG + ZONE_SIZE)
    return N;
  if (polar.theta_deg >= S_DEG - ZONE_SIZE &&
      polar.theta_deg < S_DEG + ZONE_SIZE)
    return S;
  if (polar.theta_deg >= E_DEG - ZONE_SIZE &&
      polar.theta_deg < E_DEG + ZONE_SIZE)
    return W;
  if (polar.theta_deg >= W_DEG - ZONE_SIZE &&
      polar.theta_deg < W_DEG + ZONE_SIZE)
    return E;
  if (polar.theta_deg >= NE_DEG - ZONE_SIZE &&
      polar.theta_deg < NE_DEG + ZONE_SIZE)
    return NW;
  if (polar.theta_deg >= NW_DEG - ZONE_SIZE &&
      polar.theta_deg < NW_DEG + ZONE_SIZE)
    return NE;
  if (polar.theta_deg >= SE_DEG - ZONE_SIZE &&
      polar.theta_deg < SE_DEG + ZONE_SIZE)
    return SW;
  if (polar.theta_deg >= SW_DEG - ZONE_SIZE &&
      polar.theta_deg < SW_DEG + ZONE_SIZE)
    return SE;
  return C;
}

/**
 * @brief: Check if a long press is being performed by the user.
 *
 * @param stick The joystick instance.
 * @param interval_ms The interval to wait to newly check if the joystick is
 * pressed, in milliseconds.
 * @retval true if the long press was performed.
 * @retval false if the long press was not performed.
 */
bool joystick_check_long_press(joystick_t *joystick, uint16_t interval_ms) {
  joystick_update(joystick);
  if (joystick->button_pressed) {
    haptic_short_pulse();
    sleep_ms(interval_ms);
    joystick_update(joystick);
    if (joystick->button_pressed) {
      haptic_short_pulse();
      return true;
    }
  }
  return false;
}

/**
 * @brief Prints the joystick status.
 *
 * @param stick The joystick instance.
 */
void joystick_print(joystick_t *joystick) {
  joystick_update(joystick);
  polar_coords polar = joystick_get_polar(joystick);
  uint8_t direction = joystick_get_direction(joystick);
  printf("JOYSTICK -> X: %d  Y: %d  S: %d\n",
      joystick->x_value,
      joystick->y_value,
      joystick->button_pressed);
  printf("JOYSTICK -> L: %f  THETA: %f\n", polar.l, polar.theta_deg);
  printf("JOYSTICK -> DIRECTION: %d\n", direction);
}

bool joystick_is_working(joystick_t *joystick) {
  bool x_ok = (joystick->x_value >= joystick->x_deadzone_min &&
               joystick->x_value <= joystick->x_deadzone_max);
  bool y_ok = (joystick->y_value >= joystick->y_deadzone_min &&
               joystick->y_value <= joystick->y_deadzone_max);
  bool button_ok = (joystick->button_pressed == false);
  if (x_ok && y_ok && button_ok)
    return true;
  return false;
}