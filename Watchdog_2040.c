#include <stdio.h>
#include "pico/stdlib.h"
#include <stdlib.h>
#include "hw_manager.h"
#include "msg_manager.h"
#include "virtual_keyboard.h"
#include "hardware_drivers/ssd1306.h"
#include "options_gen.h"
#include "text_editor.h"
#include "hardware_drivers/tests.h"
#include "test.h"
#include "data_structures/string_list.h"

void setup()
{
  stdio_init_all();
  wait_for_user_input();
  hardware_drivers_init();
}

void mainloop()
{
  test_message_manager();
}

int main()
{
  setup();
  mainloop();
}