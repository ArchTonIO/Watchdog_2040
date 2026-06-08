#include "apps/calendar/include/calendar.h"

#include <stdio.h>

#include "apps/calendar/include/bitmaps.h"
#include "apps/calendar/include/event.h"
#include "core/components/include/hw_manager.h"
#include "core/graphics/include/graphic_primitives.h"
#include "core/graphics/include/layout.h"
#include "core/hardware_drivers/include/haptics.h"
#include "core/hardware_drivers/include/joystick.h"
#include "core/hardware_drivers/include/rtc_time.h"
#include "core/hardware_drivers/include/ssd1306.h"

uint8_t months_len[] = {
    JAN_LEN,
    FEB_LEN,
    MAR_LEN,
    APR_LEN,
    MAY_LEN,
    JUN_LEN,
    JUL_LEN,
    AUG_LEN,
    SEP_LEN,
    OCT_LEN,
    NOV_LEN,
    DIC_LEN,
};

void build_month_page(calendar_t *this_calendar);
void run_calendar(calendar_t *this_calendar);
void redraw(calendar_t *this_calendar);

void update_bottom_bar(calendar_t *this_calendar) {
  months_len[1] = this_calendar->selected_year % 4 == 0 ? FEB_LEN + 1
                                                        : FEB_LEN;
  this_calendar->bottom_bar_bitmap_defs[0]
      .bitmap = months_bitmaps[this_calendar->selected_month];
  this_calendar->bottom_bar_bitmap_defs[1]
      .bitmap = year_digit_bitmaps[this_calendar->selected_year / 1000];
  this_calendar->bottom_bar_bitmap_defs[2]
      .bitmap = year_digit_bitmaps[(this_calendar->selected_year / 100) % 10];
  this_calendar->bottom_bar_bitmap_defs[3]
      .bitmap = year_digit_bitmaps[(this_calendar->selected_year / 10) % 10];
  this_calendar->bottom_bar_bitmap_defs[4]
      .bitmap = year_digit_bitmaps[this_calendar->selected_year % 10];
}

void update_selected_day(calendar_t *this_calendar, int8_t previous_day) {
  this_calendar->days_bitmap_defs[previous_day]->is_inverted = false;
  this_calendar->days_bitmap_defs[this_calendar->selected_day]
      ->is_inverted = true;
}

int8_t get_dotw_from_date(int8_t day, int8_t month, int16_t year) {
  static const int8_t month_offsets[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

  if (month < 2)
    year--;

  int8_t sunday_based = (year + year / 4 - year / 100 + year / 400 +
                            month_offsets[month] + day) %
                        7;
  return (sunday_based + 6) % 7;
}

void calendar_launch() {
  internal_rtc_t time_now;
  rtc_time_now(&time_now);
  int8_t today = time_now.internal_datetime.day;
  int8_t month = time_now.internal_datetime.month - 1;

  calendar_t this_calendar;
  this_calendar.actual_day = today;
  this_calendar.actual_month = month;
  this_calendar.actual_year = time_now.internal_datetime.year;
  this_calendar.selected_day = today;
  this_calendar.selected_month = month;
  this_calendar.selected_year = time_now.internal_datetime.year;

  layout *month_page_layout = layout_init();
  this_calendar.month_page_layout = month_page_layout;
  layout_add_layer(month_page_layout, DAYS_LAYER);
  layout_add_layer(month_page_layout, EVENT_DAYS);
  layout_add_layer(month_page_layout, CURRENT_DAY);
  layout_add_layer(month_page_layout, BOTTOM_BAR);
  layout_add_layer(month_page_layout, DOTW);

  bitmap_definition day1_bd;
  bitmap_definition day2_bd;
  bitmap_definition day3_bd;
  bitmap_definition day4_bd;
  bitmap_definition day5_bd;
  bitmap_definition day6_bd;
  bitmap_definition day7_bd;
  bitmap_definition day8_bd;
  bitmap_definition day9_bd;
  bitmap_definition day10_bd;
  bitmap_definition day11_bd;
  bitmap_definition day12_bd;
  bitmap_definition day13_bd;
  bitmap_definition day14_bd;
  bitmap_definition day15_bd;
  bitmap_definition day16_bd;
  bitmap_definition day17_bd;
  bitmap_definition day18_bd;
  bitmap_definition day19_bd;
  bitmap_definition day20_bd;
  bitmap_definition day21_bd;
  bitmap_definition day22_bd;
  bitmap_definition day23_bd;
  bitmap_definition day24_bd;
  bitmap_definition day25_bd;
  bitmap_definition day26_bd;
  bitmap_definition day27_bd;
  bitmap_definition day28_bd;
  bitmap_definition day29_bd;
  bitmap_definition day30_bd;
  bitmap_definition day31_bd;
  bitmap_definition day_selected_bd;
  bitmap_definition day_event_bd;
  bitmap_definition *bitmap_defs[31] = {
      &day1_bd,
      &day2_bd,
      &day3_bd,
      &day4_bd,
      &day5_bd,
      &day6_bd,
      &day7_bd,
      &day8_bd,
      &day9_bd,
      &day10_bd,
      &day11_bd,
      &day12_bd,
      &day13_bd,
      &day14_bd,
      &day15_bd,
      &day16_bd,
      &day17_bd,
      &day18_bd,
      &day19_bd,
      &day20_bd,
      &day21_bd,
      &day22_bd,
      &day23_bd,
      &day24_bd,
      &day25_bd,
      &day26_bd,
      &day27_bd,
      &day28_bd,
      &day29_bd,
      &day30_bd,
      &day31_bd,
  };

  for (size_t i = 0; i < 31; i++)
    this_calendar.days_bitmap_defs[i] = bitmap_defs[i];

  uint8_t bottom_bar_width = MONTHS_BITMAPS_WIDTH + 5 * 2 +
                             (YEAR_DIGITS_BITMAPS_WIDTH + 1) * 4 + 5;
  uint8_t xoff = (SSD1306_WIDTH - bottom_bar_width) / 2;

  bitmap_definition month_bd;
  month_bd.bitmap = months_bitmaps[this_calendar.actual_month];
  month_bd.width = MONTHS_BITMAPS_WIDTH;
  month_bd.height = MONTHS_BITMAPS_HEIGHT;
  month_bd.posx = xoff;
  month_bd.posy = SSD1306_HEIGHT - MONTHS_BITMAPS_HEIGHT;
  month_bd.is_inverted = false;
  xoff = xoff + MONTHS_BITMAPS_WIDTH + 5;

  bitmap_definition millennium_bd;
  millennium_bd.bitmap = year_digit_bitmaps[this_calendar.actual_year / 1000];
  millennium_bd.width = YEAR_DIGITS_BITMAPS_WIDTH;
  millennium_bd.height = YEAR_DIGITS_BITMAPS_HEIGHT;
  millennium_bd.posx = xoff;
  millennium_bd.posy = SSD1306_HEIGHT - YEAR_DIGITS_BITMAPS_HEIGHT;
  millennium_bd.is_inverted = false;
  xoff = xoff + YEAR_DIGITS_BITMAPS_WIDTH + 1;

  bitmap_definition century_bd;
  century_bd
      .bitmap = year_digit_bitmaps[(this_calendar.actual_year / 100) % 10];
  century_bd.width = YEAR_DIGITS_BITMAPS_WIDTH;
  century_bd.height = YEAR_DIGITS_BITMAPS_HEIGHT;
  century_bd.posx = xoff;
  century_bd.posy = SSD1306_HEIGHT - YEAR_DIGITS_BITMAPS_HEIGHT;
  century_bd.is_inverted = false;
  xoff = xoff + YEAR_DIGITS_BITMAPS_WIDTH + 1;

  bitmap_definition decade_bd;
  decade_bd.bitmap = year_digit_bitmaps[(this_calendar.actual_year / 10) % 10];
  decade_bd.width = YEAR_DIGITS_BITMAPS_WIDTH;
  decade_bd.height = YEAR_DIGITS_BITMAPS_HEIGHT;
  decade_bd.posx = xoff;
  decade_bd.posy = SSD1306_HEIGHT - YEAR_DIGITS_BITMAPS_HEIGHT;
  decade_bd.is_inverted = false;
  xoff = xoff + YEAR_DIGITS_BITMAPS_WIDTH + 1;

  bitmap_definition year_bd;
  year_bd.bitmap = year_digit_bitmaps[this_calendar.actual_year % 10];
  year_bd.width = YEAR_DIGITS_BITMAPS_WIDTH;
  year_bd.height = YEAR_DIGITS_BITMAPS_HEIGHT;
  year_bd.posx = xoff;
  year_bd.posy = SSD1306_HEIGHT - YEAR_DIGITS_BITMAPS_HEIGHT;
  year_bd.is_inverted = false;
  xoff = xoff + YEAR_DIGITS_BITMAPS_WIDTH + 5;

  this_calendar.bottom_bar_bitmap_defs[0] = month_bd;
  this_calendar.bottom_bar_bitmap_defs[1] = millennium_bd;
  this_calendar.bottom_bar_bitmap_defs[2] = century_bd;
  this_calendar.bottom_bar_bitmap_defs[3] = decade_bd;
  this_calendar.bottom_bar_bitmap_defs[4] = year_bd;

  xoff = 4;
  bitmap_definition monday_label_bd;
  monday_label_bd.bitmap = dotw_bitmaps[0];
  monday_label_bd.width = DOTW_WIDTH;
  monday_label_bd.height = DOTW_HEIGHT;
  monday_label_bd.posx = xoff;
  monday_label_bd.posy = 0;
  monday_label_bd.is_inverted = false;
  xoff = xoff + DOTW_WIDTH + 8;

  bitmap_definition tuesday_label_bd;
  tuesday_label_bd.bitmap = dotw_bitmaps[1];
  tuesday_label_bd.width = DOTW_WIDTH;
  tuesday_label_bd.height = DOTW_HEIGHT;
  tuesday_label_bd.posx = xoff;
  tuesday_label_bd.posy = 0;
  tuesday_label_bd.is_inverted = false;
  xoff = xoff + DOTW_WIDTH + 8;

  bitmap_definition wednesday_label_bd;
  wednesday_label_bd.bitmap = dotw_bitmaps[2];
  wednesday_label_bd.width = DOTW_WIDTH;
  wednesday_label_bd.height = DOTW_HEIGHT;
  wednesday_label_bd.posx = xoff;
  wednesday_label_bd.posy = 0;
  wednesday_label_bd.is_inverted = false;
  xoff = xoff + DOTW_WIDTH + 8;

  bitmap_definition thursday_label_bd;
  thursday_label_bd.bitmap = dotw_bitmaps[3];
  thursday_label_bd.width = DOTW_WIDTH;
  thursday_label_bd.height = DOTW_HEIGHT;
  thursday_label_bd.posx = xoff;
  thursday_label_bd.posy = 0;
  thursday_label_bd.is_inverted = false;
  xoff = xoff + DOTW_WIDTH + 8;

  bitmap_definition friday_label_bd;
  friday_label_bd.bitmap = dotw_bitmaps[4];
  friday_label_bd.width = DOTW_WIDTH;
  friday_label_bd.height = DOTW_HEIGHT;
  friday_label_bd.posx = xoff;
  friday_label_bd.posy = 0;
  friday_label_bd.is_inverted = false;
  xoff = xoff + DOTW_WIDTH + 8;

  bitmap_definition saturday_label_bd;
  saturday_label_bd.bitmap = dotw_bitmaps[5];
  saturday_label_bd.width = DOTW_WIDTH;
  saturday_label_bd.height = DOTW_HEIGHT;
  saturday_label_bd.posx = xoff;
  saturday_label_bd.posy = 0;
  saturday_label_bd.is_inverted = false;
  xoff = xoff + DOTW_WIDTH + 8;

  bitmap_definition sunday_label_bd;
  sunday_label_bd.bitmap = dotw_bitmaps[6];
  sunday_label_bd.width = DOTW_WIDTH;
  sunday_label_bd.height = DOTW_HEIGHT;
  sunday_label_bd.posx = xoff;
  sunday_label_bd.posy = 0;
  sunday_label_bd.is_inverted = false;

  this_calendar.dotw_bitmap_defs[0] = monday_label_bd;
  this_calendar.dotw_bitmap_defs[1] = tuesday_label_bd;
  this_calendar.dotw_bitmap_defs[2] = wednesday_label_bd;
  this_calendar.dotw_bitmap_defs[3] = thursday_label_bd;
  this_calendar.dotw_bitmap_defs[4] = friday_label_bd;
  this_calendar.dotw_bitmap_defs[5] = saturday_label_bd;
  this_calendar.dotw_bitmap_defs[6] = sunday_label_bd;

  build_month_page(&this_calendar);
  redraw(&this_calendar);
  run_calendar(&this_calendar);
}

void build_current_day_indicator(calendar_t *this_calendar) {
  int8_t dotw_day1 = get_dotw_from_date(1,
      this_calendar->actual_month,
      this_calendar->actual_year);
  uint8_t visual_index = (this_calendar->actual_day - 1) + dotw_day1;
  uint8_t col = visual_index % 7;
  uint8_t row = visual_index / 7;

  layer *current_day_layer = get_layer_by_name(
      this_calendar->month_page_layout,
      CURRENT_DAY);

  point dl = create_point(DAY_SLOT_WIDTH * col + 1,
      DAY_SLOT_HEIGHT * row + 7 + DAY_SLOT_HEIGHT - 1);
  point dr = create_point(DAY_SLOT_WIDTH * col + DAY_SLOT_WIDTH - 1,
      DAY_SLOT_HEIGHT * row + 7 + DAY_SLOT_HEIGHT - 1);
  point dl2_up = create_point(DAY_SLOT_WIDTH * col + 1,
      DAY_SLOT_HEIGHT * row + 7 + DAY_SLOT_HEIGHT - 3);
  point dr2_up = create_point(DAY_SLOT_WIDTH * col + DAY_SLOT_WIDTH - 1,
      DAY_SLOT_HEIGHT * row + 7 + DAY_SLOT_HEIGHT - 3);

  line l1 = create_line(dl, dr);
  line l2 = create_line(dl2_up, dl);
  line l3 = create_line(dr2_up, dr);

  layer_add_line(current_day_layer, l1);
  layer_add_line(current_day_layer, l2);
  layer_add_line(current_day_layer, l3);
}

void build_event_indicator(calendar_t *this_calendar, uint8_t day) {
  uint8_t dotw_day1 = get_dotw_from_date(1,
      this_calendar->actual_month,
      this_calendar->actual_year);
  uint8_t visual_index = day + dotw_day1;
  uint8_t col = visual_index % 7;
  uint8_t row = visual_index / 7;

  layer *event_days_layer = get_layer_by_name(this_calendar->month_page_layout,
      EVENT_DAYS);

  point ul = create_point(DAY_SLOT_WIDTH * col + 13,
      DAY_SLOT_HEIGHT * row + 7);
  point dr = create_point(ul.x + 1, ul.y + 2);

  rectangle event_indicator = create_rectangle(ul, dr, 0);

  layer_add_rectangle(event_days_layer, event_indicator);
}

void show_this_month_events(calendar_t *this_calendar) {
  load_days_with_events(this_calendar);
  for (size_t i = 0; i < this_calendar->days_with_events_counter; i++) {
    build_event_indicator(this_calendar,
        this_calendar->days_with_events[i] - 1);
  }
}

void build_month_page(calendar_t *this_calendar) {
  layer *days_layer = get_layer_by_name(this_calendar->month_page_layout,
      DAYS_LAYER);

  uint8_t row = 0;
  uint8_t col = 0;
  for (size_t i = 0; i < months_len[this_calendar->selected_month]; i++) {
    if (col == 7) {
      row++;
      col = 0;
    }

    if (i == 0) {
      uint8_t dotw = get_dotw_from_date(i + 1,
          this_calendar->selected_month,
          this_calendar->selected_year);
      col = col + dotw;
    }

    this_calendar->days_bitmap_defs[i]->bitmap = day_bitmaps[i];
    this_calendar->days_bitmap_defs[i]->width = DAY_SLOT_WIDTH;
    this_calendar->days_bitmap_defs[i]->height = DAY_SLOT_HEIGHT;
    this_calendar->days_bitmap_defs[i]->posx = DAY_SLOT_WIDTH * col;
    this_calendar->days_bitmap_defs[i]->posy = DAY_SLOT_HEIGHT * row + 7;
    this_calendar->days_bitmap_defs[i]->is_inverted = false;
    if (this_calendar->actual_day == i + 1) {
      build_current_day_indicator(this_calendar);
      this_calendar->days_bitmap_defs[i]->is_inverted = true;
      this_calendar->selected_day = i;
    }
    col++;
  }
}

void redraw(calendar_t *this_calendar) {
  ssd1306_clear(&(drivers->ssd1306));

  layer *dotw_layer = get_layer_by_name(this_calendar->month_page_layout,
      DOTW);
  layer *days_layer = get_layer_by_name(this_calendar->month_page_layout,
      DAYS_LAYER);
  layer *current_day_layer = get_layer_by_name(
      this_calendar->month_page_layout,
      CURRENT_DAY);
  layer *bottom_bar_layer = get_layer_by_name(this_calendar->month_page_layout,
      BOTTOM_BAR);
  layer *event_days_layer = get_layer_by_name(this_calendar->month_page_layout,
      EVENT_DAYS);

  layer_remove_bitmap_definitions(dotw_layer);
  layer_remove_bitmap_definitions(days_layer);
  layer_remove_lines(current_day_layer);
  layer_remove_rectangles(event_days_layer);
  layer_remove_bitmap_definitions(bottom_bar_layer);

  for (size_t i = 0; i < 7; i++)
    layer_add_bitmap_definition(
        get_layer_by_name(this_calendar->month_page_layout, DOTW),
        this_calendar->dotw_bitmap_defs[i]);
  for (size_t i = 0; i < months_len[this_calendar->selected_month]; i++)
    layer_add_bitmap_definition(days_layer,
        *(this_calendar->days_bitmap_defs[i]));
  for (size_t i = 0; i < 5; i++)
    layer_add_bitmap_definition(bottom_bar_layer,
        this_calendar->bottom_bar_bitmap_defs[i]);

  if (this_calendar->selected_month == this_calendar->actual_month &&
      this_calendar->selected_year == this_calendar->actual_year) {
    layout_set_layer_visibility(this_calendar->month_page_layout,
        CURRENT_DAY,
        true);
    build_current_day_indicator(this_calendar);
  } else
    layout_set_layer_visibility(this_calendar->month_page_layout,
        CURRENT_DAY,
        false);

  show_this_month_events(this_calendar);

  layout_draw_layer(this_calendar->month_page_layout, DOTW);
  layout_draw_layer(this_calendar->month_page_layout, DOTW);
  layout_draw_layer(this_calendar->month_page_layout, DAYS_LAYER);
  layout_draw_layer(this_calendar->month_page_layout, CURRENT_DAY);
  layout_draw_layer(this_calendar->month_page_layout, EVENT_DAYS);
  layout_draw_layer(this_calendar->month_page_layout, BOTTOM_BAR);

  ssd1306_show(&(drivers->ssd1306));
}

void highlight_dotw(calendar_t *this_calendar) {
  uint8_t dotw = get_dotw_from_date(this_calendar->selected_day + 1,
      this_calendar->selected_month,
      this_calendar->selected_year);
  layer *dotw_layer = get_layer_by_name(this_calendar->month_page_layout,
      DOTW);
  for (size_t i = 0; i < 7; i++) {
    if (i == dotw)
      this_calendar->dotw_bitmap_defs[i].is_inverted = true;
    else
      this_calendar->dotw_bitmap_defs[i].is_inverted = false;
    layer_add_bitmap_definition(dotw_layer,
        this_calendar->dotw_bitmap_defs[i]);
  }
}

void navigate_calendar(calendar_t *this_calendar, uint8_t direction) {
  if (direction == C)
    return;

  int8_t adder = 0;
  if (direction == S)
    adder = 7;
  if (direction == N)
    adder = -7;
  if (direction == E)
    adder = 1;
  if (direction == W)
    adder = -1;
  if (direction == SE)
    adder = 8;
  if (direction == SW)
    adder = 6;
  if (direction == NE)
    adder = -6;
  if (direction == NW)
    adder = -8;

  /*check this month days bounds*/
  if (this_calendar->selected_day + adder > -1 &&
      this_calendar->selected_day + adder <
          months_len[this_calendar->selected_month]) {
    int8_t prev_day = this_calendar->selected_day;
    this_calendar->selected_day = this_calendar->selected_day + adder;
    this_calendar->selected_dotw = get_dotw_from_date(
        this_calendar->selected_day,
        this_calendar->selected_month,
        this_calendar->selected_year);
    update_selected_day(this_calendar, prev_day);
    highlight_dotw(this_calendar);
    redraw(this_calendar);
    return;
  }

  /*decrease month/year*/
  if (this_calendar->selected_day + adder < 0) {
    int8_t prev_day = this_calendar->selected_day;
    if (this_calendar->selected_day == 0) {
      if (this_calendar->selected_month == 0) {
        this_calendar->selected_year--;
        this_calendar->selected_month = 11;
        this_calendar->selected_day = 30;
        this_calendar->selected_dotw = get_dotw_from_date(
            this_calendar->selected_day,
            this_calendar->selected_month,
            this_calendar->selected_year);
      } else {
        this_calendar->selected_month--;
        this_calendar
            ->selected_day = months_len[this_calendar->selected_month] - 1;
        this_calendar->selected_dotw = get_dotw_from_date(
            this_calendar->selected_day,
            this_calendar->selected_month,
            this_calendar->selected_year);
      }
      sleep_ms(200);
    } else
      return;
    haptic_short_pulse();
    highlight_dotw(this_calendar);
    build_month_page(this_calendar);
    update_selected_day(this_calendar, prev_day);
    update_bottom_bar(this_calendar);
    redraw(this_calendar);
    return;
  }

  /*increase month/year*/
  if (this_calendar->selected_day + adder >
      months_len[this_calendar->selected_month] - 1) {
    int8_t prev_day = this_calendar->selected_day;
    if (this_calendar->selected_day ==
        months_len[this_calendar->selected_month] - 1) {
      if (this_calendar->selected_month == 11) {
        this_calendar->selected_year++;
        this_calendar->selected_month = 0;
        this_calendar->selected_day = 0;
        this_calendar->selected_dotw = get_dotw_from_date(
            this_calendar->selected_day,
            this_calendar->selected_month,
            this_calendar->selected_year);
      } else {
        this_calendar->selected_month++;
        this_calendar->selected_day = 1;
        this_calendar->selected_dotw = get_dotw_from_date(
            this_calendar->selected_day,
            this_calendar->selected_month,
            this_calendar->selected_year);
      }
      sleep_ms(200);
    } else
      return;
    haptic_short_pulse();
    highlight_dotw(this_calendar);
    build_month_page(this_calendar);
    update_selected_day(this_calendar, prev_day);
    update_bottom_bar(this_calendar);
    redraw(this_calendar);
    return;
  }
}

void run_calendar(calendar_t *this_calendar) {
  sleep_ms(200);
  joystick_update(&(drivers->joystick));
  while (true) {
    joystick_update(&(drivers->joystick));
    navigate_calendar(this_calendar,
        joystick_get_direction(&(drivers->joystick)));
    if (drivers->joystick.button_pressed) {
      sleep_ms(200);
      joystick_update(&(drivers->joystick));
      if (drivers->joystick.button_pressed) {
        layout_free(this_calendar->month_page_layout);
        return;
      }
      open_events_page_by_day(this_calendar);
    }
  }
}