#include "core/hardware_drivers/haptics.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/_intsup.h>

#include "pico/stdlib.h"

#include "core/hardware_drivers/core1.h"
#include "core/hardware_drivers/haptics.h"
#include "hardware/gpio.h"

void haptics_init(pin motor_pin) {
  gpio_init(motor_pin);
  gpio_set_dir(motor_pin, GPIO_OUT);
  gpio_put(motor_pin, false);
}

void haptics_motor_on() { gpio_put(HAPTICS_MOTOR_PIN, true); }

void haptics_motor_off() { gpio_put(HAPTICS_MOTOR_PIN, false); }

void haptics_motor_pulse(uint16_t durations_ms[],
    uint16_t intervals[],
    uint8_t pulses) {
  for (uint8_t i = 0; i < pulses; i++) {
    gpio_put(HAPTICS_MOTOR_PIN, true);
    sleep_ms(durations_ms[i]);
    gpio_put(HAPTICS_MOTOR_PIN, false);
    sleep_ms(intervals[i]);
  }
}

/**
 *@brief Sends an auto pulse instruction to core 1.
 *An auto pulse is either a 50ms-20ms or 100ms-0ms pulse, depending on
 *how close this call is to the last pulse call,
 *this is to avoid a cotinous vibration
 *and still feel distinct pulses.
 */
void haptic_auto_pulse() { core1_push_instruction(AUTO_PULSE); }

void haptic_micro_pulse() { core1_push_instruction(MICRO_PULSE); }

void haptic_short_pulse() { core1_push_instruction(SHORT_PULSE); }

void haptic_long_pulse() { core1_push_instruction(LONG_PULSE); }

void haptic_double_pulse() { core1_push_instruction(DOUBLE_PULSE); }
