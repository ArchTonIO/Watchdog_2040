#include <stdlib.h>
#include "hardware_drivers/battery.h"
#include "hardware_drivers/joystick.h"
#include "hardware_drivers/config.h"
#include "hardware_drivers/ens160.h"
#include "hardware_drivers/rtc_time.h"
#include "hardware_drivers/sdcard.h"
#include "hardware_drivers/ssd1306.h"
#include "hardware_drivers/sx1278.h"
#include "hardware_drivers/tests.h"
#include "hw_manager.h"
#include "data_structures/string_list.h"
#include <stdint.h>

hw_drivers *drivers;

hw_drivers *hardware_drivers_init()
{
	hw_drivers *hw_man = (hw_drivers *)malloc(sizeof(hw_drivers));
	hw_man->air_quality_sensor = ens160_init(
			ENS160_SDA,
			ENS160_SCK,
			ENS160_I2C_PORT,
			ENS160_BAUDRATE,
			ENS160_ADDR);
	hw_man->oled_screen = ssd1306_init(
			SSD1306_SDA,
			SSD1306_SCK,
			SSD1306_I2C_PORT,
			SSD1306_BAUDRATE,
			SSD1306_WIDTH,
			SSD1306_HEIGHT,
			SSD1306_ADDR);
	hw_man->lora_module = sx1278_init(
			SX1278_MOSI,
			SX1278_MISO,
			SX1278_SCK,
			SX1278_CS,
			SX1278_INTERRUPT,
			SX1278_SPI_PORT,
			SX1278_SPI_BAUDRATE,
			SX1278_TX_POWER,
			NULL);
	hw_man->battery = battery_init(
			VOLTAGE_DIVIDER_RATIO,
			ADC_MAX_VALUE,
			RP_2040_VCC_MEASURED_VOLTAGE,
			BATTERY_SLOPE,
			MIN_BATTERY_VOLTAGE,
			MAX_BATTERY_VOLTAGE,
			BATTERY_PIN,
			ADC_CHANNEL);
	hw_man->joystick = joystick_init(
			JOYSTICK_X_PIN,
			JOYSTICK_Y_PIN,
			JOYSTICK_X_CHANNEL,
			JOYSTICK_Y_CHANNEL,
			JOYSTICK_BUTTON_PIN,
			JOYSTICK_SENSITIVITY);
	hw_man->sd_card = sdcard_init();
	sdcard_mount(hw_man->sd_card);
	hw_man->rtc = rtc_time_init(2025, 1, 1, 3, 0, 0, 0);
	drivers = hw_man;
	return hw_man;
}