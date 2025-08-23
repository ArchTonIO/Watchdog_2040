#include "core/components/home_page.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "apps/msg_manager/msg_manager.h"
#include "apps/time_submenus/set_alarm_submenu.h"
#include "apps/time_submenus/time_utils.h"
#include "core/components/hw_manager.h"
#include "core/components/malloc_mascot.h"
#include "core/data_structures/string_list.h"
#include "core/graphics/bitmaps.h"
#include "core/graphics/graphic_primitives.h"
#include "core/hardware_drivers/battery.h"
#include "core/hardware_drivers/rtc_time.h"
#include "core/hardware_drivers/sdcard.h"
#include "core/hardware_drivers/ssd1306.h"

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
  create_lines();
  return new_home_page;
}

void check_pheripherals() {
  if (drivers->sd_card->is_working)
    home_page_inst->sd_status = 1;
  else
    home_page_inst->sd_status = 0;

  if (drivers->lora_module->is_working)
    home_page_inst->sx1278_status = 1;
  else
    home_page_inst->sx1278_status = 0;

  if (drivers->air_quality_sensor->is_working)
    home_page_inst->en160_status = 1;
  else
    home_page_inst->en160_status = 0;
}

void process_system_state() {
  home_page_inst->battery_level = battery_get_percentage(drivers->battery);
  home_page_inst->alarm_set = drivers->rtc->alarm_set;
  home_page_inst->bpm = 0;  // TODO: implement bpm reading
  home_page_inst->spo2 = 0; // TODO: implement spo2 reading
  home_page_inst->aqi = ens160_read_aqi(drivers->air_quality_sensor);
  home_page_inst->notifications = msg_man_inst->received_msgs_count;
}

uint8_t *get_battery_level_bitmap() {
  if (home_page_inst->battery_level >= 100) {
    return battery_4_bars;
  } else if (home_page_inst->battery_level >= 75) {
    return battery_3_bars;
  } else if (home_page_inst->battery_level >= 50) {
    return battery_2_bars;
  } else if (home_page_inst->battery_level >= 25) {
    return battery_1_bar;
  } else {
    return battery_empty;
  }
}

uint8_t *get_sd_status_bitmap() {
  if (home_page_inst->sd_status)
    return microsd_working;
  else
    return microsd_not_working;
}

uint8_t *get_sx1278_status_bitmap() {
  if (home_page_inst->sx1278_status)
    return lora_working;
  else
    return lora_not_working;
}

uint8_t *get_en160_status_bitmap() {
  if (home_page_inst->en160_status)
    return ens160_working;
  else
    return ens160_not_working;
}

uint8_t *get_notifications_bitmap() {
  if (home_page_inst->notifications > 10)
    return num_msg_bitmaps[11];
  else
    return num_msg_bitmaps[home_page_inst->notifications];
}

uint8_t *get_alarm_status_bitmap() {
  if (home_page_inst->alarm_set)
    return alarm_enabled;
  else
    return alarm_disabled;
}

void update_clock_bitmaps() {
  update_time(drivers->rtc);
  int8_t hour = drivers->rtc->internal_datetime.hour;
  int8_t minute = drivers->rtc->internal_datetime.min;
  int8_t second = drivers->rtc->internal_datetime.sec;
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
  layout_add_bitmap_definition(home_page_inst->ly, battery_level_btmp_def);
  layout_add_bitmap_definition(home_page_inst->ly, sd_status_btmp_def);
  layout_add_bitmap_definition(home_page_inst->ly, sx1278_status_btmp_def);
  layout_add_bitmap_definition(home_page_inst->ly, en160_status_btmp_def);
  layout_add_bitmap_definition(home_page_inst->ly, notifications_btmp_def);
  layout_add_bitmap_definition(home_page_inst->ly, alarm_status_btmp_def);
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
  layout_add_bitmap_definition(home_page_inst->ly, hour_tens_btmp_def);
  layout_add_bitmap_definition(home_page_inst->ly, hour_units_btmp_def);
  layout_add_bitmap_definition(home_page_inst->ly, clock_dots_btmp_def);
  layout_add_bitmap_definition(home_page_inst->ly, minute_tens_btmp_def);
  layout_add_bitmap_definition(home_page_inst->ly, minute_units_btmp_def);
  layout_add_bitmap_definition(home_page_inst->ly, clock_dots_1_btmp_def);
  layout_add_bitmap_definition(home_page_inst->ly, second_tens_btmp_def);
  layout_add_bitmap_definition(home_page_inst->ly, second_units_btmp_def);
}

void update_texts() {
  text_area aqi_text = {.text = "AQI",
      .posx = 0,
      .posy = 3,
      .is_inverted = false};
  text_area aqi_value_text = {.text = " 1 ", // todo: change with real value
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
  char *weekday = from_dotw_to_weekday(drivers->rtc->internal_datetime.dotw);
  static char date_str[13];
  snprintf(date_str,
      10,
      "%02d/%02d/%02d",
      drivers->rtc->internal_datetime.day,
      drivers->rtc->internal_datetime.month,
      drivers->rtc->internal_datetime.year - 2000);
  text_area day_text = {.text = weekday,
      .posx = 7,
      .posy = 6,
      .is_inverted = false};
  text_area date_text = {.text = date_str,
      .posx = 5,
      .posy = 7,
      .is_inverted = false};
  layout_add_text_area(home_page_inst->ly, aqi_text);
  layout_add_text_area(home_page_inst->ly, aqi_value_text);
  layout_add_text_area(home_page_inst->ly, ulmp_text);
  layout_add_text_area(home_page_inst->ly, ulmp_addr_text);
  layout_add_text_area(home_page_inst->ly, sram_text);
  layout_add_text_area(home_page_inst->ly, used_ram_text);
  layout_add_text_area(home_page_inst->ly, day_text);
  layout_add_text_area(home_page_inst->ly, date_text);
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
  layout_add_line(home_page_inst->ly, l);
  layout_add_line(home_page_inst->ly, l1);
  layout_add_line(home_page_inst->ly, date_line_left);
  layout_add_line(home_page_inst->ly, date_line_right);
}

void display_home_page() {
  uint8_t start_pix_w = 28;
  uint8_t start_pix_h = 23;
  uint8_t spacing = 2;
  update_top_bar();
  update_clock_bitmaps();
  update_clock(start_pix_w, start_pix_h, spacing);
  update_texts();
  ssd1306_get_mutex(drivers->oled_screen);
  layout_draw(home_page_inst->ly);
  ssd1306_show(drivers->oled_screen);
  ssd1306_release_mutex(drivers->oled_screen);
  layout_flush_bitmap_definitions(home_page_inst->ly);
  layout_flush_text_areas(home_page_inst->ly);
}
