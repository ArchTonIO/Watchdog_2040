#ifndef BOOTUP_H
#define BOOTUP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware_drivers/sdcard.h"
#include "hardware_drivers/ssd1306.h"
#include "hardware_drivers/rtc_time.h"
#include "data_structures/string_list.h"
#include "hardware_drivers/joystick.h"
#include "text_editor.h"
#include "ulcp/ulcp.h"
#include "bitmaps.h"
#include "utils.h"
#include "hw_manager.h"
#include "msg_manager.h"
#include "options_gen.h"

#define ENS160_OK 0x0100
#define ENS160_ERR 0x0101
#define SX1278_OK 0x0200
#define SX1278_ERR 0x0201
#define BATTERY_OK 0x0300
#define BATTERY_ERR 0x0301
#define JOYSTICK_OK 0x0400
#define JOYSTICK_ERR 0x0401
#define SDCARD_OK 0x0500
#define SDCARD_ERR 0x0501
#define RTC_OK 0x0600
#define RTC_ERR 0x0601
#define CHECKS_END 0x1111

#define ANIMATION_WIDTH 22
#define ANIMATION_HEIGHT 28
#define ANIMATION_X 106
#define ANIMATION_Y 18

void display_bootup_screen();
void update_bootup_screen();
void first_time_setup();

#endif

