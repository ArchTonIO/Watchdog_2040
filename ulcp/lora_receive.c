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

void on_recv(char *msg)
{
  printf("Message received: %s\n", msg);
}