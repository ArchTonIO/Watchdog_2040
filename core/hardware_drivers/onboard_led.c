// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include "onboard_led.h"

#include <stdbool.h>

#include "pico/time.h"

#include "hardware/gpio.h"

static pin _led_pin;
static bool should_blink;
static bool last_state;

static uint32_t time_now;
static uint32_t time_from_last_toggle = 0;
static uint32_t last_toggle_time = 0;

/** @brief Initialize the onboard LED.
 * @param led_pin The GPIO pin connected to the LED.
 */
void onboard_led_init(pin led_pin) {
  _led_pin = led_pin;
  should_blink = false;
  last_state = false;
  gpio_init(_led_pin);
  gpio_set_dir(_led_pin, GPIO_OUT);
  gpio_put(_led_pin, false);
}

/** @brief Turn on the onboard LED. */
inline void onboard_led_on() {
  gpio_put(_led_pin, true);
  last_state = true;
}

/** @brief Turn off the onboard LED. */
inline void onboard_led_off() {
  gpio_put(_led_pin, false);
  last_state = false;
}

/** @brief Toggle the onboard LED. */
void onboard_led_toggle() {
  if (last_state)
    onboard_led_off();
  else
    onboard_led_on();
}

/** @brief Enable blinking for the onboard LED. */
inline void onboard_led_enable_blinking() { should_blink = true; }

/** @brief Disable blinking for the onboard LED. */
inline void onboard_led_disable_blinking() {
  should_blink = false;
  onboard_led_off();
}

/** @brief Process the blinking of the onboard LED. Should be called
 * periodically. */
void process_blinking() {
  if (!should_blink)
    return;
  time_now = to_ms_since_boot(get_absolute_time());
  time_from_last_toggle = time_now - last_toggle_time;
  if (time_from_last_toggle >= BLINK_MIN_INTERVAL_MS) {
    last_toggle_time = time_now;
    onboard_led_toggle();
  }
}