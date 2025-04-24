#ifndef HOME_SCREEN_H
#define HOME_SCREEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware_drivers/sdcard.h"
#include "hardware_drivers/ssd1306.h"
#include "hardware_drivers/rtc_time.h"
#include "data_structures/string_list.h"
#include "hardware_drivers/joystick.h"
#include "text_editor.h"
#include "ulcp/ulcp.h"
#include "bitmaps.h"
#include "utils.h"
#include "hw_manager.h"
#include "msg_manager.h"
#include "options_gen.h"

typedef struct
{
  char *timedate;
  uint8_t battery_level;
  uint8_t sd_status;
  uint8_t sx1278_status;
  uint8_t en160_status;
  bool alarm_set;
  uint8_t alarm_time;
  uint8_t aqi;
  uint8_t bpm;
  uint8_t spo2;
  uint8_t notifications;
  uint16_t your_ulcp_address;
} home_page;

home_page *home_page_init();
void check_pheripherals();
void process_peripherals();
void display_main_page();
void update_main_page();

#endif
