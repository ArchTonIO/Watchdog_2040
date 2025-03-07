#ifndef MSG_MANAGER_H
#define MSG_MANAGER_H

#include "ulcp/ulcp.h"
#include "hardware_drivers/sdcard.h"
#include <stdlib.h>
#include <string.h>
#include "utils.h"

typedef struct
{
  bool new_msg_arrived;
  sdcard *sd;
  contact *contacts;
  void (*notify)(uint16_t src_address);
  void (*eventually_save_received_msg)();
} msg_manager;

typedef struct
{
  uint16_t addr;
  char *name;
} contact;

msg_manager *msg_manager_init();

#endif