// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include "core/hardware_drivers/core1.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "pico/multicore.h"
#include "pico/stdlib.h"

#include "core/boot/bootup.h"
#include "core/hardware_drivers/haptics.h"
#include "core/hardware_drivers/ssd1306.h"

core1_scheduler *core1_scheduler_inst = NULL;

static void __core1_spin__();

/**
 * @brief Initializes the core1 scheduler.
 *
 * @return A pointer to the initialized core1 scheduler.
 */
core1_scheduler *core1_scheduler_init() {
  if (core1_scheduler_inst != NULL) {
    printf("[CORE1] scheduler already initialized! skipping...");
    return NULL;
  }
  core1_scheduler *self = malloc(sizeof(core1_scheduler));
  self->callbacks_count = 0;
  self->start_flag = false;
  core1_scheduler_inst = self;
  return self;
}

/**
 * @brief Adds a callback function to the core1 scheduler.
 * Every callback added will be excecuted in the main loop of core1
 * once the scheduler start_flag is set to true and core1_spin() is called.
 *
 * @param callback The callback function to add.
 */
void core1_scheduler_add_callback(void (*callback)(void)) {
  if (core1_scheduler_inst->callbacks_count == MAX_SCHEDULER_CALLBACKS) {
    printf("[CORE1] cannot add more callbacks in the scheduler as the limit "
           "was reached.");
    return;
  }
  core1_scheduler_inst
      ->callbacks[core1_scheduler_inst->callbacks_count] = callback;
  core1_scheduler_inst->callbacks_count++;
}

/**
 * @brief Sets the start flag of the core1 scheduler.
 *
 * @param start_flag The value to set the start flag to.
 */
inline void core1_scheduler_set_start_flag(bool start_flag) {
  core1_scheduler_inst->start_flag = start_flag;
}

/**
 * @brief Spins up core1 to execute the scheduler's callbacks and process
 * incoming instructions.
 */
inline void core1_spin() { multicore_launch_core1(__core1_spin__); }

/**
 * @brief Pushes an instruction onto the core1 instruction queue (the multicore
 * fifo).
 *
 * @param data The instruction data to push.
 */
inline void core1_push_instruction(uint8_t data) {
  multicore_fifo_push_blocking(data);
}

/**
 * @brief Awaits the completion of the current core1 operation.
 */
inline void core1_await() {
  while (multicore_fifo_pop_blocking() != CORE_1_OP_DONE)
    ;
}

void core1_exec_scheduler_callbacks() {
  if (!core1_scheduler_inst->start_flag)
    return;
  for (size_t i = 0; i < core1_scheduler_inst->callbacks_count; i++)
    core1_scheduler_inst->callbacks[i]();
}

inline void dispatch_haptic(uint8_t data) { haptic_pulse(data); }

void dispatch_instruction(uint8_t data) {
  if (data <= 0x09)
    dispatch_haptic(data);
  if (data == SHOW_BOOTUP)
    display_bootup_screen();
}

static void __core1_spin__() {
  if (core1_scheduler_inst == NULL) {
    printf("[CORE1] won't spin until the scheduler is initialized with "
           "core1_scheduler_init()");
    return;
  }
  while (true) {
    core1_exec_scheduler_callbacks();
    if (multicore_fifo_rvalid())
      dispatch_instruction(multicore_fifo_pop_blocking());
  }
}
