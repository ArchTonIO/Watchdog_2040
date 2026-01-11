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
  bool is_working;
} battery;

battery *battery_init(float adc_max_value,
    float min_voltage,
    float max_voltage,
    pin battery_control_pin,
    uint8_t adc_channel);
uint32_t battery_get_crude_adc(battery *battery);
uint8_t battery_get_percentage(battery *battery);
float battery_get_voltage(battery *battery);
char *battery_get_crude_adc_str(battery *battery);
char *battery_get_voltage_str(battery *battery);
char *battery_get_percentage_str(battery *battery);

#endif