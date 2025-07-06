#include "ens160.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "pico/stdlib.h"

#include "config.h"
#include "hardware/i2c.h"

int16_t translate_pair(uint8_t msb, uint8_t lsb);
bool ens160_is_working(ens160 *sensor);

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

int16_t ens160_read_co2(ens160 *sensor) {
  uint8_t co2_reg = 0x24;
  i2c_write_blocking(sensor->i2c_port, sensor->address, &co2_reg, 1, true);
  uint8_t data[2];
  i2c_read_blocking(sensor->i2c_port, sensor->address, data, 2, false);
  return translate_pair(data[1], data[0]);
}

int16_t ens160_read_tvoc(ens160 *sensor) {
  uint8_t tvoc_reg = 0x22;
  i2c_write_blocking(sensor->i2c_port, sensor->address, &tvoc_reg, 1, true);
  uint8_t data[2];
  i2c_read_blocking(sensor->i2c_port, sensor->address, data, 2, false);
  return translate_pair(data[1], data[0]);
}

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

bool ens160_is_working(ens160 *sensor) {
  sleep_ms(10);
  if (ens160_get_op_mode(sensor) != 2) {
    ens160_reset(sensor);
    if (ens160_get_op_mode(sensor) != 2)
      return false;
  }
  return true;
}

int16_t translate_pair(uint8_t msb, uint8_t lsb) {
  int16_t value = (msb << 8) | lsb;
  return (value >= 0x8000) ? -((65535 - value) + 1) : value;
}
