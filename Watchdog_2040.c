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
#include "home_page.h"

void setup()
{
  stdio_init_all();
  hardware_drivers_init();
}

void mainloop()
{
  display_bootup_screen();
}

int main()
{
  setup();
  mainloop();
}