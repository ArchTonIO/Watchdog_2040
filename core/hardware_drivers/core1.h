#ifndef CORE1_H
#define CORE1_H

#include <stdint.h>

/*instructions set for haptic feedback [1-9]*/
#define AUTO_PULSE 0x01
#define MICRO_PULSE 0x03
#define SHORT_PULSE 0x05
#define LONG_PULSE 0x07
#define DOUBLE_PULSE 0x09

/*instructions set for bootup check [19-45]*/
#define ENS160_OK 0x13
#define ENS160_ERR 0x15
#define SX1278_OK 0x17
#define SX1278_ERR 0x19
#define BATTERY_OK 0x1b
#define BATTERY_ERR 0x1d
#define JOYSTICK_OK 0x2f
#define JOYSTICK_ERR 0x21
#define SDCARD_OK 0x23
#define SDCARD_ERR 0x25
#define RTC_OK 0x27
#define RTC_ERR 0x29
#define CHECKS_END 0x2b
#define CORE_1_OP_DONE 0x2d

/*instructions set for ulmp operations [55-]*/
#define SHOW_NOTIFICATION 0x37

void core1_launch(void (*entry)(void));
void core1_listens_for_instructions();
void core1_push_instruction(uint8_t data);
void core1_await();
void core1_reset();

#endif