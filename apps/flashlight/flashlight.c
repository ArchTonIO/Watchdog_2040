#include "apps/flashlight/flashlight.h"

#include "core/components/hw_manager.h"
#include "core/data_structures/string_list.h"
#include "core/hardware_drivers/ssd1306.h"

void set_flashlight_on() {
  ssd1306_invert(drivers->oled_screen, true);
  ssd1306_clear(drivers->oled_screen);
  ssd1306_show(drivers->oled_screen);
}

void set_flashlight_off() {
  ssd1306_invert(drivers->oled_screen, false);
  ssd1306_clear(drivers->oled_screen);
  ssd1306_show(drivers->oled_screen);
}

void enter_flashlight_screen() {
  set_flashlight_on();
  bool flashlight_state = false;
  while (true) {
    joystick_update(drivers->joystick);
    if (joystick_get_direction(drivers->joystick) == W) {
      break;
    }
  }
  sleep_ms(200);
  set_flashlight_off();
}
