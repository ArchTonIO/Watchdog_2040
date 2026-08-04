// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "core/tools/include/launcher.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/components/include/home_page.h"
#include "core/components/include/hw_manager.h"
#include "core/data_structures/include/string_list.h"
#include "core/graphics/include/graphic_primitives.h"
#include "core/graphics/include/layout.h"
#include "core/hardware_drivers/include/haptics.h"
#include "core/hardware_drivers/include/joystick.h"
#include "core/hardware_drivers/include/ssd1306.h"
#include "core/tools/include/options_gen.h"
#include "core/utils/include/utils.h"

application create_application(char *name,
    const uint8_t *icon,
    void (*entry_point)(void)) {
  application app;
  app.name = name;
  app.icon = icon;
  app.entry_point = entry_point;
  return app;
}

/**
 * Initialize a launcher instance.
 * @param name The name of the launcher.
 * @return A pointer to the initialized launcher.
 */
launcher *launcher_init(char *name) {
  launcher *self = malloc(sizeof(launcher));
  self->name = name;
  self->apps_count = 0;
  return self;
}

/**
 * Add an application to the launcher.
 * @param self The launcher instance.
 * @param name The name of the application.
 * @param entry_point The function to call when the application is launched.
 */
void launcher_add_application(launcher *self,
    char *name,
    const uint8_t *icon,
    void (*entry_point)(void)) {
  if (self->apps_count >= MAX_APPS)
    return;

  self->applications[self->apps_count].name = name;
  self->applications[self->apps_count].icon = icon;
  self->applications[self->apps_count].entry_point = entry_point;
  self->apps_count++;
}

/**
  start the launcher and display the applications list.
  * @param self The launcher instance.
  */
void launcher_start_tui(launcher *self) {
  haptic_auto_pulse();
  str_list *options = str_list_init();
  for (int i = 0; i < self->apps_count; i++) {
    str_list_append(options, self->applications[i].name);
  }
  options_page *launcher_page = options_page_init(self->name, options);

  for (int i = 0; i < self->apps_count; i++) {
    attach_callback_to_option(launcher_page,
        i,
        self->applications[i].entry_point);
    add_icon_to_option(launcher_page, i, self->applications[i].icon);
  }

  options_page_launch(launcher_page);
  options_page_free(launcher_page);
  ssd1306_clear(&(drivers->ssd1306));
}

void launcher_start_gui(launcher *self) {
  /* creating app name label */

  text_area app_name = {.text = self->applications[0].name,
      .posx = 0,
      .posy = 0,
      .is_inverted = false};

  layout *gui_layout = layout_init();
  layout_add_layer(gui_layout, "top_bar");
  layer *top_bar_layer = get_layer_by_name(gui_layout, "top_bar");
  layer_add_text_area(top_bar_layer, app_name);

  /* creating layers with all app buttons */
  uint8_t posx = 11;
  uint8_t posy = 18;
  uint8_t icon_w = 35;
  uint8_t icon_h = 23;
  uint8_t apps_number = 0;
  uint8_t pages_number = 0;
  layout_add_layer(gui_layout, "apps_0");
  for (size_t i = 0; i < self->apps_count; i++) {
    char layer_name[7];
    snprintf(layer_name, 7, "apps_%d", pages_number);
    layer *apps = get_layer_by_name(gui_layout, layer_name);
    layer_add_button(apps,
        (button_t){(bitmap_definition){
                       .bitmap = self->applications[i].icon,
                       .width = icon_w,
                       .height = icon_h,
                       .posx = posx,
                       .posy = posy,
                       .is_inverted = false,
                   },
            .press_callback = self->applications[i].entry_point,
            .selected = i == 0 ? true : false});

    posx += icon_w;
    apps_number++;
    if (apps_number % GUI_APPS_PER_ROW == 0) {
      posx = 11;
      posy += icon_h;
    }
    if (i == 5) {
      posx = 11;
      posy = 18;
      pages_number++;
      char layer_name[7];
      snprintf(layer_name, 7, "apps_%d", pages_number);
      layout_add_layer(gui_layout, layer_name);
    }
  }

  uint8_t selected_col = 0;
  uint8_t selected_row = 0;
  uint8_t selected_page = 0;
  char layer_name[7];
  layer *current_layer;
  uint8_t btn_matrix[2][3] = {{0, 1, 2}, {3, 4, 5}};

  /* navigate and select */
  ssd1306_clear(&(drivers->ssd1306));
  sleep_ms(200);
  while (true) {
    joystick_update(&(drivers->joystick));
    switch (joystick_get_direction(&(drivers->joystick))) {
    case N:
      haptic_auto_pulse();
      if (selected_row > 0)
        selected_row--;
      break;
    case S:
      haptic_auto_pulse();
      if (selected_row < GUI_APPS_PER_COLUMN - 1 &&
          btn_matrix[selected_row][selected_col] + 3 <
              current_layer->buttons_count)
        selected_row++;
      break;
    case W:
      haptic_auto_pulse();
      if (selected_col > 0)
        selected_col--;
      else if (selected_col == 0 && selected_row == 1) {
        selected_col = 2;
        selected_row = 0;
      } else if (
          selected_col == 0 && selected_row == 0 && selected_page == 0) {
        layout_free(gui_layout);
        return;
      } else if (selected_col == 0 && selected_row == 0 && selected_page > 0) {
        layout_clear_layer(gui_layout, layer_name);
        selected_page--;
      }
      break;
    case E:
      haptic_auto_pulse();
      if (selected_col < GUI_APPS_PER_ROW - 1 &&
          btn_matrix[selected_row][selected_col] + 1 <
              current_layer->buttons_count)
        selected_col++;
      else if (selected_col == GUI_APPS_PER_ROW - 1 &&
               btn_matrix[selected_row][selected_col] + 1 <
                   current_layer->buttons_count) {
        selected_col = 0;
        selected_row++;
      } else if (selected_col == GUI_APPS_PER_ROW - 1 &&
                 selected_row == GUI_APPS_PER_COLUMN - 1 &&
                 selected_page < pages_number) {
        layer_clear_bitmap_definitions(current_layer);
        ssd1306_clear(&(drivers->ssd1306));
        selected_col = 0;
        selected_row = 0;
        selected_page++;
      }
      break;
    }

    snprintf(layer_name, 7, "apps_%i", selected_page);
    current_layer = get_layer_by_name(gui_layout, layer_name);
    layer_clear_all(current_layer);
    layer_clear_all(top_bar_layer);
    char
        *app_name = self->applications[btn_matrix[selected_row][selected_col] +
                                       6 * selected_page]
                        .name;
    char app_name_str[22];
    snprintf(app_name_str, 22, "%s", app_name);
    str_fill_spaces(app_name_str,
        22,
        (21 - strlen(app_name)) / 2,
        (21 - strlen(app_name)) / 2);
    top_bar_layer->text_areas[0].text = app_name_str;
    top_bar_layer->text_areas[0].posx = 0;

    current_layer->buttons[btn_matrix[selected_row][selected_col]]
        .selected = true;
    for (size_t i = 0; i < current_layer->buttons_count; i++)
      if (i != btn_matrix[selected_row][selected_col])
        current_layer->buttons[i].selected = false;

    if (drivers->joystick.button_pressed) {
      current_layer->buttons[btn_matrix[selected_row][selected_col]]
          .press_callback();
      ssd1306_clear(&(drivers->ssd1306));
    }

    layout_draw_layer(gui_layout, layer_name);
    layout_draw_layer(gui_layout, "top_bar");
    ssd1306_show(&(drivers->ssd1306));
  }
}

/**
 * Free the memory allocated for the launcher.
 * @param self The launcher instance to free.
 */
void launcher_free(launcher *self) {
  if (self) {
    free(self->name);
    free(self);
  }
}