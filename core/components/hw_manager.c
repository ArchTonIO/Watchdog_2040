// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "core/components/include/hw_manager.h"

#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "apps/system_app/include/system_app.h"
#include "core/data_structures/include/string_list.h"
#include "core/hardware_drivers/include/battery.h"
#include "core/hardware_drivers/include/config.h"
#include "core/hardware_drivers/include/core1.h"
#include "core/hardware_drivers/include/ds3231.h"
#include "core/hardware_drivers/include/ens160.h"
#include "core/hardware_drivers/include/haptics.h"
#include "core/hardware_drivers/include/joystick.h"
#include "core/hardware_drivers/include/onboard_led.h"
#include "core/hardware_drivers/include/rtc_time.h"
#include "core/hardware_drivers/include/sdcard.h"
#include "core/hardware_drivers/include/ssd1306.h"
#include "core/hardware_drivers/include/sx1278.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"

hw_drivers *drivers;
void wait_for_core1();

void init_i2c1_bus() {
  i2c_init(I2C1_BUS_PORT, I2C1_BUS_BAUDRATE);
  gpio_set_function(I2C1_BUS_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C1_BUS_SCK, GPIO_FUNC_I2C);
  gpio_pull_up(I2C1_BUS_SDA);
  gpio_pull_up(I2C1_BUS_SCK);
}

hw_drivers *hardware_drivers_init() {
  hw_drivers *hw_man = (hw_drivers *)malloc(sizeof(hw_drivers));

  ssd1306_t ssd1306;
  ssd1306_init(&ssd1306,
      SSD1306_SDA,
      SSD1306_SCK,
      SSD1306_I2C_PORT,
      SSD1306_BAUDRATE,
      SSD1306_WIDTH,
      SSD1306_HEIGHT,
      SSD1306_ADDR);
  hw_man->ssd1306 = ssd1306;
  drivers = hw_man;

  haptics_init(HAPTICS_MOTOR_PIN);
  onboard_led_init(ONBOARD_LED_PIN);
  core1_scheduler_init();
  core1_spin();
  sleep_ms(1000);
  core1_push_instruction(SHOW_BOOTUP);
  init_i2c1_bus();

  ens160_t ens160;
  ens160_init(&ens160, ENS160_I2C_PORT, ENS160_ADDR);
  hw_man->ens160 = ens160;
  if (hw_man->ens160.is_working)
    core1_push_instruction(ENS160_OK);
  else
    core1_push_instruction(ENS160_ERR);

  hw_man->sx1278 = sx1278_init(SX1278_MOSI,
      SX1278_MISO,
      SX1278_SCK,
      SX1278_CS,
      SX1278_INTERRUPT,
      SX1278_SPI_PORT,
      SX1278_SPI_BAUDRATE,
      SX1278_TX_POWER,
      NULL);
  if (hw_man->sx1278->is_working)
    core1_push_instruction(SX1278_OK);
  else
    core1_push_instruction(SX1278_ERR);

  mcp73871_t mcp73871;
  mcp73871_init(&mcp73871,
      MCP73871_PG_PIN,
      MCP73871_STAT1_PIN,
      MCP73871_STAT2_PIN);

  battery_t battery;
  battery_init(&battery,
      ADC_MAX_VALUE,
      BATTERY_MIN_VOLTAGE,
      BATTERY_MAX_VOLTAGE,
      BATTERY_PIN,
      ADC_CHANNEL,
      mcp73871);
  hw_man->battery = battery;
  if (hw_man->battery.is_working)
    core1_push_instruction(BATTERY_OK);
  else
    core1_push_instruction(BATTERY_ERR);

  joystick_t joystick;
  joystick_init(&joystick,
      JOYSTICK_X_PIN,
      JOYSTICK_Y_PIN,
      JOYSTICK_X_CHANNEL,
      JOYSTICK_Y_CHANNEL,
      JOYSTICK_BUTTON_PIN,
      JOYSTICK_SENSITIVITY,
      JOYSTICK_ROTATION);
  hw_man->joystick = joystick;
  if (hw_man->joystick.is_working)
    core1_push_instruction(JOYSTICK_OK);
  else
    core1_push_instruction(JOYSTICK_ERR);

  sdcard_t sd_card;
  sdcard_init(&sd_card);
  hw_man->sd_card = sd_card;
  sdcard_mount(&(hw_man->sd_card));
  if (hw_man->sd_card.is_working)
    core1_push_instruction(SDCARD_OK);
  else
    core1_push_instruction(SDCARD_ERR);

  ds3231_rtc_t external_rtc;
  ds3231_init(DS3231_I2C_PORT, DS3231_I2C_ADDRESS, &external_rtc);
  hw_man->external_rtc = external_rtc;
  internal_rtc_t internal_rtc;
  internal_rtc_init(&internal_rtc, external_rtc, 2026, 5, 9, 4, 20, 37, 00);
  hw_man->internal_rtc = internal_rtc;
  rtc_time_load_time_from_external_rtc(&hw_man->internal_rtc, &external_rtc);
  core1_push_instruction(RTC_OK);
  core1_push_instruction(CHECKS_END);
  hw_man->power_saving = false;

  return hw_man;
}

void end_loading_screen() { core1_await(); }

void write_default_config() {
  system_settings_t settings;
  settings.auto_brightness_enabled = true;
  settings.haptics_enabled = true;
  system_settings_dump(settings);
}

void load_config() {
  system_settings_t settings;
  system_settings_load(&settings);
  if (!settings.auto_brightness_enabled) {
    ssd1306_disable_auto_brightness(&(drivers->ssd1306));
    ssd1306_set_brightness(&(drivers->ssd1306), settings.brightness_level);
  }
  if (!settings.haptics_enabled)
    haptics_disable();
}

uint32_t get_total_heap(void) {
  extern char __StackLimit, __bss_end__;
  return &__StackLimit - &__bss_end__;
}

uint32_t get_free_heap(void) {
  struct mallinfo m = mallinfo();
  return get_total_heap() - m.uordblks;
}

/**
 * @brief prints how much memory is available in the heap.
 */
void print_free_heap() {
  printf("Available memory: %u bytes\n", get_free_heap());
}

inline uint get_clock_freq_khz(void) { return clock_get_hz(clk_sys) / 1000; }

float get_cpu_temp() {
  adc_set_temp_sensor_enabled(true);
  adc_select_input(4);
  uint16_t raw = adc_read();
  const float conversion_factor = 3.3f / (1 << 12);
  float voltage = raw * conversion_factor;
  return 27.0f - (voltage - 0.706f) / 0.001721f;
}

inline uint32_t get_used_flash_bytes() {
  return (uint32_t)(&__flash_binary_end) - XIP_BASE;
}

volatile bool wake_requested = false;
volatile bool continuous_rx = false;

void toggle_continuous_rx() {
  if (continuous_rx)
    continuous_rx = false;
  else
    continuous_rx = true;
}

inline bool is_rxcontinuous_enabled() { return continuous_rx; }

void joystick_irq(uint gpio, uint32_t events) {
  if (gpio == JOYSTICK_BUTTON_PIN) {
    wake_requested = true;
  }
}

void enter_idle() {
  ens160_power_down(&(drivers->ens160));
  ssd1306_clear(&(drivers->ssd1306));
  ssd1306_show(&(drivers->ssd1306));
  ssd1306_enable_mutex_support(&(drivers->ssd1306));
  if (!continuous_rx) {
    gpio_set_irq_enabled_with_callback(JOYSTICK_BUTTON_PIN,
        GPIO_IRQ_EDGE_FALL,
        true,
        &joystick_irq);
    sx1278_sleep(drivers->sx1278);
    set_sys_clock_khz(25000, true);
    sleep_ms(50);
  }
}

void wait_joystick_interrupt() {
  if (!continuous_rx)
    while (!wake_requested)
      __wfi();
  else
    while (!(drivers->joystick).button_pressed) {
      joystick_update(&(drivers->joystick));
    }
}

void exit_idle() {
  if (!continuous_rx) {
    gpio_set_irq_enabled_with_callback(JOYSTICK_BUTTON_PIN,
        GPIO_IRQ_EDGE_FALL,
        false,
        &joystick_irq);
    drivers->sx1278 = sx1278_init(SX1278_MOSI,
        SX1278_MISO,
        SX1278_SCK,
        SX1278_CS,
        SX1278_INTERRUPT,
        SX1278_SPI_PORT,
        SX1278_SPI_BAUDRATE,
        SX1278_TX_POWER,
        drivers->sx1278->message_received_callback);
    sx1278_set_mode_rx(drivers->sx1278);
    set_sys_clock_khz(125000, true);
    sleep_ms(50);
  }
  ens160_power_up(&(drivers->ens160));
  wake_requested = false;
}

void sys_idle() {
  enter_idle();
  wait_joystick_interrupt();
  exit_idle();
}