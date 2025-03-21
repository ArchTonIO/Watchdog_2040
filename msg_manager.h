#ifndef MSG_MANAGER_H
#define MSG_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
  bool new_msg_arrived;
  bool should_notify;
  uint16_t received_msgs_count;
} msg_manager;

msg_manager *msg_manager_init(uint16_t my_addr);
void process_messages();
void read_messages();
void send_message();
void add_contact();
void remove_contact();
void enable_message_notifications();
void disable_message_notifications();

#endif