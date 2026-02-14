// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "core/tools/include/options_gen.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/components/include/hw_manager.h"
#include "core/data_structures/include/string_list.h"
#include "core/hardware_drivers/include/haptics.h"
#include "core/hardware_drivers/include/joystick.h"
#include "core/hardware_drivers/include/ssd1306.h"

void handle_scroll(options_page *page);

/**
 * @brief Initializes an options page with a list of options.
 *
 * @param title The title of the options page.
 * @param options The list of options to initialize the page with.
 * @returns A pointer to the initialized options page.
 */
options_page *options_page_init(char *title, str_list *options) {
  options_page *page = (options_page *)malloc(sizeof(options_page));
  page->options_list = options;
  page->num_options = options->len;
  page->selected_option = 0;
  page->title = title;
  for (uint8_t i = 0; i < options->len; i++) {
    char *original = str_list_get(options, i);
    size_t len = strlen(original);
    char *formatted = (char *)malloc(MAX_X_CHARS + 1);
    size_t copy_len = len > (MAX_X_CHARS - 2) ? (MAX_X_CHARS - 2) : len;
    strncpy(formatted, original, copy_len);
    for (size_t j = copy_len; j < (MAX_X_CHARS - 2); j++)
      formatted[j] = ' ';
    formatted[MAX_X_CHARS - 2] = '-';
    formatted[MAX_X_CHARS - 1] = '>';
    formatted[MAX_X_CHARS] = '\0';
    page->options[i].display_name = formatted;
    page->options[i].name = original;
    page->options[i].icon = NULL;
    page->options[i].flag_callback = NULL;
    page->options[i].callback = NULL;
    page->scroll = 0;
  }
  return page;
}

void add_icon_to_option(options_page *page,
    uint8_t option_index,
    const uint8_t *icon) {
  if (option_index < page->num_options) {
    page->options[option_index].icon = icon;
    page->options[option_index].display_name[MAX_X_CHARS - 4] = '>';
    page->options[option_index].display_name[MAX_X_CHARS - 5] = '-';
  }
}

/**
 * @brief Attaches a callback function to an option in the options page.
 * The callback is called when the option is selected by moving the joystick
 * east
 *
 * @param page The options page to attach the callback to.
 * @param option_index The index of the option to attach the callback to.
 * @param callback The callback function to attach.
 */
void attach_callback_to_option(options_page *page,
    uint8_t option_index,
    void (*callback)(void)) {
  if (option_index < page->num_options) {
    page->options[option_index].callback = callback;
  }
}

/**
 * @brief Attaches a flag callback function to an option in the options page.
 * The flag callback is called when the option is selected when the joystick
 * button gets clicked on it.
 *
 * @param page The options page to attach the callback to.
 * @param option_index The index of the option to attach the callback to.
 * @param callback The callback function to attach.
 */
void attach_flag_callback_to_option(options_page *page,
    uint8_t option_index,
    char *(flag_callback)(char *input)) {
  if (option_index < page->num_options) {
    page->options[option_index].flag_callback = flag_callback;
  }
}

/**
 * @brief Launches an options page with a list of options and allows the
 * user to select one. The selected option is returned as a string, or, if
 * a callback was previously attached to the option, the callback is
 * executed instead.
 * @param page The options page to launch.
 * @returns The name of the selected option as a string.
 */
char *options_page_launch(options_page *page) {
  sleep_ms(INTERAC_TIMEOUT);
  ssd1306_enable_mutex_support(&(drivers->ssd1306));
  ssd1306_clear(&(drivers->ssd1306));
  uint8_t left_padding;
  while (1) {
    ssd1306_get_mutex(&(drivers->ssd1306));
    ssd1306_print(&(drivers->ssd1306),
        page->title,
        (uint8_t)((MAX_X_CHARS - strlen(page->title)) / 2),
        0,
        false);

    if (page->options[page->selected_option].icon != NULL) {
      ssd1306_draw_bitmap(&(drivers->ssd1306),
          0,
          0,
          page->options[page->selected_option].icon,
          8,
          8,
          false);
    }

    for (uint8_t i = 0; i < page->num_options; i++) {
      if (i < page->scroll || i >= page->scroll + MAX_OPTIONS_ON_SCREEN)
        continue;
      uint8_t screen_row = i - page->scroll + 2;

      if (i == page->selected_option) {
        left_padding = page->options[i].icon != NULL ? 2 : 0;
        ssd1306_print(&(drivers->ssd1306),
            page->options[i].display_name,
            left_padding,
            screen_row,
            true);
        if (page->options[i].icon != NULL) {
          ssd1306_draw_bitmap(&(drivers->ssd1306),
              0,
              screen_row * 8,
              page->options[i].icon,
              8,
              8,
              true);
        }
      } else {
        ssd1306_print(&(drivers->ssd1306),
            page->options[i].display_name,
            left_padding,
            screen_row,
            false);
        if (page->options[i].icon != NULL) {
          ssd1306_draw_bitmap(&(drivers->ssd1306),
              0,
              screen_row * 8,
              page->options[i].icon,
              8,
              8,
              false);
        }
      }
    }
    ssd1306_show(&(drivers->ssd1306));
    ssd1306_release_mutex(&(drivers->ssd1306));
    joystick_update(&(drivers->joystick));
    uint8_t joystick_dir = joystick_get_direction(&(drivers->joystick));
    if ((drivers->joystick).button_pressed) {
      if (page->options[page->selected_option].flag_callback != NULL) {
        sleep_ms(INTERAC_TIMEOUT);
        char *buf = page->options[page->selected_option].flag_callback(
            page->options[page->selected_option].display_name);
        free(page->options[page->selected_option].display_name);
        page->options[page->selected_option].display_name = buf;
      }
    }
    if (joystick_dir == N) {
      haptic_auto_pulse();
      if (page->selected_option > 0) {
        page->selected_option--;
      }
    } else if (joystick_dir == S) {
      haptic_auto_pulse();
      page->selected_option++;
      if (page->selected_option >= page->num_options)
        page->selected_option = page->num_options - 1;
    } else if (joystick_dir == E) {
      haptic_short_pulse();
      if (page->options[page->selected_option].callback != NULL) {
        ssd1306_disable_mutex_support(&(drivers->ssd1306));
        page->options[page->selected_option].callback();
      } else {
        ssd1306_disable_mutex_support(&(drivers->ssd1306));
        return page->options[page->selected_option].name;
      }
      ssd1306_get_mutex(&(drivers->ssd1306));
      ssd1306_clear(&(drivers->ssd1306));
      ssd1306_show(&(drivers->ssd1306));
      ssd1306_release_mutex(&(drivers->ssd1306));
    } else if (joystick_dir == W) {
      haptic_short_pulse();
      ssd1306_disable_mutex_support(&(drivers->ssd1306));
      return "";
    }
    handle_scroll(page);
  }
}

void handle_scroll(options_page *page) {
  if (page->selected_option < page->scroll) {
    page->scroll = page->selected_option;
  } else if (page->selected_option >= page->scroll + MAX_OPTIONS_ON_SCREEN) {
    page->scroll = page->selected_option - MAX_OPTIONS_ON_SCREEN + 1;
  }
}

void options_page_free(options_page *page) {
  for (uint8_t i = 0; i < page->num_options; i++) {
    free(page->options[i].display_name);
  }
  str_list_free(page->options_list);
  free(page);
}