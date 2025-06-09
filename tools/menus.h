#ifndef MENUS_H
#define MENUS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"

#include "components/hw_manager.h"
#include "components/malloc_mascot.h"
#include "components/msg_manager.h"
#include "data_structures/string_list.h"
#include "hardware_drivers/battery.h"
#include "hardware_drivers/joystick.h"
#include "hardware_drivers/sdcard.h"
#include "hardware_drivers/ssd1306.h"
#include "tools/options_gen.h"

void display_main_menu();
void display_tutorial_menu();

#endif