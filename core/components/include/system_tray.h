#include <stdbool.h>

#include "core/graphics/include/layout.h"

#ifndef SYSTEM_TRAY_H
#define SYSTEM_TRAY_H

/*GUI layers*/
#define BUTTONS "buttons"

typedef struct {
  layout *tray_layout;
  uint8_t selected_button_index;
} system_tray_t;

void system_tray_init(system_tray_t *tray);
void system_tray_expand(system_tray_t *tray);
void system_tray_collapse(system_tray_t *tray);
void system_tray_select_left(system_tray_t *tray);
void system_tray_select_right(system_tray_t *tray);
void system_tray_refresh(system_tray_t *tray);
void system_tray_press_button(system_tray_t *tray);

#endif