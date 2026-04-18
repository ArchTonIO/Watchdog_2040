// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef HAPTICS_H
#define HAPTICS_H

#include <stdbool.h>
#include <stdint.h>

#include "config.h"

void haptics_init(pin motor_pin);
void haptics_motor_on();
void haptics_motor_off();
void haptics_motor_pulse(uint16_t duration_ms[],
    uint16_t intervals[],
    uint8_t pulses);
void haptic_auto_pulse();
void haptic_micro_pulse();
void haptic_short_pulse();
void haptic_long_pulse();
void haptic_double_pulse();
void haptic_pulse(uint8_t pulse_type);
void haptics_switch_performing_core();
bool haptics_get_status();
void haptics_disable();
void haptics_enable();

#endif