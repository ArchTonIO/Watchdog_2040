#include "core/hardware_drivers/core1.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "pico/multicore.h"
#include "pico/stdlib.h"

#include "apps/msg_manager/msg_manager.h"
#include "apps/time_submenus/set_alarm_submenu.h"
#include "core/hardware_drivers/haptics.h"

static uint32_t time_now;
static uint32_t time_from_last_haptic_instruction;
static uint32_t last_haptic_instruction_time;

void dispatch_instruction(uint8_t data);
void dispatch_haptic(uint8_t data);
static void __core1_listens_for_instructions__();

inline void core1_push_instruction(uint8_t data) {
  multicore_fifo_push_blocking(data);
}

inline void core1_listens_for_instructions() {
  core1_launch(__core1_listens_for_instructions__);
}

static void __core1_listens_for_instructions__() {
  while (true) {
    process_messages();
    process_alarm();
    if (multicore_fifo_rvalid())
      dispatch_instruction(multicore_fifo_pop_blocking());
  }
}

void dispatch_instruction(uint8_t data) {
  if (data <= 0x09)
    dispatch_haptic(data);
}

void dispatch_haptic(uint8_t data) {
  time_now = to_ms_since_boot(get_absolute_time());
  if (last_haptic_instruction_time != 0)
    time_from_last_haptic_instruction = time_now -
                                        last_haptic_instruction_time;
  last_haptic_instruction_time = time_now;
  switch (data) {
  case 0x01:
    if (time_from_last_haptic_instruction < 151)
      haptics_motor_pulse((uint16_t[]){50}, (uint16_t[]){20}, 1);
    else
      haptics_motor_pulse((uint16_t[]){100}, (uint16_t[]){0}, 1);
    break;
  case 0x03:
    haptics_motor_pulse((uint16_t[]){50}, (uint16_t[]){20}, 1);
    break;
  case 0x05:
    haptics_motor_pulse((uint16_t[]){120}, (uint16_t[]){0}, 1);
    break;
  case 0x07:
    haptics_motor_pulse((uint16_t[]){1000}, (uint16_t[]){0}, 1);
    break;
  case 0x09:
    haptics_motor_pulse((uint16_t[]){200, 400}, (uint16_t[]){100, 0}, 2);
    break;
  default:
    break;
  }
}

inline void core1_reset() { multicore_reset_core1(); }
inline void core1_launch(void (*entry)(void)) {
  multicore_launch_core1(entry);
}
inline void core1_await() {
  while (multicore_fifo_pop_blocking() != CORE_1_OP_DONE)
    ;
}