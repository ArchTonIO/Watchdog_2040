// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef MSG_MANAGER_H
#define MSG_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

#include "core/ulmp/include/ulmp.h"

#define INFO_PAGES_TIMEOUT 2000
#define MAX_CONVERSATION_UPDATES 50

typedef struct {
  uint16_t contact_addr;
  char *message;
  uint8_t status;
} conversation_update;

typedef struct {
  bool new_msg_arrived;
  bool should_notify;
  uint16_t received_msgs_count;
  lora_instance *ulmp_impl;
  conversation_update conversation_updates[MAX_CONVERSATION_UPDATES];
  uint8_t conversation_updates_count;
  uint16_t answer_addr;
  bool awaiting_answer;
} msg_manager;

msg_manager *msg_manager_init(uint16_t my_addr);
void process_messages();
void process_answer();
void update_conversations();
void read_messages();
void send_message();
void scan_and_send_message();
uint16_t choose_from_contacts();
uint16_t choose_from_online_contacts();
void enable_message_notifications();
void disable_message_notifications();

extern msg_manager *msg_man_inst;

#endif
