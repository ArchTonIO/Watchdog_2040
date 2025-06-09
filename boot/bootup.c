#include "boot/bootup.h"

#include "pico/multicore.h"

#include "components/hw_manager.h"
#include "graphics/bitmaps.h"
#include "hardware_drivers/ssd1306.h"

void display_boot_animation() {
  for (uint8_t i = 0; i < 4; i++) {
    ssd1306_draw_bitmap(drivers->oled_screen,
        ANIMATION_X,
        ANIMATION_Y,
        startup_animation[i],
        ANIMATION_WIDTH,
        ANIMATION_HEIGHT,
        0);
    ssd1306_show(drivers->oled_screen);
  }
}

void display_splash_screen() {
  ssd1306_clear(drivers->oled_screen);
  ssd1306_print(drivers->oled_screen, "WATCHDOG_2040", 0, 4, 0);
  for (uint8_t t = 0; t < 2; t++)
    for (uint8_t i = 0; i < 16; i++) {
      ssd1306_draw_bitmap(drivers->oled_screen,
          ANIMATION_X,
          ANIMATION_Y,
          startup_animation[i],
          ANIMATION_WIDTH,
          ANIMATION_HEIGHT,
          0);
      ssd1306_show(drivers->oled_screen);
    }
  ssd1306_clear(drivers->oled_screen);
  ssd1306_show(drivers->oled_screen);
}

void display_running_checks(uint32_t fifo_data) {
  if (fifo_data == ENS160_OK)
    ssd1306_print(drivers->oled_screen, "[OK] ENS160", 0, 2, 0);
  else if (fifo_data == ENS160_ERR)
    ssd1306_print(drivers->oled_screen, "[ERR] ENS160", 0, 2, 0);
  if (fifo_data == SX1278_OK)
    ssd1306_print(drivers->oled_screen, "[OK] SX1278", 0, 3, 0);
  else if (fifo_data == SX1278_ERR)
    ssd1306_print(drivers->oled_screen, "[ERR] SX1278", 0, 3, 0);
  if (fifo_data == BATTERY_OK)
    ssd1306_print(drivers->oled_screen, "[OK] BATTERY", 0, 4, 0);
  if (fifo_data == BATTERY_ERR)
    ssd1306_print(drivers->oled_screen, "[ERR] BATTERY", 0, 4, 0);
  if (fifo_data == JOYSTICK_OK)
    ssd1306_print(drivers->oled_screen, "[OK] JOYSTICK", 0, 5, 0);
  if (fifo_data == JOYSTICK_ERR)
    ssd1306_print(drivers->oled_screen, "[ERR] JOYSTICK", 0, 5, 0);
  if (fifo_data == SDCARD_OK)
    ssd1306_print(drivers->oled_screen, "[OK] SD CARD", 0, 6, 0);
  else if (fifo_data == SDCARD_ERR)
    ssd1306_print(drivers->oled_screen, "[ERR] SD CARD", 0, 6, 0);
  if (fifo_data == RTC_OK)
    ssd1306_print(drivers->oled_screen, "[OK] RTC", 0, 7, 0);
  ssd1306_show(drivers->oled_screen);
}

void display_bootup_screen() {
  ssd1306_print(drivers->oled_screen, "INITIALIZING", 0, 0, 0);
  ssd1306_print(drivers->oled_screen, "HARDWARE DRIVERS...", 0, 1, 0);
  uint32_t fifo_data = 0x0000;
  while (fifo_data != CHECKS_END) {
    fifo_data = multicore_fifo_pop_blocking();
    display_boot_animation();
    display_running_checks(fifo_data);
  }
  display_splash_screen();
  multicore_fifo_push_blocking(CORE_1_OP_DONE);
}