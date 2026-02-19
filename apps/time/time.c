#include "apps/time/include/time.h"

#include "apps/time/include/bitmaps.h"
#include "apps/time/include/set_alarm.h"
#include "apps/time/include/set_date.h"
#include "apps/time/include/set_time.h"
#include "apps/time/include/stopwatch.h"
#include "apps/time/include/timer.h"
#include "core/tools/include/launcher.h"

DEFINE_LAUNCHER(time_launcher,
    "Time",
    {"Set time", set_time_icon, set_time_launch},
    {"Set date", set_date_icon, set_date_launch},
    {"Set alarm", set_alarm_icon, set_alarm_launch},
    {"Unset alarm", unset_alarm_icon, unset_alarm},
    {"Stopwatch", stopwatch_icon, stopwatch_launch},
    {"Timer", timer_icon, timer_launch})

void time_launch() { launcher_start(&time_launcher); }