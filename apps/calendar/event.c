#include "apps/calendar/include/event.h"

#include <pico/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apps/calendar/include/bitmaps.h"
#include "apps/calendar/include/calendar.h"
#include "apps/text_editor/include/text_editor.h"
#include "apps/time/include/set_alarm.h"
#include "apps/time/include/time_utils.h"
#include "core/components/include/hw_manager.h"
#include "core/components/include/sys_paths_manager.h"
#include "core/graphics/include/graphic_primitives.h"
#include "core/graphics/include/layout.h"
#include "core/hardware_drivers/include/haptics.h"
#include "core/hardware_drivers/include/joystick.h"
#include "core/hardware_drivers/include/rtc_time.h"
#include "core/hardware_drivers/include/ssd1306.h"
#include "core/tools/include/crud_list.h"
#include "core/tools/include/options_gen.h"
#include "core/utils/include/path.h"
#include "core/utils/include/utils.h"

int16_t selected_year;
int8_t selected_month;
int8_t selected_day;
int8_t selected_dotw;

void open_events_page_by_day(calendar_t *this_calendar) {
  path *calendar_events_path = path_init(CALENDAR_EVENTS_DIR);
  path *calendar_events_full_path = path_concat(sys_paths->dirs->user_path,
      calendar_events_path);
  selected_year = this_calendar->selected_year;
  selected_month = this_calendar->selected_month;
  selected_day = this_calendar->selected_day;
  selected_dotw = this_calendar->selected_dotw;
  char today_str[11];
  snprintf(today_str,
      11,
      "%04d-%02d-%02d",
      selected_year,
      selected_month + 1,
      selected_day + 1);
  path *today_events_path = path_concat(calendar_events_full_path,
      path_init(today_str));
  if (!path_exists(calendar_events_full_path))
    path_mkdir(calendar_events_full_path);
  if (!path_exists(today_events_path))
    path_mkdir(today_events_path);

  crud_list events;
  events.name = "events";
  events.items_category_name = "event";
  events.workdir = today_events_path;
  events.create_as_dir = false;
  events.create_callback = create_event;
  events.read_update_callback = edit_event;
  events.delete_callback = delete_event;
  events.flag_callback = flag_event;
  events.flag_string = EVENT_ALARM_ON;
  events.unflag_string = EVENT_ALARM_OFF;
  launch_crud_list(&events);

  str_list *created_events = path_listdir(today_events_path);
  if (created_events->len == 0) {
    path_fdelete(today_events_path);
  }

  path_free(calendar_events_path);
  path_free(calendar_events_full_path);
  path_free(today_events_path);
}

void create_event(crud_list *clist) {
  /*Asking event name*/
  text_editor *name_editor = text_editor_launch("# Type in the event name",
      true);
  char *name_buf = text_editor_get_buf(name_editor);
  text_editor_kill(name_editor);
  if (strcmp(name_buf, "# Type in the event name") == 0 ||
      strlen(name_buf) == 0) {
    print_usr_error("Event name cannot be\n empty!");
    free(name_buf);
    return;
  }

  /*Asking event description*/
  bool description_set = false;
  text_editor *description_editor = text_editor_launch(
      "# Type in the event description (optional)",
      true);
  char *description_buf = text_editor_get_buf(description_editor);
  text_editor_kill(description_editor);
  if (strcmp(description_buf, "# Type in the event description (optional)") ==
      0)
    free(description_buf);
  else
    description_set = true;

  /*Asking event duration*/
  bool all_day = false;
  bool end_time_set = false;
  time_digits *start_time = time_digits_init();
  time_digits *end_time = time_digits_init();
  str_list *time_options = str_list_init();
  char start_time_buf[6];
  char end_time_buf[6];
  str_list_append(time_options, "all day");
  str_list_append(time_options, "set start time");
  str_list_append(time_options, "set start/end time");
  options_page *time_options_page = options_page_init("Set duration",
      time_options);
  char *selected_option = options_page_launch(time_options_page);
  if (strcmp(selected_option, "all day") == 0) {
    all_day = true;
    end_time_set = true;
    start_time_buf[0] = '0';
    start_time_buf[1] = '0';
    start_time_buf[2] = '_';
    start_time_buf[3] = '0';
    start_time_buf[4] = '0';
    end_time_buf[0] = '2';
    end_time_buf[1] = '3';
    end_time_buf[2] = '_';
    end_time_buf[3] = '5';
    end_time_buf[4] = '9';
  } else if (strcmp(selected_option, "set start time") == 0) {
    set_alarm_time(start_time);
    time_digits_to_str(start_time, start_time_buf, 6);
    print_info("Start time set!");
  } else if (strcmp(selected_option, "set start/end time") == 0) {
    set_alarm_time(start_time);
    time_digits_to_str(start_time, start_time_buf, 6);
    print_info("Start time set!");
    printf("Start time: %s\n", start_time_buf);
    set_alarm_time(end_time);
    time_digits_to_str(end_time, end_time_buf, 6);
    print_info("End time set!");
    printf("End time: %s\n", end_time_buf);
    end_time_set = true;
  } else {
    print_usr_error("Quitting event\ncreation!");
    free(name_buf);
    if (description_set)
      free(description_buf);
    free(start_time);
    free(end_time);
    options_page_free(time_options_page);
    return;
  }
  start_time_buf[5] = '\0';
  end_time_buf[5] = '\0';

  /*Saving event*/
  datetime_t datetime_start_time;
  datetime_start_time.year = selected_year;
  datetime_start_time.month = selected_month;
  datetime_start_time.day = selected_day;
  datetime_start_time.dotw = selected_dotw;
  datetime_start_time.hour = ((int8_t)start_time_buf[0] - '0') * 10 +
                             ((int8_t)start_time_buf[1] - '0');
  datetime_start_time.min = ((int8_t)start_time_buf[3] - '0') * 10 +
                            ((int8_t)start_time_buf[4] - '0');
  datetime_start_time.sec = 0;

  datetime_t datetime_end_time;
  if (end_time_set) {
    datetime_end_time.year = selected_year;
    datetime_end_time.month = selected_month;
    datetime_end_time.day = selected_day;
    datetime_end_time.dotw = selected_dotw;
    datetime_end_time.hour = ((int8_t)end_time_buf[0] - '0') * 10 +
                             ((int8_t)end_time_buf[1] - '0');
    datetime_end_time.min = ((int8_t)end_time_buf[3] - '0') * 10 +
                            ((int8_t)end_time_buf[4] - '0');
    datetime_end_time.sec = 0;
  }
  event_t new_event;
  new_event.name = name_buf;
  new_event.description = description_set ? description_buf : "";
  new_event.start_time = datetime_start_time;
  new_event.end_time = datetime_end_time;
  new_event.end_time_set = end_time_set;
  new_event.all_day = all_day;
  event_dump(clist, &new_event);

  /*freeing memory*/
  free(name_buf);
  if (description_set)
    free(description_buf);
  free(start_time);
  free(end_time);
  options_page_free(time_options_page);
}

str_list *time_to_str_list(datetime_t time) {
  str_list *time_str_list = str_list_init();
  char year_buf[5];
  char month_buf[3];
  char day_buf[3];
  char dotw_buf[3];
  char hour_buf[3];
  char min_buf[3];
  char sec_buf[3];
  snprintf(year_buf, sizeof(year_buf), "%04d", time.year);
  snprintf(month_buf, sizeof(month_buf), "%02d", time.month);
  snprintf(day_buf, sizeof(day_buf), "%02d", time.day);
  snprintf(dotw_buf, sizeof(dotw_buf), "%02d", time.dotw);
  snprintf(hour_buf, sizeof(hour_buf), "%02d", time.hour);
  snprintf(min_buf, sizeof(min_buf), "%02d", time.min);
  snprintf(sec_buf, sizeof(sec_buf), "%02d", time.sec);
  str_list_append(time_str_list, year_buf);
  str_list_append(time_str_list, month_buf);
  str_list_append(time_str_list, day_buf);
  str_list_append(time_str_list, dotw_buf);
  str_list_append(time_str_list, hour_buf);
  str_list_append(time_str_list, min_buf);
  str_list_append(time_str_list, sec_buf);
  return time_str_list;
}

void event_dump(crud_list *clist, event_t *event) {
  size_t filename_len = strlen(EVENT_ALARM_ON) + 1 + strlen("00_00/00_00") +
                        1 + strlen(event->name) + 1 + 1;
  size_t filecontent_len = strlen(event->description) + 1;
  char filename_buf[filename_len];
  char file_buf[filecontent_len];

  str_list *start_time_parts = time_to_str_list(event->start_time);
  str_list *end_time_parts = time_to_str_list(event->end_time);
  char start_time_buf[6];
  snprintf(start_time_buf,
      6,
      "%s_%s",
      str_list_get(start_time_parts, -3),
      str_list_get(start_time_parts, -2));
  char end_time_buf[6];
  snprintf(end_time_buf,
      6,
      "%s_%s",
      str_list_get(end_time_parts, -3),
      str_list_get(end_time_parts, -2));

  if (!event->end_time_set || event->all_day)
    snprintf(filename_buf,
        filename_len,
        "%s %s %s",
        EVENT_ALARM_OFF,
        event->all_day ? "all_day" : start_time_buf,
        event->name);
  else
    snprintf(filename_buf,
        filename_len,
        "%s %s-%s %s",
        EVENT_ALARM_OFF,
        start_time_buf,
        end_time_buf,
        event->name);

  path *file_path_part = path_init(filename_buf);
  path *file_path = path_concat(clist->workdir, file_path_part);
  path_key_value_dump(file_path, 'w', "description", event->description);
  path_key_value_dump(file_path,
      'a',
      "start_time",
      str_list_concat(start_time_parts, '|'));
  if (event->end_time_set)
    path_key_value_dump(file_path,
        'a',
        "end_time",
        str_list_concat(end_time_parts, '|'));

  path_free(file_path);
  str_list_free(start_time_parts);
  str_list_free(end_time_parts);
}

void edit_event(crud_list *clist, const char *event_data) {
  path *event_path = get_item_path(clist, event_data);
  str_list *fcontent = path_fread(event_path);
  char *event_descr = str_list_concat(fcontent, NO_SEP);
  str_list *slices = string_split(event_data, ' ');

  char *alarm = str_list_get(slices, 0);
  char *time = str_list_get(slices, 1);
  char *name = str_list_get(slices, 2);

  char event_details_buf[50];
  snprintf(event_details_buf,
      50,
      "Name: %s\nTime: %s\nDescr: %s",
      name,
      time,
      event_descr);

  text_editor *event_editor = text_editor_launch(event_details_buf, false);
  char *buf = text_editor_get_buf(event_editor);
  free(buf);
  text_editor_kill(event_editor);
  path_free(event_path);
  str_list_free(fcontent);
  str_list_free(slices);
  free(event_descr);
}

void save_event_alarm(crud_list *clist,
    const char *event_data,
    time_digits *alarm_time) {
  path *calendar_alarms_path = path_init(CALENDAR_ALARMS_DIR);
  path *calendar_alarms_full_path = path_concat(sys_paths->dirs->user_path,
      calendar_alarms_path);

  path *event_path = get_item_path(clist, event_data);
  str_list *fcontent = path_fread(event_path);
  char *event_descr = str_list_concat(fcontent, NO_SEP);
  str_list *slices = string_split(event_data, ' ');

  char *alarm = str_list_get(slices, 0);
  char *time = str_list_get(slices, 1);
  char *name = str_list_get(slices, 2);
}

void create_event_alarm(crud_list *clist, const char *event_data) {
  str_list *alarm_options = str_list_init();
  str_list_append(alarm_options, "30 min before");
  str_list_append(alarm_options, "1 day before");
  str_list_append(alarm_options, "1 week before");
  str_list_append(alarm_options, "1 month before");
  options_page *alarm_op_page = options_page_init("Select alarm option",
      alarm_options);
  char *time_before = options_page_launch(alarm_op_page);

  str_list *alarm_repeat = str_list_init();
  str_list_append(alarm_repeat, "No repeat");
  str_list_append(alarm_repeat, "Repeat weekly");
  str_list_append(alarm_repeat, "Repeat monthly");
  str_list_append(alarm_repeat, "Repeat yearly");
  options_page *alarm_repeat_page = options_page_init("Select a repeat type",
      alarm_repeat);
  char *time_repeat = options_page_launch(alarm_repeat_page);

  path *event_path = get_item_path(clist, event_data);
  str_list *fcontent = path_fread(event_path);
  char *event_descr = str_list_concat(fcontent, NO_SEP);
  str_list *slices = string_split(event_data, ' ');

  char *alarm = str_list_get(slices, 0);
  char *str_time = str_list_get(slices, 1);
  char *name = str_list_get(slices, 2);

  time_digits *event_time;
  event_time->hour_tens = str_time[0];
  event_time->hour_units = str_time[1];
  event_time->minute_tens = str_time[3];
  event_time->minute_units = str_time[4];
  int8_t event_hour = event_time->hour_tens * 10 + event_time->hour_units;
  int8_t event_minute = event_time->minute_tens * 10 +
                        event_time->minute_units;

  // if (strcmp(time_before, "30 min before") == 0) {

  // }
}

char *flag_event(char *event_data) {
  char *new_str;
  if (strstr(event_data, EVENT_ALARM_OFF) != NULL) {
    new_str = string_substring_replace(event_data,
        EVENT_ALARM_OFF,
        EVENT_ALARM_ON);
    // create_event_alarm();
  } else {
    new_str = string_substring_replace(event_data,
        EVENT_ALARM_ON,
        EVENT_ALARM_OFF);
  }
  return new_str;
}

void delete_event(crud_list *clist, const char *event_data) {
  path *event_path = get_item_path(clist, event_data);
  delete_item_basic(clist, event_data);

  path *day_folder = event_path->parent;
  str_list *events_on_this_day = path_listdir(day_folder);
  if (events_on_this_day->len == 0)
    path_rmtree(day_folder);
  free(event_path);
  str_list_free(events_on_this_day);
}

void load_days_with_events(calendar_t *this_calendar) {
  path *calendar_events_path = path_init(CALENDAR_EVENTS_DIR);
  path *calendar_events_full_path = path_concat(sys_paths->dirs->user_path,
      calendar_events_path);
  for (size_t i = 0; i < 31; i++) {
    this_calendar->days_with_events[i] = 0;
  }

  this_calendar->days_with_events_counter = 0;
  str_list *events_by_day = path_listdir(calendar_events_full_path);
  for (size_t i = 0; i < events_by_day->len; i++) {
    str_list *file_name_parts = string_split(str_list_get(events_by_day, i),
        '-');
    char *file_month = str_list_get(file_name_parts, 1);
    if (atoi(file_month) - 1 == this_calendar->selected_month) {
      this_calendar
          ->days_with_events[this_calendar->days_with_events_counter] = atoi(
          str_list_get(file_name_parts, 2));
      this_calendar->days_with_events_counter++;
    }
    str_list_free(file_name_parts);
  }

  path_free(calendar_events_full_path);
  path_free(calendar_events_path);
  str_list_free(events_by_day);
}