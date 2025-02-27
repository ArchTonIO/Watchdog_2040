#include "hardware/rtc.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"
#include "rtc_time.h"
#include <stdlib.h>

rtc_time *rtc_time_init(
	int16_t year,
	int8_t month,
	int8_t day,
	int8_t weekday,
	int8_t hour,
	int8_t minute,
	int8_t second)
{
	rtc_time *new_rtc = (rtc_time *)malloc(sizeof(rtc_time));
	new_rtc->year = year;
	new_rtc->month = month;
	new_rtc->day = day;
	new_rtc->weekday = weekday;
	new_rtc->hour = hour;
	new_rtc->minute = minute;
	new_rtc->second = second;
	char datetime_buf[256];
	char *datatetime_str = &datetime_buf[0];
	datetime_t t = {
		.year = year,
		.month = month,
		.day = day,
		.dotw = weekday,
		.hour = hour,
		.min = minute,
		.sec = second};
	new_rtc->internal_datetime = t;
	rtc_init();
	rtc_set_datetime(&new_rtc->internal_datetime);
	sleep_us(64);
	return new_rtc;
}

void rtc_time_set_time(rtc_time *rtc, int16_t year, int8_t month, int8_t day, int8_t weekday, int8_t hour, int8_t minute, int8_t second)
{
	char datetime_buf[256];
	char *datetime_str = &datetime_buf[0];
	datetime_t t = {
		.year = year,
		.month = month,
		.day = day,
		.dotw = weekday,
		.hour = hour,
		.min = minute,
		.sec = second};
	rtc->internal_datetime = t;
	rtc_set_datetime(&rtc->internal_datetime);
	sleep_us(64);
}

char *rtc_time_now(rtc_time *rtc)
{
	rtc_get_datetime(&rtc->internal_datetime);
	datetime_to_str(rtc->internal_datetime_buf, sizeof(rtc->internal_datetime_buf), &rtc->internal_datetime);
	return rtc->internal_datetime_buf;
}

void rtc_time_add_alarm(rtc_time *rtc, int8_t hour, int8_t minute, int8_t second, rtc_callback_t callback)
{
	datetime_t target_time = {
		.year = rtc->year,
		.month = rtc->month,
		.day = rtc->day,
		.dotw = rtc->weekday,
		.hour = hour,
		.min = minute,
		.sec = second};
	rtc_set_alarm(&target_time, callback);
	rtc_enable_alarm();
}

void rtc_time_remove_alarm(rtc_time *rtc)
{
	rtc_disable_alarm();
}