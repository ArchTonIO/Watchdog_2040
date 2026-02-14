// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "core/tools/include/launcher.h"

#include <stdbool.h>
#include <stdlib.h>

#include "core/components/include/hw_manager.h"
#include "core/data_structures/include/string_list.h"
#include "core/hardware_drivers/include/haptics.h"
#include "core/hardware_drivers/include/ssd1306.h"
#include "core/tools/include/options_gen.h"

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
void launcher_start(launcher *self) {
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