// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef SET_ALARM_SUBMENU_H
#define SET_ALARM_SUBMENU_H

#include "apps/time/include/time_utils.h"

#define ALARM_ON "[#ON]"
#define ALARM_OFF "[OFF]"

void set_alarm_launch();
void process_alarm();
void load_alarms();
void set_alarm_time(time_digits *initial_time);

#endif