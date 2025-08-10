#include "lora_send.h"

#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"

#include "hardware_drivers/sx1278.h"
#include "stdint.h"
#include "ulmp.h"

uint8_t *serialize_packet(message *msg);

uint8_t *build_packet(uint16_t src_addr,
    uint16_t dest_addr,
    uint8_t packet_type,
    char *transaction_uid,
    char *payload);

void send_start_packet(uint16_t dest_addr, char *transaction_uid) {
  sleep_ms(PACKET_TIMEOUT);
  uint8_t *packet = build_packet(this_lora->address,
      dest_addr,
      START,
      transaction_uid,
      "");
  sx1278_send_raw(this_lora->radio, packet, HEADER_SIZE);
  free(packet);
}

void send_end_packet(uint16_t dest_addr, char *transaction_uid) {
  sleep_ms(PACKET_TIMEOUT);
  uint8_t *packet = build_packet(this_lora->address,
      dest_addr,
      END,
      transaction_uid,
      "");
  sx1278_send_raw(this_lora->radio, packet, HEADER_SIZE);
  free(packet);
}

void send_msg_packet(uint16_t dest_addr,
    char *transaction_uid,
    char *payload) {
  sleep_ms(PACKET_TIMEOUT);
  uint8_t *packet = build_packet(this_lora->address,
      dest_addr,
      MSG,
      transaction_uid,
      payload);
  sx1278_send_raw(this_lora->radio, packet, HEADER_SIZE + strlen(payload));
  free(packet);
}

void send_ack_packet(uint16_t dest_addr, char *transaction_uid) {
  sleep_ms(PACKET_TIMEOUT);
  uint8_t *packet = build_packet(this_lora->address,
      dest_addr,
      ACK,
      transaction_uid,
      "");
  sx1278_send_raw(this_lora->radio, packet, HEADER_SIZE);
  free(packet);
}

void send_ping_packet(uint16_t dest_addr, char *transaction_uid) {
  sleep_ms(PACKET_TIMEOUT);
  uint8_t *packet = build_packet(this_lora->address,
      dest_addr,
      PING,
      transaction_uid,
      "");
  sx1278_send_raw(this_lora->radio, packet, HEADER_SIZE);
  free(packet);
}

void send_pong_packet(uint16_t dest_addr, char *transaction_uid) {
  sleep_ms(PACKET_TIMEOUT);
  uint8_t *packet = build_packet(this_lora->address,
      dest_addr,
      PONG,
      transaction_uid,
      "");
  sx1278_send_raw(this_lora->radio, packet, HEADER_SIZE);
  free(packet);
}

uint8_t *serialize_packet(message *msg) {
  size_t packet_size = HEADER_SIZE + msg->header->payload_length;
  uint8_t *buf = calloc(packet_size, sizeof(uint8_t));
  if (!buf)
    return NULL;

  uint8_t offset = 0;
  memcpy(buf + offset, &msg->header->src_address, sizeof(uint16_t));
  offset += sizeof(uint16_t);
  memcpy(buf + offset, &msg->header->dest_address, sizeof(uint16_t));
  offset += sizeof(uint16_t);
  memcpy(buf + offset, msg->header->transaction_uid, TRANSACTION_UID_LENGTH);
  offset += TRANSACTION_UID_LENGTH;
  memcpy(buf + offset, &msg->header->packet_type, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  memcpy(buf + offset, &msg->header->payload_length, sizeof(uint16_t));
  offset += sizeof(uint16_t);
  memcpy(buf + offset, msg->payload, msg->header->payload_length);

  return buf;
}

uint8_t *build_packet(uint16_t src_addr,
    uint16_t dest_addr,
    uint8_t packet_type,
    char *transaction_uid,
    char *payload) {
  message *msg = malloc(sizeof(message));
  header *head = malloc(sizeof(header));
  head->src_address = src_addr;
  head->dest_address = dest_addr;
  head->transaction_uid = strdup(transaction_uid);
  head->packet_type = packet_type;
  head->payload_length = (uint16_t)strlen(payload);
  msg->header = head;
  msg->payload = strdup(payload);
  uint8_t *serialized = serialize_packet(msg);
  free(head->transaction_uid);
  free(msg->payload);
  free(head);
  free(msg);
  return serialized;
}
