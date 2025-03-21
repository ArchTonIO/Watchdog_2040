#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware_drivers/sdcard.h"
#include "hardware_drivers/rtc_time.h"
#include "data_structures/string_list.h"
#include "ulcp/ulcp.h"
#include "utils.h"
#include "hw_manager.h"
#include "msg_manager.h"

msg_manager *msg_man_inst;

void eventually_save_received_msg();
void eventually_save_sent_msg(uint16_t dest_addr, char *message);
void notify(uint16_t src_address);
char *compose_message();
uint16_t select_contact();
char *ask_for_contact_name();
uint16_t ask_for_contact_addr();
void display_sent_message_status(uint8_t result, uint16_t dest_addr);
void save_contact(char *name, uint16_t addr);
uint16_t find_contact_addr_by_name(char *name);
char *find_contact_name_by_addr(uint16_t addr);
void display_received_message(uint16_t src_address);

msg_manager *msg_manager_init(uint16_t my_addr)
{
  msg_manager *msg_man = (msg_manager *)malloc(sizeof(msg_manager));
  msg_man->new_msg_arrived = false;
  msg_man->should_notify = true;
  lora_init(my_addr, drivers->lora_module, notify);
  lora_receive();
  msg_man_inst = msg_man;
  return msg_man;
}

void process_messages()
{
  lora_eventually_send_ack();
  sleep_ms(10);
  // eventually_save_received_msg();
}

void read_messages()
{
  //! TODO implement
}

void send_message()
{
  uint16_t dest_addr = 2;
  // uint16_t dest_addr = select_contact();
  char *msg = "ciao";
  // char *msg = compose_message();
  printf("message: %s, will be sent to %u\n", msg, dest_addr);
  uint8_t result = lora_send_msg(dest_addr, msg);
  display_sent_message_status(result, dest_addr);
  if (result == 0)
    eventually_save_sent_msg(dest_addr, msg);
}

void add_contact()
{
  // char *name = ask_for_contact_name();
  // uint16_t addr = ask_for_contact_addr();
  char *name = "flavio";
  uint16_t addr = 2;
  printf("saving contact %s with address %u\n", name, addr);
  save_contact(name, addr);
  // free(name);
}

void remove_contact()
{
  //! TODO implement
}

void eventually_save_received_msg()
{
  if (!msg_man_inst->new_msg_arrived)
    return;
  char addr[6];
  sprintf(addr, "%u", this_lora->rx->must_send_ack_dest);
  char *to_write = string_add(rtc_time_now(drivers->rtc), " - new message received from: ");
  to_write = string_add(to_write, addr);
  to_write = string_add(to_write, "\n");
  to_write = string_add(to_write, this_lora->rx->recv_payloads_buf);
  sdcard_write_file(drivers->sd_card, "messages.txt", to_write, 'a');
  msg_man_inst->new_msg_arrived = false;
}

void eventually_save_sent_msg(uint16_t dest_addr, char *message)
{
  char addr[6];
  sprintf(addr, "%u", dest_addr);
  char *to_write = string_add(rtc_time_now(drivers->rtc), " - message sent to: ");
  to_write = string_add(to_write, addr);
  to_write = string_add(to_write, "\n");
  to_write = string_add(to_write, message);
  sdcard_write_file(drivers->sd_card, "messages.txt", to_write, 'a');
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
  char addr_str[8];
  sprintf(addr_str, "%u", addr);
  size_t total_len = strlen(name) + strlen(addr_str) + 3; // name~addr\n\0
  char *to_write = malloc(total_len);
  if (!to_write)
    return;
  snprintf(to_write, total_len, "%s~%s\n", name, addr_str);
  sdcard_write_file(drivers->sd_card, "contacts.txt", to_write, 'a');
  free(to_write);
  printf("Contact saved\n");
}

uint16_t find_contact_addr_by_name(char *name)
{
  str_list *contacts_file_content = sdcard_read_file(drivers->sd_card, "contacts.txt");
  for (uint16_t i = 0; i < contacts_file_content->len; i++)
  {
    char *line = lstget(contacts_file_content, i);
    char *delimiter = strchr(line, '~');
    *delimiter = '\0';
    uint16_t addr;
    if (strcmp(line, name) == 0)
      if (sscanf(delimiter + 1, "%hu", &addr) == 1)
        return addr;
  }
  return 0;
}

char *find_contact_name_by_addr(uint16_t addr)
{
  char *name = (char *)malloc(10);
  str_list *contacts_file_content = sdcard_read_file(drivers->sd_card, "contacts.txt");
  for (uint16_t i = 0; i < contacts_file_content->len; i++)
  {
    char *line = lstget(contacts_file_content, i);
    char *delimiter = strchr(line, '~');
    *delimiter = '\0';
    uint16_t line_addr;
    if (sscanf(delimiter + 1, "%hu", &line_addr) == 1)
      if (line_addr == addr)
        return line;
  }
  return NULL;
}

// !mock function, must be replaced with actual screen and joystick implementation
char *compose_message()
{
  printf("\nEnter the message: ");
  char msg[100];
  char *output_msg = (char *)malloc(100);
  uint8_t i = 0;
  char ch;
  while ((ch = getchar()) != '\n' && i < sizeof(msg) - 1)
  {
    msg[i++] = ch;
  }
  msg[i] = '\0';
  strcpy(output_msg, msg);
  return output_msg;
}

// !mock function, must be replaced with actual screen and joystick implementation
uint16_t select_contact()
{
  printf("\nEnter the contact name: ");
  char name[10];
  uint8_t i = 0;
  char ch;
  while ((ch = getchar()) != '\n' && i < sizeof(name) - 1)
  {
    name[i++] = ch;
  }
  name[i] = '\0';
  return find_contact_addr_by_name(name);
}

//! mock function, must be replaced with actual screen and joystick implementation
char *ask_for_contact_name()
{
  printf("\nEnter the contact name: ");
  char temp[64];
  uint8_t i = 0;
  char ch;
  while ((ch = getchar()) != '\n' && i < sizeof(temp) - 1)
  {
    temp[i++] = ch;
  }
  temp[i] = '\0';
  char *output_name = malloc(strlen(temp) + 1);
  if (!output_name)
    return NULL;
  strcpy(output_name, temp);
  return output_name;
}

//! mock function, must be replaced with actual screen and joystick implementation
uint16_t ask_for_contact_addr()
{
  printf("\nEnter the contact address: ");
  char temp[16];
  uint8_t i = 0;
  char ch;
  while ((ch = getchar()) != '\n' && i < sizeof(temp) - 1)
    temp[i++] = ch;
  temp[i] = '\0';
  uint16_t addr = 0;
  if (sscanf(temp, "%hu", &addr) == 1)
  {
    return addr;
  }
  printf("Conversion error!\n");
  return 0;
}

//! mock function, must be replaced with actual screen and joystick implementation
void display_sent_message_status(uint8_t status, uint16_t dest_addr)
{
  if (status == 0)
    printf("\nThe message has been received\n");
  else if (status == 1)
    printf("\nThe message has been sent but no ack was received\n");
  else if (status == 2)
    printf("\nThe message has not been sent\n");
  return;
}

//! mock function, must be replaced with actual screen and joystick implementation
void display_received_message(uint16_t src_address)
{
  printf("New message from: %s\n", find_contact_name_by_addr(src_address));
}

void enable_message_notifications()
{
  msg_man_inst->should_notify = true;
}

void disable_message_notifications()
{
  msg_man_inst->should_notify = false;
}
