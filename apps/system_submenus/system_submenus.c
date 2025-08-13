
#include "apps/system_submenus/system_submenus.h"

#include <math.h>
#include <string.h>

#include "core/components/hw_manager.h"
#include "core/components/sys_paths_manager.h"
#include "core/graphics/graphic_primitives.h"
#include "core/hardware_drivers/battery.h"
#include "core/hardware_drivers/joystick.h"
#include "core/hardware_drivers/ssd1306.h"
#include "core/tools/options_gen.h"
#include "core/utils/path.h"
#include "core/utils/utils.h"
#include "device.h"
#include "hardware/watchdog.h"

void display_system_info_wrapped() { display_system_info(false); }

void display_system_info(bool serial_output) {
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
  if (serial_output) {
    for (uint8_t i = 0; i < options->len; i++)
      printf("%s\n", get(options, i));
    list_free(options);
    return;
  }
  options_page *system_info_page = options_page_init("System info", options);
  options_page_launch(system_info_page);
  options_page_free(system_info_page);
}

void reset_system() {
  ssd1306_clear(drivers->oled_screen);
  ssd1306_print(drivers->oled_screen,
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
  ssd1306_show(drivers->oled_screen);
  sleep_ms(200);
  joystick_update(drivers->joystick);
  while (joystick_get_direction(drivers->joystick) != E) {
    joystick_update(drivers->joystick);
    sleep_ms(100);
  }
  str_list *options = list_init();
  list_append(options, "Yes");
  list_append(options, "No");
  options_page *yesno_page = options_page_init("Are you sure?", options);
  char *answer = options_page_launch(yesno_page);
  if (strcmp(answer, "Yes") != 0) {
    options_page_free(yesno_page);
    return;
  }
  options_page_free(yesno_page);
  if (!request_password("# Type your password to confirm"))
    return;
  ssd1306_print(drivers->oled_screen, "Resetting system ...", 0, 0, false);
  ssd1306_show(drivers->oled_screen);
  path_fdelete(sys_paths->files->first_boot_file);
  path_fdelete(sys_paths->files->user_file);
  path_rmtree(sys_paths->dirs->home_path);
  ssd1306_print(drivers->oled_screen,
      "System reset done!\n"
      "Rebooting ...",
      0,
      0,
      false);
  ssd1306_show(drivers->oled_screen);
  watchdog_enable(3000, 1);
  sleep_ms(3000);
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

void display_tutorial_page() {
  str_list *options = list_init();
  list_append(options, "...");
  options_page *tutorial_page = options_page_init("Tutorial Page", options);
  options_page_launch(tutorial_page);
  options_page_free(tutorial_page);
}