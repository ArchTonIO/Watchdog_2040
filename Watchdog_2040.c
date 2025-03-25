#include <stdio.h>
#include "pico/stdlib.h"
#include <stdlib.h>
#include "hw_manager.h"
#include "msg_manager.h"
#include "virtual_keyboard.h"
#include "hardware_drivers/ssd1306.h"

#define THIS_LORA_ADDR 2

void directly_test_sd()
{
  getchar();
  printf("writing to sd card\n");
  sdcard_write_file(drivers->sd_card, "test.txt", "hello world\n", 'a');
  printf("reading from sd card\n");
  str_list *content = sdcard_read_file(drivers->sd_card, "test.txt");
  lstprint(content);
  lstclear(content);
  printf("list files\n");
  content = sdcard_list_files(drivers->sd_card);
  lstprint(content);
  lstdel(content);
  printf("done\n");
}

void setup()
{
  stdio_init_all();
  hardware_drivers_init();
  msg_manager_init(THIS_LORA_ADDR);
}

void mainloop()
{
  getchar();
  // add_contact();
  send_message();
  printf("Done\n");
}

void dev_1()
{
  setup();
  mainloop();
}

void dev_2()
{
  setup();
  while (1)
  {
    process_messages();
  }
}

int main()
{
  // dev_2();
  setup();
  virtual_keyboard *keyboard = virtual_keyboard_init();
  draw_keyboard(keyboard);
  char *word = virtual_keyboard_write(keyboard);
  printf("Word: %s\n", word);
  free(word);
  return 0;
}