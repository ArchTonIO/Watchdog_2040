// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef SYSTEM_APP_H
#define SYSTEM_APP_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  bool auto_brightness_enabled;
  uint8_t brightness_level;
  bool haptics_enabled;
} system_settings_t;

void system_settings_dump(system_settings_t settings);
void system_settings_load(system_settings_t *settings);

void system_app_launch();
void display_system_info(bool serial_output);
void display_system_info_wrapped();
void reset_system();
void display_battery_status();
void display_joystick_check();
void display_reboot_screen();
void display_reboot_to_bootsel_screen();
void display_tutorial_page();

#endif