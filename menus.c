#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hw_manager.h"
#include "msg_manager.h"
#include "hardware_drivers/battery.h"
#include "hardware_drivers/ssd1306.h"
#include "hardware_drivers/joystick.h"
#include "malloc_mascot.h"
#include "hardware_drivers/sdcard.h"
#include "data_structures/string_list.h"
#include "options_gen.h"

void display_ulmp_menu();
void display_air_quality_menu();
void display_bpm_menu();
void display_time_menu();
void display_system_menu();
void display_malloc_menu();

void display_main_menu()
{
  str_list *options = list();
  lstappend(options, "ULMP");
  lstappend(options, "Air quality indexes");
  lstappend(options, "BPM and blood oxygen");
  lstappend(options, "Time");
  lstappend(options, "System");
  lstappend(options, "Malloc");
  options_page *main_menu = options_page_init("Main menu", options);
  attach_callback_to_option(main_menu, 0, display_ulmp_menu);
  attach_callback_to_option(main_menu, 1, display_air_quality_menu);
  attach_callback_to_option(main_menu, 2, display_bpm_menu);
  attach_callback_to_option(main_menu, 3, display_time_menu);
  attach_callback_to_option(main_menu, 4, display_system_menu);
  attach_callback_to_option(main_menu, 5, display_malloc_menu);
  options_page_launch(main_menu);
  options_page_free(main_menu);
}

void display_ulmp_menu()
{
  str_list *options = list();
  lstappend(options, "Send message");
  lstappend(options, "Read messages");
  lstappend(options, "Add contact");
  lstappend(options, "Remove contact");
  lstappend(options, "Scan online contacts");
  options_page *ulmp_menu = options_page_init("ULMP", options);
  attach_callback_to_option(ulmp_menu, 0, send_message);
  attach_callback_to_option(ulmp_menu, 1, read_messages);
  attach_callback_to_option(ulmp_menu, 2, add_contact);
  attach_callback_to_option(ulmp_menu, 3, remove_contact);
  attach_callback_to_option(ulmp_menu, 4, scan_online_contacts);
  options_page_launch(ulmp_menu);
  options_page_free(ulmp_menu);
}

void display_air_quality_menu()
{
  str_list *options = list();
  lstappend(options, "AQI");
  lstappend(options, "TVOC");
  lstappend(options, "eCO2");
  options_page *air_quality_menu = options_page_init("Air quality indexes", options);
  options_page_launch(air_quality_menu);
  options_page_free(air_quality_menu);
}

void display_bpm_menu()
{
  str_list *options = list();
  lstappend(options, "BPM");
  lstappend(options, "SPO2");
  options_page *bpm_menu = options_page_init("BPM and blood oxygen", options);
  options_page_launch(bpm_menu);
  options_page_free(bpm_menu);
}

void display_time_menu()
{
  str_list *options = list();
  lstappend(options, "Set time");
  lstappend(options, "Set alarm");
  options_page *time_menu = options_page_init("Time", options);
  options_page_launch(time_menu);
  options_page_free(time_menu);
}

void display_system_menu()
{
  str_list *options = list();
  lstappend(options, "System info");
  lstappend(options, "System reset");
  lstappend(options, "Battery status");
  lstappend(options, "Check joystick");
  options_page *system_menu = options_page_init("System", options);
  options_page_launch(system_menu);
  options_page_free(system_menu);
}

void display_malloc_menu()
{
  str_list *options = list();
  lstappend(options, "Talk with Malloc");
  lstappend(options, "See Malloc memories");
  options_page *malloc_menu = options_page_init("Malloc", options);
  options_page_launch(malloc_menu);
  options_page_free(malloc_menu);
}
