#include "apps/calendar/include/event.h"

#include <pico/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apps/calendar/include/calendar.h"
#include "apps/calendar/include/event_creation_gui.h"
#include "apps/calendar/include/utils.h"
#include "apps/text_editor/include/text_editor.h"
#include "apps/time/include/set_alarm.h"
#include "apps/time/include/time_utils.h"
#include "core/components/include/hw_manager.h"
#include "core/components/include/sys_paths_manager.h"
#include "core/hardware_drivers/include/rtc_time.h"
#include "core/tools/include/crud_list.h"
#include "core/tools/include/options_gen.h"
#include "core/utils/include/path.h"
#include "core/utils/include/utils.h"

static int16_t selected_year;
static int8_t selected_month;
static int8_t selected_day;
static int8_t selected_dotw;

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
  path *today_events_dir_name = path_init(today_str);
  path *today_events_path = path_concat(calendar_events_full_path,
      today_events_dir_name);
  path_free(today_events_dir_name);
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
  events.flag_callback = NULL;
  launch_crud_list(&events);

  str_list *created_events = path_listdir(today_events_path);
  if (created_events->len == 0) {
    path_fdelete(today_events_path);
  }
  str_list_free(created_events);
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
    set_alarm_time(end_time);
    time_digits_to_str(end_time, end_time_buf, 6);
    print_info("End time set!");
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
  datetime_start_time.month = selected_month + 1;
  datetime_start_time.day = selected_day + 1;
  datetime_start_time.dotw = selected_dotw;
  datetime_start_time.hour = ((int8_t)start_time_buf[0] - '0') * 10 +
                             ((int8_t)start_time_buf[1] - '0');
  datetime_start_time.min = ((int8_t)start_time_buf[3] - '0') * 10 +
                            ((int8_t)start_time_buf[4] - '0');
  datetime_start_time.sec = 0;

  datetime_t datetime_end_time = datetime_start_time;
  if (end_time_set) {
    datetime_end_time.year = selected_year;
    datetime_end_time.month = selected_month + 1;
    datetime_end_time.day = selected_day + 1;
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
  new_event.repeat = REPEAT_ONCE;
  new_event.alarms_counter = 0;

  event_creation_gui_t gui;
  event_creation_gui_init(&gui, &new_event);
  event_creation_gui_run(&gui);
  event_dump(clist, &new_event, clist->workdir);
  event_dump_repeats(clist, &new_event);

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

void event_dump(crud_list *clist, event_t *event, path *base_dir) {
  size_t filename_len = strlen("00_00/00_00") + 1 + strlen(event->name) + 1 +
                        1;
  size_t filecontent_len = strlen(event->description) + 1;
  char filename_buf[filename_len];
  char file_buf[filecontent_len];

  str_list *start_time_parts = time_to_str_list(event->start_time);
  str_list *end_time_parts = NULL;
  char start_time_buf[6];
  snprintf(start_time_buf,
      6,
      "%s_%s",
      str_list_get(start_time_parts, -3),
      str_list_get(start_time_parts, -2));
  char end_time_buf[6];
  if (event->end_time_set) {
    end_time_parts = time_to_str_list(event->end_time);
    snprintf(end_time_buf,
        6,
        "%s_%s",
        str_list_get(end_time_parts, -3),
        str_list_get(end_time_parts, -2));
  }

  if (!event->end_time_set || event->all_day)
    snprintf(filename_buf,
        filename_len,
        "%s %s",
        event->all_day ? "all_day" : start_time_buf,
        event->name);
  else
    snprintf(filename_buf,
        filename_len,
        "%s-%s %s",
        start_time_buf,
        end_time_buf,
        event->name);

  path *file_path_part = path_init(filename_buf);
  path *file_path = path_concat(base_dir, file_path_part);
  path_free(file_path_part);
  path_key_value_dump(file_path, 'w', "description", event->description);
  char *start_time_str = str_list_concat(start_time_parts, '|');
  path_key_value_dump(file_path, 'a', "start_time", start_time_str);
  free(start_time_str);
  if (event->end_time_set) {
    char *end_time_str = str_list_concat(end_time_parts, '|');
    path_key_value_dump(file_path, 'a', "end_time", end_time_str);
    free(end_time_str);
  }
  if (event->repeat) {
    char repeat_buf[2];
    snprintf(repeat_buf, 2, "%d", event->repeat);
    path_key_value_dump(file_path, 'a', "repeat", repeat_buf);
  }
  for (size_t i = 0; i < event->alarms_counter; i++) {
    str_list *alarm_time_parts = time_to_str_list(event->alarms[i]);
    str_list_print(alarm_time_parts);
    char alarm_time_buf[6];
    snprintf(alarm_time_buf,
        6,
        "%s_%s",
        str_list_get(alarm_time_parts, -3),
        str_list_get(alarm_time_parts, -2));
    char alarm_name[8];
    snprintf(alarm_name, 8, "alarm_%d", i);
    path_key_value_dump(file_path, 'a', alarm_name, alarm_time_buf);
    str_list_free(alarm_time_parts);
  }

  path_free(file_path);
  str_list_free(start_time_parts);
  if (end_time_parts)
    str_list_free(end_time_parts);
}

path *datetime_to_path(crud_list *clist, datetime_t dt, event_t *event) {
  char iso_datetime[20];
  snprintf(iso_datetime,
      20,
      "%04d-%02d-%02d",
      event->start_time.year,
      event->start_time.month,
      event->start_time.day);
  path *base_dir = path_init(iso_datetime);
  path *full_base_dir = path_concat(clist->workdir->parent, base_dir);
  if (!path_exists(full_base_dir))
    path_mkdir(full_base_dir);
  path_free(base_dir);
  return full_base_dir;
}

void event_dump_repeats(crud_list *clist, event_t *event) {
  if (!event->repeat)
    return;
  event_t base_event = *event;
  int base_year = 0;
  int base_month = 0;
  int base_day = 0;
  if (sscanf(clist->workdir->full_name,
          "%d-%d-%d",
          &base_year,
          &base_month,
          &base_day) == 3) {
    base_event.start_time.year = (int16_t)base_year;
    base_event.start_time.month = (int8_t)base_month;
    base_event.start_time.day = (int8_t)base_day;
    if (base_event.end_time_set) {
      base_event.end_time.year = (int16_t)base_year;
      base_event.end_time.month = (int8_t)base_month;
      base_event.end_time.day = (int8_t)base_day;
    }
  }
  if (event->repeat == REPEAT_DAILY)
    for (size_t i = 0; i < MAX_REPEATS; i++) {
      event_t new_event = base_event;
      add_days(&new_event.start_time, (i + 1));
      add_days(&new_event.end_time, (i + 1));
      path *full_base_dir = datetime_to_path(clist,
          new_event.start_time,
          &new_event);
      event_dump(clist, &new_event, full_base_dir);
      path_free(full_base_dir);
      event_attach_alarm(&new_event);
    }
  if (event->repeat == REPEAT_WEEKLY)
    for (size_t i = 0; i < MAX_REPEATS; i++) {
      event_t new_event = base_event;
      add_weeks(&new_event.start_time, (i + 1));
      add_weeks(&new_event.end_time, (i + 1));
      path *full_base_dir = datetime_to_path(clist,
          new_event.start_time,
          &new_event);
      event_dump(clist, &new_event, full_base_dir);
      path_free(full_base_dir);
      event_attach_alarm(&new_event);
    }
  if (event->repeat == REPEAT_MONTHLY)
    for (size_t i = 0; i < MAX_REPEATS; i++) {
      event_t new_event = base_event;
      add_months(&new_event.start_time, (i + 1));
      add_months(&new_event.end_time, (i + 1));
      path *full_base_dir = datetime_to_path(clist,
          new_event.start_time,
          &new_event);
      event_dump(clist, &new_event, full_base_dir);
      path_free(full_base_dir);
      event_attach_alarm(&new_event);
    }
  if (event->repeat == REPEAT_YEARLY)
    for (size_t i = 0; i < MAX_REPEATS; i++) {
      event_t new_event = base_event;
      add_years(&new_event.start_time, (i + 1));
      add_years(&new_event.end_time, (i + 1));
      path *full_base_dir = datetime_to_path(clist,
          new_event.start_time,
          &new_event);
      event_dump(clist, &new_event, full_base_dir);
      path_free(full_base_dir);
      event_attach_alarm(&new_event);
    }
}

void event_attach_alarm(event_t *event) {}

void edit_event(crud_list *clist, const char *event_data) {
  print_info("Event edit not\nimplemented yet!");
  return;
} // todo: implement

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

void delete_event(crud_list *clist, const char *event_data) {
  path *event_path = get_item_path(clist, event_data);
  delete_item_basic(clist, event_data);

  path *day_folder = event_path->parent;
  str_list *events_on_this_day = path_listdir(day_folder);
  if (events_on_this_day->len == 0)
    path_rmtree(day_folder);
  path_free(event_path);
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

str_list *get_today_events(void) {
  str_list *today_events = str_list_init();
  char *time_now = rtc_time_now(&(drivers->internal_rtc));
  int16_t current_year = 0;
  int8_t current_month = 0;
  int8_t current_day = 0;

  char weekday_buf[16];
  char month_name_buf[16];
  int parsed_day = 0;
  int parsed_hour = 0;
  int parsed_minute = 0;
  int parsed_second = 0;
  int parsed_year = 0;

  current_day = (int8_t)parsed_day;
  current_year = (int16_t)parsed_year;

  if (strcmp(month_name_buf, "January") == 0) {
    current_month = 1;
  } else if (strcmp(month_name_buf, "February") == 0) {
    current_month = 2;
  } else if (strcmp(month_name_buf, "March") == 0) {
    current_month = 3;
  } else if (strcmp(month_name_buf, "April") == 0) {
    current_month = 4;
  } else if (strcmp(month_name_buf, "May") == 0) {
    current_month = 5;
  } else if (strcmp(month_name_buf, "June") == 0) {
    current_month = 6;
  } else if (strcmp(month_name_buf, "July") == 0) {
    current_month = 7;
  } else if (strcmp(month_name_buf, "August") == 0) {
    current_month = 8;
  } else if (strcmp(month_name_buf, "September") == 0) {
    current_month = 9;
  } else if (strcmp(month_name_buf, "October") == 0) {
    current_month = 10;
  } else if (strcmp(month_name_buf, "November") == 0) {
    current_month = 11;
  } else if (strcmp(month_name_buf, "December") == 0) {
    current_month = 12;
  }
  char today_str[11];
  snprintf(today_str,
      sizeof(today_str),
      "%04d-%02d-%02d",
      current_year,
      current_month,
      current_day);

  path *calendar_events_path = path_init(CALENDAR_EVENTS_DIR);
  path *calendar_events_full_path = path_concat(sys_paths->dirs->user_path,
      calendar_events_path);
  path *today_str_path = path_init(today_str);
  path *today_events_path = path_concat(calendar_events_full_path,
      today_str_path);
  path_free(today_str_path);

  if (!path_exists(today_events_path)) {
    path_free(calendar_events_path);
    path_free(calendar_events_full_path);
    path_free(today_events_path);
    return today_events;
  }

  str_list *event_files = path_listdir(today_events_path);
  for (size_t i = 0; i < event_files->len; i++) {
    char *event_file_name = str_list_get(event_files, i);
    path *event_file_name_path = path_init(event_file_name);
    path *event_file_path = path_concat(today_events_path,
        event_file_name_path);
    path_free(event_file_name_path);

    char *description = path_key_value_get(event_file_path, "description");
    char *start_time_value = path_key_value_get(event_file_path, "start_time");
    char *end_time_value = path_key_value_get(event_file_path, "end_time");

    event_t event;
    memset(&event, 0, sizeof(event));
    str_list *event_file_name_parts = string_split(event_file_name, ' ');
    char *display_name = strdup(str_list_get(event_file_name_parts, 1));
    event.name = display_name;
    str_list_free(event_file_name_parts);

    event.description = description ? description : "";

    event.end_time_set = (end_time_value != NULL);
    event.all_day = (strcmp(event_file_name, "") != 0 &&
                     strstr(event_file_name, "all_day") != NULL);

    if (start_time_value != NULL) {
      str_list *start_parts = string_split(start_time_value, '|');
      if (start_parts->len >= 7) {
        event.start_time.year = atoi(str_list_get(start_parts, 0));
        event.start_time.month = atoi(str_list_get(start_parts, 1));
        event.start_time.day = atoi(str_list_get(start_parts, 2));
        event.start_time.dotw = atoi(str_list_get(start_parts, 3));
        event.start_time.hour = atoi(str_list_get(start_parts, 4));
        event.start_time.min = atoi(str_list_get(start_parts, 5));
        event.start_time.sec = atoi(str_list_get(start_parts, 6));
      }
      str_list_free(start_parts);
    }

    if (end_time_value != NULL) {
      str_list *end_parts = string_split(end_time_value, '|');
      if (end_parts->len >= 7) {
        event.end_time.year = atoi(str_list_get(end_parts, 0));
        event.end_time.month = atoi(str_list_get(end_parts, 1));
        event.end_time.day = atoi(str_list_get(end_parts, 2));
        event.end_time.dotw = atoi(str_list_get(end_parts, 3));
        event.end_time.hour = atoi(str_list_get(end_parts, 4));
        event.end_time.min = atoi(str_list_get(end_parts, 5));
        event.end_time.sec = atoi(str_list_get(end_parts, 6));
      }
      str_list_free(end_parts);
    }

    char display_buf[80];
    if (event.all_day) {
      snprintf(display_buf, sizeof(display_buf), "all day-%s", event.name);
    } else {
      snprintf(display_buf,
          sizeof(display_buf),
          "%02d:%02d-%s",
          event.start_time.hour,
          event.start_time.min,
          event.name);
    }

    str_list_append(today_events, display_buf);

    free(display_name);
    free(description);
    free(start_time_value);
    free(end_time_value);
    path_free(event_file_path);
  }

  str_list_free(event_files);
  path_free(calendar_events_path);
  path_free(calendar_events_full_path);
  path_free(today_events_path);
  return today_events;
}