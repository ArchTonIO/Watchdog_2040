#ifndef HW_MANAGER_H
#define HW_MANAGER_H

#include <stdlib.h>
#include "hardware_drivers/battery.h"
#include "hardware_drivers/joystick.h"
#include "hardware_drivers/config.h"
#include "hardware_drivers/ens160.h"
#include "hardware_drivers/rtc_time.h"
#include "hardware_drivers/sdcard.h"
#include "hardware_drivers/ssd1306.h"
#include "hardware_drivers/sx1278.h"
#include "data_structures/string_list.h"
#include <stdint.h>

/*hw_driver just contains instances of all connected hardware modules drivers.*/
typedef struct
{
  ens160 *air_quality_sensor;
  ssd1306 *oled_screen;
  sx1278 *lora_module;
  battery *battery;
  joystick *joystick;
  sdcard *sd_card;
  rtc_time *rtc;
} hw_drivers;

extern hw_drivers *drivers;

hw_drivers *hardware_drivers_init();
uint32_t get_free_heap();

#endif
