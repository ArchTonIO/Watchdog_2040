#ifndef MSG_MANAGER_H
#define MSG_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_CONTACT_NAME_LENGTH 30
#define MAX_CONTACTS 50

typedef struct
{
  char name[MAX_CONTACT_NAME_LENGTH];
  uint16_t addr;
} contact;

typedef struct
{
  bool new_msg_arrived;
  bool should_notify;
  contact contacts[MAX_CONTACTS];
  uint16_t received_msgs_count;
  uint8_t contacts_count;
} msg_manager;

msg_manager *msg_manager_init(uint16_t my_addr);
void process_messages();
void read_messages();
void send_message();
void add_contact();
void save_contact(char *name, uint16_t addr);
void remove_contact();
str_list *get_all_contacts();
void enable_message_notifications();
void disable_message_notifications();

extern msg_manager *msg_man_inst;

#endif