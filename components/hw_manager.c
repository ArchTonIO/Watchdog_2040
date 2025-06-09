#include "components/hw_manager.h"

#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>

#include "pico/multicore.h"

#include "boot/bootup.h"
#include "data_structures/string_list.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware_drivers/battery.h"
#include "hardware_drivers/config.h"
#include "hardware_drivers/ens160.h"
#include "hardware_drivers/joystick.h"
#include "hardware_drivers/rtc_time.h"
#include "hardware_drivers/sdcard.h"
#include "hardware_drivers/ssd1306.h"
#include "hardware_drivers/sx1278.h"

hw_drivers *drivers;
void wait_for_core1();

hw_drivers *hardware_drivers_init() {
  hw_drivers *hw_man = (hw_drivers *)malloc(sizeof(hw_drivers));
  hw_man->oled_screen = ssd1306_init(SSD1306_SDA,
      SSD1306_SCK,
      SSD1306_I2C_PORT,
      SSD1306_BAUDRATE,
      SSD1306_WIDTH,
      SSD1306_HEIGHT,
      SSD1306_ADDR);
  drivers = hw_man;
  multicore_launch_core1(display_bootup_screen);
  hw_man->air_quality_sensor = ens160_init(ENS160_SDA,
      ENS160_SCK,
      ENS160_I2C_PORT,
      ENS160_BAUDRATE,
      ENS160_ADDR);
  if (hw_man->air_quality_sensor->is_working)
    multicore_fifo_push_blocking(ENS160_OK);
  else
    multicore_fifo_push_blocking(ENS160_ERR);
  hw_man->lora_module = sx1278_init(SX1278_MOSI,
      SX1278_MISO,
      SX1278_SCK,
      SX1278_CS,
      SX1278_INTERRUPT,
      SX1278_SPI_PORT,
      SX1278_SPI_BAUDRATE,
      SX1278_TX_POWER,
      NULL);
  if (hw_man->lora_module->is_working)
    multicore_fifo_push_blocking(SX1278_OK);
  else
    multicore_fifo_push_blocking(SX1278_ERR);
  hw_man->battery = battery_init(VOLTAGE_DIVIDER_RATIO,
      ADC_MAX_VALUE,
      RP_2040_VCC_MEASURED_VOLTAGE,
      BATTERY_SLOPE,
      MIN_BATTERY_VOLTAGE,
      MAX_BATTERY_VOLTAGE,
      BATTERY_PIN,
      ADC_CHANNEL);
  if (hw_man->battery->is_working)
    multicore_fifo_push_blocking(BATTERY_OK);
  else
    multicore_fifo_push_blocking(BATTERY_ERR);
  hw_man->joystick = joystick_init(JOYSTICK_X_PIN,
      JOYSTICK_Y_PIN,
      JOYSTICK_X_CHANNEL,
      JOYSTICK_Y_CHANNEL,
      JOYSTICK_BUTTON_PIN,
      JOYSTICK_SENSITIVITY,
      -90);
  if (hw_man->joystick->is_working)
    multicore_fifo_push_blocking(JOYSTICK_OK);
  else
    multicore_fifo_push_blocking(JOYSTICK_ERR);
  hw_man->sd_card = sdcard_init();
  sdcard_mount(hw_man->sd_card);
  if (hw_man->sd_card->is_working)
    multicore_fifo_push_blocking(SDCARD_OK);
  else
    multicore_fifo_push_blocking(SDCARD_ERR);
  hw_man->rtc = rtc_time_init(2025, 5, 9, 4, 20, 37, 00);
  multicore_fifo_push_blocking(RTC_OK);
  multicore_fifo_push_blocking(CHECKS_END);
  wait_for_core1();
  multicore_reset_core1();
  return hw_man;
}

void wait_for_core1() {
  while (multicore_fifo_pop_blocking() != CORE_1_OP_DONE)
    ;
}

uint32_t get_total_heap(void) {
  extern char __StackLimit, __bss_end__;
  return &__StackLimit - &__bss_end__;
}

uint32_t get_free_heap(void) {
  struct mallinfo m = mallinfo();
  return get_total_heap() - m.uordblks;
}

uint get_clock_freq_khz(void) { return clock_get_hz(clk_sys) / 1000; }

float get_cpu_temp() {
  adc_init();
  adc_set_temp_sensor_enabled(true);
  adc_select_input(4);
  uint16_t raw = adc_read();
  const float conversion_factor = 3.3f / (1 << 12);
  float voltage = raw * conversion_factor;
  return 27.0f - (voltage - 0.706f) / 0.001721f;
}

uint32_t get_used_flash_bytes() {
  return (uint32_t)(&__flash_binary_end) - XIP_BASE;
}
