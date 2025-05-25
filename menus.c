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
#include "graphs.h"

void display_ulmp_menu();
void display_air_quality_indexes();
void display_bpm_menu();
void display_time_menu();
void display_system_menu();
void display_malloc_menu();
void display_tutorial_page();

void display_main_menu()
{
  str_list *options = list_init();
  list_append(options, "ULMP");
  list_append(options, "Air quality indexes");
  list_append(options, "BPM and blood oxygen");
  list_append(options, "Time");
  list_append(options, "System");
  list_append(options, "Malloc");
  options_page *main_menu = options_page_init("Main menu", options);
  attach_callback_to_option(main_menu, 0, display_ulmp_menu);
  attach_callback_to_option(main_menu, 1, display_air_quality_indexes);
  attach_callback_to_option(main_menu, 2, display_bpm_menu);
  attach_callback_to_option(main_menu, 3, display_time_menu);
  attach_callback_to_option(main_menu, 4, display_system_menu);
  attach_callback_to_option(main_menu, 5, display_malloc_menu);
  options_page_launch(main_menu);
  options_page_free(main_menu);
  ssd1306_clear(drivers->oled_screen);
}

void display_notifications_menu()
{
  str_list *options = list_init();
  list_append(options, "Enable notifications");
  list_append(options, "Disable notifications");
  options_page *notifications_menu = options_page_init("Notifications", options);
  attach_callback_to_option(notifications_menu, 0, enable_message_notifications);
  attach_callback_to_option(notifications_menu, 1, disable_message_notifications);
  options_page_launch(notifications_menu);
  options_page_free(notifications_menu);
}

void display_ulmp_menu()
{
  str_list *options = list_init();
  list_append(options, "Send message");
  list_append(options, "Read messages");
  list_append(options, "Add contact");
  list_append(options, "Remove contact");
  list_append(options, "Dump to MicroSD");
  list_append(options, "Scan online contacts");
  list_append(options, "Notifications");
  options_page *ulmp_menu = options_page_init("ULMP", options);
  attach_callback_to_option(ulmp_menu, 0, send_message);
  attach_callback_to_option(ulmp_menu, 1, read_messages);
  attach_callback_to_option(ulmp_menu, 2, add_contact);
  attach_callback_to_option(ulmp_menu, 3, remove_contact);
  attach_callback_to_option(ulmp_menu, 4, dump_contacts_to_sd);
  attach_callback_to_option(ulmp_menu, 5, scan_online_contacts);
  options_page_launch(ulmp_menu);
  options_page_free(ulmp_menu);
}

void display_air_quality_indexes()
{
  graph *g_aqi = graph_init("AQI", 48, 40, 0, 16, 0, 5);
  graph *g_co2 = graph_init("eCO2", 48, 40, 43, 16, 400, 2000);
  graph *g_tvoc = graph_init("TVOC", 48, 40, 86, 16, 0, 600);
  while (joystick_get_direction(drivers->joystick) != E)
  {
    joystick_update(drivers->joystick);
    graph_push_value(g_aqi, ens160_read_aqi(drivers->air_quality_sensor));
    graph_push_value(g_co2, ens160_read_co2(drivers->air_quality_sensor));
    graph_push_value(g_tvoc, ens160_read_tvoc(drivers->air_quality_sensor));
    ssd1306_clear(drivers->oled_screen);
    graph_update(g_aqi);
    graph_update(g_co2);
    graph_update(g_tvoc);
    ssd1306_show(drivers->oled_screen);
    sleep_ms(500);
  }
  graph_free(g_aqi);
  graph_free(g_co2);
  graph_free(g_tvoc);
}

void display_bpm_menu()
{
  str_list *options = list_init();
  list_append(options, "BPM");
  list_append(options, "SPO2");
  options_page *bpm_menu = options_page_init("BPM and blood oxygen", options);
  options_page_launch(bpm_menu);
  options_page_free(bpm_menu);
}

void display_time_menu()
{
  str_list *options = list_init();
  list_append(options, "Set time");
  list_append(options, "Set alarm");
  options_page *time_menu = options_page_init("Time", options);
  options_page_launch(time_menu);
  options_page_free(time_menu);
}

void display_system_menu()
{
  str_list *options = list_init();
  list_append(options, "System info");
  list_append(options, "System reset");
  list_append(options, "Battery status");
  list_append(options, "Check joystick");
  options_page *system_menu = options_page_init("System", options);
  options_page_launch(system_menu);
  options_page_free(system_menu);
}

void display_malloc_menu()
{
  str_list *options = list_init();
  list_append(options, "Talk with Malloc");
  list_append(options, "See Malloc memories");
  options_page *malloc_menu = options_page_init("Malloc", options);
  options_page_launch(malloc_menu);
  options_page_free(malloc_menu);
}

void display_tutorial_menu()
{
  str_list *options = list_init();
  list_append(options, "Page 0");
  list_append(options, "Page 1");
  list_append(options, "Page 2");
  list_append(options, "Page 3");
  list_append(options, "Page 4");
  options_page *tutorial = options_page_init("Tutorial", options);
  options_page_launch(tutorial);
  options_page_free(tutorial);
}

void display_tutorial_page()
{
  str_list *options = list_init();
  list_append(options, "...");
  options_page *tutorial_page = options_page_init("Tutorial Page", options);
  options_page_launch(tutorial_page);
  options_page_free(tutorial_page);
}
