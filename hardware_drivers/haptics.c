#include "hardware_drivers/haptics.h"

#include <stdint.h>
#include <stdio.h>

#include "pico/multicore.h"
#include "pico/stdlib.h"

#include "hardware/gpio.h"

void core_1_listens_for_haptic_instructions();

void haptics_init(pin motor_pin) {
  gpio_init(motor_pin);
  gpio_set_dir(motor_pin, GPIO_OUT);
  gpio_put(motor_pin, false);
  multicore_launch_core1(core_1_listens_for_haptic_instructions);
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
void haptic_auto_pulse() { multicore_fifo_push_blocking(0); }

void haptic_micro_pulse() { multicore_fifo_push_blocking(1); }

void haptic_short_pulse() { multicore_fifo_push_blocking(2); }

void haptic_long_pulse() { multicore_fifo_push_blocking(3); }

void haptic_double_pulse() { multicore_fifo_push_blocking(4); }

void core_1_listens_for_haptic_instructions() {
  static uint32_t last_instruction_time = 0;
  uint32_t time_now = 0;
  uint32_t time_from_last_instruction = 0;
  while (true) {
    if (multicore_fifo_rvalid()) {
      uint8_t instruction = multicore_fifo_pop_blocking();
      if (instruction <= 4) {
        time_now = to_ms_since_boot(get_absolute_time()); // or /1e6 for sec
        if (last_instruction_time != 0)
          time_from_last_instruction = time_now - last_instruction_time;
        last_instruction_time = time_now;
        switch (instruction) {
        case 0:
          if (time_from_last_instruction < 151)
            haptics_motor_pulse((uint16_t[]){50}, (uint16_t[]){20}, 1);
          else
            haptics_motor_pulse((uint16_t[]){100}, (uint16_t[]){0}, 1);
          break;
        case 1:
          haptics_motor_pulse((uint16_t[]){50}, (uint16_t[]){20}, 1);
          break;
        case 2:
          haptics_motor_pulse((uint16_t[]){120}, (uint16_t[]){0}, 1);
          break;
        case 3:
          haptics_motor_pulse((uint16_t[]){1000}, (uint16_t[]){0}, 1);
          break;
        case 4:
          haptics_motor_pulse((uint16_t[]){200, 400}, (uint16_t[]){100, 0}, 2);
          break;
        default:
          break;
        }
      }
    }
  }
}