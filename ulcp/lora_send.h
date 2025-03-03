#ifndef LORA_SEND_H
#define LORA_SEND_H

#include "ulcp.h"
#include "hardware_drivers/sx1278.h"

void send_start_packet(uint16_t dest_addr, char *transaction_uid);
void send_end_packet(uint16_t dest_addr, char *transaction_uid);
void send_msg_packet(uint16_t dest_addr, char *transaction_uid, char *payload);
void send_ack_packet(uint16_t dest_addr, char *transaction_uid);
void send_ping_packet(uint16_t dest_addr, char *transaction_uid);
void send_pong_packet(uint16_t dest_addr, char *transaction_uid);

#endif