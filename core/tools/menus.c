// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include <stdbool.h>
#include <stdio.h>

#include "apps/AQI/aqi.h"
#include "apps/flashlight/flashlight.h"
#include "apps/msg_manager/contacts.h"
#include "apps/msg_manager/msg_manager.h"
#include "apps/notes/notes.h"
#include "apps/system_submenus/system_submenus.h"
#include "apps/terminal/terminal.h"
#include "apps/time_submenus/set_alarm_submenu.h"
#include "apps/time_submenus/set_date_submenu.h"
#include "apps/time_submenus/set_time_submenu.h"
#include "apps/time_submenus/stopwatch_submenu.h"
#include "apps/time_submenus/timer_submenu.h"
#include "core/data_structures/string_list.h"
#include "core/graphics/bitmaps.h"
#include "core/tools/launcher.h"

void display_ulmp_menu();
void display_time_menu();
void display_system_menu();
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
    "Main menu",
    {"ULMP", connections_icon, display_ulmp_menu},
    {"Time", set_time_icon, display_time_menu},
    {"Terminal", cli_icon, terminal_launch},
    {"Air quality", AQI_icon, display_air_quality_indexes},
    {"System", qfn_package_icon, display_system_menu},
    {"Flashlight", flashlight_icon, enter_flashlight_screen},
    {"Notes", notes_icon, enter_notes_submenu},
    {"Games", pong_icon, display_games_menu},
    {"Malloc", malloc_icon, display_malloc_menu})

DEFINE_LAUNCHER(ulmp_launcher,
    "ULMP menu",
    {"Send message", send_msg_icon, send_message},
    {"Read messages", read_messages_icon, read_messages},
    {"Contacts", contacts_icon, enter_contacts_submenu},
    {"Scan online", scan_contacts_icon, scan_online_contacts},
    {"Notifications", notifications_icon, display_notifications_menu})

DEFINE_LAUNCHER(time_launcher,
    "Time menu",
    {"Set time", set_time_icon, enter_set_time_submenu},
    {"Set date", set_date_icon, enter_set_date_submenu},
    {"Set alarm", set_alarm_icon, enter_set_alarm_submenu},
    {"Unset alarm", unset_alarm_icon, unset_alarm},
    {"Stopwatch", stopwatch_icon, enter_stopwatch_submenu},
    {"Timer", timer_icon, enter_timer_submenu})

DEFINE_LAUNCHER(system_launcher,
    "System menu",
    {"System info", system_info_icon, display_system_info_wrapped},
    {"Battery status", battery_status_icon, display_battery_status},
    {"Check joystick", check_joystick_icon, display_joystick_check},
    {"System reset", reset_icon, reset_system})

DEFINE_LAUNCHER(malloc_launcher,
    "Malloc menu",
    {"Talk with Malloc", NO_ICON, malloc_talk_with},
    {"See Malloc memories", NO_ICON, malloc_see_memories})

DEFINE_LAUNCHER(notifications_launcher,
    "Notifications menu",
    {"Enable notifications",
        enable_notifications_icon,
        enable_message_notifications},
    {"Disable notifications",
        disable_notifications_icon,
        disable_message_notifications})

DEFINE_LAUNCHER(tutorial_launcher,
    "Tutorial menu",
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
void display_malloc_menu() { launcher_start(&malloc_launcher); }
void display_notifications_menu() { launcher_start(&notifications_launcher); }
void display_tutorial_menu() { launcher_start(&tutorial_launcher); }