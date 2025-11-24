// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include "lora_receive.h"

#include <stdlib.h>
#include <string.h>

#include "core/data_structures/string_list.h"
#include "core/ulmp/ulmp.h"
#include "stdint.h"

message *deserialize_packet(uint8_t *packet) {
  message *msg = malloc(sizeof(message));
  header *head = malloc(sizeof(header));
  uint8_t offset = 0;
  memcpy(&head->src_address, packet + offset, sizeof(uint16_t));
  offset += sizeof(uint16_t);
  memcpy(&head->dest_address, packet + offset, sizeof(uint16_t));
  offset += sizeof(uint16_t);
  head->transaction_uid = malloc(TRANSACTION_UID_LENGTH + 1);
  memcpy(head->transaction_uid, packet + offset, TRANSACTION_UID_LENGTH);
  head->transaction_uid[TRANSACTION_UID_LENGTH] = '\0';
  offset += TRANSACTION_UID_LENGTH;
  memcpy(&head->packet_type, packet + offset, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  memcpy(&head->payload_length, packet + offset, sizeof(uint16_t));
  offset += sizeof(uint16_t);
  msg->header = head;
  msg->payload = malloc(head->payload_length + 1);
  memcpy(msg->payload, packet + offset, head->payload_length);
  msg->payload[head->payload_length] = '\0';
  return msg;
}

void free_message(message *msg) {
  if (!msg)
    return;
  if (msg->header) {
    free(msg->header->transaction_uid);
    free(msg->header);
  }
  free(msg->payload);
  free(msg);
}

void on_recv(char *msg, float rssi) {
  message *deserialized = deserialize_packet((uint8_t *)msg);
  if (deserialized->header->dest_address != this_lora->address) {
    free_message(deserialized);
    return;
  }
  this_lora->rx->last_rssi = rssi;
  switch (deserialized->header->packet_type) {
  case PING:
    strcpy(this_lora->rx->must_send_pong_transac_uid,
        deserialized->header->transaction_uid);
    this_lora->rx->must_send_pong_dest = deserialized->header->src_address;
    this_lora->rx->must_send_pong = true;
    break;
  case PONG:
    if (strcmp(deserialized->header->transaction_uid,
            this_lora->tx->sent_transac_uid) == 0) {
      this_lora->tx->pong_received = true;
    }
    break;
  case ACK:
    this_lora->tx->ack_received = true;
    break;
  case START:
    this_lora->rx->recv_payloads_buf[0] = '\0';
    strcpy(this_lora->rx->recv_transac_uid,
        deserialized->header->transaction_uid);
    break;
  case MSG:
    if (strcmp(deserialized->header->transaction_uid,
            this_lora->rx->recv_transac_uid) == 0) {
      size_t new_size = strlen(this_lora->rx->recv_payloads_buf) +
                        strlen(deserialized->payload) + 1;
      char *new_buf = realloc(this_lora->rx->recv_payloads_buf, new_size);
      if (new_buf) {
        this_lora->rx->recv_payloads_buf = new_buf;
        strcat(this_lora->rx->recv_payloads_buf, deserialized->payload);
      }
    }
    break;
  case END:
    if (strcmp(deserialized->header->transaction_uid,
            this_lora->rx->recv_transac_uid) == 0) {
      this_lora->rx->must_send_ack_dest = deserialized->header->src_address;
      strcpy(this_lora->rx->must_send_ack_transac_uid,
          deserialized->header->transaction_uid);
      this_lora->rx->must_send_ack = true;
    } else {
      free(this_lora->rx->recv_transac_uid);
      this_lora->rx->recv_transac_uid = calloc(TRANSACTION_UID_LENGTH + 1,
          sizeof(char));
    }
    break;
  }
  free_message(deserialized);
}