// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef ENS160_H
#define ENS160_H

#include <stdbool.h>
#include <stdint.h>

#include "hardware/i2c.h"

typedef struct {
  i2c_inst_t *i2c_port;
  uint32_t address;
  bool is_working;
  bool is_on;
  bool manually_turned_off;
} ens160_t;

void ens160_init(ens160_t *ens160, i2c_inst_t *i2c_port, uint32_t address);
bool ens160_is_working(ens160_t *sensor);
uint8_t ens160_get_op_mode(ens160_t *sensor);
void ens160_set_op_mode(ens160_t *sensor, uint8_t mode);
int16_t ens160_read_co2(ens160_t *sensor);
int16_t ens160_read_tvoc(ens160_t *sensor);
uint8_t ens160_read_aqi(ens160_t *sensor);
void ens160_power_down(ens160_t *sensor);
void ens160_power_up(ens160_t *sensor);
void ens160_reset(ens160_t *sensor);

#endif