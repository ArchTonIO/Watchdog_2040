// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef SX1278_H
#define SX1278_H

#define REG_00_FIFO 0x00
#define REG_01_OP_MODE 0x01
#define REG_06_FRF_MSB 0x06
#define REG_07_FRF_MID 0x07
#define REG_08_FRF_LSB 0x08
#define REG_09_PA_CONFIG 0x09
#define REG_0E_FIFO_TX_BASE_ADDR 0x0e
#define REG_0F_FIFO_RX_BASE_ADDR 0x0f
#define REG_10_FIFO_RX_CURRENT_ADDR 0x10
#define REG_12_IRQ_FLAGS 0x12
#define REG_13_RX_NB_BYTES 0x13
#define REG_1D_MODEM_CONFIG1 0x1d
#define REG_1E_MODEM_CONFIG2 0x1e
#define REG_19_PKT_SNR_VALUE 0x19
#define REG_1A_PKT_RSSI_VALUE 0x1a
#define REG_20_PREAMBLE_MSB 0x20
#define REG_21_PREAMBLE_LSB 0x21
#define REG_22_PAYLOAD_LENGTH 0x22
#define REG_26_MODEM_CONFIG3 0x26
#define REG_11_REG_OCP 0x2b
#define REG_4D_PA_DAC 0x4d
#define REG_40_DIO_MAPPING1 0x40
#define REG_0D_FIFO_ADDR_PTR 0x0d

#define PA_DAC_ENABLE 0x07
#define PA_DAC_DISABLE 0x04
#define PA_SELECT 0x80
#define CAD_DETECTED_MASK 0x01
#define RX_DONE 0x40
#define TX_DONE 0x08
#define CAD_DONE 0x04
#define CAD_DETECTED 0x01
#define LONG_RANGE_MODE 0x80
#define MODE_SLEEP 0x00
#define MODE_STDBY 0x01
#define MODE_TX 0x03
#define MODE_RXCONTINUOUS 0x05
#define MODE_CAD 0x07
#define REG_OCP_ON (1 << 5)
#define OCP_TRIM(x) ((x) & 0x1F)

#define FXOSC 32000000.0

#include <stdbool.h>

#include "config.h"
#include "hardware/spi.h"

typedef struct {
  pin mosi;
  pin miso;
  pin sck;
  pin cs;
  pin interrupt;
  uint8_t addr;
  spi_inst_t *spi_port;
  uint32_t baudrate;
  uint8_t tx_power;
  uint8_t mode;
  uint8_t packet_sent_timeout_ms;
  volatile uint8_t irq_flags;
  void (*message_received_callback)(char *msg, float rssi);
  bool is_working;
  bool is_on;

} sx1278_t;

sx1278_t *sx1278_init(pin mosi,
    pin miso,
    pin sck,
    pin cs,
    pin interrupt,
    spi_inst_t *spi_port,
    uint32_t baudrate,
    uint8_t tx_power,
    void (*message_received_callback)(char *msg, float rssi));

void sx1278_sleep(sx1278_t *radio);
void sx1278_reset(sx1278_t *radio);
void sx1278_attach_isr(sx1278_t *radio,
    void (*new_callback)(char *msg, float rssi));
void sx1278_set_mode_tx(sx1278_t *radio);
void sx1278_set_mode_rx(sx1278_t *radio);
void sx1278_set_mode_idle(sx1278_t *radio);
void sx1278_send_str(sx1278_t *radio, char *data);
void sx1278_send_raw(sx1278_t *radio, uint8_t *data, size_t length);

#endif