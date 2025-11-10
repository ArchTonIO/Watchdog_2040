// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#ifndef ENS160_H
#define ENS160_H

#include <stdbool.h>
#include <stdint.h>

#include "config.h"
#include "hardware/i2c.h"

typedef struct {
  pin sda;
  pin sck;
  i2c_inst_t *i2c_port;
  uint32_t baudrate;
  uint32_t address;
  bool is_working;
} ens160;

ens160 *ens160_init(pin sda,
    pin sck,
    i2c_inst_t *i2c_port,
    uint32_t baudrate,
    uint32_t address);
uint8_t ens160_get_op_mode(ens160 *sensor);
void ens160_set_op_mode(ens160 *sensor, uint8_t mode);
int16_t ens160_read_co2(ens160 *sensor);
int16_t ens160_read_tvoc(ens160 *sensor);
uint8_t ens160_read_aqi(ens160 *sensor);
void ens160_power_down(ens160 *sensor);
void ens160_power_up(ens160 *sensor);
void ens160_reset(ens160 *sensor);

#endif