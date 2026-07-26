#include "apps/calendar/include/event_creation_gui.h"

#include <stdio.h>
#include <string.h>

#include "apps/calendar/include/bitmaps.h"
#include "apps/calendar/include/event.h"
#include "apps/calendar/include/utils.h"
#include "core/components/include/bitmaps.h"
#include "core/components/include/hw_manager.h"
#include "core/graphics/include/layout.h"
#include "core/hardware_drivers/include/config.h"
#include "core/hardware_drivers/include/joystick.h"

static uint8_t repeat = REPEAT_ONCE;
static bool alarm_30_min;
static bool alarm_24_hrs;
static bool alarm_1_week;
static bool should_exit = false;

static inline void set_repeat_day() {
  printf("set repeat: day\n");
  repeat = REPEAT_DAILY;
}
static inline void set_repeat_week() {
  printf("set repeat: week\n");
  repeat = REPEAT_WEEKLY;
}
static inline void set_repeat_month() {
  printf("set repeat: month\n");
  repeat = REPEAT_MONTHLY;
}
static inline void set_repeat_year() {
  printf("set repeat: year\n");
  repeat = REPEAT_YEARLY;
}
static inline void set_alarm_30_min() {
  printf("set alarm: 30 min\n");
  alarm_30_min = true;
}
static inline void set_alarm_24_hrs() {
  printf("set alarm: 24 hrs\n");
  alarm_24_hrs = true;
}
static inline void set_alarm_1_week() {
  printf("set alarm: 1 week\n");
  alarm_1_week = true;
}

static inline void unset_repeat() {
  printf("unset repeat\n");
  repeat = REPEAT_ONCE;
}

static inline void unset_alarm_30_min() {
  printf("unset alarm: 30 min\n");
  alarm_30_min = false;
}
static inline void unset_alarm_24_hrs() {
  printf("unset alarm: 24 hrs\n");
  alarm_24_hrs = false;
}
static inline void unset_alarm_1_week() {
  printf("unset alarm: 1 week\n");
  alarm_1_week = false;
}
static inline void exit_event_creation_gui() {
  printf("exit event creation gui\n");
  should_exit = true;
}

void event_set_datetime_alarm(event_t *event, uint8_t alarm_type) {
  datetime_t alarm_time = event->start_time;

  switch (alarm_type) {
  case ALARM_30_MIN:
    alarm_time.min -= 30;
    normalize_datetime(&alarm_time);
    break;
  case ALARM_24_HRS:
    subtract_days(&alarm_time, 1);
    break;
  case ALARM_1_WEEK:
    subtract_days(&alarm_time, 7);
    break;
  default:
    return;
  }

  event->alarms[alarm_type] = alarm_time;
}

void event_creation_gui_init(event_creation_gui_t *gui, event_t *event) {
  gui->gui_layout = layout_init();
  gui->event = event;
  gui->selected_button_index = 0;

  uint8_t h_spacing = 19;
  uint8_t xoff = 0;
  uint8_t yoff = 16;
  uint8_t icon_w = 24;
  uint8_t icon_h = 16;
  uint8_t selection_w = 40;
  uint8_t selection_h = 7;

  /* icons */
  bitmap_definition clock_bd = {.bitmap = event_clock,
      .width = icon_w,
      .height = icon_h,
      .posx = xoff,
      .posy = yoff,
      .is_inverted = false};
  xoff += icon_w + h_spacing;
  bitmap_definition repeat_bd = {.bitmap = event_repeat,
      .width = icon_w,
      .height = icon_h,
      .posx = xoff,
      .posy = yoff,
      .is_inverted = false};
  xoff += icon_w + h_spacing;
  bitmap_definition alarm_bd = {.bitmap = notif_on_icon,
      .width = icon_w,
      .height = icon_h,
      .posx = xoff,
      .posy = yoff,
      .is_inverted = false};

  /* repeat selections */
  xoff = 42;
  yoff = 33;
  uint8_t v_spacing = 1;
  bitmap_definition day_unselected_bd = {.bitmap = day_unselected,
      .width = selection_w,
      .height = selection_h,
      .posx = xoff,
      .posy = yoff,
      .is_inverted = false};
  bitmap_definition day_selected_bd = {.bitmap = day_selected,
      .width = selection_w,
      .height = selection_h,
      .posx = xoff,
      .posy = yoff,
      .is_inverted = false};
  yoff += selection_h + v_spacing;
  bitmap_definition week_unselected_bd = {.bitmap = week_unselected,
      .width = selection_w,
      .height = selection_h,
      .posx = xoff,
      .posy = yoff,
      .is_inverted = false};
  bitmap_definition week_selected_bd = {.bitmap = week_selected,
      .width = selection_w,
      .height = selection_h,
      .posx = xoff,
      .posy = yoff,
      .is_inverted = false};
  yoff += selection_h + v_spacing;
  bitmap_definition month_unselected_bd = {.bitmap = month_unselected,
      .width = selection_w,
      .height = selection_h,
      .posx = xoff,
      .posy = yoff,
      .is_inverted = false};
  bitmap_definition month_selected_bd = {.bitmap = month_selected,
      .width = selection_w,
      .height = selection_h,
      .posx = xoff,
      .posy = yoff,
      .is_inverted = false};
  yoff += selection_h + v_spacing;
  bitmap_definition year_unselected_bd = {.bitmap = year_unselected,
      .width = selection_w,
      .height = selection_h,
      .posx = xoff,
      .posy = yoff,
      .is_inverted = false};
  bitmap_definition year_selected_bd = {.bitmap = year_selected,
      .width = selection_w,
      .height = selection_h,
      .posx = xoff,
      .posy = yoff,
      .is_inverted = false};

  xoff = 84;
  yoff = 33;

  bitmap_definition alarm_30_min_selected_bd = {
      .bitmap = alarm_30_min_selected,
      .width = selection_w,
      .height = selection_h,
      .posx = xoff,
      .posy = yoff,
      .is_inverted = false};
  bitmap_definition alarm_30_min_unselected_bd = {
      .bitmap = alarm_30_min_unselected,
      .width = selection_w,
      .height = selection_h,
      .posx = xoff,
      .posy = yoff,
      .is_inverted = false};
  yoff += selection_h + v_spacing;
  bitmap_definition alarm_24_hrs_selected_bd = {
      .bitmap = alarm_24_hrs_selected,
      .width = selection_w,
      .height = selection_h,
      .posx = xoff,
      .posy = yoff,
      .is_inverted = false};
  bitmap_definition alarm_24_hrs_unselected_bd = {
      .bitmap = alarm_24_hrs_unselected,
      .width = selection_w,
      .height = selection_h,
      .posx = xoff,
      .posy = yoff,
      .is_inverted = false};
  yoff += selection_h + v_spacing;
  bitmap_definition alarm_1_week_selected_bd = {
      .bitmap = alarm_1_week_selected,
      .width = selection_w,
      .height = selection_h,
      .posx = xoff,
      .posy = yoff,
      .is_inverted = false};
  bitmap_definition alarm_1_week_unselected_bd = {
      .bitmap = alarm_1_week_unselected,
      .width = selection_w,
      .height = selection_h,
      .posx = xoff,
      .posy = yoff,
      .is_inverted = false};

  yoff += selection_h + v_spacing;
  bitmap_definition save_bd = {.bitmap = save,
      .width = selection_w,
      .height = selection_h,
      .posx = xoff,
      .posy = yoff,
      .is_inverted = false};

  toggle_button_t repeat_day_button;
  repeat_day_button.bd_state_1 = day_unselected_bd;
  repeat_day_button.bd_state_2 = day_selected_bd;
  repeat_day_button.press_callback_1 = unset_repeat;
  repeat_day_button.press_callback_2 = set_repeat_day;
  repeat_day_button.selected = false;
  repeat_day_button.state = false;

  toggle_button_t repeat_week_button;
  repeat_week_button.bd_state_1 = week_unselected_bd;
  repeat_week_button.bd_state_2 = week_selected_bd;
  repeat_week_button.press_callback_1 = unset_repeat;
  repeat_week_button.press_callback_2 = set_repeat_week;
  repeat_week_button.selected = false;
  repeat_week_button.state = false;

  toggle_button_t repeat_month_button;
  repeat_month_button.bd_state_1 = month_unselected_bd;
  repeat_month_button.bd_state_2 = month_selected_bd;
  repeat_month_button.press_callback_1 = unset_repeat;
  repeat_month_button.press_callback_2 = set_repeat_month;
  repeat_month_button.selected = false;
  repeat_month_button.state = false;

  toggle_button_t repeat_year_button;
  repeat_year_button.bd_state_1 = year_unselected_bd;
  repeat_year_button.bd_state_2 = year_selected_bd;
  repeat_year_button.press_callback_1 = unset_repeat;
  repeat_year_button.press_callback_2 = set_repeat_year;
  repeat_year_button.selected = false;
  repeat_year_button.state = false;

  toggle_button_t alarm_30_min_button;
  alarm_30_min_button.bd_state_1 = alarm_30_min_unselected_bd;
  alarm_30_min_button.bd_state_2 = alarm_30_min_selected_bd;
  alarm_30_min_button.press_callback_1 = unset_alarm_30_min;
  alarm_30_min_button.press_callback_2 = set_alarm_30_min;
  alarm_30_min_button.selected = false;
  alarm_30_min_button.state = false;

  toggle_button_t alarm_24_hrs_button;
  alarm_24_hrs_button.bd_state_1 = alarm_24_hrs_unselected_bd;
  alarm_24_hrs_button.bd_state_2 = alarm_24_hrs_selected_bd;
  alarm_24_hrs_button.press_callback_1 = unset_alarm_24_hrs;
  alarm_24_hrs_button.press_callback_2 = set_alarm_24_hrs;
  alarm_24_hrs_button.selected = false;
  alarm_24_hrs_button.state = false;

  toggle_button_t alarm_1_week_button;
  alarm_1_week_button.bd_state_1 = alarm_1_week_unselected_bd;
  alarm_1_week_button.bd_state_2 = alarm_1_week_selected_bd;
  alarm_1_week_button.press_callback_1 = unset_alarm_1_week;
  alarm_1_week_button.press_callback_2 = set_alarm_1_week;
  alarm_1_week_button.selected = false;
  alarm_1_week_button.state = false;

  toggle_button_t save_button;
  save_button.bd_state_1 = save_bd;
  save_button.bd_state_2 = save_bd;
  save_button.press_callback_1 = NULL;
  save_button.press_callback_2 = exit_event_creation_gui;
  save_button.selected = false;
  save_button.state = false;

  static char time_text[12];
  if (gui->event->end_time_set)
    snprintf(time_text,
        12,
        "%02d:%02d\n%02d:%02d",
        gui->event->start_time.hour,
        gui->event->start_time.min,
        gui->event->end_time.hour,
        gui->event->end_time.min);
  if (!gui->event->end_time_set) {
    snprintf(time_text,
        12,
        "%02d:%02d",
        gui->event->start_time.hour,
        gui->event->start_time.min);
    printf("TIME TEXT: %s\n", time_text);
  }
  text_area time_area;
  time_area.text = time_text;
  time_area.posx = 0;
  time_area.posy = 5;
  time_area.is_inverted = false;

  text_area title;
  title.text = (char *)event->name;
  title.posx = strlen(event->name) * 6 < SSD1306_WIDTH
                   ? ((SSD1306_WIDTH - strlen(event->name) * 6) / 2) / 6
                   : 0;
  title.posy = 0;
  title.is_inverted = false;

  layout_add_layer(gui->gui_layout, ICONS);
  layout_add_layer(gui->gui_layout, SELECTIONS);
  layout_add_layer(gui->gui_layout, TEXTS);
  layer *icons_layer = get_layer_by_name(gui->gui_layout, ICONS);
  layer *selections_layer = get_layer_by_name(gui->gui_layout, SELECTIONS);
  layer *texts_layer = get_layer_by_name(gui->gui_layout, TEXTS);
  layer_add_bitmap_definition(icons_layer, clock_bd);
  layer_add_bitmap_definition(icons_layer, repeat_bd);
  layer_add_bitmap_definition(icons_layer, alarm_bd);
  layer_add_toggle_button(selections_layer, repeat_day_button);
  layer_add_toggle_button(selections_layer, repeat_week_button);
  layer_add_toggle_button(selections_layer, repeat_month_button);
  layer_add_toggle_button(selections_layer, repeat_year_button);
  layer_add_toggle_button(selections_layer, alarm_30_min_button);
  layer_add_toggle_button(selections_layer, alarm_24_hrs_button);
  layer_add_toggle_button(selections_layer, alarm_1_week_button);
  layer_add_toggle_button(selections_layer, save_button);
  layer_add_text_area(texts_layer, time_area);
  layer_add_text_area(texts_layer, title);
}

void event_creation_gui_run(event_creation_gui_t *gui) {
  ssd1306_clear(&(drivers->ssd1306));
  joystick_update(&(drivers->joystick));
  layer *selections_layer = get_layer_by_name(gui->gui_layout, SELECTIONS);
  while (true) {
    joystick_update(&(drivers->joystick));
    if (joystick_get_direction(&(drivers->joystick)) == S &&
        gui->selected_button_index < 7)
      gui->selected_button_index++;
    if (joystick_get_direction(&(drivers->joystick)) == N &&
        gui->selected_button_index > 0)
      gui->selected_button_index--;
    if (joystick_get_direction(&(drivers->joystick)) == E &&
        gui->selected_button_index < 4)
      gui->selected_button_index = 4;
    if (joystick_get_direction(&(drivers->joystick)) == W &&
        gui->selected_button_index > 3 && gui->selected_button_index < 8)
      gui->selected_button_index = 0;

    layer_clear_bitmap_definitions(selections_layer);

    for (size_t i = 0; i < selections_layer->toggle_buttons_count + 1; i++) {
      if (gui->selected_button_index == i) {
        selections_layer->toggle_buttons[i].selected = true;
        continue;
      }
      selections_layer->toggle_buttons[i].selected = false;
    }

    if (drivers->joystick.button_pressed) {
      sleep_ms(200);
      joystick_update(&(drivers->joystick));
      if (should_exit) {
        gui->event->repeat = repeat;
        if (alarm_30_min) {
          event_set_datetime_alarm(gui->event, 0);
          gui->event->alarms_counter++;
        }
        if (alarm_24_hrs) {
          event_set_datetime_alarm(gui->event, 1);
          gui->event->alarms_counter++;
        }
        if (alarm_1_week) {
          event_set_datetime_alarm(gui->event, 2);
          gui->event->alarms_counter++;
        }
        if (!alarm_30_min && !alarm_24_hrs && !alarm_1_week) {
          event_set_datetime_alarm(gui->event, 3);
          gui->event->alarms_counter = 0;
        }
        layout_free(gui->gui_layout);
        return;
      }
      if (selections_layer->toggle_buttons[gui->selected_button_index].state) {
        selections_layer->toggle_buttons[gui->selected_button_index]
            .press_callback_1();
        selections_layer->toggle_buttons[gui->selected_button_index]
            .state = false;
      } else {
        selections_layer->toggle_buttons[gui->selected_button_index]
            .press_callback_2();
        selections_layer->toggle_buttons[gui->selected_button_index]
            .state = true;
      }
    }
    layout_draw_all_layers(gui->gui_layout);
    ssd1306_show(&drivers->ssd1306);
  }
}