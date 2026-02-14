// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "apps/flashlight/flashlight.h"

#include "core/components/hw_manager.h"
#include "core/hardware_drivers/joystick.h"
#include "core/hardware_drivers/ssd1306.h"

void set_flashlight_on() {
  ssd1306_invert(&(drivers->ssd1306), true);
  ssd1306_clear(&(drivers->ssd1306));
  ssd1306_show(&(drivers->ssd1306));
}

void set_flashlight_off() {
  ssd1306_invert(&(drivers->ssd1306), false);
  ssd1306_clear(&(drivers->ssd1306));
  ssd1306_show(&(drivers->ssd1306));
}

void enter_flashlight_screen() {
  set_flashlight_on();
  bool flashlight_state = false;
  while (true) {
    joystick_update(drivers->joystick);
    if (joystick_get_direction(drivers->joystick) == W) {
      break;
    }
  }
  sleep_ms(200);
  set_flashlight_off();
}
