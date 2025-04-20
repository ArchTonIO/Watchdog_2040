#ifndef SW_TEST_H
#define SW_TEST_H

#include <stdio.h>
#include "pico/stdlib.h"
#include <stdlib.h>
#include "hw_manager.h"
#include "msg_manager.h"
#include "virtual_keyboard.h"
#include "hardware_drivers/ssd1306.h"
#include "text_editor.h"

void wait_for_user_input();
void test_text_editor();
void test_message_manager();

#endif