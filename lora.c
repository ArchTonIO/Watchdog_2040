#include "hardware_config.h"
#include "pico/rand.h"
#include "stdio.h"
#include "stdint.h"
#include "sx1278.h"
#include "lora.h"
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"

lora_instance *this_lora;

void mes_received_callback(char *msg)
{
  printf("Message received: %s\n", msg);
}

/*
* @brief Initializes the lora hardware module, self address and recv callback

* @param this_addr: the address of this lora module
*/
void init_lora(uint16_t this_addr)
{
  this_lora = malloc(sizeof(lora_instance));
  this_lora->address = this_addr;
  this_lora->on_recv_callback = mes_received_callback;
  this_lora->radio = sx1278_init(
      SX1278_MOSI,
      SX1278_MISO,
      SX1278_SCK,
      SX1278_CS,
      SX1278_INTERRUPT,
      0,
      SX1278_SPI_PORT,
      SX1278_SPI_BAUDRATE,
      SX1278_TX_POWER,
      this_lora->on_recv_callback);
}

/*
 * @brief Puts the lora module in receive mode
 */
void lora_receive()
{
  sx1278_set_mode_rx(this_lora->radio);
}

char *gen_random_string(uint8_t length)
{
  char *s = malloc(length + 1);
  for (uint8_t i = 0; i < length; i++)
  {
    s[i] = 'A' + get_rand_32() % 26;
  }
  s[length] = '\0';
  return s;
}

char *stringify_packet(message *msg)
{
  size_t packet_size = HEADER_SIZE + msg->header->payload_length;
  char *buf = (char *)calloc(packet_size, sizeof(uint8_t));
  if (!buf)
  {
    return NULL;
  };
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
  offset += msg->header->payload_length;
  return buf;
}

char *build_packet(
    uint16_t src_addr,
    uint16_t dest_addr,
    uint8_t packet_type,
    char *transaction_uid,
    char *payload)
{
  message *msg = (message *)malloc(sizeof(message));
  header *head = (header *)malloc(sizeof(header));
  head->src_address = src_addr;
  head->dest_address = dest_addr;
  head->transaction_uid = transaction_uid;
  head->packet_type = packet_type;
  head->payload_length = (uint16_t)strlen(payload);
  msg->header = head;
  msg->payload = payload;
  char *stringed = stringify_packet(msg);
  free(head);
  free(msg);
  return stringed;
}

void send_start_packet(uint16_t dest_addr, char *transaction_uid)
{
  char *packet = build_packet(
      this_lora->address,
      dest_addr,
      START,
      transaction_uid,
      "");
  sx1278_send_raw(this_lora->radio, packet, HEADER_SIZE);
  free(packet);
  sleep_ms(PACKET_TIMEOUT);
}

void send_end_packet(uint16_t dest_addr, char *transaction_uid)
{
  char *packet = build_packet(
      this_lora->address,
      dest_addr,
      END,
      transaction_uid,
      "");
  sx1278_send_raw(this_lora->radio, packet, HEADER_SIZE);
  free(packet);
  sleep_ms(PACKET_TIMEOUT);
}

void send_msg_packet(uint16_t dest_addr, char *transaction_uid, char *payload)
{
  char *packet = build_packet(
      this_lora->address,
      dest_addr,
      MSG,
      transaction_uid,
      payload);
  sx1278_send_raw(this_lora->radio, packet, HEADER_SIZE + strlen(payload));
  free(packet);
  sleep_ms(PACKET_TIMEOUT);
}

void send_ack_packet(uint16_t dest_addr, char *transaction_uid)
{
  char *packet = build_packet(
      this_lora->address,
      dest_addr,
      ACK,
      transaction_uid,
      "");
  sx1278_send_raw(this_lora->radio, packet, HEADER_SIZE);
  free(packet);
  sleep_ms(PACKET_TIMEOUT);
}

void send_ping_packet(uint16_t dest_addr, char *transaction_uid)
{
  char *packet = build_packet(
      this_lora->address,
      dest_addr,
      PING,
      transaction_uid,
      "");
  sx1278_send_raw(this_lora->radio, packet, HEADER_SIZE);
  free(packet);
  sleep_ms(PACKET_TIMEOUT);
}

void send_pong_packet(uint16_t dest_addr, char *transaction_uid)
{
  char *packet = build_packet(
      this_lora->address,
      dest_addr,
      PONG,
      transaction_uid,
      "");
  sx1278_send_raw(this_lora->radio, packet, HEADER_SIZE);
  free(packet);
  sleep_ms(PACKET_TIMEOUT);
}

/*
 * @brief Sends a message to a destination address
 *
 * @param dest_address: the address of the destination lora module
 * @param payload: the message to be sent
 */
uint8_t send_msg(uint16_t dest_address, char *payload)
{
  char *transaction_uid = gen_random_string(TRANSACTION_UID_LENGTH);
  send_start_packet(dest_address, transaction_uid);
  size_t payload_len = strlen(payload);
  uint8_t num_packets = (payload_len / PAYLOAD_MAX_SIZE) + (payload_len % PAYLOAD_MAX_SIZE ? 1 : 0);
  for (uint8_t i = 0; i < num_packets; i++)
  {
    size_t chunk_size = (i == num_packets - 1) ? (payload_len % PAYLOAD_MAX_SIZE) : PAYLOAD_MAX_SIZE;
    char *packet_payload = (char *)malloc(chunk_size + 1);
    if (!packet_payload)
    {
      return 1;
    }
    memcpy(packet_payload, payload + i * PAYLOAD_MAX_SIZE, chunk_size);
    packet_payload[chunk_size] = '\0';
    send_msg_packet(dest_address, transaction_uid, packet_payload);
    free(packet_payload);
  }
  send_end_packet(dest_address, transaction_uid);
  return 0;
}