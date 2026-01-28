// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef HW_MANAGER_H
#define HW_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

#include "core/data_structures/string_list.h"
#include "core/hardware_drivers/battery.h"
#include "core/hardware_drivers/ens160.h"
#include "core/hardware_drivers/joystick.h"
#include "core/hardware_drivers/rtc_time.h"
#include "core/hardware_drivers/sdcard.h"
#include "core/hardware_drivers/ssd1306.h"
#include "core/hardware_drivers/sx1278.h"

#define FLASH_TARGET_OFFSET 0

/*hw_driver just contains instances of all connected hardware modules
 * drivers.*/
typedef struct {
  ens160 *air_quality_sensor;
  ssd1306 *oled_screen;
  sx1278 *lora_module;
  battery *battery;
  joystick *joystick;
  sdcard *sd_card;
  rtc_time *rtc;
  bool power_saving;
} hw_drivers;

extern hw_drivers *drivers;
extern uint8_t __flash_binary_end;

hw_drivers *hardware_drivers_init();
void end_loading_screen();
uint32_t get_free_heap();
void print_free_heap();
uint get_clock_freq_khz();
float get_cpu_temp();
uint32_t get_used_flash_bytes();
void enable_ens160();
void disable_ens160();
void enable_sx1278();
void disable_sx1278();
void enable_power_saving_mode();
void disable_power_saving_mode();
void process_power_saving();

#endif
