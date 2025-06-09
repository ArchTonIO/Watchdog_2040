/*
An Uncomplicated LoRa Communication Protocol (ULCP)
Author: Antonio Del Cogliano
*/

#include <stdint.h>

#include "hardware_drivers/sx1278.h"

#ifndef ULCP_H
#define ULCP_H

/*Headers fields lenght*/
#define SRC_ADDRESS_LEN 2
#define DEST_ADDRESS_LEN 2
#define TRANSACTION_UID_LENGTH 8
#define PACKET_TYPE_LEN 1
#define PAYLOAD_LENGTH_LEN 2
#define PACKET_MAX_SIZE 135
#define HEADER_SIZE                                                            \
  (SRC_ADDRESS_LEN + DEST_ADDRESS_LEN + TRANSACTION_UID_LENGTH +               \
      PACKET_TYPE_LEN + PAYLOAD_LENGTH_LEN)

/*Payload max size for each packet*/
#define PAYLOAD_MAX_SIZE (PACKET_MAX_SIZE - HEADER_SIZE)

/*The number of milliseconds to wait after a packet is sent*/
#define PACKET_TIMEOUT 200

/*The number of milliseconds to wait after a transaction is sent*/
#define TRANSAC_TIMEOUT 600

/*The limit of packets for a single transaction*/
#define MAX_PACKET_FOR_TRANSACTION 25

/*The limit of payload for a single transaction*/
#define MAX_PAYLOAD_FOR_TRANSACTION                                            \
  ((MAX_PACKET_FOR_TRANSACTION - 2) *                                          \
      PAYLOAD_MAX_SIZE) /*-2 for start and end packets*/

/*The number of retries for sending a transaction*/
#define MAX_SENDING_ATTEMPTS 5

/*Packet types*/
#define PING 0x00
#define PONG 0x01
#define START 0x02
#define END 0x03
#define MSG 0x04
#define ACK 0x05

typedef struct {
  uint8_t transac_sending_attempts;
  char *sent_transac_uid;
  bool ack_received;
  bool pong_received;
} tx_fields;

typedef struct {
  char *recv_transac_uid;
  char *must_send_ack_transac_uid;
  uint16_t must_send_ack_dest;
  bool must_send_ack;
  char *recv_payloads_buf;
} rx_fields;

typedef struct {
  sx1278 *radio;
  uint16_t address;
  tx_fields *tx;
  rx_fields *rx;
} lora_instance;

typedef struct {
  uint16_t src_address;
  uint16_t dest_address;
  char *transaction_uid;
  uint8_t packet_type;
  uint16_t payload_length;
} header;

typedef struct {
  header *header;
  char *payload;
} message;

extern lora_instance *this_lora;

lora_instance *lora_init(uint16_t this_addr, sx1278 *sx1278_radio);
void lora_receive();
uint8_t lora_send_msg(uint16_t dest_address,
    char *payload,
    void (*status_update_callback)(uint8_t progress));
uint8_t lora_ping(uint16_t dest_address);
void lora_send_ack(void (*notify)(uint16_t src_address));

#endif