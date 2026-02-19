// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include <stdbool.h>

#include "apps/AQI/include/aqi.h"
#include "apps/AQI/include/bitmaps.h"
#include "apps/flashlight/include/bitmaps.h"
#include "apps/flashlight/include/flashlight.h"
#include "apps/messaging/include/bitmaps.h"
#include "apps/messaging/include/messaging.h"
#include "apps/notes/include/bitmaps.h"
#include "apps/notes/include/notes.h"
#include "apps/pwd_manager/include/bitmaps.h"
#include "apps/pwd_manager/include/pwd_manager.h"
#include "apps/system_submenus/include/bitmaps.h"
#include "apps/system_submenus/include/system_submenus.h"
#include "apps/terminal/include/bitmaps.h"
#include "apps/terminal/include/terminal.h"
#include "apps/time/include/bitmaps.h"
#include "apps/time/include/time.h"
#include "apps/todo/include/bitmaps.h"
#include "apps/todo/include/todo.h"
#include "core/components/include/bitmaps.h"
#include "core/data_structures/include/string_list.h"
#include "core/hardware_drivers/include/haptics.h"
#include "core/tools/include/launcher.h"

void display_ulmp_menu();
void display_time_menu();
void display_system_menu();
void display_hardware_manager_menu();
void display_sx1278_menu();
void display_ens160_menu();
void display_haptic_menu();
void display_power_save_menu();
void display_malloc_menu();
void display_notifications_menu();

DEFINE_LAUNCHER(main_launcher,
    "Apps",
    {"Time", set_time_icon, time_launch},
    {"Air quality", AQI_icon, aqi_launch},
    {"Messaging", ulmp_icon, messaging_launch},
    {"Terminal", cli_icon, terminal_launch},
    {"Notes", notes_icon, notes_launch},
    {"Todo", todo_icon, todo_launch},
    {"Password manager", password_manager_icon, password_manager_launch},
    {"Flashlight", flashlight_icon, enter_flashlight_screen},
    {"System", qfn_package_icon, display_system_menu}, )

DEFINE_LAUNCHER(system_launcher,
    "System",
    {"Reboot system", reset_icon, display_reboot_screen},
    {"System info", system_info_icon, display_system_info_wrapped},
    {"Battery status", battery_status_icon, display_battery_status},
    {"Check joystick", check_joystick_icon, display_joystick_check},
    {"System reset", reset_icon, reset_system})

DEFINE_LAUNCHER(haptic_launcher,
    "Haptic feedback",
    {"Enable haptic", haptic_icon, haptic_enable},
    {"Disable haptic", haptic_disabled, haptic_disable})

DEFINE_LAUNCHER(tutorial_launcher,
    "Tutorial",
    {"Entry 0", NO_ICON, display_tutorial_page},
    {"Entry 1", NO_ICON, display_tutorial_page},
    {"Entry 2", NO_ICON, display_tutorial_page},
    {"Entry 3", NO_ICON, display_tutorial_page},
    {"Entry 4", NO_ICON, display_tutorial_page},
    {"Entry 5", NO_ICON, display_tutorial_page},
    {"Entry 6", NO_ICON, display_tutorial_page},
    {"Entry 7", NO_ICON, display_tutorial_page})

void display_main_menu() { launcher_start(&main_launcher); }

void display_system_menu() { launcher_start(&system_launcher); }
void display_haptic_menu() { launcher_start(&haptic_launcher); }

void display_tutorial_menu() { launcher_start(&tutorial_launcher); }