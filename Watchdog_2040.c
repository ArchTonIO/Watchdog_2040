#include <stdbool.h>
#include <stdint.h>
#include <sys/_intsup.h>

#include "pico/stdlib.h"

#include "apps/msg_manager/msg_manager.h"
#include "components/home_page.h"
#include "components/hw_manager.h"
#include "components/malloc_mascot.h"
#include "components/sys_paths_manager.h"
#include "data_structures/string_list.h"
#include "hardware/adc.h"
#include "hardware_drivers/haptics.h"
#include "hardware_drivers/joystick.h"
#include "hardware_drivers/ssd1306.h"
#include "tools/menus.h"
#include "utils/path.h"

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
  // wait_for_user_input();
  first_boot_file = path_init(FIRST_BOOT_FILE);
  if (is_first_boot()) {
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
}

void sys_mainloop() {
  uint8_t loops = 0;
  bool ledvalue = false;
  uint8_t screen_up_seconds = 10;
  uint32_t screen_up_start;
  while (true) {
    joystick_update(drivers->joystick);
    check_pheripherals();
    process_system_state();
    if (joystick_get_direction(drivers->joystick) != C) {
      haptic_short_pulse();
      screen_up_start = to_us_since_boot(get_absolute_time()) / 1000000;
      while (true) {
        check_pheripherals();
        process_system_state();
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
