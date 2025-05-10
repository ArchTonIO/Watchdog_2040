#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware_drivers/sdcard.h"
#include "hardware_drivers/ssd1306.h"
#include "hardware_drivers/rtc_time.h"
#include "data_structures/string_list.h"
#include "text_editor.h"
#include "ulcp/ulcp.h"
#include "bitmaps.h"
#include "utils.h"
#include "hw_manager.h"
#include "msg_manager.h"
#include "options_gen.h"

msg_manager *msg_man_inst;

void notify(uint16_t src_address);
char *compose_message();
uint16_t select_contact();
void delete_contact(uint16_t addr);
char *ask_for_contact_name();
uint16_t ask_for_contact_addr();
void display_sent_message_status(uint8_t result, uint16_t dest_addr);
void save_contact(char *name, uint16_t addr);
uint16_t find_contact_addr_by_name(char *name);
char *find_contact_name_by_addr(uint16_t addr);
void display_received_message(uint16_t src_address);

/**
 * @brief Initializes the message manager with the given address.
 *
 * @param my_addr The address of the device.
 * @return A pointer to the initialized message manager.
 */
msg_manager *msg_manager_init(uint16_t my_addr)
{
  if (my_addr == 0)
  {
    printf("[MSG MANAGER] (ERR): the ulcp address of a device cannot be 0\n");
    return NULL;
  }
  msg_manager *msg_man = (msg_manager *)malloc(sizeof(msg_manager));
  msg_man->new_msg_arrived = false;
  msg_man->should_notify = true;
  msg_man->contacts_count = 0;
  msg_man->received_msgs_count = 0;
  lora_init(my_addr, drivers->lora_module, notify);
  lora_receive();
  msg_man_inst = msg_man;
  return msg_man;
}

/**
 * @brief Processes incoming messages and sends acknowledgments if necessary.
 */
void process_messages()
{
  lora_eventually_send_ack();
  sleep_ms(10);
}

void read_messages()
{
  //! TODO implement
}

/**
 * @brief Sends a message to a selected contact.
 */
void send_message()
{
  uint16_t dest_addr = select_contact();
  char *msg = compose_message();
  printf("message: %s, will be sent to %u\n", msg, dest_addr);
  uint8_t result = lora_send_msg(dest_addr, msg);
  display_sent_message_status(result, dest_addr);
}

/**
 * @brief Add a new contact to the contact list.
 */
void add_contact()
{
  char *name = ask_for_contact_name();
  uint16_t addr = ask_for_contact_addr();
  save_contact(name, addr);
  char addr_str[8];
  sprintf(addr_str, "%u", addr);
  ssd1306_print(drivers->oled_screen, "Contact saved !", 2, 0, false);
  ssd1306_print(drivers->oled_screen, name, 0, 7, false);
  ssd1306_print(drivers->oled_screen, ":", strlen(name) + 1, 7, false);
  ssd1306_print(drivers->oled_screen, addr_str, strlen(name) + 3, 7, false);
  ssd1306_draw_bitmap(drivers->oled_screen, 50, 22, contact_saved, 28, 20, 0);
  ssd1306_show(drivers->oled_screen);
  free(name);
  sleep_ms(3000);
  ssd1306_clear(drivers->oled_screen);
  ssd1306_show(drivers->oled_screen);
}

/**
 * @brief Removes a contact from the contact list.
 */
void remove_contact()
{
  uint16_t to_remove = select_contact();
  str_list *options = list_init();
  list_append(options, "Yes");
  list_append(options, "No");
  options_page *yesno_page = options_page_init("You sure?", options);
  char *sure = options_page_launch(yesno_page);
  free(options);
  free(yesno_page);
  if (strcmp(sure, "Yes") == 0)
    delete_contact(to_remove);
}

void notify(uint16_t src_address)
{
  msg_man_inst->new_msg_arrived = true;
  msg_man_inst->received_msgs_count++;
  if (msg_man_inst->should_notify)
    printf("a new message has been received");
}

void save_contact(char *name, uint16_t addr)
{
  strncpy(msg_man_inst->contacts[msg_man_inst->contacts_count].name, name, strlen(name) + 1);
  msg_man_inst->contacts[msg_man_inst->contacts_count].addr = addr;
  msg_man_inst->contacts_count++;
}

void delete_contact(uint16_t addr)
{
  for (uint16_t i = 0; i < msg_man_inst->contacts_count; i++)
  {
    if (msg_man_inst->contacts[i].addr == addr)
    {
      for (uint16_t j = i; j < msg_man_inst->contacts_count - 1; j++)
      {
        msg_man_inst->contacts[j] = msg_man_inst->contacts[j + 1];
      }
      msg_man_inst->contacts_count--;
      break;
    }
  }
}

str_list *get_all_contacts()
{
  str_list *contacts = list_init();
  for (uint16_t i = 0; i < msg_man_inst->contacts_count; i++)
  {
    list_append(contacts, msg_man_inst->contacts[i].name);
  }
  return contacts;
}

uint16_t find_contact_addr_by_name(char *name)
{
  for (uint16_t i = 0; i < msg_man_inst->contacts_count; i++)
  {
    if (strcmp(msg_man_inst->contacts[i].name, name) == 0)
      return msg_man_inst->contacts[i].addr;
  }
  return 0;
}

char *find_contact_name_by_addr(uint16_t addr)
{
  for (uint16_t i = 0; i < msg_man_inst->contacts_count; i++)
  {
    if (msg_man_inst->contacts[i].addr == addr)
      return msg_man_inst->contacts[i].name;
  }
  return NULL;
}

char *compose_message()
{
  text_editor *editor = text_editor_launch("# Type in your message");
  char *message = text_editor_get_buf(editor);
  text_editor_kill(editor);
  return message;
}

uint16_t select_contact()
{
  str_list *contacts = get_all_contacts();
  options_page *page = options_page_init("Select a contact", contacts);
  char *name = options_page_launch(page);
  options_page_free(page);
  return find_contact_addr_by_name(name);
}

char *ask_for_contact_name()
{
  text_editor *editor = text_editor_launch("# Type in the contact name");
  char *name = text_editor_get_buf(editor);
  text_editor_kill(editor);
  return name;
}

uint16_t ask_for_contact_addr()
{
  text_editor *editor = text_editor_launch("# Type in the contact address");
  char *temp = text_editor_get_buf(editor);
  text_editor_kill(editor);
  uint16_t addr = 0;
  if (sscanf(temp, "%hu", &addr) == 1)
  {
    return addr;
  }
  printf("Conversion error!\n");
  return 0;
}

void display_sent_message_status(uint8_t status, uint16_t dest_addr)
{
  char *name = find_contact_name_by_addr(dest_addr);
  ssd1306_print(drivers->oled_screen, "Message status:", 0, 0, false);
  ssd1306_print(drivers->oled_screen, "sent to", 5, 2, false);
  ssd1306_print(drivers->oled_screen, "recv by", 5, 4, false);
  ssd1306_print(drivers->oled_screen, name, 14, 2, false);
  ssd1306_print(drivers->oled_screen, name, 14, 4, false);
  if (status == 0)
  {
    ssd1306_print(drivers->oled_screen, "[OK]", 0, 2, false);
    ssd1306_print(drivers->oled_screen, "[OK]", 0, 4, false);
  }
  else if (status == 1)
  {
    ssd1306_print(drivers->oled_screen, "[OK]", 0, 2, false);
    ssd1306_print(drivers->oled_screen, "[NO]", 0, 4, false);
  }
  else if (status == 2)
  {
    ssd1306_print(drivers->oled_screen, "[NO]", 0, 2, false);
    ssd1306_print(drivers->oled_screen, "[NO]", 0, 4, false);
  }
  ssd1306_show(drivers->oled_screen);
}

void display_received_message(uint16_t src_address)
{
  char *name = find_contact_name_by_addr(src_address);
  ssd1306_print(drivers->oled_screen, "New message from", 0, 0, false);
  ssd1306_print(drivers->oled_screen, "New message from", 16, 0, false);
  ssd1306_draw_bitmap(drivers->oled_screen, 50, 22, message_received, 28, 20, 0);
  ssd1306_show(drivers->oled_screen);
}

void scan_online_contacts()
{
  // todo implement
}

void enable_message_notifications()
{
  msg_man_inst->should_notify = true;
}

void disable_message_notifications()
{
  msg_man_inst->should_notify = false;
}
