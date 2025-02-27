/*
An Uncomplicated LoRa Communication Protocol (ULCP)
Author: Antonio Del Cogliano
*/

#ifndef ULCP_H
#define ULCP_H

/*Headers fields lenght*/
#define SRC_ADDRESS_LEN 2
#define DEST_ADDRESS_LEN 2
#define TRANSACTION_UID_LENGTH 8
#define PAYLOAD_TYPE_LEN 1
#define PAYLOAD_LENGTH_LEN 2
#define PACKET_MAX_SIZE 230
#define HEADER_SIZE (SRC_ADDRESS_LEN + DEST_ADDRESS_LEN + TRANSACTION_UID_LENGTH + PAYLOAD_TYPE_LEN + PAYLOAD_LENGTH_LEN)

/*Payload max size for each packet*/
#define PAYLOAD_MAX_SIZE (PACKET_MAX_SIZE - HEADER_SIZE)

/*The number of milliseconds to wait after a packet is sent*/
#define PACKET_TIMEOUT 200

/*Packet types*/
#define PING 0x00
#define PONG 0x01
#define START 0x02
#define END 0x03
#define MSG 0x04
#define ACK 0x05

#include "hardware_drivers/config.h"
#include "stdio.h"
#include "stdint.h"
#include "hardware_drivers/sx1278.h"

typedef struct
{
  sx1278 *radio;
  uint16_t address;
  void (*on_recv_callback)(char *msg);
} lora_instance;

typedef struct
{
  uint16_t src_address;
  uint16_t dest_address;
  char *transaction_uid;
  uint8_t packet_type;
  uint16_t payload_length;
} header;

typedef struct
{
  header *header;
  char *payload;
} message;

void lora_init(uint16_t this_addr);
void lora_receive();
uint8_t lora_send_msg(uint16_t dest_address, char *payload);
uint8_t lora_ping(uint16_t dest_address);

#endif