#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware_config.h"
#include "ens160.h"
#include "rtc_time.h"
#include "ssd1306.h"
#include "battery.h"
#include "sx1278.h"
#include "sdcard.h"
#include "string_list.h"
#include <stdlib.h>
#include "tests.h"

void test_ens160()
{
	printf("\n\n### Testing ens160 ###\n");
	ens160 *sensor = ens160_init(
			ENS160_SDA,
			ENS160_SCK,
			ENS160_I2C_PORT,
			ENS160_BAUDRATE,
			ENS160_ADDR);
	for (uint i = 0; i < 50; i++)
	{
		printf(
				"CO2: %d, AQI: %d, TVOC: %d\n",
				ens160_read_co2(sensor),
				ens160_read_aqi(sensor),
				ens160_read_tvoc(sensor));
		sleep_ms(500);
	}
}

void rtc_alarm_callback()
{
	printf("\n###### THE TIME ARRIVED ! #####\n\n");
}

void test_rtc()
{
	printf("\n\n### Testing rtc ###\n");
	rtc_time *rtc = rtc_time_init(2025, 01, 05, 0, 23, 52, 30);
	rtc_time_add_alarm(rtc, 23, 53, 00, rtc_alarm_callback);
	for (uint i = 0; i < 40; i++)
	{
		printf("%s\n", rtc_time_now(rtc));
		sleep_ms(1000);
	}
}

void test_ssd1306()
{
	printf("\n\n### Testing ssd1306 ###\n");
	ssd1306 *oled = ssd1306_init(
			SSD1306_SDA,
			SSD1306_SCK,
			SSD1306_I2C_PORT,
			SSD1306_BAUDRATE,
			SSD1306_WIDTH,
			SSD1306_HEIGHT,
			SSD1306_ADDR);
	for (uint i = 0; i < 8; i++)
	{
		ssd1306_clear(oled);
		ssd1306_print(oled, "Hello, World!", 0, i);
		ssd1306_show(oled);
		sleep_ms(1000);
	}
	ssd1306_clear(oled);
	ssd1306_show(oled);
}

void test_battery()
{
	printf("\n\n### Testing battery ###\n");
	battery *bat = battery_init(
			VOLTAGE_DIVIDER_RATIO,
			ADC_MAX_VALUE,
			RP_2040_VCC_MEASURED_VOLTAGE,
			BATTERY_SLOPE,
			MIN_BATTERY_VOLTAGE,
			MAX_BATTERY_VOLTAGE,
			BATTERY_PIN,
			ADC_CHANNEL);
	for (uint i = 0; i < 50; i++)
	{
		printf("Battery percentage: %d, Battery voltage: %f\n", battery_get_percentage(bat), battery_get_voltage(bat));
		sleep_ms(500);
	}
}

void test_sx1278()
{
	printf("\n\n### Testing sx1278 ###\n");
	printf("waiting 10 sec for serial connection...\n");
	sleep_ms(10000);
	sx1278 *lora = sx1278_init(
			SX1278_MOSI,
			SX1278_MISO,
			SX1278_SCK,
			SX1278_CS,
			SX1278_ON_RECV_INTERRUPT,
			0,
			SX1278_SPI_PORT,
			SX1278_SPI_BAUDRATE,
			SX1278_TX_POWER);
	printf("\n\n### Testing sx1278 receive ###\n");
	sx1278_set_mode_rx(lora);
	for (uint i = 0; i < 50; i++)
		sleep_ms(500);
	printf("\n\n### Testing sx1278 send ###\n");
	for (uint i = 0; i < 50; i++)
	{
		sx1278_send_str(lora, "Hello, LoRa!");
		printf("Message sent!\n");
		sleep_ms(5000);
	}
}

void test_sd_card()
{
	printf("\n\n### Testing sd card ###\n");
	sdcard *sd = sdcard_init();
	sdcard_mount(sd);
	printf("Writing a new file called test.txt, and writing stuff in it\n");
	sdcard_write_file(sd, "test.txt", "Hello\nThis is a test file to see if micro sd works\nBye!", 'w');
	printf("Retrieving lines list from file test.txt:\n");
	str_list *lines = sdcard_read_file(sd, "test.txt");
	lstprint(lines);
	printf("Listing all files in the sd card:\n");
	str_list *files = sdcard_list_files(sd);
	lstprint(files);
	// printf("Deleting file test.txt\n");
	// sdcard_delete_file(sd, "test.txt");
	// printf("Listing all files in the sd card:\n");
	// files = sdcard_list_files(sd);
	// lstprint(files);
	// printf("Formatting sd card\n");
	// sdcard_format(sd);
	// printf("Listing all files in the sd card:\n");
	// files = sdcard_list_files(sd);
	// lstprint(files);
	lstdel(lines);
	lstdel(files);
	sdcard_unmount(sd);
	printf("Card unmounted\n");
	free(sd);
}

void test_all_hardware()
{
	printf("Waiting for user to input 'a' in the serial monitor to start tests");
	char buf[100];
	while (true)
	{
		buf[0] = getchar();
		if ((buf[0] == 'a'))
		{
			break;
		}
	}
	// test_battery();
	// test_rtc();
	// test_ens160();
	// test_ssd1306();
	// test_sx1278();
	test_sd_card();
	printf("\n\n### All tests completed ! ###\n");
}