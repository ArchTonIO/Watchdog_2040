#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "data_structures/string_list.h"
#include "hardware_drivers/ssd1306.h"
#include "hardware_drivers/joystick.h"
#include "hardware_drivers/sdcard.h"
#include "home_page.h"
#include "hw_manager.h"
#include "msg_manager.h"
#include "malloc_mascot.h"
#include "options_gen.h"
#include "test.h"
#include "menus.h"
#include "device.h"

bool is_first_startup()
{
  return !sdcard_file_exists(drivers->sd_card, FIRST_TIME_FILE);
}

void write_first_startup_file()
{
  sdcard_write_file(drivers->sd_card, FIRST_TIME_FILE, "", 'w');
}

void sys_setup()
{
  stdio_init_all();
  hardware_drivers_init();
  // wait_for_user_input();
  if (is_first_startup())
  {
    write_first_startup_file();
    start_malloc_mascot_tutorial();
    dump_malloc_memories_to_sd();
  }
  load_malloc_memories_from_sd();
  msg_manager_init(malloc_memories_inst->ulmp_addr);
  home_page_init();
}

void sys_mainloop()
{
  uint8_t screen_up_seconds = 10;
  uint32_t screen_up_start;
  while (true)
  {
    joystick_update(drivers->joystick);
    check_pheripherals();
    process_system_state();
    if (joystick_get_direction(drivers->joystick) != C)
    {
      screen_up_start = to_us_since_boot(get_absolute_time()) / 1000000;
      while (true)
      {
        check_pheripherals();
        process_system_state();
        display_home_page();
        joystick_update(drivers->joystick);
        if (joystick_get_direction(drivers->joystick) == W)
        {
          display_main_menu();
          screen_up_start = to_us_since_boot(get_absolute_time()) / 1000000;
        }
        if ((to_us_since_boot(get_absolute_time()) / 1000000) - screen_up_start > screen_up_seconds)
          break;
      }
      ssd1306_clear(drivers->oled_screen);
      ssd1306_show(drivers->oled_screen);
    }
  }
}

int main()
{
  sys_setup();
  sys_mainloop();
}
