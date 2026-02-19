// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "core/components/include/home_page.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "apps/messaging/include/msg_manager.h"
#include "apps/time/include/time_utils.h"
#include "core/components/include/bitmaps.h"
#include "core/components/include/hw_manager.h"
#include "core/components/include/malloc_mascot.h"
#include "core/data_structures/include/string_list.h"
#include "core/graphics/include/graphic_primitives.h"
#include "core/graphics/include/layout.h"
#include "core/hardware_drivers/include/battery.h"
#include "core/hardware_drivers/include/ens160.h"
#include "core/hardware_drivers/include/rtc_time.h"
#include "core/hardware_drivers/include/sdcard.h"
#include "core/hardware_drivers/include/ssd1306.h"
#include "core/hardware_drivers/include/sx1278.h"

home_page *home_page_inst;

void create_lines();

home_page *home_page_init() {
  home_page *new_home_page = (home_page *)malloc(sizeof(home_page));
  new_home_page->timedate = (char *)malloc(sizeof(char) * 20);
  new_home_page->battery_level = 100;
  new_home_page->sd_status = 0;
  new_home_page->sx1278_status = 0;
  new_home_page->en160_status = 0;
  new_home_page->alarm_set = false;
  new_home_page->clock_bmp = (clock_bitmaps *)malloc(sizeof(clock_bitmaps));
  new_home_page->alarm_time = 0;
  new_home_page->aqi = 0;
  new_home_page->bpm = 0;
  new_home_page->spo2 = 0;
  new_home_page->notifications = 0;
  new_home_page->ly = layout_init();
  home_page_inst = new_home_page;
  layout_add_layer(home_page_inst->ly, "top_bar_bitmaps");
  layout_add_layer(home_page_inst->ly, "clock_bitmaps");
  layout_add_layer(home_page_inst->ly, "text_areas");
  layout_add_layer(home_page_inst->ly, "lines");
  create_lines();
  return new_home_page;
}

void check_peripherals() {
  if ((drivers->sd_card).is_working)
    home_page_inst->sd_status = 1;
  else
    home_page_inst->sd_status = 0;
  if (drivers->sx1278->is_working && drivers->sx1278->is_on)
    home_page_inst->sx1278_status = 1;
  else
    home_page_inst->sx1278_status = 0;
  if ((&(drivers->ens160))->is_working && (&(drivers->ens160))->is_on)
    home_page_inst->en160_status = 1;
  else
    home_page_inst->en160_status = 0;
}

void process_system_state() {
  home_page_inst->battery_level = battery_get_percentage(&(drivers->battery));
  home_page_inst->alarm_set = drivers->internal_rtc.alarm_set;
  home_page_inst->aqi = ens160_read_aqi(&(drivers->ens160));
  home_page_inst->notifications = msg_man_inst->received_msgs_count;
}

const uint8_t *get_battery_level_bitmap() {
  if (home_page_inst->battery_level >= 75) {
    return battery_4_bars;
  } else if (home_page_inst->battery_level >= 50) {
    return battery_3_bars;
  } else if (home_page_inst->battery_level >= 25) {
    return battery_2_bars;
  } else if (home_page_inst->battery_level >= 10) {
    return battery_1_bar;
  } else {
    return battery_empty;
  }
}

const uint8_t *get_sd_status_bitmap() {
  if (home_page_inst->sd_status)
    return microsd_working;
  else
    return microsd_not_working;
}

const uint8_t *get_sx1278_status_bitmap() {
  if (home_page_inst->sx1278_status)
    return lora_working;
  else
    return lora_not_working;
}

const uint8_t *get_en160_status_bitmap() {
  if (home_page_inst->en160_status)
    return ens160_working;
  else
    return ens160_not_working;
}

const uint8_t *get_notifications_bitmap() {
  if (home_page_inst->notifications > 10)
    return num_msg_bitmaps[11];
  else
    return num_msg_bitmaps[home_page_inst->notifications];
}

const uint8_t *get_alarm_status_bitmap() {
  if (home_page_inst->alarm_set)
    return alarm_enabled;
  else
    return alarm_disabled;
}

const uint8_t *get_rxcontinuous_indicator_bitmap() {
  if (is_rxcontinuous_enabled())
    return rxcontinuous_enabled;
  return rxcontinuous_disabled;
}

void update_clock_bitmaps() {
  update_time(&(drivers->internal_rtc));
  int8_t hour = drivers->internal_rtc.internal_datetime.hour;
  int8_t minute = drivers->internal_rtc.internal_datetime.min;
  int8_t second = drivers->internal_rtc.internal_datetime.sec;
  int8_t hour_tens = hour / 10;
  int8_t hour_units = hour % 10;
  int8_t minute_tens = minute / 10;
  int8_t minute_units = minute % 10;
  int8_t second_tens = second / 10;
  int8_t second_units = second % 10;
  home_page_inst->clock_bmp->hour_tens_bitmap = clock_digits[hour_tens];
  home_page_inst->clock_bmp->hour_units_bitmap = clock_digits[hour_units];
  home_page_inst->clock_bmp->minute_tens_bitmap = clock_digits[minute_tens];
  home_page_inst->clock_bmp->minute_units_bitmap = clock_digits[minute_units];
  home_page_inst->clock_bmp->second_tens_bitmap = clock_digits[second_tens];
  home_page_inst->clock_bmp->second_units_bitmap = clock_digits[second_units];
}

void update_top_bar() {
  bitmap_definition battery_level_btmp_def = {
      .bitmap = get_battery_level_bitmap(),
      .width = TOP_BAR_BITMAPS_W,
      .height = TOP_BAR_BITMAPS_H,
      .posx = 0,
      .posy = 0,
      .is_inverted = false};
  bitmap_definition sd_status_btmp_def = {.bitmap = get_sd_status_bitmap(),
      .width = TOP_BAR_BITMAPS_W,
      .height = TOP_BAR_BITMAPS_H,
      .posx = TOP_BAR_BITMAPS_W,
      .posy = 0,
      .is_inverted = false};
  bitmap_definition sx1278_status_btmp_def = {
      .bitmap = get_sx1278_status_bitmap(),
      .width = TOP_BAR_BITMAPS_W,
      .height = TOP_BAR_BITMAPS_H,
      .posx = TOP_BAR_BITMAPS_W * 2,
      .posy = 0,
      .is_inverted = false};
  bitmap_definition en160_status_btmp_def = {
      .bitmap = get_en160_status_bitmap(),
      .width = TOP_BAR_BITMAPS_W,
      .height = TOP_BAR_BITMAPS_H,
      .posx = TOP_BAR_BITMAPS_W * 3,
      .posy = 0,
      .is_inverted = false};
  bitmap_definition notifications_btmp_def = {
      .bitmap = get_notifications_bitmap(),
      .width = TOP_BAR_BITMAPS_W,
      .height = TOP_BAR_BITMAPS_H,
      .posx = TOP_BAR_BITMAPS_W * 4,
      .posy = 0,
      .is_inverted = false};
  bitmap_definition alarm_status_btmp_def = {
      .bitmap = get_alarm_status_bitmap(),
      .width = TOP_BAR_BITMAPS_W,
      .height = TOP_BAR_BITMAPS_H,
      .posx = TOP_BAR_BITMAPS_W * 5,
      .posy = 0,
      .is_inverted = false};
  layer *top_bar_bitmaps_ly = get_layer_by_name(home_page_inst->ly,
      "top_bar_bitmaps");
  layer_add_bitmap_definition(top_bar_bitmaps_ly, battery_level_btmp_def);
  layer_add_bitmap_definition(top_bar_bitmaps_ly, sd_status_btmp_def);
  layer_add_bitmap_definition(top_bar_bitmaps_ly, sx1278_status_btmp_def);
  layer_add_bitmap_definition(top_bar_bitmaps_ly, en160_status_btmp_def);
  layer_add_bitmap_definition(top_bar_bitmaps_ly, notifications_btmp_def);
  layer_add_bitmap_definition(top_bar_bitmaps_ly, alarm_status_btmp_def);
}

void update_rxcontinuous_indicator() {
  bitmap_definition rxcontinuous_indicator_btmp_def = {
      .bitmap = get_rxcontinuous_indicator_bitmap(),
      .width = 21,
      .height = 16,
      .posx = TOP_BAR_BITMAPS_W * 5, // aligned with the alarm top tray icon
      .posy = 24,
      .is_inverted = false,
  };
  layer *top_bar_bitmaps_ly = get_layer_by_name(home_page_inst->ly,
      "top_bar_bitmaps");
  layer_add_bitmap_definition(top_bar_bitmaps_ly,
      rxcontinuous_indicator_btmp_def);
}

void update_clock(uint8_t start_pix_w, uint8_t start_pix_h, uint8_t spacing) {
  bitmap_definition hour_tens_btmp_def = {
      .bitmap = home_page_inst->clock_bmp->hour_tens_bitmap,
      .width = CLOCK_DIGIT_BITMAPS_W,
      .height = CLOCK_DIGIT_BITMAPS_H,
      .posx = start_pix_w,
      .posy = start_pix_h,
      .is_inverted = false};
  start_pix_w += CLOCK_DIGIT_BITMAPS_W + spacing;
  bitmap_definition hour_units_btmp_def = {
      .bitmap = home_page_inst->clock_bmp->hour_units_bitmap,
      .width = CLOCK_DIGIT_BITMAPS_W,
      .height = CLOCK_DIGIT_BITMAPS_H,
      .posx = start_pix_w,
      .posy = start_pix_h,
      .is_inverted = false};
  start_pix_w += CLOCK_DIGIT_BITMAPS_W + spacing;
  bitmap_definition clock_dots_btmp_def = {.bitmap = clock_dots,
      .width = CLOCK_DOTS_BITMAPS_W,
      .height = CLOCK_DOTS_BITMAPS_H,
      .posx = start_pix_w,
      .posy = start_pix_h,
      .is_inverted = false};
  start_pix_w += CLOCK_DOTS_BITMAPS_W + spacing;
  bitmap_definition minute_tens_btmp_def = {
      .bitmap = home_page_inst->clock_bmp->minute_tens_bitmap,
      .width = CLOCK_DIGIT_BITMAPS_W,
      .height = CLOCK_DIGIT_BITMAPS_H,
      .posx = start_pix_w,
      .posy = start_pix_h,
      .is_inverted = false};
  start_pix_w += CLOCK_DIGIT_BITMAPS_W + spacing;
  bitmap_definition minute_units_btmp_def = {
      .bitmap = home_page_inst->clock_bmp->minute_units_bitmap,
      .width = CLOCK_DIGIT_BITMAPS_W,
      .height = CLOCK_DIGIT_BITMAPS_H,
      .posx = start_pix_w,
      .posy = start_pix_h,
      .is_inverted = false};
  start_pix_w += CLOCK_DIGIT_BITMAPS_W + spacing;
  bitmap_definition clock_dots_1_btmp_def = {.bitmap = clock_dots,
      .width = CLOCK_DOTS_BITMAPS_W,
      .height = CLOCK_DOTS_BITMAPS_H,
      .posx = start_pix_w,
      .posy = start_pix_h,
      .is_inverted = false};
  start_pix_w += CLOCK_DOTS_BITMAPS_W + spacing;
  bitmap_definition second_tens_btmp_def = {
      .bitmap = home_page_inst->clock_bmp->second_tens_bitmap,
      .width = CLOCK_DIGIT_BITMAPS_W,
      .height = CLOCK_DIGIT_BITMAPS_H,
      .posx = start_pix_w,
      .posy = start_pix_h,
      .is_inverted = false};
  start_pix_w += CLOCK_DIGIT_BITMAPS_W + spacing;
  bitmap_definition second_units_btmp_def = {
      .bitmap = home_page_inst->clock_bmp->second_units_bitmap,
      .width = CLOCK_DIGIT_BITMAPS_W,
      .height = CLOCK_DIGIT_BITMAPS_H,
      .posx = start_pix_w,
      .posy = start_pix_h,
      .is_inverted = false};
  layer *clock_bitmaps_ly = get_layer_by_name(home_page_inst->ly,
      "clock_bitmaps");
  layer_add_bitmap_definition(clock_bitmaps_ly, hour_tens_btmp_def);
  layer_add_bitmap_definition(clock_bitmaps_ly, hour_units_btmp_def);
  layer_add_bitmap_definition(clock_bitmaps_ly, clock_dots_btmp_def);
  layer_add_bitmap_definition(clock_bitmaps_ly, minute_tens_btmp_def);
  layer_add_bitmap_definition(clock_bitmaps_ly, minute_units_btmp_def);
  layer_add_bitmap_definition(clock_bitmaps_ly, clock_dots_1_btmp_def);
  layer_add_bitmap_definition(clock_bitmaps_ly, second_tens_btmp_def);
  layer_add_bitmap_definition(clock_bitmaps_ly, second_units_btmp_def);
}

void update_texts() {
  uint8_t aqi_value = ens160_read_aqi(&(drivers->ens160));
  char *aqi_value_str;
  if (aqi_value == 1) {
    aqi_value_str = "good";
  } else if (aqi_value == 2) {
    aqi_value_str = "fair";
  } else if (aqi_value == 3) {
    aqi_value_str = "mid ";
  } else if (aqi_value == 4) {
    aqi_value_str = "poor";
  } else if (aqi_value == 5) {
    aqi_value_str = "bad ";
  } else {
    aqi_value_str = "N/A ";
  }
  text_area aqi_text = {.text = "AQI",
      .posx = 0,
      .posy = 3,
      .is_inverted = false};
  text_area aqi_value_text = {.text = aqi_value_str,
      .posx = 0,
      .posy = 4,
      .is_inverted = false};
  text_area ulmp_text = {.text = "ULMP",
      .posx = 0,
      .posy = 6,
      .is_inverted = false};
  text_area ulmp_addr_text = {.text = malloc_memories_inst->ulmp_addr_str,
      .posx = 0,
      .posy = 7,
      .is_inverted = false};
  text_area sram_text = {.text = "SRAM",
      .posx = 12,
      .posy = 6,
      .is_inverted = false};
  static char used_ram_str[11];
  uint32_t used_ram = get_free_heap();
  uint32_t used_ram_kb = used_ram * 0.009765625;
  sprintf(used_ram_str, "%u", used_ram_kb);
  text_area used_ram_text = {.text = used_ram_str,
      .posx = 12,
      .posy = 7,
      .is_inverted = false};
  char *weekday = from_dotw_to_weekday(
      drivers->internal_rtc.internal_datetime.dotw);
  static char date_str[13];
  snprintf(date_str,
      10,
      "%02d/%02d/%02d",
      drivers->internal_rtc.internal_datetime.day,
      drivers->internal_rtc.internal_datetime.month,
      drivers->internal_rtc.internal_datetime.year - 2000);
  text_area day_text = {.text = weekday,
      .posx = 7,
      .posy = 6,
      .is_inverted = false};
  text_area date_text = {.text = date_str,
      .posx = 5,
      .posy = 7,
      .is_inverted = false};
  layer *text_areas_ly = get_layer_by_name(home_page_inst->ly, "text_areas");
  layer_add_text_area(text_areas_ly, aqi_text);
  layer_add_text_area(text_areas_ly, aqi_value_text);
  layer_add_text_area(text_areas_ly, ulmp_text);
  layer_add_text_area(text_areas_ly, ulmp_addr_text);
  layer_add_text_area(text_areas_ly, sram_text);
  layer_add_text_area(text_areas_ly, used_ram_text);
  layer_add_text_area(text_areas_ly, day_text);
  layer_add_text_area(text_areas_ly, date_text);
}

void create_lines() {
  uint8_t start_pix_h = 23;
  uint8_t line_padding = 4;
  line l = create_line(create_point(0, start_pix_h - line_padding),
      create_point(SSD1306_WIDTH - 1, start_pix_h - line_padding));
  line l1 = create_line(
      create_point(0, start_pix_h + CLOCK_DOTS_BITMAPS_H + line_padding),
      create_point(SSD1306_WIDTH - 1,
          start_pix_h + CLOCK_DOTS_BITMAPS_H + line_padding));
  line date_line_left = create_line(
      create_point(32,
          start_pix_h + CLOCK_DOTS_BITMAPS_H + line_padding * 2 - 3),
      create_point(32, SSD1306_HEIGHT - 1));
  line date_line_right = create_line(
      create_point(SSD1306_WIDTH - 35,
          start_pix_h + CLOCK_DOTS_BITMAPS_H + line_padding * 2 - 3),
      create_point(SSD1306_WIDTH - 35, SSD1306_HEIGHT - 1));
  layer *lines_ly = get_layer_by_name(home_page_inst->ly, "lines");
  layer_add_line(lines_ly, l);
  layer_add_line(lines_ly, l1);
  layer_add_line(lines_ly, date_line_left);
  layer_add_line(lines_ly, date_line_right);
}

void display_home_page() {
  uint8_t start_pix_w = 28;
  uint8_t start_pix_h = 23;
  uint8_t spacing = 2;
  update_top_bar();
  update_rxcontinuous_indicator();
  update_clock_bitmaps();
  update_clock(start_pix_w, start_pix_h, spacing);
  update_texts();
  ssd1306_get_mutex(&(drivers->ssd1306));
  layout_draw_all_layers(home_page_inst->ly);
  ssd1306_show(&(drivers->ssd1306));
  ssd1306_release_mutex(&(drivers->ssd1306));
  layer *top_bar_bitmaps = get_layer_by_name(home_page_inst->ly,
      "top_bar_bitmaps");
  layer *clock_bitmaps = get_layer_by_name(home_page_inst->ly,
      "clock_bitmaps");
  layer *text_areas = get_layer_by_name(home_page_inst->ly, "text_areas");
  layer_remove_bitmap_definitions(top_bar_bitmaps);
  layer_remove_bitmap_definitions(clock_bitmaps);
  layer_remove_text_areas(text_areas);
}
