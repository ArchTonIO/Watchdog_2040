#include "tools/options_gen.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "components/hw_manager.h"
#include "data_structures/string_list.h"
#include "hardware_drivers/haptics.h"
#include "hardware_drivers/joystick.h"
#include "hardware_drivers/ssd1306.h"

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
    char *original = get(options, i);
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
    page->options[i].selected = false;
    page->options[i].callback = NULL;
    page->scroll = 0;
  }
  return page;
}

/**
 * @brief Attaches a callback function to an option in the options page.
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
 * @brief Launches an options page with a list of options and allows the user
 * to select one. The selected option is returned as a string, or, if a
 * callback was previously attached to the option, the callback is executed
 * instead.
 * @param page The options page to launch.
 * @returns The name of the selected option as a string.
 */
char *options_page_launch(options_page *page) {
  sleep_ms(INTERAC_TIMEOUT);
  ssd1306_clear(drivers->oled_screen);
  while (1) {
    ssd1306_print(drivers->oled_screen,
        page->title,
        (uint8_t)((MAX_X_CHARS - strlen(page->title)) / 2),
        0,
        false);
    for (uint8_t i = 0; i < page->num_options; i++) {
      // if (i < abs(page->scroll))
      //   continue;
      if (i < page->scroll || i >= page->scroll + MAX_OPTIONS_ON_SCREEN)
        continue;
      uint8_t screen_row = i - page->scroll +
                           2; // +2 per lasciare spazio al titolo
      if (i == page->selected_option) {
        ssd1306_print(drivers->oled_screen,
            page->options[i].display_name,
            0,
            screen_row,
            true);
      } else {
        ssd1306_print(drivers->oled_screen,
            page->options[i].display_name,
            0,
            screen_row,
            false);
      }
    }
    ssd1306_show(drivers->oled_screen);
    joystick_update(drivers->joystick);
    uint8_t joystick_dir = joystick_get_direction(drivers->joystick);
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
      if (page->options[page->selected_option].callback != NULL)
        page->options[page->selected_option].callback();
      else
        return page->options[page->selected_option].name;
      ssd1306_clear(drivers->oled_screen);
      ssd1306_show(drivers->oled_screen);
    } else if (joystick_dir == W) {
      haptic_short_pulse();
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
  list_free(page->options_list);
  free(page);
}