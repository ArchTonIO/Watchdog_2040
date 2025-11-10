// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#ifndef ONBOARD_LED
#define ONBOARD_LED

#include "config.h"

#define BLINK_MIN_INTERVAL_MS 500

void onboard_led_init(pin led_pin);
void onboard_led_on();
void onboard_led_off();
void onboard_leg_toggle();
void onboard_led_enable_blinking();
void onboard_led_disable_blinking();
void process_blinking();

#endif