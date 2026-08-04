// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "apps/system_app/include/system_app.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "pico/bootrom.h"
#include "pico/unique_id.h"

#include "apps/system_app/include/bitmaps.h"
#include "core/components/include/hw_manager.h"
#include "core/components/include/sys_paths_manager.h"
#include "core/data_structures/include/string_list.h"
#include "core/graphics/include/graphic_primitives.h"
#include "core/hardware_drivers/include/battery.h"
#include "core/hardware_drivers/include/haptics.h"
#include "core/hardware_drivers/include/joystick.h"
#include "core/hardware_drivers/include/ssd1306.h"
#include "core/tools/include/launcher.h"
#include "core/tools/include/options_gen.h"
#include "core/utils/include/path.h"
#include "core/utils/include/utils.h"
#include "device.h"
#include "hardware/watchdog.h"

static bool haptics = true;

void system_settings_launch();

DEFINE_LAUNCHER(system_app_launcher,
    "System",
    {"Reboot system", reboot_icon, display_reboot_screen},
    {"Bootsel", bootsel_icon, display_reboot_to_bootsel_screen},
    {"System settings", system_settings_icon, system_settings_launch},
    {"System info", system_info_icon, display_system_info_wrapped},
    {"Battery status", battery_status_icon, display_battery_status},
    {"Check joystick", check_joystick_icon, display_joystick_check},
    {"System reset", reset_icon, reset_system})

char *auto_brightness_onoff_flag(char *input) {
  char *new_str;
  if (strstr(input, "[ON]") != NULL) {
    new_str = string_substring_replace(input, "[ON]", "[OFF]");
    ssd1306_disable_auto_brightness(&(drivers->ssd1306));
  } else {
    new_str = string_substring_replace(input, "[OFF]", "[ON]");
    ssd1306_enable_auto_brightness(&(drivers->ssd1306));
  }
  return new_str;
}

void system_settings_dump(system_settings_t settings) {
  path_key_value_dump(sys_paths->files->config_file,
      'w',
      "auto_brightness",
      drivers->ssd1306.auto_brightness ? "on" : "off");
  char brightness_str[4];
  snprintf(brightness_str, 4, "%d", settings.brightness_level);
  path_key_value_dump(sys_paths->files->config_file,
      'a',
      "brightness",
      brightness_str);
  path_key_value_dump(sys_paths->files->config_file,
      'a',
      "haptics",
      settings.haptics_enabled ? "on" : "off");
}

void system_settings_load(system_settings_t *settings) {
  char *auto_b = path_key_value_get(sys_paths->files->config_file,
      "auto_brightness");
  char *b_level = path_key_value_get(sys_paths->files->config_file,
      "brightness");
  char *haptics = path_key_value_get(sys_paths->files->config_file, "haptics");
  char *auto_b_no_lfd = string_remove_linefeed(auto_b);
  char *b_level_no_lfd = string_remove_linefeed(b_level);
  char *haptics_no_lfd = string_remove_linefeed(haptics);
  uint8_t b_level_uint = atoi(b_level_no_lfd);
  settings->auto_brightness_enabled = strcmp(auto_b_no_lfd, "on") == 0 ? true
                                                                       : false;
  settings->brightness_level = b_level_uint;
  settings->haptics_enabled = strcmp(haptics_no_lfd, "on") == 0 ? true : false;
  free(auto_b_no_lfd);
  free(b_level_no_lfd);
  free(haptics_no_lfd);
}

char *haptics_onoff_flag(char *input) {
  char *new_str;
  if (strstr(input, "[ON]") != NULL) {
    new_str = string_substring_replace(input, "[ON]", "[OFF]");
    haptics_disable();
  } else {
    new_str = string_substring_replace(input, "[OFF]", "[ON]");
    haptics_enable();
  }
  return new_str;
}

void set_brightness() {
  drivers->ssd1306.auto_brightness = false;
  uint8_t level = (uint8_t)drivers->ssd1306.current_brightness / 25;
  sleep_ms(200);
  while (true) {
    joystick_update(&(drivers->joystick));
    if (joystick_get_direction(&(drivers->joystick)) == W && level > 0) {
      haptic_auto_pulse();
      level--;
    }
    if (joystick_get_direction(&(drivers->joystick)) == E && level < 10) {
      haptic_auto_pulse();
      level++;
    }
    if (joystick_check_long_press(&(drivers->joystick), 500))
      break;
    ssd1306_draw_bitmap(&(drivers->ssd1306),
        0,
        0,
        brightness_bar[level],
        128,
        64,
        false);
    ssd1306_print(&(drivers->ssd1306), "Long press to exit", 1, 0, false);
    ssd1306_set_brightness(&(drivers->ssd1306), level * 23);
    ssd1306_show(&(drivers->ssd1306));
    sleep_ms(100);
  }
}

void system_settings_launch() {
  str_list *options = str_list_init();
  char bright_stat_str[22];
  char haptic_stat_str[22];
  snprintf(bright_stat_str,
      22,
      "%s Auto brightness",
      drivers->ssd1306.auto_brightness ? "[ON]" : "[OFF]");
  snprintf(haptic_stat_str,
      22,
      "%s Haptic feedback",
      haptics_get_status() ? "[ON]" : "[OFF]");
  str_list_append(options, bright_stat_str);
  str_list_append(options, haptic_stat_str);
  str_list_append(options, "Set screen brightness");

  options_page *settings_page = options_page_init("Settings", options);
  attach_flag_callback_to_option(settings_page, 0, auto_brightness_onoff_flag);
  attach_flag_callback_to_option(settings_page, 1, haptics_onoff_flag);
  attach_callback_to_option(settings_page, 2, set_brightness);
  options_page_launch(settings_page);
  options_page_free(settings_page);

  system_settings_t settings;
  settings.auto_brightness_enabled = &(drivers->ssd1306.auto_brightness);
  settings.brightness_level = drivers->ssd1306.current_brightness;
  settings.haptics_enabled = haptics_get_status();
  system_settings_dump(settings);
}

void system_app_launch() { launcher_start_tui(&system_app_launcher); }

void display_system_info_wrapped() { display_system_info(false); }

void display_system_info(bool serial_output) {
  str_list *options = str_list_init();
  char board_uid[17];
  pico_get_unique_board_id_string(board_uid, 17);
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
  str_list_append(options, "Device:");
  str_list_append(options, DEVICE_NAME);
  str_list_append(options, "Board uid:");
  str_list_append(options, board_uid);
  str_list_append(options, "Hardware version:");
  str_list_append(options, HARDWARE_VERSION);
  str_list_append(options, "Firmware version:");
  str_list_append(options, FIRMWARE_VERSION);
  str_list_append(options, "Free heap memory: ");
  str_list_append(options, free_heap_str);
  str_list_append(options, "Used flash memory: ");
  str_list_append(options, used_flash_str);
  str_list_append(options, "System uptime: ");
  str_list_append(options, uptime_str);
  str_list_append(options, "Clock frequency: ");
  str_list_append(options, clock_freq_khz_str);
  str_list_append(options, "MCU temperature: ");
  str_list_append(options, cpu_temp_str);
  if (serial_output) {
    for (uint8_t i = 0; i < options->len; i++)
      printf("%s\n", str_list_get(options, i));
    str_list_free(options);
    return;
  }
  options_page *system_info_page = options_page_init("System info", options);
  options_page_launch(system_info_page);
  options_page_free(system_info_page);
}

void reset_system() {
  ssd1306_clear(&(drivers->ssd1306));
  ssd1306_print(&(drivers->ssd1306),
      "RISKY STUFF        ->\n"
      "Resetting system is\n"
      "IRREVERSIBLE, data\n"
      "will be lost, includi\n"
      "ng contacts and\n"
      "messages, your ULMP\n"
      "address will change.",
      0,
      0,
      false);
  ssd1306_show(&(drivers->ssd1306));
  sleep_ms(200);
  joystick_update(&(drivers->joystick));
  while (joystick_get_direction(&(drivers->joystick)) != E) {
    joystick_update(&(drivers->joystick));
    sleep_ms(100);
  }
  str_list *options = str_list_init();
  str_list_append(options, "Yes");
  str_list_append(options, "No");
  options_page *yesno_page = options_page_init("Are you sure?", options);
  char *answer = options_page_launch(yesno_page);
  if (strcmp(answer, "Yes") != 0) {
    options_page_free(yesno_page);
    return;
  }
  options_page_free(yesno_page);
  if (!request_password())
    return;
  ssd1306_print(&(drivers->ssd1306), "Resetting system ...", 0, 0, false);
  ssd1306_show(&(drivers->ssd1306));
  path_fdelete(sys_paths->files->first_boot_file);
  path_fdelete(sys_paths->files->user_file);
  path_rmtree(sys_paths->dirs->home_path);
  ssd1306_print(&(drivers->ssd1306),
      "System reset done!\n"
      "Rebooting ...",
      0,
      0,
      false);
  ssd1306_show(&(drivers->ssd1306));
  watchdog_enable(3000, 1);
  sleep_ms(3000);
}

void display_battery_status() {
  ssd1306_clear(&(drivers->ssd1306));
  joystick_update(&(drivers->joystick));
  while (joystick_get_direction(&(drivers->joystick)) != W) {
    joystick_update(&(drivers->joystick));
    ssd1306_print(&(drivers->ssd1306), "Battery status", 3, 0, false);
    ssd1306_print(&(drivers->ssd1306), "Percentage:     ", 0, 2, false);
    ssd1306_print(&(drivers->ssd1306),
        battery_get_percentage_str(&(drivers->battery)),
        12,
        2,
        false);
    ssd1306_print(&(drivers->ssd1306), "Voltage:        ", 0, 3, false);
    ssd1306_print(&(drivers->ssd1306),
        battery_get_voltage_str(&(drivers->battery)),
        12,
        3,
        false);
    ssd1306_print(&(drivers->ssd1306), "Crude ADC:      ", 0, 4, false);
    ssd1306_print(&(drivers->ssd1306),
        battery_get_crude_adc_str(&(drivers->battery)),
        12,
        4,
        false);
    uint8_t status = battery_get_status(&(drivers->battery));
    ssd1306_print(&(drivers->ssd1306), "Status:", 0, 5, false);
    ssd1306_print(&(drivers->ssd1306),
        battery_status_to_str(status),
        10,
        5,
        false);
    ssd1306_show(&(drivers->ssd1306));
    sleep_ms(100);
  }
}

void display_joystick_check() {
  ssd1306_clear(&(drivers->ssd1306));
  ssd1306_print(&(drivers->ssd1306), "Joystick test", 0, 0, false);
  ssd1306_print(&(drivers->ssd1306), "Long press to exit", 0, 1, false);
  ssd1306_print(&(drivers->ssd1306), "X:", 0, 4, false);
  ssd1306_print(&(drivers->ssd1306), "Y:", 0, 5, false);
  ssd1306_print(&(drivers->ssd1306), "Theta:", 11, 3, false);
  ssd1306_print(&(drivers->ssd1306), "Rho:", 11, 5, false);
  char x_str[10];
  char y_str[10];
  char theta_str[10];
  char rho_str[10];
  circle c = create_circle(create_point(64, 40), 20);
  circle c1 = create_circle(create_point(64, 40), 7);
  draw_circle(c);
  draw_circle(c1);
  while (!joystick_check_long_press(&(drivers->joystick), 2000)) {
    joystick_update(&(drivers->joystick));
    clear_circle(c1);
    polar_coords polar = joystick_get_polar(&(drivers->joystick));
    float theta_rad = polar.theta_deg * (M_PI / 180.0f);
    c1 = create_circle(create_point(64 + polar.l * 10 * cosf(theta_rad) * -1,
                           40 - polar.l * 10 * sinf(theta_rad)),
        7);
    draw_circle(c1);
    snprintf(x_str, sizeof(x_str), "%u", (drivers->joystick).x_value);
    snprintf(y_str, sizeof(y_str), "%u", (drivers->joystick).y_value);
    snprintf(theta_str, sizeof(theta_str), "%.2f", polar.theta_deg);
    snprintf(rho_str, sizeof(rho_str), "%.2f", polar.l);
    ssd1306_print(&(drivers->ssd1306), x_str, 2, 4, false);
    ssd1306_print(&(drivers->ssd1306), y_str, 2, 5, false);
    ssd1306_print(&(drivers->ssd1306), theta_str, 11, 4, false);
    ssd1306_print(&(drivers->ssd1306), rho_str, 11, 6, false);
    ssd1306_show(&(drivers->ssd1306));
  }
}

void display_reboot_screen() {
  str_list *options = str_list_init();
  str_list_append(options, "Yes");
  str_list_append(options, "No");
  options_page *yesno_page = options_page_init("Reboot now?", options);
  char *buf = options_page_launch(yesno_page);
  if (strcmp(buf, "Yes") == 0) {
    print_info("Rebooting ...");
    watchdog_reboot(0, 0, 0);
  }
  options_page_free(yesno_page);
  free(buf);
}

void display_reboot_to_bootsel_screen() {
  str_list *options = str_list_init();
  str_list_append(options, "Yes");
  str_list_append(options, "No");
  options_page *yesno_page = options_page_init("Enter bootsel now?", options);
  char *buf = options_page_launch(yesno_page);
  if (strcmp(buf, "Yes") == 0) {
    print_info("Rebooting to bootsel mode...");
    reset_usb_boot(0, 0);
  }
  options_page_free(yesno_page);
  free(buf);
}

void display_tutorial_page() {
  str_list *options = str_list_init();
  str_list_append(options, "...");
  options_page *tutorial_page = options_page_init("Tutorial Page", options);
  options_page_launch(tutorial_page);
  options_page_free(tutorial_page);
}

inline void enable_auto_brightness() {
  ssd1306_enable_auto_brightness(&(drivers->ssd1306));
}

inline void disable_auto_brightness() {
  ssd1306_disable_auto_brightness(&(drivers->ssd1306));
}