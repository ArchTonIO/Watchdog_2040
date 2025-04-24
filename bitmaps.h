#ifndef BITMAPS_H
#define BITMAPS_H

#include <stdint.h>

extern uint8_t *battery_level_bitmaps[10];
extern uint8_t microsd_working[];
extern uint8_t microsd_not_working[];
extern uint8_t lora_working[];
extern uint8_t lora_not_working[];
extern uint8_t ens160_working[];
extern uint8_t ens160_not_working[];
extern uint8_t *num_msg_bitmaps[12];
extern uint8_t alarm_enabled[];
extern uint8_t alarm_disabled[];
extern uint8_t *clock_digits[10];
extern uint8_t clock_dots[];
extern uint8_t message_received[];
extern uint8_t message_received_open[];
extern uint8_t contact_saved[];
extern uint8_t contact_deleted[];
extern uint8_t malloc_the_mascot[];
extern uint8_t *startup_animation[16];
extern uint8_t malloc_saying_hi[];
extern uint8_t malloc_with_glasses[];
extern uint8_t malloc_with_pointy_eyes[];
extern uint8_t malloc_with_both_eyes[];

#endif