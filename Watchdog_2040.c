// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include <hardware/gpio.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/_intsup.h>

#include "pico/stdio.h"
#include "pico/time.h"

#include "apps/msg_manager/include/msg_manager.h"
#include "apps/time/include/set_alarm.h"
#include "core/components/include/home_page.h"
#include "core/components/include/hw_manager.h"
#include "core/components/include/malloc_mascot.h"
#include "core/components/include/sys_paths_manager.h"
#include "core/data_structures/include/string_list.h"
#include "core/hardware_drivers/include/core1.h"
#include "core/hardware_drivers/include/haptics.h"
#include "core/hardware_drivers/include/joystick.h"
#include "core/hardware_drivers/include/onboard_led.h"
#include "core/hardware_drivers/include/ssd1306.h"
#include "core/tools/include/menus.h"
#include "core/utils/include/path.h"
#include "hardware/adc.h"

path *first_boot_file;

bool is_first_boot() { return (!path_exists(first_boot_file)); }

void write_first_boot_file() { path_ftouch(first_boot_file); }

void attach_background_routines() {
  core1_scheduler_add_callback(process_messages);
  core1_scheduler_add_callback(process_alarm);
  core1_scheduler_add_callback(process_blinking);
  core1_scheduler_set_start_flag(true);
}

void sys_setup() {
  adc_init();
  stdio_init_all();
  hardware_drivers_init();
  sys_paths_manager_init();
  first_boot_file = path_init(FIRST_BOOT_FILE);
  bool should_end_loading_screen = true;
  if (is_first_boot()) {
    end_loading_screen();
    should_end_loading_screen = false;
    start_malloc_mascot_tutorial();
    sys_paths_manager_load();
    sys_paths_manager_make();
    write_first_boot_file();
    dump_malloc_memories_to_sd();
  }
  path_free(first_boot_file);
  load_malloc_memories_from_sd();
  sys_paths_manager_load();
  msg_manager_init(malloc_memories_inst->ulmp_addr);
  home_page_init();
  if (should_end_loading_screen)
    end_loading_screen();
  attach_background_routines();
  check_peripherals();
}

void sys_mainloop() {
  bool first_run = true;
  uint8_t screen_up_seconds = 10;
  uint32_t screen_up_start;
  while (true) {
    if (!first_run)
      sys_idle();
    joystick_update(&(drivers->joystick));
    update_conversations();
    haptic_short_pulse();
    first_run = false;
    screen_up_start = to_us_since_boot(get_absolute_time()) / 1000000;
    while (true) {
      check_peripherals();
      process_system_state();
      update_conversations();
      display_home_page();
      joystick_update(&(drivers->joystick));
      if ((drivers->joystick).button_pressed)
        toggle_continuous_rx();
      if (joystick_get_direction(&(drivers->joystick)) == E) {
        display_main_menu();
        sleep_ms(200);
        screen_up_start = to_us_since_boot(get_absolute_time()) / 1000000;
      }
      joystick_update(&(drivers->joystick));
      if (joystick_get_direction(&(drivers->joystick)) == W) {
        sleep_ms(200);
        break;
      }
      if ((to_us_since_boot(get_absolute_time()) / 1000000) - screen_up_start >
          screen_up_seconds)
        break;
    }
    ssd1306_clear(&(drivers->ssd1306));
    ssd1306_show(&(drivers->ssd1306));
  }
}

int main() {
  sys_setup();
  sys_mainloop();
}
