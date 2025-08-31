// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include <stdbool.h>
#include <stdio.h>

#include "apps/AQI/aqi.h"
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
    {"ULMP", display_ulmp_menu},
    {"Time", display_time_menu},
    {"Terminal", terminal_launch},
    {"Air quality indexes", display_air_quality_indexes},
    {"System", display_system_menu},
    {"Notes", enter_notes_submenu},
    {"Games", display_games_menu},
    {"Malloc", display_malloc_menu})

DEFINE_LAUNCHER(ulmp_launcher,
    "ULMP menu",
    {"Send message", send_message},
    {"Read messages", read_messages},
    {"Contacts", enter_contacts_submenu},
    {"Scan online contacts", scan_online_contacts},
    {"Notifications", display_notifications_menu})

DEFINE_LAUNCHER(time_launcher,
    "Time menu",
    {"Set time", enter_set_time_submenu},
    {"Set date", enter_set_date_submenu},
    {"Set alarm", enter_set_alarm_submenu},
    {"Unset alarm", unset_alarm},
    {"Stopwatch", enter_stopwatch_submenu},
    {"Timer", enter_timer_submenu})

DEFINE_LAUNCHER(system_launcher,
    "System menu",
    {"System info", display_system_info_wrapped},
    {"Battery status", display_battery_status},
    {"Check joystick", display_joystick_check},
    {"System reset", reset_system})

DEFINE_LAUNCHER(malloc_launcher,
    "Malloc menu",
    {"Talk with Malloc", malloc_talk_with},
    {"See Malloc memories", malloc_see_memories})

DEFINE_LAUNCHER(notifications_launcher,
    "Notifications menu",
    {"Enable notifications", enable_message_notifications},
    {"Disable notifications", disable_message_notifications})

DEFINE_LAUNCHER(tutorial_launcher,
    "Tutorial menu",
    {"Entry 0", display_tutorial_page},
    {"Entry 1", display_tutorial_page},
    {"Entry 2", display_tutorial_page},
    {"Entry 3", display_tutorial_page},
    {"Entry 4", display_tutorial_page},
    {"Entry 5", display_tutorial_page},
    {"Entry 6", display_tutorial_page},
    {"Entry 7", display_tutorial_page})

void display_main_menu() { launcher_start(&main_launcher); }
void display_ulmp_menu() { launcher_start(&ulmp_launcher); }
void display_time_menu() { launcher_start(&time_launcher); }
void display_system_menu() { launcher_start(&system_launcher); }
void display_malloc_menu() { launcher_start(&malloc_launcher); }
void display_notifications_menu() { launcher_start(&notifications_launcher); }
void display_tutorial_menu() { launcher_start(&tutorial_launcher); }