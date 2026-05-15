// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef HW_MANAGER_H
#define HW_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

#include "core/data_structures/include/string_list.h"
#include "core/hardware_drivers/include/battery.h"
#include "core/hardware_drivers/include/ds3231.h"
#include "core/hardware_drivers/include/ens160.h"
#include "core/hardware_drivers/include/joystick.h"
#include "core/hardware_drivers/include/rtc_time.h"
#include "core/hardware_drivers/include/sdcard.h"
#include "core/hardware_drivers/include/ssd1306.h"
#include "core/hardware_drivers/include/sx1278.h"

#define FLASH_TARGET_OFFSET 0

/*hw_driver just contains instances of all connected hardware modules
 * drivers.*/
typedef struct {
  ens160_t ens160;
  ssd1306_t ssd1306;
  sx1278_t *sx1278;
  battery_t battery;
  joystick_t joystick;
  sdcard_t sd_card;
  ds3231_rtc_t external_rtc;
  internal_rtc_t internal_rtc;
  bool power_saving;
} hw_drivers;

extern hw_drivers *drivers;
extern uint8_t __flash_binary_end;

hw_drivers *hardware_drivers_init();
void end_loading_screen();
void write_default_config();
void load_config();
uint32_t get_free_heap();
void print_free_heap();
uint get_clock_freq_khz();
float get_cpu_temp();
uint32_t get_used_flash_bytes();
void sys_idle();
void toggle_continuous_rx();
bool is_rxcontinuous_enabled();
#endif
