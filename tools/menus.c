#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "components/hw_manager.h"
#include "components/msg_manager/contacts_manager.h"
#include "components/msg_manager/msg_manager.h"
#include "components/sys_paths_manager.h"
#include "data_structures/string_list.h"
#include "device.h"
#include "graphics/graphic_primitives.h"
#include "graphics/graphs.h"
#include "hardware_drivers/battery.h"
#include "hardware_drivers/haptics.h"
#include "hardware_drivers/joystick.h"
#include "hardware_drivers/ssd1306.h"
#include "tools/options_gen.h"
#include "tools/submenus/set_alarm_submenu.h"
#include "tools/submenus/set_date_submenu.h"
#include "tools/submenus/set_time_submenu.h"
#include "tools/submenus/stopwatch_submenu.h"
#include "tools/submenus/timer_submenu.h"
#include "tools/terminal/terminal.h"
#include "tools/text_editor.h"
#include "utils/path.h"
#include "utils/utils.h"

void display_ulmp_menu();
void display_air_quality_indexes();
void display_tools_menu();
void display_time_menu();
void display_system_menu();
void display_malloc_menu();
void display_tutorial_page();

void display_main_menu() {
  haptic_short_pulse();
  str_list *options = list_init();
  list_append(options, "ULMP");
  list_append(options, "Air quality indexes");
  list_append(options, "tools");
  list_append(options, "Time");
  list_append(options, "System");
  list_append(options, "Malloc");
  options_page *main_menu = options_page_init("Main menu", options);
  attach_callback_to_option(main_menu, 0, display_ulmp_menu);
  attach_callback_to_option(main_menu, 1, display_air_quality_indexes);
  attach_callback_to_option(main_menu, 2, display_tools_menu);
  attach_callback_to_option(main_menu, 3, display_time_menu);
  attach_callback_to_option(main_menu, 4, display_system_menu);
  attach_callback_to_option(main_menu, 5, display_malloc_menu);
  options_page_launch(main_menu);
  options_page_free(main_menu);
  ssd1306_clear(drivers->oled_screen);
}

void display_notifications_menu() {
  str_list *options = list_init();
  list_append(options, "Enable notifications");
  list_append(options, "Disable notifications");
  options_page *notifications_menu = options_page_init("Notifications",
      options);
  attach_callback_to_option(notifications_menu,
      0,
      enable_message_notifications);
  attach_callback_to_option(notifications_menu,
      1,
      disable_message_notifications);
  options_page_launch(notifications_menu);
  options_page_free(notifications_menu);
}

void display_ulmp_menu() {
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
  attach_callback_to_option(ulmp_menu, 6, display_notifications_menu);
  options_page_launch(ulmp_menu);
  options_page_free(ulmp_menu);
}

void display_air_quality_indexes() {
  graph *g_aqi = graph_init("AQI", 48, 40, 0, 16, 0, 5);
  graph *g_co2 = graph_init("eCO2", 48, 40, 43, 16, 400, 2000);
  graph *g_tvoc = graph_init("TVOC", 48, 40, 86, 16, 0, 600);
  while (joystick_get_direction(drivers->joystick) != W) {
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

void take_note() {
  ssd1306_clear(drivers->oled_screen);
  ssd1306_print(drivers->oled_screen, "Taking note...", 0, 0, false);
  ssd1306_show(drivers->oled_screen);
  text_editor *name_editor = text_editor_launch("# Name of your note...",
      true);
  char *name = text_editor_get_buf(name_editor);
  text_editor_kill(name_editor);
  text_editor *note_editor = text_editor_launch("# Write your note here...",
      true);
  char *note = text_editor_get_buf(name_editor);
  if (note != NULL) {
    path *name_path = path_init(name);
    path *note_path = path_concat(sys_paths->dirs->notes_path, name_path);
    path_free(name_path);
    path_fwrite(note_path, note, 'w');
    free(note);
    path_free(note_path);
  }
  text_editor_kill(name_editor);
}

void read_notes() // todo: implement
{}

void display_games_menu() // todo: implement
{}

void enter_terminal() { terminal_launch(); }

void display_tools_menu() {
  str_list *options = list_init();
  list_append(options, "terminal");
  list_append(options, "take note");
  list_append(options, "read notes");
  list_append(options, "games");
  options_page *tools_menu = options_page_init("Tools", options);
  attach_callback_to_option(tools_menu, 0, enter_terminal);
  attach_callback_to_option(tools_menu, 1, take_note);
  attach_callback_to_option(tools_menu, 2, read_notes);
  attach_callback_to_option(tools_menu, 3, display_games_menu);
  options_page_launch(tools_menu);
  options_page_free(tools_menu);
}

void display_time_menu() {
  str_list *options = list_init();
  list_append(options, "Set time");
  list_append(options, "Set date");
  list_append(options, "Set alarm");
  list_append(options, "Unset alarm");
  list_append(options, "Stopwatch");
  list_append(options, "Timer");
  options_page *time_menu = options_page_init("Time", options);
  attach_callback_to_option(time_menu, 0, enter_set_time_submenu);
  attach_callback_to_option(time_menu, 1, enter_set_date_submenu);
  attach_callback_to_option(time_menu, 2, enter_set_alarm_submenu);
  attach_callback_to_option(time_menu, 3, unset_alarm);
  attach_callback_to_option(time_menu, 4, enter_stopwatch_submenu);
  attach_callback_to_option(time_menu, 5, enter_timer_submenu);
  options_page_launch(time_menu);
  options_page_free(time_menu);
}

void display_system_info() {
  str_list *options = list_init();
  uint64_t us_since_boot = to_us_since_boot(get_absolute_time());
  us_since_boot /= 1000000;
  char uptime_str[20];
  uint32_t free_heap = get_free_heap();
  char free_heap_str[20];
  uint32_t clock_freq_khz = get_clock_freq_khz();
  char clock_freq_khz_str[20];
  uint used_flash = get_used_flash_bytes();
  char used_flash_str[20];
  float cpu_temp = get_cpu_temp();
  char cpu_temp_str[20];
  snprintf(free_heap_str, sizeof(free_heap_str), "%u bytes", free_heap);
  snprintf(used_flash_str, sizeof(used_flash_str), "%u bytes", used_flash);
  snprintf(uptime_str, sizeof(uptime_str), "%llu seconds", us_since_boot);
  snprintf(clock_freq_khz_str,
      sizeof(clock_freq_khz_str),
      "%u kHz",
      clock_freq_khz);
  snprintf(cpu_temp_str, sizeof(cpu_temp_str), "%.2f C", cpu_temp);
  list_append(options, "Device:");
  list_append(options, DEVICE_NAME);
  list_append(options, "Hardware version:");
  list_append(options, HARDWARE_VERSION);
  list_append(options, "Firmware version:");
  list_append(options, FIRMWARE_VERSION);
  list_append(options, "Free heap memory: ");
  list_append(options, free_heap_str);
  list_append(options, "Used flash memory: ");
  list_append(options, used_flash_str);
  list_append(options, "System uptime: ");
  list_append(options, uptime_str);
  list_append(options, "Clock frequency: ");
  list_append(options, clock_freq_khz_str);
  list_append(options, "CPU temperature: ");
  list_append(options, cpu_temp_str);
  options_page *system_info_page = options_page_init("System info", options);
  options_page_launch(system_info_page);
  options_page_free(system_info_page);
}

void reset_system() {
  str_list *options = list_init();
  list_append(options, "Yes");
  list_append(options, "No");
  options_page *yesno_page = options_page_init("Are you sure?", options);
  char *answer = options_page_launch(yesno_page);
  if (strcmp(answer, "Yes") == 0) {
    ssd1306_clear(drivers->oled_screen);
    ssd1306_print(drivers->oled_screen, "Resetting system", 0, 0, false);
    ssd1306_print(drivers->oled_screen, "right now can", 0, 1, false);
    ssd1306_print(drivers->oled_screen, "only be done", 0, 2, false);
    ssd1306_print(drivers->oled_screen, "by unplugging", 0, 3, false);
    ssd1306_print(drivers->oled_screen, "the sd card", 0, 4, false);
    ssd1306_print(drivers->oled_screen, "and wiping", 0, 5, false);
    ssd1306_print(drivers->oled_screen, "its content", 0, 6, false);
    ssd1306_show(drivers->oled_screen);
    sleep_ms(3000);
  }
  free(answer);
  options_page_free(yesno_page);
}

void display_battery_status() {
  ssd1306_clear(drivers->oled_screen);
  joystick_update(drivers->joystick);
  while (joystick_get_direction(drivers->joystick) != W) {
    joystick_update(drivers->joystick);
    ssd1306_print(drivers->oled_screen, "Battery status", 3, 0, false);
    ssd1306_print(drivers->oled_screen, "Percentage:     ", 0, 2, false);
    ssd1306_print(drivers->oled_screen,
        battery_get_percentage_str(drivers->battery),
        12,
        2,
        false);
    ssd1306_print(drivers->oled_screen, "Voltage:        ", 0, 3, false);
    ssd1306_print(drivers->oled_screen,
        battery_get_voltage_str(drivers->battery),
        12,
        3,
        false);
    ssd1306_print(drivers->oled_screen, "Crude ADC:      ", 0, 4, false);
    ssd1306_print(drivers->oled_screen,
        battery_get_crude_adc_str(drivers->battery),
        12,
        4,
        false);
    ssd1306_show(drivers->oled_screen);
    sleep_ms(100);
  }
}

void display_joystick_check() {
  ssd1306_clear(drivers->oled_screen);
  ssd1306_print(drivers->oled_screen, "Joystick test", 0, 0, false);
  ssd1306_print(drivers->oled_screen, "Long press to exit", 0, 1, false);
  ssd1306_print(drivers->oled_screen, "X:", 0, 4, false);
  ssd1306_print(drivers->oled_screen, "Y:", 0, 5, false);
  ssd1306_print(drivers->oled_screen, "Theta:", 11, 3, false);
  ssd1306_print(drivers->oled_screen, "Rho:", 11, 5, false);
  char x_str[10];
  char y_str[10];
  char theta_str[10];
  char rho_str[10];
  circle c = create_circle(create_point(64, 40), 20);
  circle c1 = create_circle(create_point(64, 40), 5);
  draw_circle(c);
  draw_circle(c1);
  while (!joystick_check_long_press(drivers->joystick, 2000)) {
    joystick_update(drivers->joystick);
    clear_circle(c1);
    polar_coords polar = joystick_get_polar(drivers->joystick);
    float theta_rad = polar.theta_deg * (M_PI / 180.0f);
    c1 = create_circle(create_point(64 + polar.l * 10 * cosf(theta_rad),
                           40 - polar.l * 10 * sinf(theta_rad)),
        7);
    draw_circle(c1);
    snprintf(x_str, sizeof(x_str), "%u", drivers->joystick->x_value);
    snprintf(y_str, sizeof(y_str), "%u", drivers->joystick->y_value);
    snprintf(theta_str, sizeof(theta_str), "%.2f", polar.theta_deg);
    snprintf(rho_str, sizeof(rho_str), "%.2f", polar.l);
    ssd1306_print(drivers->oled_screen, x_str, 2, 4, false);
    ssd1306_print(drivers->oled_screen, y_str, 2, 5, false);
    ssd1306_print(drivers->oled_screen, theta_str, 11, 4, false);
    ssd1306_print(drivers->oled_screen, rho_str, 11, 6, false);
    ssd1306_show(drivers->oled_screen);
  }
}

void display_system_menu() {
  str_list *options = list_init();
  list_append(options, "System info");
  list_append(options, "System reset");
  list_append(options, "Battery status");
  list_append(options, "Check joystick");
  options_page *system_menu = options_page_init("System", options);
  attach_callback_to_option(system_menu, 0, display_system_info);
  attach_callback_to_option(system_menu, 1, reset_system);
  attach_callback_to_option(system_menu, 2, display_battery_status);
  attach_callback_to_option(system_menu, 3, display_joystick_check);
  options_page_launch(system_menu);
  options_page_free(system_menu);
}

void display_malloc_menu() {
  str_list *options = list_init();
  list_append(options, "Talk with Malloc");
  list_append(options, "See Malloc memories");
  options_page *malloc_menu = options_page_init("Malloc", options);
  options_page_launch(malloc_menu);
  options_page_free(malloc_menu);
}

void display_tutorial_menu() {
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

void display_tutorial_page() {
  str_list *options = list_init();
  list_append(options, "...");
  options_page *tutorial_page = options_page_init("Tutorial Page", options);
  options_page_launch(tutorial_page);
  options_page_free(tutorial_page);
}
