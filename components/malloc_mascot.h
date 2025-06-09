#ifndef MALLOC_MASCOT_H
#define MALLOC_MASCOT_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char username[15];
  char user_folder[20];
  uint32_t ulmp_addr;
  char ulmp_addr_str[6];
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
void dump_malloc_memories_to_sd();
malloc_memories *load_malloc_memories_from_sd();

extern malloc_memories *malloc_memories_inst;

#endif