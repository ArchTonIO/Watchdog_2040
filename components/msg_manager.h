#ifndef MSG_MANAGER_H
#define MSG_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

#include "ulcp/ulcp.h"
#include "utils/path.h"

#define MAX_CONTACT_NAME_LENGTH 30
#define MAX_CONTACTS 50

#define INFO_PAGES_TIMEOUT 2000

typedef struct {
  char name[MAX_CONTACT_NAME_LENGTH];
  uint16_t addr;
} contact;

typedef struct {
  char *message;
  char contact_name[MAX_CONTACT_NAME_LENGTH];
  char contact_addr_str[6];
  uint16_t contact_addr;
  bool is_sent;
  uint8_t status;      // 0: delivered, 1: sent, 2: not sent 3: received
  char status_str[10]; // "delivered", "sent", "not sent", "received"
  char timestamp[20];
} msg_record;

typedef struct {
  bool new_msg_arrived;
  bool should_notify;
  contact contacts[MAX_CONTACTS];
  uint16_t received_msgs_count;
  uint8_t contacts_count;
  lora_instance *ulmp_impl;
  path *contacts_addr_file;
  path *contacts_names_file;
} msg_manager;

msg_record *msg_record_init(uint16_t contact_addr,
    char *message,
    bool is_sent,
    uint8_t status);
void msg_record_dump(msg_record *record);
msg_record *msg_record_load(const char *stringified_record);
str_list *get_stored_messages_by_user(uint16_t contact_addr);
void msg_record_free(msg_record *record);
void msg_record_print(msg_record *record);

msg_manager *msg_manager_init(uint16_t my_addr);
void process_messages();
void read_messages();
void send_message();
void add_contact();
void save_contact(char *name, uint16_t addr);
void remove_contact();
void dump_contacts_to_sd();
void scan_online_contacts();
str_list *get_all_contacts();
void enable_message_notifications();
void disable_message_notifications();

extern msg_manager *msg_man_inst;

#endif
