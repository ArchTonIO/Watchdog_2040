// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "include/sx1278.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/time.h"

#include "hardware/gpio.h"
#include "include/config.h"

sx1278_t *instance = NULL;

void spi_write_reg_single_byte(sx1278_t *radio, uint8_t reg, uint8_t payload);
void spi_write_reg_multi_byte(sx1278_t *radio,
    uint8_t reg,
    uint8_t *payload,
    size_t length);
uint8_t spi_read_reg_single_byte(sx1278_t *radio, uint8_t reg);
void spi_read_reg_multi_byte(sx1278_t *radio,
    uint8_t reg,
    uint8_t *buffer,
    size_t length);
bool wait_packet_sent(sx1278_t *radio);
void irq_handler(uint gpio, uint32_t events);
void message_sent_callback();
void message_received_callback();

/**
 * @brief Initializes the SX1278 radio module.
 *
 * This function sets up the SPI communication, configures the GPIO pins,
 * and initializes the radio module with default settings.
 *
 * @param mosi The MOSI pin for SPI communication.
 * @param miso The MISO pin for SPI communication.
 * @param sck The SCK pin for SPI communication.
 * @param cs The chip select pin for the radio module.
 * @param interrupt The interrupt pin for the radio module.
 * @param spi_port The SPI port to use.
 * @param baudrate The baud rate for SPI communication.
 * @param tx_power The transmission power level (5-23).
 * @param message_received_callback Callback function to handle received
 * messages.
 * @return A pointer to the initialized sx1278 instance.
 */
sx1278_t *sx1278_init(pin mosi,
    pin miso,
    pin sck,
    pin cs,
    pin interrupt,
    spi_inst_t *spi_port,
    uint32_t baudrate,
    uint8_t tx_power,
    void (*message_received_callback)(char *msg, float rssi)) {
  sx1278_t *new_radio = (sx1278_t *)malloc(sizeof(sx1278_t));
  new_radio->mosi = mosi;
  new_radio->miso = miso;
  new_radio->sck = sck;
  new_radio->cs = cs;
  new_radio->interrupt = interrupt;
  new_radio->spi_port = spi_port;
  new_radio->baudrate = baudrate;
  new_radio->tx_power = tx_power;
  new_radio->mode = 0;
  new_radio->irq_flags = 0;
  new_radio->packet_sent_timeout_ms = 200;
  new_radio->message_received_callback = message_received_callback;
  new_radio->is_working = true;
  new_radio->is_on = true;

  // interrupt set up
  gpio_init(interrupt);
  gpio_set_dir(interrupt, GPIO_IN);
  gpio_set_irq_enabled_with_callback(interrupt,
      GPIO_IRQ_EDGE_RISE,
      true,
      irq_handler);

  // spi setup
  spi_init(spi_port, baudrate);
  gpio_set_function(sck, GPIO_FUNC_SPI);
  gpio_set_function(mosi, GPIO_FUNC_SPI);
  gpio_set_function(miso, GPIO_FUNC_SPI);

  // cs setup
  gpio_init(cs);
  gpio_set_dir(cs, GPIO_OUT);
  gpio_put(cs, 1);

  // set mode
  spi_write_reg_single_byte(new_radio,
      REG_01_OP_MODE,
      MODE_SLEEP | LONG_RANGE_MODE);
  sleep_ms(10);

  // check initialisation
  uint8_t reg_01_op_mode_content = spi_read_reg_single_byte(new_radio,
      REG_01_OP_MODE);
  if (reg_01_op_mode_content != (MODE_SLEEP | LONG_RANGE_MODE)) {
    printf("SX1278 initialization error, reading on REG_01_OP_MODE resulted "
           "in: %d, expecting: %d\n",
        reg_01_op_mode_content,
        MODE_SLEEP | LONG_RANGE_MODE);
    new_radio->is_working = false;
    return new_radio;
  }

  spi_write_reg_single_byte(new_radio, REG_0E_FIFO_TX_BASE_ADDR, 0x00);
  spi_write_reg_single_byte(new_radio, REG_0F_FIFO_RX_BASE_ADDR, 0x00);
  sx1278_set_mode_idle(new_radio);

  // set modem config
  spi_write_reg_single_byte(new_radio, REG_1D_MODEM_CONFIG1, 0x72);
  spi_write_reg_single_byte(new_radio, REG_1E_MODEM_CONFIG2, 0x74);
  spi_write_reg_single_byte(new_radio, REG_26_MODEM_CONFIG3, 0x04);

  // set preamble length
  spi_write_reg_single_byte(new_radio, REG_20_PREAMBLE_MSB, 0x00);
  spi_write_reg_single_byte(new_radio, REG_21_PREAMBLE_LSB, 0x08);

  // set frequency
  spi_write_reg_single_byte(new_radio, REG_06_FRF_MSB, 0x6c);
  spi_write_reg_single_byte(new_radio, REG_07_FRF_MID, 0x40);
  spi_write_reg_single_byte(new_radio, REG_08_FRF_LSB, 0x00);

  // set tx power
  spi_write_reg_single_byte(new_radio, REG_4D_PA_DAC, 0x87);
  spi_write_reg_single_byte(new_radio,
      REG_09_PA_CONFIG,
      (1 << 7) | (7 << 4) | (15));
  spi_write_reg_single_byte(new_radio,
      REG_11_REG_OCP,
      REG_OCP_ON | OCP_TRIM(27));
  instance = new_radio;
  return new_radio;
}

/**
 * @brief Attaches a new ISR callback to the radio instance.
 *
 * This function allows the user to set a custom callback function that
 * will be called when a message is received by the radio.
 *
 * @param radio Pointer to the sx1278 radio instance.
 * @param new_callback The callback function to be called when a message is
 * received.
 */
void sx1278_attach_isr(sx1278_t *radio,
    void (*new_callback)(char *msg, float rssi)) {
  radio->message_received_callback = new_callback;
}

void sx1278_reset(sx1278_t *radio) {
  spi_write_reg_single_byte(radio,
      REG_01_OP_MODE,
      MODE_SLEEP | LONG_RANGE_MODE);
  spi_write_reg_single_byte(radio, REG_12_IRQ_FLAGS, 0x80);
}

/**
 * @brief Sets the radio module to sleep mode.
 *
 * This function puts the radio module into sleep mode, which is the lowest
 * power consumption state. It can be woken up by setting it to a different
 * mode.
 *
 * @param radio Pointer to the sx1278 radio instance.
 */
void sx1278_sleep(sx1278_t *radio) {
  if (radio->mode != MODE_SLEEP) {
    spi_write_reg_single_byte(radio, REG_01_OP_MODE, MODE_SLEEP);
    radio->mode = MODE_SLEEP;
    radio->is_on = false;
  }
}

/**
 * @brief Sets the radio module to transmit mode.
 *
 * This function puts the radio module into transmit mode, allowing it to send
 * data. It also configures the DIO mapping for transmission.
 *
 * @param radio Pointer to the sx1278 radio instance.
 */
void sx1278_set_mode_tx(sx1278_t *radio) {
  if (radio->mode != MODE_TX) {
    spi_write_reg_single_byte(radio, REG_01_OP_MODE, MODE_TX);
    spi_write_reg_single_byte(radio, REG_40_DIO_MAPPING1, 0x40);
    radio->mode = MODE_TX;
  }
}

/**
 * @brief Sets the radio module to receive mode.
 *
 * This function puts the radio module into continuous receive mode, allowing
 * it to listen for incoming messages. It also configures the DIO mapping for
 * receiving.
 *
 * @param radio Pointer to the sx1278 radio instance.
 */
void sx1278_set_mode_rx(sx1278_t *radio) {
  if (radio->mode != MODE_RXCONTINUOUS) {
    spi_write_reg_single_byte(radio, REG_01_OP_MODE, MODE_RXCONTINUOUS);
    spi_write_reg_single_byte(radio, REG_40_DIO_MAPPING1, 0x00);
    radio->mode = MODE_RXCONTINUOUS;
    radio->is_on = true;
  }
}

/**
 * @brief Sets the radio module to idle mode.
 *
 * This function puts the radio module into standby mode, which is a low-power
 * state that allows for quick transitions to transmit or receive modes.
 *
 * @param radio Pointer to the sx1278 radio instance.
 */
void sx1278_set_mode_idle(sx1278_t *radio) {
  if (radio->mode != MODE_STDBY) {
    spi_write_reg_single_byte(radio, REG_01_OP_MODE, MODE_STDBY);
    radio->mode = MODE_STDBY;
  }
}

/**
 * @brief Waits for the packet to be sent.
 *
 * This function blocks until the radio module is no longer in transmit mode,
 * indicating that the packet has been sent. It uses a timeout to prevent
 * indefinite blocking.
 *
 * @param radio Pointer to the sx1278 radio instance.
 * @return `true` if the packet was sent successfully, `false` if the timeout
 * occurred.
 */
bool wait_packet_sent(sx1278_t *radio) {
  uint32_t start_ms = to_ms_since_boot(get_absolute_time());
  while ((to_ms_since_boot(get_absolute_time()) - start_ms) <
         radio->packet_sent_timeout_ms) {
    if (radio->mode != MODE_TX) {
      return true;
    }
  }
  return false;
}

/**
 * @brief Sends a string message using the radio module.
 *
 * This function prepares the radio module to send a string message by
 * writing the data to the FIFO buffer and setting the payload length.
 * It then switches the radio to transmit mode.
 *
 * @param radio Pointer to the sx1278 radio instance.
 * @param data The string data to be sent (null-terminated).
 */
void sx1278_send_str(sx1278_t *radio, char *data) {
  wait_packet_sent(radio);
  sx1278_set_mode_idle(radio);
  spi_write_reg_single_byte(radio, REG_0D_FIFO_ADDR_PTR, 0x00);
  spi_write_reg_multi_byte(radio, REG_00_FIFO, (uint8_t *)data, strlen(data));
  spi_write_reg_single_byte(radio, REG_22_PAYLOAD_LENGTH, strlen(data));
  sx1278_set_mode_tx(radio);
}

/**
 * @brief Sends raw data using the radio module.
 *
 * This function prepares the radio module to send raw bytes data by writing
 * the data to the FIFO buffer and setting the payload length. It then switches
 * the radio to transmit mode.
 *
 * @param radio Pointer to the sx1278 radio instance.
 * @param data Pointer to the raw data to be sent.
 * @param length The length of the raw data in bytes.
 */
void sx1278_send_raw(sx1278_t *radio, uint8_t *data, size_t length) {
  wait_packet_sent(radio);
  sx1278_set_mode_idle(radio);
  spi_write_reg_single_byte(radio, REG_0D_FIFO_ADDR_PTR, 0x00);
  spi_write_reg_multi_byte(radio, REG_00_FIFO, (uint8_t *)data, length);
  spi_write_reg_single_byte(radio, REG_22_PAYLOAD_LENGTH, length);
  sx1278_set_mode_tx(radio);
}

void spi_write_reg_single_byte(sx1278_t *radio, uint8_t reg, uint8_t payload) {
  uint8_t buffer[2] = {reg | 0x80, payload};
  gpio_put(radio->cs, 0);
  spi_write_blocking(radio->spi_port, buffer, 2);
  gpio_put(radio->cs, 1);
}

void spi_write_reg_multi_byte(sx1278_t *radio,
    uint8_t reg,
    uint8_t *payload,
    size_t length) {
  uint8_t buffer[length + 1];
  buffer[0] = reg | 0x80;
  memcpy(&buffer[1], payload, length);
  gpio_put(radio->cs, 0);
  spi_write_blocking(radio->spi_port, buffer, length + 1);
  gpio_put(radio->cs, 1);
}

uint8_t spi_read_reg_single_byte(sx1278_t *radio, uint8_t reg) {
  uint8_t tx_buffer = reg & 0x7F;
  uint8_t rx_buffer[2] = {0};
  gpio_put(radio->cs, 0);
  spi_write_read_blocking(radio->spi_port, &tx_buffer, rx_buffer, 2);
  gpio_put(radio->cs, 1);
  return rx_buffer[1];
}

void spi_read_reg_multi_byte(sx1278_t *radio,
    uint8_t reg,
    uint8_t *buffer,
    size_t length) {
  uint8_t tx_buffer = reg & 0x7F;
  uint8_t rx_buffer[length + 1];
  gpio_put(radio->cs, 0);
  spi_write_read_blocking(radio->spi_port, &tx_buffer, rx_buffer, length + 1);
  gpio_put(radio->cs, 1);
  memcpy(buffer, &rx_buffer[1], length);
}

void irq_handler(uint gpio, uint32_t event_mask) {
  if (!instance || !instance->is_working) {
    printf(
        "[ERROR] IRQ handler called but instance is NULL or not working.\n");
    return;
  }
  instance->irq_flags = spi_read_reg_single_byte(instance, REG_12_IRQ_FLAGS);
  spi_write_reg_single_byte(instance, REG_12_IRQ_FLAGS, instance->irq_flags);
  if (instance->mode == MODE_TX && instance->irq_flags & TX_DONE) {
    message_sent_callback();
    return;
  } else if (
      instance->mode == MODE_RXCONTINUOUS && instance->irq_flags & RX_DONE) {
    message_received_callback();
    return;
  }
}

void message_sent_callback() { sx1278_set_mode_idle(instance); }

void message_received_callback() {
  uint8_t length = spi_read_reg_single_byte(instance, REG_13_RX_NB_BYTES);
  if (length == 0 || length > 255) {
    printf("[WARN] Invalid RX packet length: %u\n", length);
    return;
  }
  int8_t raw_snr = (int8_t)spi_read_reg_single_byte(instance,
      REG_1B_PKT_SNR_VALUE);
  float snr = raw_snr / 4.0f;

  uint8_t raw_rssi = spi_read_reg_single_byte(instance, REG_1A_PKT_RSSI_VALUE);
  float rssi = (snr < 0) ? (-164 + raw_rssi + snr) : (-164 + raw_rssi);
  spi_write_reg_single_byte(instance,
      REG_0D_FIFO_ADDR_PTR,
      spi_read_reg_single_byte(instance, REG_10_FIFO_RX_CURRENT_ADDR));
  static uint8_t buffer[256];
  spi_read_reg_multi_byte(instance, REG_00_FIFO, buffer, length);
  buffer[length] = '\0';
  if (instance->message_received_callback) {
    instance->message_received_callback((char *)buffer, rssi);
  }
}
