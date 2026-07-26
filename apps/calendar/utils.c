#include "apps/calendar/include/utils.h"

#include <pico/time.h>
#include <stdbool.h>
#include <stdint.h>

bool is_leap_year(int16_t year) {
  return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

uint8_t days_in_month(int16_t year, int8_t month) {
  switch (month) {
  case 2:
    return is_leap_year(year) ? 29 : 28;
  case 4:
  case 6:
  case 9:
  case 11:
    return 30;
  default:
    return 31;
  }
}

void normalize_datetime(datetime_t *dt) {
  while (dt->sec >= 60) {
    dt->sec -= 60;
    dt->min += 1;
  }
  while (dt->sec < 0) {
    dt->sec += 60;
    dt->min -= 1;
  }
  while (dt->min >= 60) {
    dt->min -= 60;
    dt->hour += 1;
  }
  while (dt->min < 0) {
    dt->min += 60;
    dt->hour -= 1;
  }
  while (dt->hour >= 24) {
    dt->hour -= 24;
    dt->day += 1;
  }
  while (dt->hour < 0) {
    dt->hour += 24;
    dt->day -= 1;
  }
  while (dt->month > 12) {
    dt->month -= 12;
    dt->year += 1;
  }
  while (dt->month < 1) {
    dt->month += 12;
    dt->year -= 1;
  }
  while (dt->day > days_in_month(dt->year, dt->month)) {
    dt->day -= days_in_month(dt->year, dt->month);
    dt->month += 1;
    if (dt->month > 12) {
      dt->month = 1;
      dt->year += 1;
    }
  }
  while (dt->day < 1) {
    dt->month -= 1;
    if (dt->month < 1) {
      dt->month = 12;
      dt->year -= 1;
    }
    dt->day += days_in_month(dt->year, dt->month);
  }
}

void add_days(datetime_t *dt, uint16_t days) {
  while (days > 0) {
    dt->day += 1;
    if (dt->day > days_in_month(dt->year, dt->month)) {
      dt->day = 1;
      dt->month += 1;
      if (dt->month > 12) {
        dt->month = 1;
        dt->year += 1;
      }
    }
    days -= 1;
  }
}

void subtract_days(datetime_t *dt, uint16_t days) {
  while (days > 0) {
    dt->day -= 1;
    if (dt->day < 1) {
      dt->month -= 1;
      if (dt->month < 1) {
        dt->month = 12;
        dt->year -= 1;
      }
      dt->day = days_in_month(dt->year, dt->month);
    }
    days -= 1;
  }
}

void add_weeks(datetime_t *dt, uint16_t weeks) {
  while (weeks > 0) {
    add_days(dt, 7);
    weeks -= 1;
  }
}

void subtract_weeks(datetime_t *dt, uint16_t weeks) {
  while (weeks > 0) {
    subtract_days(dt, 7);
    weeks -= 1;
  }
}

void add_months(datetime_t *dt, uint16_t months) {
  while (months > 0) {
    dt->month += 1;
    if (dt->month > 12) {
      dt->month = 1;
      dt->year += 1;
    }
    months -= 1;
  }
  if (dt->day > days_in_month(dt->year, dt->month)) {
    dt->day = days_in_month(dt->year, dt->month);
  }
}

void subtract_months(datetime_t *dt, uint16_t months) {
  while (months > 0) {
    dt->month -= 1;
    if (dt->month < 1) {
      dt->month = 12;
      dt->year -= 1;
    }
    months -= 1;
  }
  if (dt->day > days_in_month(dt->year, dt->month)) {
    dt->day = days_in_month(dt->year, dt->month);
  }
}

void add_years(datetime_t *dt, uint16_t years) {
  dt->year += years;
  if (dt->month == 2 && dt->day == 29 && !is_leap_year(dt->year)) {
    dt->day = 28;
  }
}

void subtract_years(datetime_t *dt, uint16_t years) {
  dt->year -= years;
  if (dt->month == 2 && dt->day == 29 && !is_leap_year(dt->year)) {
    dt->day = 28;
  }
}
