#include <stdio.h>
#include "pico/stdlib.h"
#include <stdlib.h>
#include "hw_manager.h"
#include "msg_manager.h"
#include "virtual_keyboard.h"
#include "hardware_drivers/ssd1306.h"
#include "text_editor.h"

void setup()
{
  stdio_init_all();
  hardware_drivers_init();
}

void mainloop()
{
  virtual_keyboard *keyboard = virtual_keyboard_init();
  ssd1306_clear(drivers->oled_screen);
  text_editor *editor = text_editor_init(keyboard);
  getchar();
  while (1)
  {
    draw_keyboard(keyboard);
    char *paragraph = text_editor_start(editor);
    ssd1306_clear(drivers->oled_screen);
    printf("Paragraph: %s\n", paragraph);
    free(paragraph);
  }
}

int main()
{
  setup();
  mainloop();
}