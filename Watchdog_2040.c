#include <stdio.h>
#include "pico/stdlib.h"
#include <stdlib.h>
#include "hw_manager.h"
#include "msg_manager.h"
#include "virtual_keyboard.h"
#include "hardware_drivers/ssd1306.h"
#include "text_editor.h"
#include "hardware_drivers/tests.h"
#include "test.h"

void setup()
{
  stdio_init_all();
  hardware_drivers_init();
}

void mainloop()
{
  wait_for_user_input();
  test_ens160();
  test_rtc();
  test_sx1278();
  // test_message_manager();
}

int main()
{
  setup();
  mainloop();
}