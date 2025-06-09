#ifndef BATTERY_H
#define BATTERY_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "config.h"

typedef struct {
  float voltage_divider_ratio;
  float adc_max_value;
  float vcc_measured_voltage;
  float battery_slope;
  float min_battery_voltage;
  float max_battery_voltage;
  pin battery_control_pin;
  uint8_t adc_channel;
  char *battery_percentage_str;
  char *battery_voltage_str;
  bool is_working;

} battery;

battery *battery_init(float voltage_divider_ratio,
    float adc_max_value,
    float vcc_measured_voltage,
    float battery_slope,
    float min_battery_voltage,
    float max_battery_voltage,
    pin battery_control_pin,
    uint8_t adc_channel);
uint8_t battery_get_percentage(battery *battery);
float battery_get_voltage(battery *battery);
char *battery_get_voltage_str(battery *battery);
char *battery_get_percentage_str(battery *battery);

#endif