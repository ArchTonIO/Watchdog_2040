// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include "core/hardware_drivers/haptics.h"

#include <stdint.h>
#include <sys/_intsup.h>

#include "pico/time.h"

#include "core/hardware_drivers/core1.h"
#include "core/hardware_drivers/haptics.h"
#include "hardware/gpio.h"

volatile uint8_t performing_core = 1;
static uint32_t time_now;
static uint32_t time_from_last_haptic_instruction;
static uint32_t last_haptic_instruction_time;

void haptics_init(pin motor_pin) {
  gpio_init(motor_pin);
  gpio_set_dir(motor_pin, GPIO_OUT);
  gpio_put(motor_pin, false);
}

inline void haptics_motor_on() { gpio_put(HAPTICS_MOTOR_PIN, true); }

inline void haptics_motor_off() { gpio_put(HAPTICS_MOTOR_PIN, false); }

void haptics_motor_pulse(uint16_t durations_ms[],
    uint16_t intervals[],
    uint8_t pulses) {
  for (uint8_t i = 0; i < pulses; i++) {
    haptics_motor_on();
    sleep_ms(durations_ms[i]);
    haptics_motor_off();
    sleep_ms(intervals[i]);
  }
}

inline void haptics_switch_performing_core() {
  performing_core = performing_core == 1 ? 0 : 1;
}

void haptic_pulse(uint8_t pulse_type) {
  time_now = to_ms_since_boot(get_absolute_time());
  if (last_haptic_instruction_time != 0)
    time_from_last_haptic_instruction = time_now -
                                        last_haptic_instruction_time;
  last_haptic_instruction_time = time_now;
  switch (pulse_type) {
  case AUTO_PULSE:
    if (time_from_last_haptic_instruction < 151)
      haptics_motor_pulse((uint16_t[]){50}, (uint16_t[]){20}, 1);
    else
      haptics_motor_pulse((uint16_t[]){100}, (uint16_t[]){0}, 1);
    break;
  case MICRO_PULSE:
    haptics_motor_pulse((uint16_t[]){50}, (uint16_t[]){20}, 1);
    break;
  case SHORT_PULSE:
    haptics_motor_pulse((uint16_t[]){120}, (uint16_t[]){0}, 1);
    break;
  case LONG_PULSE:
    haptics_motor_pulse((uint16_t[]){1000}, (uint16_t[]){0}, 1);
    break;
  case DOUBLE_PULSE:
    haptics_motor_pulse((uint16_t[]){200, 400}, (uint16_t[]){100, 0}, 2);
    break;
  default:
    break;
  }
}

/**
 *@brief Sends an auto pulse instruction to core 1.
 *An auto pulse is either a 50ms-20ms or 100ms-0ms pulse, depending on
 *how close this call is to the last pulse call,
 *this is to avoid a cotinous vibration
 *and still feel distinct pulses.
 */
void haptic_auto_pulse() {
  performing_core == 1 ? core1_push_instruction(AUTO_PULSE)
                       : haptic_pulse(AUTO_PULSE);
}

void haptic_micro_pulse() {
  performing_core == 1 ? core1_push_instruction(MICRO_PULSE)
                       : haptic_pulse(MICRO_PULSE);
}

void haptic_short_pulse() {
  performing_core == 1 ? core1_push_instruction(SHORT_PULSE)
                       : haptic_pulse(SHORT_PULSE);
}

void haptic_long_pulse() {
  performing_core == 1 ? core1_push_instruction(LONG_PULSE)
                       : haptic_pulse(LONG_PULSE);
}

void haptic_double_pulse() {
  performing_core == 1 ? core1_push_instruction(DOUBLE_PULSE)
                       : haptic_pulse(DOUBLE_PULSE);
}
