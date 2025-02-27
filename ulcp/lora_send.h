#ifndef LORA_SEND_H
#define LORA_SEND_H

#include "ulcp.h"
#include "hardware_drivers/sx1278.h"

void send_start_packet(lora_instance *this_lora, uint16_t dest_addr, char *transaction_uid);
void send_end_packet(lora_instance *this_lora, uint16_t dest_addr, char *transaction_uid);
void send_msg_packet(lora_instance *this_lora, uint16_t dest_addr, char *transaction_uid, char *payload);
void send_ack_packet(lora_instance *this_lora, uint16_t dest_addr, char *transaction_uid);
void send_ping_packet(lora_instance *this_lora, uint16_t dest_addr, char *transaction_uid);
void send_pong_packet(lora_instance *this_lora, uint16_t dest_addr, char *transaction_uid);

#endif