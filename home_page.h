#ifndef HOME_PAGE_H
#define HOME_PAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
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
  uint8_t *hour_tens_bitmap;
  uint8_t *hour_units_bitmap;
  uint8_t *minute_tens_bitmap;
  uint8_t *minute_units_bitmap;
  uint8_t *second_tens_bitmap;
  uint8_t *second_units_bitmap;
} clock_bitmaps;

typedef struct
{
  char *timedate;
  uint8_t battery_level;
  bool sd_status;
  bool sx1278_status;
  bool en160_status;
  bool alarm_set;
  clock_bitmaps *clock_bmp;
  uint8_t alarm_time;
  uint8_t aqi;
  uint8_t bpm;
  uint8_t spo2;
  uint16_t notifications;
} home_page;

home_page *home_page_init();
void check_pheripherals();
void process_system_state();
void display_home_page();

#endif
