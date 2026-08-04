// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "apps/flashlight/include/flashlight.h"

#include "apps/flashlight/include/bitmaps.h"
#include "core/components/include/hw_manager.h"
#include "core/hardware_drivers/include/config.h"
#include "core/hardware_drivers/include/joystick.h"
#include "core/hardware_drivers/include/ssd1306.h"
#include "core/tools/include/launcher.h"
#include "hardware/gpio.h"

void scr_flashlight_launch();
void led_flashlight_launch();

DEFINE_LAUNCHER(flashlight_app_launcher,
    "Flashlight",
    {"Screen", scr_light_icon, scr_flashlight_launch},
    {"Led", led_light_icon, led_flashlight_launch}, )

void flashlight_launch() { launcher_start_tui(&flashlight_app_launcher); }

void init_flashlight() {
  gpio_init(FLASH_LED_PIN);
  gpio_set_dir(FLASH_LED_PIN, true);
}

void set_led_flashlight_on() { gpio_put(FLASH_LED_PIN, true); }

void set_led_flashlight_off() { gpio_put(FLASH_LED_PIN, false); }

void set_scr_flashlight_on() {
  ssd1306_disable_auto_brightness(&(drivers->ssd1306));
  ssd1306_set_brightness(&(drivers->ssd1306), 0xff);
  ssd1306_invert(&(drivers->ssd1306), true);
  ssd1306_clear(&(drivers->ssd1306));
  ssd1306_show(&(drivers->ssd1306));
}
void set_scr_flashlight_off() {
  ssd1306_enable_auto_brightness(&(drivers->ssd1306));
  ssd1306_invert(&(drivers->ssd1306), false);
  ssd1306_clear(&(drivers->ssd1306));
  ssd1306_show(&(drivers->ssd1306));
}

void toggle_flashlight(void (*on_callback)(void), void (*off_callback)(void)) {
  init_flashlight();
  on_callback();
  bool flashlight_state = false;
  while (true) {
    joystick_update(&(drivers->joystick));
    if (joystick_get_direction(&(drivers->joystick)) == W) {
      break;
    }
  }
  sleep_ms(200);
  off_callback();
}

inline void scr_flashlight_launch() {
  toggle_flashlight(set_scr_flashlight_on, set_scr_flashlight_off);
}

inline void led_flashlight_launch() {
  toggle_flashlight(set_led_flashlight_on, set_led_flashlight_off);
}
