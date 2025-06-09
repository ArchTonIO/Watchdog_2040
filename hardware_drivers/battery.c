#include "battery.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "hardware/adc.h"

bool battery_is_working(battery *bat);

battery *battery_init(float voltage_divider_ratio,
    float adc_max_value,
    float vcc_measured_voltage,
    float battery_slope,
    float min_battery_voltage,
    float max_battery_voltage,
    pin battery_control_pin,
    uint8_t adc_channel) {
  battery *new_battery = (battery *)malloc(sizeof(battery));
  new_battery->voltage_divider_ratio = voltage_divider_ratio;
  new_battery->adc_max_value = adc_max_value;
  new_battery->vcc_measured_voltage = vcc_measured_voltage;
  new_battery->battery_slope = battery_slope;
  new_battery->min_battery_voltage = min_battery_voltage;
  new_battery->max_battery_voltage = max_battery_voltage;
  new_battery->battery_control_pin = battery_control_pin;
  new_battery->adc_channel = adc_channel;
  new_battery->battery_percentage_str = (char *)malloc(sizeof(char) * 10);
  new_battery->battery_voltage_str = (char *)malloc(sizeof(char) * 10);
  adc_init();
  adc_gpio_init(battery_control_pin);
  new_battery->is_working = battery_is_working(new_battery);
  return new_battery;
}

uint8_t battery_get_percentage(battery *bat) {
  adc_select_input(bat->adc_channel);
  float v_bat =
      ((adc_read() * bat->vcc_measured_voltage) / bat->adc_max_value) *
      bat->voltage_divider_ratio;
  if (v_bat < bat->min_battery_voltage)
    return 0;
  if (v_bat >= bat->max_battery_voltage)
    return 100;
  return (v_bat - bat->min_battery_voltage) * bat->battery_slope;
}

float battery_get_voltage(battery *bat) {
  adc_select_input(bat->adc_channel);
  return ((adc_read() * bat->vcc_measured_voltage) / bat->adc_max_value) *
         bat->voltage_divider_ratio;
}

char *battery_get_percentage_str(battery *bat) {
  sprintf(bat->battery_percentage_str, "%d%%", battery_get_percentage(bat));
  return bat->battery_percentage_str;
}

char *battery_get_voltage_str(battery *bat) {
  sprintf(bat->battery_voltage_str, "%.2fV", battery_get_voltage(bat));
  return bat->battery_voltage_str;
}

bool battery_is_working(battery *bat) {
  if ((battery_get_voltage(bat) < bat->min_battery_voltage) ||
      battery_get_voltage(bat) > bat->max_battery_voltage)
    return false;
  return true;
}