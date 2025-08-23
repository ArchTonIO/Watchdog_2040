// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include "core/hardware_drivers/core1.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "pico/multicore.h"
#include "pico/stdlib.h"

#include "apps/msg_manager/msg_manager.h"
#include "apps/time_submenus/set_alarm_submenu.h"
#include "core/hardware_drivers/haptics.h"

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

inline void dispatch_haptic(uint8_t data) { haptic_pulse(data); }

inline void core1_reset() { multicore_reset_core1(); }
inline void core1_launch(void (*entry)(void)) {
  multicore_launch_core1(entry);
}
inline void core1_await() {
  while (multicore_fifo_pop_blocking() != CORE_1_OP_DONE)
    ;
}