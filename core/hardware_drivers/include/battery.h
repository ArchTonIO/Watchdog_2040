// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef BATTERY_H
#define BATTERY_H

#include <stdbool.h>
#include <stdint.h>

#include "config.h"

typedef struct {
  uint16_t adc_value;
  float voltage;
  uint8_t percentage;
} voltage_level;

enum battery_status {
  UNDEFINED,
  MCP_FAULT,
  NO_BATTERY,
  CHARGING,
  CHARGE_COMPLETE
};

typedef struct {
  pin pg;
  pin stat1;
  pin stat2;
  bool pg_state;
  bool stat1_state;
  bool stat2_state;
} mcp73871_t;

typedef struct {
  float adc_max_value;
  float vsys_measured_voltage;
  float min_voltage;
  float max_voltage;
  pin battery_control_pin;
  uint8_t adc_channel;
  char *battery_percentage_str;
  char *battery_voltage_str;
  char *battery_crude_adc_str;
  mcp73871_t mcp73871;
  uint8_t status;
  bool is_working;
} battery_t;

void mcp73871_init(mcp73871_t *mcp73871, pin pg, pin stat1, pin stat2);

void mcp73871_update_status(mcp73871_t *mcp73871);

void battery_init(battery_t *battery,
    float adc_max_value,
    float min_voltage,
    float max_voltage,
    pin battery_control_pin,
    uint8_t adc_channel,
    mcp73871_t mcp73871);
uint32_t battery_get_crude_adc(battery_t *battery);
uint8_t battery_get_percentage(battery_t *battery);
float battery_get_voltage(battery_t *battery);
char *battery_get_crude_adc_str(battery_t *battery);
char *battery_get_voltage_str(battery_t *battery);
char *battery_get_percentage_str(battery_t *battery);
uint8_t battery_get_status(battery_t *bat);
char *battery_status_to_str(uint8_t status);

#endif