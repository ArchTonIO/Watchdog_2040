// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include "battery.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "pico/time.h"

#include "config.h"
#include "hardware/adc.h"

voltage_level battery_table[] = {{1700, 4.20, 100},
    {1640, 3.98, 75},
    {1550, 3.84, 50},
    {1530, 3.75, 25},
    {1355, 3.27, 0}};

size_t battery_table_size = sizeof(battery_table) / sizeof(voltage_level);

bool battery_is_working(battery *bat);

/**
 * @brief Initializes a battery instance with the given parameters.
 *
 * @param adc_max_value The maximum value of the microcontroller ADC, given by
 * its resolution.
 * @param min_voltage The minimum voltage of the battery.
 * @param max_voltage The maximum voltage of the battery.
 * @param battery_control_pin The GPIO pin used to control the battery.
 * @param adc_channel The ADC channel used to read the battery voltage.
 * @return A pointer to the initialized battery instance.
 */
battery *battery_init(float adc_max_value,
    float min_voltage,
    float max_voltage,
    pin battery_control_pin,
    uint8_t adc_channel) {
  battery *new_battery = (battery *)malloc(sizeof(battery));
  new_battery->adc_max_value = adc_max_value;
  new_battery->min_voltage = min_voltage;
  new_battery->max_voltage = max_voltage;
  new_battery->battery_control_pin = battery_control_pin;
  new_battery->adc_channel = adc_channel;
  new_battery->battery_percentage_str = (char *)malloc(sizeof(char) * 4);
  new_battery->battery_voltage_str = (char *)malloc(sizeof(char) * 6);
  new_battery->battery_crude_adc_str = (char *)malloc(sizeof(char) * 5);
  adc_gpio_init(battery_control_pin);
  new_battery->is_working = battery_is_working(new_battery);
  return new_battery;
}

/**
 * @brief Gets the crude ADC value for the battery.
 *
 * @param bat Pointer to the battery instance.
 * @return The raw ADC value read from the battery.
 */
uint32_t battery_get_crude_adc(battery *bat) {
  adc_select_input(bat->adc_channel);
  uint8_t samples = 10;
  uint32_t sum = 0;
  for (uint8_t i = 0; i < samples; i++) {
    sum += adc_read();
    sleep_ms(5);
  }
  return sum / samples;
}

/**
 * @brief Gets the battery voltage in volts.
 *
 * @param bat Pointer to the battery instance.
 * @return The battery voltage in volts.
 */

float battery_get_voltage(battery *bat) {
  uint32_t adc_value = battery_get_crude_adc(bat);
  for (size_t i = 0; i < battery_table_size; i++) {
    if (adc_value >= battery_table[i].adc_value) {
      return battery_table[i].voltage;
    }
  }
  return 0.0f;
}

/**
 * @brief Gets the battery percentage based on the current voltage.
 *
 * @param bat Pointer to the battery instance.
 * @return The battery percentage (0-100).
 */
uint8_t battery_get_percentage(battery *bat) {
  uint32_t adc_value = battery_get_crude_adc(bat);
  for (size_t i = 0; i < battery_table_size; i++) {
    if (adc_value >= battery_table[i].adc_value) {
      return battery_table[i].percentage;
    }
  }
  return 0;
}

/**
 * @brief Gets the crude ADC value as a string.
 *
 * @param bat Pointer to the battery instance.
 * @return A string representation of the crude ADC value (0-4095).
 */
char *battery_get_crude_adc_str(battery *bat) {
  sprintf(bat->battery_crude_adc_str, "%u", battery_get_crude_adc(bat));
  return bat->battery_crude_adc_str;
}

/**
 * @brief Gets the battery voltage as a string.
 *
 * @param bat Pointer to the battery instance.
 * @return A string representation of the battery voltage.
 */
char *battery_get_voltage_str(battery *bat) {
  sprintf(bat->battery_voltage_str, "%.2fV", battery_get_voltage(bat));
  return bat->battery_voltage_str;
}

/**
 * @brief Gets the battery percentage as a string.
 *
 * @param bat Pointer to the battery instance.
 * @return A string representation of the battery percentage.
 */
char *battery_get_percentage_str(battery *bat) {
  sprintf(bat->battery_percentage_str, "%d%%", battery_get_percentage(bat));
  return bat->battery_percentage_str;
}

bool battery_is_working(battery *bat) {
  if ((battery_get_voltage(bat) < bat->min_voltage) ||
      battery_get_voltage(bat) > bat->max_voltage)
    return false;
  return true;
}