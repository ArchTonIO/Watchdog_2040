#ifndef CALENDAR_UTILS_H
#define CALENDAR_UTILS_H

#include <pico/time.h>
#include <stdbool.h>
#include <stdint.h>

bool is_leap_year(int16_t year);
uint8_t days_in_month(int16_t year, int8_t month);
void normalize_datetime(datetime_t *dt);
void add_days(datetime_t *dt, uint16_t days);
void subtract_days(datetime_t *dt, uint16_t days);
void add_weeks(datetime_t *dt, uint16_t weeks);
void subtract_weeks(datetime_t *dt, uint16_t weeks);
void add_months(datetime_t *dt, uint16_t months);
void subtract_months(datetime_t *dt, uint16_t months);
void add_years(datetime_t *dt, uint16_t years);
void subtract_years(datetime_t *dt, uint16_t years);

#endif