#ifndef MENUS_H
#define MENUS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hw_manager.h"
#include "msg_manager.h"
#include "hardware_drivers/battery.h"
#include "hardware_drivers/ssd1306.h"
#include "hardware_drivers/joystick.h"
#include "malloc_mascot.h"
#include "hardware_drivers/sdcard.h"
#include "data_structures/string_list.h"
#include "options_gen.h"

void display_main_menu();

#endif