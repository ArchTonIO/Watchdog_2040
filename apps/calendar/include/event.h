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
#define REPEAT_DAILY 1
#define REPEAT_WEEKLY 2
#define REPEAT_MONTHLY 3
#define REPEAT_YEARLY 4

#define MAX_REPEATS 30

#define ALARM_30_MIN 0
#define ALARM_24_HRS 1
#define ALARM_1_WEEK 2
#define ALARM_UNSET 3

typedef struct {
  const char *name;
  const char *description;
  datetime_t start_time;
  datetime_t end_time;
  bool end_time_set;
  bool all_day;
  uint8_t repeat;
  uint8_t alarms_counter;
  datetime_t alarms[3];
} event_t;

void load_all_event_alarms();

void event_dump(crud_list *clist, event_t *event, path *base_dir);
void event_dump_repeats(crud_list *clist, event_t *event);
void event_load(event_t *event);
void event_attach_alarm(event_t *event);

void open_events_page_by_day(calendar_t *this_calendar);
void create_event(crud_list *clist);
char *flag_event(char *event_data);
void edit_event(crud_list *clist, const char *event_data);
void delete_event(crud_list *clist, const char *event_data);
// void delete_event(event_t *e);
// void dump_event(event_t *e);
void load_days_with_events(calendar_t *this_calendar);
str_list *get_today_events(void);
void process_calendar_events();

#endif
