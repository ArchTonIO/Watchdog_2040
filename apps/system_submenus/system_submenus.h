// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#ifndef SYSTEM_SUBMENUS_H
#define SYSTEM_SUBMENUS_H

#include <stdbool.h>

void display_system_info(bool serial_output);
void display_system_info_wrapped();
void reset_system();
void display_battery_status();
void display_joystick_check();
void display_tutorial_page();

#endif