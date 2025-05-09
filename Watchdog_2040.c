#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hw_manager.h"
#include "msg_manager.h"
#include "hardware_drivers/battery.h"
#include "hardware_drivers/ssd1306.h"
#include "hardware_drivers/joystick.h"
#include "home_page.h"
#include "malloc_mascot.h"
#include "hardware_drivers/sdcard.h"
#include "data_structures/string_list.h"
#include "test.h"
#include "options_gen.h"
#include "menus.h"

#define FIRST_TIME_FILE ".booted"

str_list *calls_stack = NULL;

bool is_first_startup()
{
  str_list *sd_files = sdcard_list_files(drivers->sd_card);
  bool first_time = false;
  if (index_of(sd_files, FIRST_TIME_FILE) == -1)
  {
    sdcard_write_file(drivers->sd_card, FIRST_TIME_FILE, "", 'w');
    first_time = true;
  }
  else
    first_time = false;
  lstdel(sd_files);
  free(sd_files);
  return first_time;
}

void sys_setup()
{
  stdio_init_all();
  hardware_drivers_init();
  // wait_for_user_input();
  if (is_first_startup())
  {
    start_malloc_mascot_tutorial();
    dump_malloc_memories_to_sd();
  }
  load_malloc_memories_from_sd();
  msg_manager_init(malloc_memories_inst->ulmp_addr);
  home_page_init();
  calls_stack = list();
}

void sys_mainloop()
{
  while (true)
  {
    joystick_update(drivers->joystick);
    check_pheripherals();
    process_system_state();
    if (drivers->joystick->button_pressed || joystick_get_direction(drivers->joystick) != C)
    {
      uint8_t screen_up_seconds = 10;
      uint32_t screen_up_start = to_us_since_boot(get_absolute_time()) / 1000000;
      while (true)
      {
        display_home_page();
        joystick_update(drivers->joystick);
        if (joystick_get_direction(drivers->joystick) == W)
        {
          display_main_menu();
          ssd1306_clear(drivers->oled_screen);
          ssd1306_show(drivers->oled_screen);
          break;
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