#ifndef CALENDAR_EVENT_H
#define CALENDAR_EVENT_H

#include <stdbool.h>

#include "pico/util/datetime.h"

#include "apps/calendar/include/calendar.h"
#include "apps/time/include/time_utils.h"
#include "core/tools/include/crud_list.h"

#define CALENDAR_EVENTS_DIR "calendar_events"
#define CALENDAR_ALARMS_DIR "calendar_alarms"

#define EVENT_ALARM_ON "[AL]"
#define EVENT_ALARM_OFF "[__]"

#define REPEAT_ONCE 0
#define REPEAT_WEEKLY 1
#define REPEAT_MONTHLY 2
#define REPEAT_YEARLY 3

typedef struct {
  bool enabled;
  datetime_t *time;
  const char *event_name;
  uint8_t repeat;
} event_alarm_t;

typedef struct {
  const char *name;
  const char *description;
  datetime_t start_time;
  datetime_t end_time;
  bool end_time_set;
  bool all_day;
} event_t;

void event_alarm_dump(event_alarm_t *alarm);
void event_alarm_load(event_alarm_t *alarm);
void load_all_event_alarms();

void event_dump(crud_list *clist, event_t *event);
void event_load(event_t *event);

void open_events_page_by_day(calendar_t *this_calendar);
void create_event(crud_list *clist);
char *flag_event(char *event_data);
void edit_event(crud_list *clist, const char *event_data);
void delete_event(crud_list *clist, const char *event_data);
// void delete_event(event_t *e);
// void dump_event(event_t *e);
void load_days_with_events(calendar_t *this_calendar);
void process_calendar_events();

#endif
