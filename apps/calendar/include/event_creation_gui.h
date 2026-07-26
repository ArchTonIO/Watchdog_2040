#ifndef EVENT_CREATION_GUI_H
#define EVENT_CREATION_GUI_H
#include <stdint.h>

#include "apps/calendar/include/event.h"
#include "core/graphics/include/layout.h"

#define ICONS "icons"
#define SELECTIONS "selections"
#define TEXTS "texts"

#define REPEAT_OPTIONS 4
#define ALARM_OPTIONS 3

typedef struct {
  layout *gui_layout;
  event_t *event;
  uint8_t selected_button_index;
} event_creation_gui_t;

void event_creation_gui_init(event_creation_gui_t *gui, event_t *event);
void event_creation_gui_run(event_creation_gui_t *gui);

#endif