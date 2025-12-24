// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include "ens160.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "config.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

int16_t translate_pair(uint8_t msb, uint8_t lsb);
bool ens160_is_working(ens160 *sensor);

/**
 * @brief Initializes an ENS160 air quality sensor instance.
 * @param sda The SDA pin for I2C communication.
 * @param sck The SCK pin for I2C communication.
 * @param i2c_port The I2C port to use.
 * @param baudrate The baud rate for I2C communication.
 * @param address The I2C address of the ENS160 sensor.
 * @return A pointer to the initialized ens160 instance.
 */
ens160 *ens160_init(pin sda,
    pin sck,
    i2c_inst_t *i2c_port,
    uint32_t baudrate,
    uint32_t address) {
  ens160 *new_sensor = (ens160 *)malloc(sizeof(ens160));
  new_sensor->sda = sda;
  new_sensor->sck = sck;
  new_sensor->i2c_port = i2c_port;
  new_sensor->baudrate = baudrate;
  new_sensor->address = address;
  i2c_init(i2c_port, baudrate);
  gpio_set_function(sda, GPIO_FUNC_I2C);
  gpio_set_function(sck, GPIO_FUNC_I2C);
  gpio_pull_up(sda);
  gpio_pull_up(sck);
  new_sensor->is_working = ens160_is_working(new_sensor);
  new_sensor->is_on = true;
  new_sensor->manually_turned_off = false;
  return new_sensor;
}

uint8_t ens160_get_op_mode(ens160 *sensor) {
  uint8_t mode_reg = 0x10;
  i2c_write_blocking(sensor->i2c_port, sensor->address, &mode_reg, 1, true);
  uint8_t buffer[1];
  i2c_read_blocking(sensor->i2c_port, sensor->address, buffer, 1, false);
  return buffer[0];
}

void ens160_set_op_mode(ens160 *sensor, uint8_t mode) {
  uint8_t data[2] = {0x10, mode};
  i2c_write_blocking(sensor->i2c_port, sensor->address, data, 2, false);
}

/**
 * @brief Reads the CO2 concentration from the ENS160 sensor.
 * @param sensor Pointer to the ens160 instance.
 * @return The CO2 concentration in ppm.
 */
int16_t ens160_read_co2(ens160 *sensor) {
  uint8_t co2_reg = 0x24;
  i2c_write_blocking(sensor->i2c_port, sensor->address, &co2_reg, 1, true);
  uint8_t data[2];
  i2c_read_blocking(sensor->i2c_port, sensor->address, data, 2, false);
  return translate_pair(data[1], data[0]);
}

/**
 * @brief Reads the TVOC (Total Volatile Organic Compounds) concentration from
 * the ENS160 sensor.
 * @param sensor Pointer to the ens160 instance.
 * @return The TVOC concentration in ppb.
 */
int16_t ens160_read_tvoc(ens160 *sensor) {
  uint8_t tvoc_reg = 0x22;
  i2c_write_blocking(sensor->i2c_port, sensor->address, &tvoc_reg, 1, true);
  uint8_t data[2];
  i2c_read_blocking(sensor->i2c_port, sensor->address, data, 2, false);
  return translate_pair(data[1], data[0]);
}

/**
 * @brief Reads the AQI (Air Quality Index) from the ENS160 sensor.
 * @param sensor Pointer to the ens160 instance.
 * @return The AQI value.
 */
uint8_t ens160_read_aqi(ens160 *sensor) {
  uint8_t aqi_reg = 0x21;
  i2c_write_blocking(sensor->i2c_port, sensor->address, &aqi_reg, 1, true);
  uint8_t data[1];
  i2c_read_blocking(sensor->i2c_port, sensor->address, data, 1, false);
  return data[0];
}

void ens160_reset(ens160 *sensor) {
  sleep_ms(1000);
  ens160_set_op_mode(sensor, 1);
  sleep_ms(250);
  uint8_t data[2] = {0x12, 0x00};
  i2c_write_blocking(sensor->i2c_port, sensor->address, data, 2, false);
  sleep_ms(150);
  uint8_t data2[2] = {0x12, 0xcc};
  i2c_write_blocking(sensor->i2c_port, sensor->address, data2, 2, false);
  sleep_ms(350);
  ens160_set_op_mode(sensor, 2);
  sleep_ms(500);
}

void ens160_power_down(ens160 *sensor) {
  if (!sensor->is_on)
    return;
  ens160_set_op_mode(sensor, 0x00);
  sensor->is_on = false;
}

void ens160_power_up(ens160 *sensor) {
  if (sensor->is_on || sensor->manually_turned_off)
    return;
  ens160_set_op_mode(sensor, 0x01);
  sleep_ms(20);
  ens160_set_op_mode(sensor, 0x02);
  sensor->is_on = true;
}

bool ens160_is_working(ens160 *sensor) {
  sleep_ms(10);
  if (ens160_get_op_mode(sensor) != 2) {
    ens160_reset(sensor);
    if (ens160_get_op_mode(sensor) != 2) {
      sensor->is_working = false;
      return false;
    }
  }
  sensor->is_working = true;
  return true;
}

int16_t translate_pair(uint8_t msb, uint8_t lsb) {
  uint16_t raw = ((uint16_t)msb << 8) | lsb;
  return (raw >= 0x8000) ? -((int16_t)((~raw + 1) & 0xFFFF)) : (int16_t)raw;
}
