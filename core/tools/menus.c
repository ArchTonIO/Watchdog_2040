// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include <stdbool.h>
#include <stdio.h>

#include "apps/AQI/aqi.h"
#include "apps/drawing_board/board.h"
#include "apps/flashlight/flashlight.h"
#include "apps/msg_manager/contacts.h"
#include "apps/msg_manager/msg_manager.h"
#include "apps/notes/notes.h"
#include "apps/pwd_manager/pwd_manager.h"
#include "apps/system_submenus/system_submenus.h"
#include "apps/terminal/terminal.h"
#include "apps/time_submenus/set_alarm_submenu.h"
#include "apps/time_submenus/set_date_submenu.h"
#include "apps/time_submenus/set_time_submenu.h"
#include "apps/time_submenus/stopwatch_submenu.h"
#include "apps/time_submenus/timer_submenu.h"
#include "apps/todo/todo.h"
#include "core/components/hw_manager.h"
#include "core/data_structures/string_list.h"
#include "core/graphics/bitmaps.h"
#include "core/hardware_drivers/haptics.h"
#include "core/tools/launcher.h"

void display_ulmp_menu();
void display_time_menu();
void display_system_menu();
void display_hardware_manager_menu();
void display_sx1278_menu();
void display_ens160_menu();
void display_haptic_menu();
void display_power_save_menu();
void display_games_menu() {
  printf("[MENUS] Games menu not implemented yet.\n");
}
void display_malloc_menu();

void display_notifications_menu();

void malloc_talk_with() {
  printf("[MENUS] Malloc talk with not implemented yet.\n");
}
void malloc_see_memories() {
  printf("[MENUS] Malloc see memories not implemented yet.\n");
}

DEFINE_LAUNCHER(main_launcher,
    "Apps",
    {"ULMP", ulmp_icon, display_ulmp_menu},
    {"Time", set_time_icon, display_time_menu},
    {"Air quality", AQI_icon, display_air_quality_indexes},
    {"Todo", todo_icon, todo_launch},
    {"Notes", notes_icon, notes_launch},
    {"Drawing board", drawing_board_icon, drawing_board_launch},
    {"Password manager", password_manager_icon, password_manager_launch},
    {"Flashlight", flashlight_icon, enter_flashlight_screen},
    {"Terminal", cli_icon, terminal_launch},
    {"System", qfn_package_icon, display_system_menu},
    {"Games", pong_icon, display_games_menu},
    {"Malloc", malloc_icon, display_malloc_menu})

DEFINE_LAUNCHER(ulmp_launcher,
    "ULMP",
    {"Send message", send_msg_icon, send_message},
    {"Read messages", read_messages_icon, read_messages},
    {"Contacts", contacts_icon, enter_contacts_submenu},
    {"Notifications", notifications_icon, display_notifications_menu}, )

DEFINE_LAUNCHER(time_launcher,
    "Time",
    {"Set time", set_time_icon, enter_set_time_submenu},
    {"Set date", set_date_icon, enter_set_date_submenu},
    {"Set alarm", set_alarm_icon, enter_set_alarm_submenu},
    {"Unset alarm", unset_alarm_icon, unset_alarm},
    {"Stopwatch", stopwatch_icon, enter_stopwatch_submenu},
    {"Timer", timer_icon, enter_timer_submenu})

DEFINE_LAUNCHER(system_launcher,
    "System",
    {"Power save mode", qfn_package_icon, display_power_save_menu},
    {"System info", system_info_icon, display_system_info_wrapped},
    {"Battery status", battery_status_icon, display_battery_status},
    {"Hardware manager", qfn_package_icon, display_hardware_manager_menu},
    {"Reboot system", reset_icon, display_reboot_screen},
    {"Check joystick", check_joystick_icon, display_joystick_check},
    {"System reset", reset_icon, reset_system})

DEFINE_LAUNCHER(hardware_management,
    "Hardware management",
    {"SX1278 Module", sx1278_icon, display_sx1278_menu},
    {"ENS160 Sensor", ens160_icon, display_ens160_menu},
    {"haptic feedback", ens160_icon, display_haptic_menu})

DEFINE_LAUNCHER(sx1278_launcher,
    "SX1278 Module",
    {"Enable module", sx1278_icon, enable_sx1278},
    {"Disable module", sx1278_disabled_icon, disable_sx1278})

DEFINE_LAUNCHER(ens160_launcher,
    "ENS160 Sensor",
    {"Enable sensor", ens160_icon, enable_ens160},
    {"Disable sensor", ens160_disabled_icon, disable_ens160})

DEFINE_LAUNCHER(haptic_launcher,
    "Haptic feedback",
    {"Enable haptic", haptic_icon, haptic_enable},
    {"Disable haptic", haptic_disabled, haptic_disable})

DEFINE_LAUNCHER(power_save_launcher,
    "Power save",
    {"Enable power save", haptic_icon, enable_power_saving_mode},
    {"Disable power save", haptic_disabled, disable_power_saving_mode})

DEFINE_LAUNCHER(malloc_launcher,
    "Malloc",
    {"Talk with Malloc", NO_ICON, malloc_talk_with},
    {"See Malloc memories", NO_ICON, malloc_see_memories})

DEFINE_LAUNCHER(notifications_launcher,
    "Notifications",
    {"Enable notifications",
        enable_notifications_icon,
        enable_message_notifications},
    {"Disable notifications",
        disable_notifications_icon,
        disable_message_notifications})

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
void display_ulmp_menu() { launcher_start(&ulmp_launcher); }
void display_time_menu() { launcher_start(&time_launcher); }
void display_system_menu() { launcher_start(&system_launcher); }
void display_hardware_manager_menu() { launcher_start(&hardware_management); }
void display_ens160_menu() { launcher_start(&ens160_launcher); }
void display_sx1278_menu() { launcher_start(&sx1278_launcher); }
void display_power_save_menu() { launcher_start(&power_save_launcher); }
void display_haptic_menu() { launcher_start(&haptic_launcher); }
void display_malloc_menu() { launcher_start(&malloc_launcher); }
void display_notifications_menu() { launcher_start(&notifications_launcher); }
void display_tutorial_menu() { launcher_start(&tutorial_launcher); }