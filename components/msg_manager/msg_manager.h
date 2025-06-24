#ifndef MSG_MANAGER_H
#define MSG_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

#include "ulmp/ulmp.h"

#define INFO_PAGES_TIMEOUT 2000

typedef struct {
  bool new_msg_arrived;
  bool should_notify;
  uint16_t received_msgs_count;
  lora_instance *ulmp_impl;
} msg_manager;

msg_manager *msg_manager_init(uint16_t my_addr);
void process_messages();
void read_messages();
void send_message();
void scan_online_contacts();
void enable_message_notifications();
void disable_message_notifications();

extern msg_manager *msg_man_inst;

#endif
