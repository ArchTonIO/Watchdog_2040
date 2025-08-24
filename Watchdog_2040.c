// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include <stdbool.h>
#include <stdint.h>
#include <sys/_intsup.h>

#include "pico/stdlib.h"

#include "apps/msg_manager/msg_manager.h"
#include "apps/time_submenus/set_alarm_submenu.h"
#include "core/components/home_page.h"
#include "core/components/hw_manager.h"
#include "core/components/malloc_mascot.h"
#include "core/components/sys_paths_manager.h"
#include "core/data_structures/string_list.h"
#include "core/hardware_drivers/core1.h"
#include "core/hardware_drivers/haptics.h"
#include "core/hardware_drivers/joystick.h"
#include "core/hardware_drivers/ssd1306.h"
#include "core/tools/menus.h"
#include "core/utils/path.h"
#include "hardware/adc.h"

path *first_boot_file;

bool is_first_boot() { return (!path_exists(first_boot_file)); }

void write_first_boot_file() { path_ftouch(first_boot_file); }

void sys_setup() {
  gpio_init(25);
  gpio_set_dir(25, true);
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
}

void attach_background_routines() {
  core1_scheduler_add_callback(process_messages);
  core1_scheduler_add_callback(process_alarm);
  core1_scheduler_set_start_flag(true);
}

void sys_mainloop() {
  attach_background_routines();
  bool first_run = true;
  uint8_t loops = 0;
  bool ledvalue = false;
  uint8_t screen_up_seconds = 10;
  uint32_t screen_up_start;
  check_pheripherals();
  while (true) {
    joystick_update(drivers->joystick);
    process_system_state();
    update_conversations();
    if (joystick_get_direction(drivers->joystick) != C || first_run) {
      first_run = false;
      haptic_short_pulse();
      screen_up_start = to_us_since_boot(get_absolute_time()) / 1000000;
      while (true) {
        process_system_state();
        update_conversations();
        display_home_page();
        joystick_update(drivers->joystick);
        if (joystick_get_direction(drivers->joystick) == E) {
          display_main_menu();
          screen_up_start = to_us_since_boot(get_absolute_time()) / 1000000;
        }
        if ((to_us_since_boot(get_absolute_time()) / 1000000) -
                screen_up_start >
            screen_up_seconds)
          break;
      }
      ssd1306_clear(drivers->oled_screen);
      ssd1306_show(drivers->oled_screen);
    }
    if (loops % 10 == 0) {
      gpio_put(25, ledvalue);
      ledvalue = !ledvalue;
    }
    if (loops < 254)
      loops++;
    else
      loops = 0;
  }
}

int main() {
  sys_setup();
  sys_mainloop();
}
