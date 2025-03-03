#include "hardware_drivers/config.h"
#include "pico/rand.h"
#include "stdio.h"
#include "stdint.h"
#include "hardware_drivers/sx1278.h"
#include "ulcp.h"
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "lora_receive.h"
#include "lora_send.h"
#include "data_structures/string_list.h"

message *deserialize_packet(uint8_t *packet)
{
  message *msg = (message *)malloc(sizeof(message));
  header *head = (header *)malloc(sizeof(header));
  uint8_t offset = 0;
  memcpy(&head->src_address, packet + offset, sizeof(uint16_t));
  offset += sizeof(uint16_t);
  memcpy(&head->dest_address, packet + offset, sizeof(uint16_t));
  offset += sizeof(uint16_t);
  head->transaction_uid = (char *)malloc(TRANSACTION_UID_LENGTH + 1);
  memcpy(head->transaction_uid, packet + offset, TRANSACTION_UID_LENGTH);
  head->transaction_uid[TRANSACTION_UID_LENGTH] = '\0';
  offset += TRANSACTION_UID_LENGTH;
  memcpy(&head->packet_type, packet + offset, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  memcpy(&head->payload_length, packet + offset, sizeof(uint16_t));
  offset += sizeof(uint16_t);
  msg->header = head;
  msg->payload = (char *)malloc(head->payload_length + 1);
  memcpy(msg->payload, packet + offset, head->payload_length);
  msg->payload[head->payload_length] = '\0';
  return msg;
}

void free_message(message *msg)
{
  free(msg->header->transaction_uid);
  free(msg->header);
  free(msg->payload);
  free(msg);
}

void on_recv(char *msg)
{
  message *deserialized = deserialize_packet(msg);
  if (deserialized->header->dest_address != this_lora->address)
  {
    free_message(deserialized);
    return;
  }
  if (deserialized->header->packet_type == PING)
  {
    send_pong_packet(deserialized->header->src_address, deserialized->header->transaction_uid);
    free_message(deserialized);
    return;
  }
  if (deserialized->header->packet_type == PONG && strcmp(deserialized->header->transaction_uid, this_lora->tx->sent_transac_uid) == 0)
  {
    printf("PONG RECEIVED\n");
    this_lora->tx->pong_received = true;
    free_message(deserialized);
    return;
  }
  if (deserialized->header->packet_type == ACK && strcmp(deserialized->header->transaction_uid, this_lora->tx->sent_transac_uid) == 0)
  {
    // printf("ACK RECEIVED\n");
    this_lora->tx->ack_received = true;
    free_message(deserialized);
    return;
  }
  if (deserialized->header->packet_type == START)
  {
    lstclear(this_lora->rx->recv_payloads_list);
    strcpy(this_lora->rx->recv_transac_uid, deserialized->header->transaction_uid);
    free_message(deserialized);
    return;
  }
  if (deserialized->header->packet_type == MSG)
  {
    if (strcmp(deserialized->header->transaction_uid, this_lora->rx->recv_transac_uid) != 0)
    {
      free_message(deserialized);
      return;
    }
    lstappend(this_lora->rx->recv_payloads_list, deserialized->payload);
    free_message(deserialized);
    return;
  }
  if (deserialized->header->packet_type == END)
  {
    if (strcmp(deserialized->header->transaction_uid, this_lora->rx->recv_transac_uid) != 0)
    {
      free_message(deserialized);
      this_lora->rx->recv_transac_uid = calloc(TRANSACTION_UID_LENGTH + 1, sizeof(char));
      return;
    }
    this_lora->rx->must_send_ack_dest = deserialized->header->src_address;
    this_lora->rx->must_send_ack_transac_uid = deserialized->header->transaction_uid;
    this_lora->rx->must_send_ack = true;
    printf("TRANSAC RECEIVED\n");
    return;
  }
}