#include "components/home_page.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "components/hw_manager.h"
#include "components/malloc_mascot.h"
#include "components/msg_manager.h"
#include "data_structures/string_list.h"
#include "graphics/bitmaps.h"
#include "graphics/graphic_primitives.h"
#include "hardware_drivers/battery.h"
#include "hardware_drivers/rtc_time.h"
#include "hardware_drivers/sdcard.h"
#include "hardware_drivers/ssd1306.h"

home_page *home_page_inst;

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
  home_page_inst = new_home_page;
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

uint8_t smooth_out_battery_level() {
  uint8_t samples = 10;
  uint16_t sum = 0;
  for (uint8_t i = 0; i < samples; i++) {
    sum += battery_get_percentage(drivers->battery);
    sleep_ms(5);
  }
  return sum / samples;
}

void process_system_state() {
  home_page_inst->battery_level = smooth_out_battery_level();
  home_page_inst->alarm_set = drivers->rtc->alarm_set;
  home_page_inst->bpm = 0;  // TODO: implement bpm reading
  home_page_inst->spo2 = 0; // TODO: implement spo2 reading
  home_page_inst->aqi = ens160_read_aqi(drivers->air_quality_sensor);
  home_page_inst->notifications = msg_man_inst->received_msgs_count;
  process_messages();
}

uint8_t *get_battery_level_bitmap() {
  if (home_page_inst->battery_level == 100)
    home_page_inst->battery_level = 99;
  return battery_level_bitmaps[home_page_inst->battery_level / 10];
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

void display_home_page() {
  ssd1306_draw_bitmap(drivers->oled_screen,
      0,
      0,
      get_battery_level_bitmap(),
      TOP_BAR_BITMAPS_W,
      TOP_BAR_BITMAPS_H,
      false);
  ssd1306_draw_bitmap(drivers->oled_screen,
      TOP_BAR_BITMAPS_W,
      0,
      get_sd_status_bitmap(),
      TOP_BAR_BITMAPS_W,
      TOP_BAR_BITMAPS_H,
      false);
  ssd1306_draw_bitmap(drivers->oled_screen,
      TOP_BAR_BITMAPS_W * 2,
      0,
      get_sx1278_status_bitmap(),
      TOP_BAR_BITMAPS_W,
      TOP_BAR_BITMAPS_H,
      false);
  ssd1306_draw_bitmap(drivers->oled_screen,
      TOP_BAR_BITMAPS_W * 3,
      0,
      get_en160_status_bitmap(),
      TOP_BAR_BITMAPS_W,
      TOP_BAR_BITMAPS_H,
      false);
  ssd1306_draw_bitmap(drivers->oled_screen,
      TOP_BAR_BITMAPS_W * 4,
      0,
      get_notifications_bitmap(),
      TOP_BAR_BITMAPS_W,
      TOP_BAR_BITMAPS_H,
      false);
  ssd1306_draw_bitmap(drivers->oled_screen,
      TOP_BAR_BITMAPS_W * 5,
      0,
      get_alarm_status_bitmap(),
      TOP_BAR_BITMAPS_W,
      TOP_BAR_BITMAPS_H,
      false);
  update_clock_bitmaps();
  uint8_t start_pix_w = 28;
  uint8_t start_pix_h = 23;
  uint8_t spacing = 2;
  ssd1306_draw_bitmap(drivers->oled_screen,
      start_pix_w,
      start_pix_h,
      home_page_inst->clock_bmp->hour_tens_bitmap,
      CLOCK_DIGIT_BITMAPS_W,
      CLOCK_DIGIT_BITMAPS_H,
      false);
  start_pix_w += CLOCK_DIGIT_BITMAPS_W + spacing;
  ssd1306_draw_bitmap(drivers->oled_screen,
      start_pix_w,
      start_pix_h,
      home_page_inst->clock_bmp->hour_units_bitmap,
      CLOCK_DIGIT_BITMAPS_W,
      CLOCK_DIGIT_BITMAPS_H,
      false);
  start_pix_w += CLOCK_DIGIT_BITMAPS_W + spacing;
  ssd1306_draw_bitmap(drivers->oled_screen,
      start_pix_w,
      start_pix_h,
      clock_dots,
      CLOCK_DOTS_BITMAPS_W,
      CLOCK_DOTS_BITMAPS_H,
      false);
  start_pix_w += CLOCK_DOTS_BITMAPS_W + spacing;
  ssd1306_draw_bitmap(drivers->oled_screen,
      start_pix_w,
      start_pix_h,
      home_page_inst->clock_bmp->minute_tens_bitmap,
      CLOCK_DIGIT_BITMAPS_W,
      CLOCK_DIGIT_BITMAPS_H,
      false);
  start_pix_w += CLOCK_DIGIT_BITMAPS_W + spacing;
  ssd1306_draw_bitmap(drivers->oled_screen,
      start_pix_w,
      start_pix_h,
      home_page_inst->clock_bmp->minute_units_bitmap,
      CLOCK_DIGIT_BITMAPS_W,
      CLOCK_DIGIT_BITMAPS_H,
      false);
  start_pix_w += CLOCK_DIGIT_BITMAPS_W + spacing;
  ssd1306_draw_bitmap(drivers->oled_screen,
      start_pix_w,
      start_pix_h,
      clock_dots,
      CLOCK_DOTS_BITMAPS_W,
      CLOCK_DOTS_BITMAPS_H,
      false);
  start_pix_w += CLOCK_DOTS_BITMAPS_W + spacing;
  ssd1306_draw_bitmap(drivers->oled_screen,
      start_pix_w,
      start_pix_h,
      home_page_inst->clock_bmp->second_tens_bitmap,
      CLOCK_DIGIT_BITMAPS_W,
      CLOCK_DIGIT_BITMAPS_H,
      false);
  start_pix_w += CLOCK_DIGIT_BITMAPS_W + spacing;
  ssd1306_draw_bitmap(drivers->oled_screen,
      start_pix_w,
      start_pix_h,
      home_page_inst->clock_bmp->second_units_bitmap,
      CLOCK_DIGIT_BITMAPS_W,
      CLOCK_DIGIT_BITMAPS_H,
      false);
  ssd1306_print(drivers->oled_screen, "BPM", 0, 3, false);
  ssd1306_print(drivers->oled_screen,
      " 70",
      0,
      4,
      false); // todo: change with real value
  ssd1306_print(drivers->oled_screen,
      "AQI",
      13,
      3,
      false); // todo: change with real value
  ssd1306_print(drivers->oled_screen, " 1 ", 13, 4, false);
  uint8_t line_padding = 4;
  line l = create_line(create_point(0, start_pix_h - line_padding),
      create_point(SSD1306_WIDTH - 1, start_pix_h - line_padding));
  draw_line(l);
  line l1 = create_line(
      create_point(0, start_pix_h + CLOCK_DOTS_BITMAPS_H + line_padding),
      create_point(SSD1306_WIDTH - 1,
          start_pix_h + CLOCK_DOTS_BITMAPS_H + line_padding));
  draw_line(l1);
  uint32_t used_ram = get_free_heap();
  uint32_t used_ram_kb = used_ram * 0.009765625;
  char used_ram_str[11];
  sprintf(used_ram_str, "%u", used_ram_kb);
  ssd1306_print(drivers->oled_screen, "ULMP", 0, 6, false);
  ssd1306_print(drivers->oled_screen,
      malloc_memories_inst->ulmp_addr_str,
      0,
      7,
      false);
  ssd1306_print(drivers->oled_screen, "SRAM", 12, 6, false);
  ssd1306_print(drivers->oled_screen, used_ram_str, 12, 7, false);
  ssd1306_show(drivers->oled_screen);
}
