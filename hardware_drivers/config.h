#ifndef CONFIG_HPP
#define CONFIG_HPP

#define pin uint8_t

/*ENS160 air quality sensor settings*/
#define ENS160_SDA 2
#define ENS160_SCK 3
#define ENS160_I2C_PORT i2c1
#define ENS160_BAUDRATE 400 * 1000
#define ENS160_ADDR 0x53

/*MAX30102 pulse oximeter settings*/
#define MAX30102_SDA 4
#define MAX30102_SCK 5
#define MAX30102_I2C_PORT i2c0
#define MAX30102_BAUDRATE 400 * 1000
#define MAX30102_ADDR 0x57

/*SSD1306 oled screen settings*/
#define SSD1306_SDA 0
#define SSD1306_SCK 1
#define SSD1306_I2C_PORT i2c0
#define SSD1306_BAUDRATE 100 * 1000
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define SSD1306_ADDR 0x3C

/*SX1278 lora module settings*/
#define SX1278_MOSI 19
#define SX1278_MISO 16
#define SX1278_CS 17
#define SX1278_SCK 18
#define SX1278_INTERRUPT 21
#define SX1278_SPI_PORT spi0
#define SX1278_SPI_BAUDRATE 5000 * 1000
#define SX1278_TX_POWER 15

/*Micro sd reader module settings*/
/*Beware that changing those pins will have no effects,
as the SD card driver code is not wrote by me, this only serves as reference.
If you wish to change the pins go to lib/no-Os-FatFS-SD-SPI-RPi-Pico/FatFs_SPI/sd_driver/hw_config.c*/
#define MICRO_SD_MOSI 15
#define MICRO_SD_MISO 12
#define MICRO_SD_CS 13
#define MICRO_SD_SCK 14
#define MICRO_SD_SPI_PORT spi1
#define MICRO_SD_SPI_BAUDRATE 40000000

/*joystick settings*/
#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27
#define JOYSTICK_X_CHANNEL 0
#define JOYSTICK_Y_CHANNEL 1
#define JOYSTICK_BUTTON_PIN 22
#define JOYSTICK_DEAFULT_DEADZONE_MIN 1900
#define JOYSTICK_DEFAULT_DEADZONE_MAX 2100
#define JOYSTICK_DEFAULT_CENTER 2000
#define JOYSTICK_DEFAULT_MAX_L 2800
#define JOYSTICK_AUTO_CALIBRATION_SAMPLES 150
#define JOYSTICK_AUTO_CALIBRATION_INTERVAL_MS 10

/*battery settings*/
#define VOLTAGE_DIVIDER_RATIO 3.125
#define ADC_MAX_VALUE 4095.0
#define RP_2040_VCC_MEASURED_VOLTAGE 3.34
#define BATTERY_PIN 28
#define ADC_CHANNEL 2
#define BATTERY_SLOPE 111.1
#define MIN_BATTERY_VOLTAGE 3.3
#define MAX_BATTERY_VOLTAGE 4.2

#endif