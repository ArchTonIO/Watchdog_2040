#include <stdio.h>
#include "pico/stdlib.h"
#include <stdlib.h>
#include <stdbool.h>
#include "hw_manager.h"
#include "msg_manager.h"
#include "hardware_drivers/battery.h"
#include "hardware_drivers/ssd1306.h"
#include "hardware_drivers/joystick.h"
#include "test.h"
#include "home_page.h"
#include "bootup.h"
#include "malloc_mascot.h"

void sys_setup()
{
  stdio_init_all();
  hardware_drivers_init();
  msg_manager_init(23097);
  home_page_init(23097);
}

void sys_mainloop()
{
  while (true)
  {
    joystick_update(drivers->joystick);
    check_pheripherals();
    process_system_state();
    if (drivers->joystick->button_pressed)
    {
      uint8_t screen_up_seconds = 10;
      uint32_t screen_up_start = to_us_since_boot(get_absolute_time()) / 1000000;
      while (true)
      {
        check_pheripherals();
        process_system_state();
        display_home_page();
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