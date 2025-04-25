#ifndef MALLOC_MASCOT_H
#define MALLOC_MASCOT_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware_drivers/ssd1306.h"
#include "hardware_drivers/joystick.h"
#include "bitmaps.h"
#include "hw_manager.h"

typedef struct
{
  char username[15];
  uint32_t ulcp_addr;
} malloc_memories;

void start_malloc_mascot_tutorial();
malloc_memories *malloc_memories_init();
void malloc_greets_you();
void malloc_explains_you_joystick();
void malloc_explains_you_menu();
void malloc_explains_you_text_editor();
void malloc_asks_your_name();
void malloc_explains_you_home_screen();
void malloc_generates_ulcp_address();
void malloc_says_goodbye();
#endif