#ifndef MALLOC_MASCOT_H
#define MALLOC_MASCOT_H

#include <stdint.h>

#define MIN_ULMP_ADDR 10000
#define MAX_ULMP_ADDR 65535

typedef struct {
  char username[15];
  char user_folder[20];
  uint32_t ulmp_addr;
  char ulmp_addr_str[6];
  char user_password[20];
  char user_password_hashed[65];
} malloc_memories;

void start_malloc_mascot_tutorial();
malloc_memories *malloc_memories_init();
void malloc_greets_you();
void malloc_explains_you_joystick();
void malloc_explains_you_menu();
void malloc_explains_you_text_editor();
void malloc_asks_your_name();
void malloc_asks_for_password();
void malloc_explains_you_home_screen();
void malloc_generates_ulcp_address();
void malloc_says_goodbye();
void dump_malloc_memories_to_sd();
malloc_memories *load_malloc_memories_from_sd();

extern malloc_memories *malloc_memories_inst;

#endif