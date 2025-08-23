// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

// === PATCHED: ulmp.c ===

#include "core/ulmp/ulmp.h"

#include <stdlib.h>
#include <string.h>

#include "pico/rand.h"
#include "pico/stdlib.h"

#include "core/hardware_drivers/sx1278.h"
#include "core/utils/utils.h"
#include "lora_receive.h"
#include "lora_send.h"
#include "stdint.h"
#include "stdio.h"

lora_instance *this_lora;

void reset_ack() {
  this_lora->tx->ack_received = false;
  this_lora->tx->transac_sending_attempts = 0;
}

/**
 * @brief Initializes the LoRa module with the given address and radio
 * instance.
 *
 * This function allocates memory for the LoRa instance and initializes its
 * fields, including the radio instance, address, and transaction fields.
 *
 * @param this_addr The address of this LoRa module.
 * @param sx1278_radio Pointer to the SX1278 radio instance.
 * @return Pointer to the initialized LoRa instance.
 */
lora_instance *lora_init(uint16_t this_addr, sx1278 *sx1278_radio) {
  this_lora = malloc(sizeof(lora_instance));
  this_lora->tx = malloc(sizeof(tx_fields));
  this_lora->rx = malloc(sizeof(rx_fields));
  this_lora->radio = sx1278_radio;
  this_lora->address = this_addr;

  this_lora->tx->transac_sending_attempts = 0;
  this_lora->tx->sent_transac_uid = calloc(TRANSACTION_UID_LENGTH + 1,
      sizeof(char));
  this_lora->tx->ack_received = false;
  this_lora->tx->pong_received = false;

  this_lora->rx->recv_transac_uid = calloc(TRANSACTION_UID_LENGTH + 1,
      sizeof(char));
  this_lora->rx->must_send_ack_transac_uid = calloc(TRANSACTION_UID_LENGTH + 1,
      sizeof(char));
  this_lora->rx->must_send_pong_transac_uid = calloc(
      TRANSACTION_UID_LENGTH + 1,
      sizeof(char));
  this_lora->rx->must_send_ack_dest = 0;
  this_lora->rx->must_send_pong_dest = 0;
  this_lora->rx->must_send_ack = false;
  this_lora->rx->must_send_pong = false;
  this_lora->rx->recv_payloads_buf = malloc(16);

  this_lora->radio->message_received_callback = on_recv;
  return this_lora;
}

/**
 * @brief Puts the lora module in receive mode
 *
 * In receive mode the lora module listens for incoming messages and calls the
 * on_recv callback when a message is received (using an HW interrupt)
 */
void lora_receive() { sx1278_set_mode_rx(this_lora->radio); }

void attempt_single_transaction(uint16_t dest_address, char *payload) {
  char *transaction_uid = gen_random_string(TRANSACTION_UID_LENGTH);
  strncpy(this_lora->tx->sent_transac_uid,
      transaction_uid,
      TRANSACTION_UID_LENGTH);
  this_lora->tx->sent_transac_uid[TRANSACTION_UID_LENGTH] = '\0';
  send_start_packet(dest_address, transaction_uid);

  size_t payload_len = strlen(payload);
  uint8_t num_packets = (payload_len / PAYLOAD_MAX_SIZE) +
                        (payload_len % PAYLOAD_MAX_SIZE ? 1 : 0);
  for (uint8_t i = 0; i < num_packets; i++) {
    size_t chunk_size = (i == num_packets - 1)
                            ? (payload_len % PAYLOAD_MAX_SIZE)
                            : PAYLOAD_MAX_SIZE;
    char *packet_payload = malloc(chunk_size + 1);
    if (!packet_payload) {
      free(transaction_uid);
      return;
    }
    memcpy(packet_payload, payload + i * PAYLOAD_MAX_SIZE, chunk_size);
    packet_payload[chunk_size] = '\0';
    send_msg_packet(dest_address, transaction_uid, packet_payload);
    free(packet_payload);
  }
  send_end_packet(dest_address, transaction_uid);
  free(transaction_uid);
  printf("TRANSAC ENDED\n");
}

/**
 * @brief Sends a message to the specified destination address.
 *
 * This function sends a message over the LoRa network, handling packet
 * segmentation if necessary. It also waits for an acknowledgment (ACK) from
 * the receiver.
 *
 * @param dest_address The address of the destination LoRa module.
 * @param payload      Pointer to the message to be sent (null-terminated
 * string). The payload must not exceed MAX_PAYLOAD_FOR_TRANSACTION, actually
 * set at 2760 bytes (for security reasons), everything beyond that will be
 * ignored.
 *
 * @retval `0`: The message was sent successfully and an ACK was received.
 * @retval `1`: The message was sent successfully but no ACK was received.
 * @retval `2`: The message transmission failed.
 */
uint8_t lora_send_msg(uint16_t dest_address,
    char *payload,
    void (*status_update_callback)(uint8_t progress)) {
  if (strlen(payload) > MAX_PAYLOAD_FOR_TRANSACTION)
    return 2;

  while (!this_lora->tx->ack_received &&
         this_lora->tx->transac_sending_attempts < MAX_SENDING_ATTEMPTS) {
    this_lora->tx->transac_sending_attempts++;
    attempt_single_transaction(dest_address, payload);
    status_update_callback(this_lora->tx->transac_sending_attempts);
    lora_receive();
    sleep_ms(TRANSAC_TIMEOUT - 200);
  }

  uint8_t result = this_lora->tx->ack_received ? 0 : 1;
  reset_ack();
  return result;
}

/**
 * @brief Sends a ping packet to the specified destination address.
 *
 * This function sends a ping packet to the specified destination address and
 * waits for a pong response.
 *
 * @param dest_address The address of the destination LoRa module.
 * @return `0` if pong is received, `1` if pong is not received.
 */
uint8_t lora_ping(uint16_t dest_address) {
  char *transaction_uid = gen_random_string(TRANSACTION_UID_LENGTH);
  strncpy(this_lora->tx->sent_transac_uid,
      transaction_uid,
      TRANSACTION_UID_LENGTH);
  this_lora->tx->sent_transac_uid[TRANSACTION_UID_LENGTH] = '\0';
  send_ping_packet(dest_address, transaction_uid);
  free(transaction_uid);
  sleep_ms(10);
  lora_receive();
  sleep_ms(TRANSAC_TIMEOUT);
  if (this_lora->tx->pong_received) {
    this_lora->tx->pong_received = false;
    return 0;
  }
  return 1;
}

/**
 * @brief Sends an acknowledgment packet to the specified destination address.
 *
 * This function sends an acknowledgment packet to the specified destination
 * address calls a notification callback.
 * This function should be called in a loop to ensure that ACKs are sent
 * whenever necessary.
 *
 * @param notify A callback function that is called after sending the ACK
 */
void lora_send_ack(void (*notify)(uint16_t src_address)) {
  if (!this_lora->rx->must_send_ack)
    return;
  send_ack_packet(this_lora->rx->must_send_ack_dest,
      this_lora->rx->must_send_ack_transac_uid);
  this_lora->rx->must_send_ack = false;
  sleep_ms(PACKET_TIMEOUT);
  notify(this_lora->rx->must_send_ack_dest);
  lora_receive();
}

/**
 * @brief Sends a pong packet in response to a ping request.
 *
 * This function sends a pong packet to the specified destination address if
 * there is a pending ping request.
 * This function should be called in a loop to ensure that pong packets
 * are sent whenever necessary.
 */
void lora_send_pong() {
  if (!this_lora->rx->must_send_pong)
    return;
  send_pong_packet(this_lora->rx->must_send_pong_dest,
      this_lora->rx->must_send_pong_transac_uid);
  this_lora->rx->must_send_pong = false;
  sleep_ms(PACKET_TIMEOUT);
  lora_receive();
}

/**
 * @brief Resets the receive buffer to its initial small size to prevent memory
 * leaks.
 *
 * This function should be called after processing a received message to
 * reclaim memory that was allocated during message reception.
 */
void lora_reset_recv_buffer() {
  if (this_lora->rx->recv_payloads_buf) {
    free(this_lora->rx->recv_payloads_buf);
    this_lora->rx->recv_payloads_buf = malloc(16);
    this_lora->rx->recv_payloads_buf[0] = '\0';
  }
}