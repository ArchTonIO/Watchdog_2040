#include "core/components/include/system_tray.h"

#include <pico/time.h>

#include "apps/flashlight/include/flashlight.h"
#include "apps/messaging/include/msg_manager.h"
#include "apps/system_app/include/system_app.h"
#include "core/components/include/bitmaps.h"
#include "core/components/include/hw_manager.h"
#include "core/graphics/include/graphic_primitives.h"
#include "core/graphics/include/layout.h"
#include "core/hardware_drivers/include/haptics.h"
#include "core/hardware_drivers/include/ssd1306.h"

void system_tray_init(system_tray_t *tray) {
  init_flashlight();

  uint8_t spacing = 4;
  bitmap_definition settings_bd = {.bitmap = settings_icon,
      .width = TOP_BAR_BITMAPS_W,
      .height = TOP_BAR_BITMAPS_H,
      .posx = 0,
      .posy = 0,
      .is_inverted = false};
  bitmap_definition notif_on_bd = {.bitmap = notif_on_icon,
      .width = TOP_BAR_BITMAPS_W,
      .height = TOP_BAR_BITMAPS_H,
      .posx = TOP_BAR_BITMAPS_W + spacing,
      .posy = 0,
      .is_inverted = false};
  bitmap_definition notif_off_bd = {.bitmap = notif_off_icon,
      .width = TOP_BAR_BITMAPS_W,
      .height = TOP_BAR_BITMAPS_H,
      .posx = TOP_BAR_BITMAPS_W + spacing,
      .posy = 0,
      .is_inverted = false};
  bitmap_definition flashlight_on_bd = {.bitmap = flashlight_on_icon,
      .width = TOP_BAR_BITMAPS_W,
      .height = TOP_BAR_BITMAPS_H,
      .posx = TOP_BAR_BITMAPS_W * 2 + spacing,
      .posy = 0,
      .is_inverted = false};
  bitmap_definition flashlight_off_bd = {.bitmap = flashlight_off_icon,
      .width = TOP_BAR_BITMAPS_W,
      .height = TOP_BAR_BITMAPS_H,
      .posx = TOP_BAR_BITMAPS_W * 2 + spacing,
      .posy = 0,
      .is_inverted = false};
  bitmap_definition haptics_on_bd = {.bitmap = haptics_on_icon,
      .width = TOP_BAR_BITMAPS_W,
      .height = TOP_BAR_BITMAPS_H,
      .posx = TOP_BAR_BITMAPS_W * 3 + spacing,
      .posy = 0,
      .is_inverted = false};
  bitmap_definition haptics_off_bd = {.bitmap = haptics_off_icon,
      .width = TOP_BAR_BITMAPS_W,
      .height = TOP_BAR_BITMAPS_H,
      .posx = TOP_BAR_BITMAPS_W * 3 + spacing,
      .posy = 0,
      .is_inverted = false};
  bitmap_definition brightness_auto_bd = {.bitmap = brightness_auto_icon,
      .width = TOP_BAR_BITMAPS_W,
      .height = TOP_BAR_BITMAPS_H,
      .posx = TOP_BAR_BITMAPS_W * 4 + spacing,
      .posy = 0,
      .is_inverted = false};
  bitmap_definition brightness_manual_bd = {.bitmap = brightness_manual_icon,
      .width = TOP_BAR_BITMAPS_W,
      .height = TOP_BAR_BITMAPS_H,
      .posx = TOP_BAR_BITMAPS_W * 4 + spacing,
      .posy = 0,
      .is_inverted = false};
  bitmap_definition rxcontinuous_on_bd = {.bitmap = rxcontinuous_on_icon,
      .width = TOP_BAR_BITMAPS_W,
      .height = TOP_BAR_BITMAPS_H,
      .posx = TOP_BAR_BITMAPS_W * 5 + spacing,
      .posy = 0,
      .is_inverted = false};
  bitmap_definition rxcontinuous_off_bd = {.bitmap = rxcontinuous_off_icon,
      .width = TOP_BAR_BITMAPS_W,
      .height = TOP_BAR_BITMAPS_H,
      .posx = TOP_BAR_BITMAPS_W * 5 + spacing,
      .posy = 0,
      .is_inverted = false};

  button_t settings_button;
  settings_button.bd = settings_bd;
  settings_button.press_callback = system_app_launch;
  settings_button.selected = false;

  toggle_button_t notif_button;
  notif_button.bd_state_1 = notif_on_bd;
  notif_button.bd_state_2 = notif_off_bd;
  notif_button.press_callback_1 = enable_message_notifications;
  notif_button.press_callback_2 = disable_message_notifications;
  notif_button.selected = false;
  notif_button.state = false;

  toggle_button_t flashlight_button;
  flashlight_button.bd_state_1 = flashlight_off_bd;
  flashlight_button.bd_state_2 = flashlight_on_bd;
  flashlight_button.press_callback_1 = set_led_flashlight_off;
  flashlight_button.press_callback_2 = set_led_flashlight_on;
  flashlight_button.selected = false;
  flashlight_button.state = false;

  toggle_button_t haptics_button;
  haptics_button.bd_state_1 = haptics_on_bd;
  haptics_button.bd_state_2 = haptics_off_bd;
  haptics_button.press_callback_1 = haptics_enable;
  haptics_button.press_callback_2 = haptics_disable;
  haptics_button.selected = false;
  haptics_button.state = false;

  toggle_button_t brightness_button;
  brightness_button.bd_state_1 = brightness_auto_bd;
  brightness_button.bd_state_2 = brightness_manual_bd;
  brightness_button.press_callback_1 = enable_auto_brightness;
  brightness_button.press_callback_2 = disable_auto_brightness;
  brightness_button.selected = false;
  brightness_button.state = false;

  toggle_button_t rxcontinuous_button;
  rxcontinuous_button.bd_state_1 = rxcontinuous_off_bd;
  rxcontinuous_button.bd_state_2 = rxcontinuous_on_bd;
  rxcontinuous_button.press_callback_1 = toggle_continuous_rx;
  rxcontinuous_button.press_callback_2 = toggle_continuous_rx;
  rxcontinuous_button.selected = false;
  rxcontinuous_button.state = false;

  tray->tray_layout = layout_init();
  tray->selected_button_index = 0;
  layout_add_layer(tray->tray_layout, BUTTONS);
  layer *buttons_layer = get_layer_by_name(tray->tray_layout, BUTTONS);
  layer_add_button(buttons_layer, settings_button);
  layer_add_toggle_button(buttons_layer, notif_button);
  layer_add_toggle_button(buttons_layer, flashlight_button);
  layer_add_toggle_button(buttons_layer, haptics_button);
  layer_add_toggle_button(buttons_layer, brightness_button);
  layer_add_toggle_button(buttons_layer, rxcontinuous_button);
}

void system_tray_refresh(system_tray_t *tray) {
  layer *buttons_layer = get_layer_by_name(tray->tray_layout, BUTTONS);
  layer_clear_bitmap_definitions(buttons_layer);
  if (tray->selected_button_index == 0) {
    buttons_layer->buttons[0].selected = true;
    buttons_layer->toggle_buttons[0].selected = false;
    for (size_t i = 0; i < buttons_layer->buttons_count; i++) {
      if (i == 0)
        continue;
      buttons_layer->buttons[i].selected = false;
    }
  } else {
    buttons_layer->buttons[0].selected = false;
    for (size_t i = 0; i < buttons_layer->toggle_buttons_count; i++) {
      if (tray->selected_button_index - 1 == i) {
        buttons_layer->toggle_buttons[i].selected = true;
        continue;
      }
      buttons_layer->toggle_buttons[i].selected = false;
    }
  }
  layout_draw_all_layers(tray->tray_layout);
  ssd1306_show(&(drivers->ssd1306));
}

void system_tray_press_button(system_tray_t *tray) {
  layer *buttons_layer = get_layer_by_name(tray->tray_layout, BUTTONS);
  if (tray->selected_button_index == 0) {
    buttons_layer->buttons[0].press_callback();
    return;
  }
  if (buttons_layer->toggle_buttons[tray->selected_button_index - 1].state) {
    buttons_layer->toggle_buttons[tray->selected_button_index - 1]
        .press_callback_1();
    buttons_layer->toggle_buttons[tray->selected_button_index - 1]
        .state = false;
  } else {
    buttons_layer->toggle_buttons[tray->selected_button_index - 1]
        .press_callback_2();
    buttons_layer->toggle_buttons[tray->selected_button_index - 1]
        .state = true;
  }
}

inline void system_tray_select_left(system_tray_t *tray) {
  if (tray->selected_button_index > 0)
    tray->selected_button_index--;
}

inline void system_tray_select_right(system_tray_t *tray) {
  if (tray->selected_button_index < 5)
    tray->selected_button_index++;
}

void system_tray_expand(system_tray_t *tray) {
  haptic_auto_pulse();
  ssd1306_show(&(drivers->ssd1306));
}

void system_tray_collapse(system_tray_t *tray) {
  haptic_auto_pulse();
  tray->selected_button_index = 0;
  layer *buttons_layer = get_layer_by_name(tray->tray_layout, BUTTONS);
  for (size_t i = 0; i < buttons_layer->buttons_count; i++) {
    buttons_layer->buttons[i].selected = false;
  }
  for (size_t i = 0; i < buttons_layer->toggle_buttons_count; i++) {
    buttons_layer->toggle_buttons[i].selected = false;
  }
  layer_clear_bitmap_definitions(buttons_layer);
  ssd1306_show(&(drivers->ssd1306));
}