#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hw_manager.h"
#include "data_structures/string_list.h"
#include "hardware_drivers/ssd1306.h"
#include "hardware_drivers/joystick.h"
#include "options_gen.h"

options_page *options_page_init(str_list *options)
{
  options_page *page = (options_page *)malloc(sizeof(options_page));
  page->num_options = options->len;
  page->selected_option = 0;
  for (uint8_t i = 0; i < options->len; i++)
  {
    page->options[i].name = lstget(options, i);
    page->options[i].selected = false;
    page->options[i].callback = NULL;
  }
  return page;
}

void attach_callback_to_option(
    options_page *page,
    uint8_t option_index,
    void (*callback)(void))
{
  if (option_index < page->num_options)
  {
    page->options[option_index].callback = callback;
  }
}

char *options_page_launch(options_page *page)
{
  while (1)
  {
    ssd1306_clear(drivers->oled_screen);
    for (uint8_t i = 0; i < page->num_options; i++)
    {
      if (i == page->selected_option)
      {
        ssd1306_print(
            drivers->oled_screen,
            page->options[i].name,
            (uint8_t)(SSD1306_WIDTH - (strlen(page->options[i].name) * CHAR_WIDTH) / 2),
            i * CHAR_HEIGHT,
            true);
      }
      else
      {
        ssd1306_print(
            drivers->oled_screen,
            page->options[i].name,
            (uint8_t)(SSD1306_WIDTH - (strlen(page->options[i].name) * CHAR_WIDTH) / 2),
            i * CHAR_HEIGHT,
            false);
      }
    }
    ssd1306_show(drivers->oled_screen);
    joystick_update(drivers->joystick);
    uint8_t joystick_dir = joystick_get_direction(drivers->joystick);
    if (joystick_dir == N)
    {
      page->selected_option--;
      if (page->selected_option < 0)
        page->selected_option = page->num_options - 1;
    }
    else if (joystick_dir == S)
    {
      page->selected_option++;
      if (page->selected_option >= page->num_options)
        page->selected_option = 0;
    }
    else if (drivers->joystick->button_pressed)
    {
      if (page->options[page->selected_option].callback != NULL)
        page->options[page->selected_option].callback();
      else
        return page->options[page->selected_option].name;
      ssd1306_clear(drivers->oled_screen);
      ssd1306_show(drivers->oled_screen);
    }
  }
}
