#ifndef CALENDAR_H
#define CALENDAR_H

// #include "apps/calendar/include/event.h"
#include <stdlib.h>

#include "core/graphics/include/graphic_primitives.h"
#include "core/graphics/include/layout.h"

#define JAN_LEN 31
#define FEB_LEN 28
#define MAR_LEN 31
#define APR_LEN 30
#define MAY_LEN 31
#define JUN_LEN 30
#define JUL_LEN 31
#define AUG_LEN 31
#define SEP_LEN 30
#define OCT_LEN 31
#define NOV_LEN 30
#define DIC_LEN 31

#define DAYS_LAYER "days"
#define CURRENT_DAY "selected_day"
#define EVENT_DAYS "event_days"
#define BOTTOM_BAR "bottom_bar"
#define DOTW "dotw"

typedef struct {
  bitmap_definition *days_bitmap_defs[31];
  bitmap_definition bottom_bar_bitmap_defs[5];
  bitmap_definition dotw_bitmap_defs[7];
  int8_t actual_day;
  int8_t actual_month;
  int16_t actual_year;
  int8_t selected_day;
  int8_t selected_dotw;
  int8_t selected_month;
  int16_t selected_year;
  layout *month_page_layout;
  uint8_t days_with_events[31];
  size_t days_with_events_counter;
} calendar_t;

void calendar_launch();

#endif
