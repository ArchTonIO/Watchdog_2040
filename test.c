#include <stdio.h>
#include "pico/stdlib.h"
#include <stdlib.h>
#include <stdint.h>
#include "hw_manager.h"
#include "virtual_keyboard.h"
#include "hardware_drivers/ssd1306.h"
#include "msg_manager.h"
#include "text_editor.h"
#include "test.h"
#include "pico/stdio_usb.h"

void wait_for_user_input()
{
  while (!stdio_usb_connected())
  {
    sleep_ms(100);
  }
  printf("Watchdog_2040 tester, send any key to continue...\n");
  while (1)
  {
    getchar();
    break;
  }
}

void test_text_editor()
{
  virtual_keyboard *keyboard = virtual_keyboard_init();
  ssd1306_clear(drivers->oled_screen);
  text_editor *editor = text_editor_init(keyboard, false);
  while (1)
  {
    draw_keyboard(keyboard);
    char *paragraph = text_editor_get_buf(editor);
    ssd1306_clear(drivers->oled_screen);
    printf("Paragraph: %s\n", paragraph);
    free(paragraph);
  }
}

void test_message_manager()
{
  int this_addr;
  printf("Select the ulcp address: ");
  scanf("%d", &this_addr);
  msg_manager_init((uint16_t)this_addr);
  save_contact("Giustino", 12345);
  save_contact("Leone", 23456);
  save_contact("Ottaviano", 34567);
  while (1)
  {
    int action;
    printf("Select the action: 0: add contact, 1: send message, 2: spin, 3: print contacts, 4: delete contact\n");
    scanf("%d", &action);
    if (action == 0)
      add_contact();
    else if (action == 1)
      send_message();
    else if (action == 2)
      while (1)
      {
        process_messages();
      }
    else if (action == 3)
    {
      str_list *contacts = get_all_contacts();
      lstprint(contacts);
      free(contacts);
    }
    else if (action == 4)
      remove_contact();
    else
      printf("Invalid action\n");
  }
}