#include "hardware_drivers/config.h"
#include "pico/rand.h"
#include "stdio.h"
#include "stdint.h"
#include "hardware_drivers/sx1278.h"
#include "ulcp.h"
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "lora_send.h"
#include "lora_receive.h"

lora_instance *this_lora;

void reset_ack();

/**
 * @brief Initializes the lora hardware module, self address and recv callback
 * @param this_addr: the address of this lora module
 */
lora_instance *lora_init(
    uint16_t this_addr,
    sx1278 *sx1278_radio,
    void (*on_transac_ended_callback)(uint16_t src_address))
{
  this_lora = malloc(sizeof(lora_instance));
  this_lora->tx = malloc(sizeof(tx_fields));
  this_lora->rx = malloc(sizeof(rx_fields));
  this_lora->radio = sx1278_radio;
  this_lora->address = this_addr;
  /*tx fields initialization*/
  this_lora->tx->transac_sending_attempts = 0;
  this_lora->tx->sent_transac_uid = (char *)calloc(TRANSACTION_UID_LENGTH + 1, sizeof(char));
  this_lora->tx->ack_received = false;
  this_lora->tx->pong_received = false;
  /*rx fields initialization*/
  this_lora->rx->recv_transac_uid = (char *)calloc(TRANSACTION_UID_LENGTH + 1, sizeof(char));
  this_lora->rx->must_send_ack_transac_uid = (char *)calloc(TRANSACTION_UID_LENGTH + 1, sizeof(char));
  this_lora->rx->must_send_ack_dest = 0;
  this_lora->rx->must_send_ack = false;
  this_lora->rx->recv_payloads_buf = (char *)malloc(16);
  this_lora->radio->message_received_callback = on_recv;
  this_lora->rx->on_transac_ended_callback = on_transac_ended_callback;
  return this_lora;
}

void reset_ack()
{
  this_lora->tx->ack_received = false;
  this_lora->tx->transac_sending_attempts = 0;
}

/**
 * @brief Puts the lora module in receive mode
 *
 * In receive mode the lora module listens for incoming messages and calls the on_recv callback
 * when a message is received (using an HW interrupt)
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

void attempt_single_transaction(uint16_t dest_address, char *payload)
{
  char *transaction_uid = gen_random_string(TRANSACTION_UID_LENGTH);
  this_lora->tx->sent_transac_uid = transaction_uid;
  send_start_packet(dest_address, transaction_uid);
  size_t payload_len = strlen(payload);
  uint8_t num_packets = (payload_len / PAYLOAD_MAX_SIZE) + (payload_len % PAYLOAD_MAX_SIZE ? 1 : 0);
  for (uint8_t i = 0; i < num_packets; i++)
  {
    size_t chunk_size = (i == num_packets - 1) ? (payload_len % PAYLOAD_MAX_SIZE) : PAYLOAD_MAX_SIZE;
    char *packet_payload = (char *)malloc(chunk_size + 1);
    if (!packet_payload)
    {
      return;
    }
    memcpy(packet_payload, payload + i * PAYLOAD_MAX_SIZE, chunk_size);
    packet_payload[chunk_size] = '\0';
    send_msg_packet(dest_address, transaction_uid, packet_payload);
    free(packet_payload);
  }
  send_end_packet(dest_address, transaction_uid);
  printf("TRANSAC ENDED\n");
}

/**
 * @brief Sends a message to the specified destination address.
 *
 * This function sends a message over the LoRa network, handling packet segmentation
 * if necessary. It also waits for an acknowledgment (ACK) from the receiver.
 *
 * @param dest_address The address of the destination LoRa module.
 * @param payload      Pointer to the message to be sent (null-terminated string).
 * The payload must not exceed MAX_PAYLOAD_FOR_TRANSACTION, actually set at 2760 bytes
 * (for security reasons), everything beyond that will be ignored.
 *
 * @retval `0`: The message was sent successfully and an ACK was received.
 * @retval `1`: The message was sent successfully but no ACK was received.
 * @retval `2`: The message transmission failed.
 */
uint8_t lora_send_msg(uint16_t dest_address, char *payload, void (*status_update_callback)(uint8_t progress))
{
  if (strlen(payload) > MAX_PAYLOAD_FOR_TRANSACTION)
    return 2;
  while (!this_lora->tx->ack_received && this_lora->tx->transac_sending_attempts < MAX_SENDING_ATTEMPTS)
  {
    this_lora->tx->transac_sending_attempts++;
    attempt_single_transaction(dest_address, payload);
    status_update_callback(this_lora->tx->transac_sending_attempts);
    lora_receive();
    sleep_ms(TRANSAC_TIMEOUT - 200);
  }
  if (this_lora->tx->ack_received)
  {
    reset_ack();
    return 0;
  }
  if (this_lora->tx->transac_sending_attempts >= MAX_SENDING_ATTEMPTS)
  {
    reset_ack();
    return 1;
  }
  reset_ack();
  return 2;
}

/**
 * @brief Sends a ping message to the specified destination address.
 *
 * This function sends a ping message to the specified destination address.
 *
 * @param dest_address The address of the destination LoRa module.
 *
 * @retval `0`: The ping message was sent successfully and a pong message was received.
 * @retval `1`: The ping message was sent successfully but no pong message was received.
 */
uint8_t lora_ping(uint16_t dest_address)
{
  char *transaction_uid = gen_random_string(TRANSACTION_UID_LENGTH);
  this_lora->tx->sent_transac_uid = transaction_uid;
  send_ping_packet(dest_address, transaction_uid);
  sleep_ms(TRANSAC_TIMEOUT);
  if (this_lora->tx->pong_received)
  {
    this_lora->tx->pong_received = false;
    return 0;
  }
  return 1;
}

void lora_eventually_send_ack()
{
  if (!this_lora->rx->must_send_ack)
    return;
  send_ack_packet(this_lora->rx->must_send_ack_dest, this_lora->rx->must_send_ack_transac_uid);
  this_lora->rx->must_send_ack = false;
  sleep_ms(PACKET_TIMEOUT);
  lora_receive();
  printf("ACK SENT\n");
}