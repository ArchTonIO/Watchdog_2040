#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"

#include "components/home_page.h"
#include "components/hw_manager.h"
#include "components/malloc_mascot.h"
#include "components/msg_manager.h"
#include "data_structures/string_list.h"
#include "device.h"
#include "hardware_drivers/joystick.h"
#include "hardware_drivers/sdcard.h"
#include "hardware_drivers/ssd1306.h"
#include "tools/menus.h"
#include "tools/options_gen.h"
#include "utils/path.h"
#include "utils/utils.h"

path *first_boot_file;

bool is_first_boot() { return (!path_exists(first_boot_file)); }

void write_first_boot_file() { path_ftouch(first_boot_file); }

void create_dir_tree() {
  path *user_file = path_init(USER_FILE);
  path_key_value_dump(user_file,
      'w',
      "username",
      malloc_memories_inst->username);
  path_free(user_file);
  str_list *dirs = list_init();
  char *USER_DIR = string_add(HOME_DIR, malloc_memories_inst->username);
  list_append(dirs, HOME_DIR);
  list_append(dirs, USER_DIR);
  list_append(dirs, string_add(USER_DIR, MALLOC_MASCOT_DIR));
  list_append(dirs, string_add(USER_DIR, MESSAGES_DIR));
  list_append(dirs, string_add(USER_DIR, CONTACTS_DIR));
  list_append(dirs, string_add(USER_DIR, LOGS_DIR));
  list_append(dirs, string_add(USER_DIR, CONFIG_DIR));
  list_append(dirs, string_add(USER_DIR, NOTES_DIR));
  list_append(dirs, string_add(USER_DIR, SENSORS_DIR));
  ssd1306_clear(drivers->oled_screen);
  ssd1306_print(drivers->oled_screen, "Creating sys dir tree", 0, 0, false);
  ssd1306_show(drivers->oled_screen);
  for (uint8_t i = 0; i < dirs->len; i++) {
    path *dir = path_init(get(dirs, i));
    if (path_mkdir(dir)) {
      ssd1306_print(drivers->oled_screen, "[OK] ", 0, 1 + i, false);
      ssd1306_print(drivers->oled_screen, get(dirs, i), 4, 1 + i, false);
    } else {
      ssd1306_print(drivers->oled_screen, "[ERR] ", 0, 1 + i, false);
      ssd1306_print(drivers->oled_screen, get(dirs, i), 5, 1 + i, false);
    }
    ssd1306_show(drivers->oled_screen);
    path_free(dir);
  }
  list_free(dirs);
  ssd1306_clear(drivers->oled_screen);
  ssd1306_show(drivers->oled_screen);
}

void sys_setup() {
  stdio_init_all();
  hardware_drivers_init();
  //  wait_for_user_input();
  first_boot_file = path_init(FIRST_BOOT_FILE);
  if (is_first_boot()) {
    start_malloc_mascot_tutorial();
    write_first_boot_file();
    create_dir_tree();
    dump_malloc_memories_to_sd();
  }
  path_free(first_boot_file);
  load_malloc_memories_from_sd();
  msg_manager_init(malloc_memories_inst->ulmp_addr);
  // msg_manager_init(22345);
  home_page_init();
}

void sys_mainloop() {
  uint8_t screen_up_seconds = 10;
  uint32_t screen_up_start;
  while (true) {
    joystick_update(drivers->joystick);
    check_pheripherals();
    process_system_state();
    if (joystick_get_direction(drivers->joystick) != C) {
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
  }
}

int main() {
  sys_setup();
  sys_mainloop();
}

// int main()
// {
//   sys_setup();
//   uint8_t ledpin = 25;
//   gpio_init(ledpin);
//   gpio_set_dir(ledpin, true);
//   while (true)
//   {
//     gpio_put(ledpin, true);
//     sleep_ms(1000);
//     gpio_put(ledpin, false);
//     sleep_ms(1000);
//   }
// }