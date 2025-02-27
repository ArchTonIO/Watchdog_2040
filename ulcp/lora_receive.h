#ifndef LORA_RECEIVE_H
#define LORA_RECEIVE_H

#include "ulcp.h"
#include "hardware_drivers/sx1278.h"

void on_recv(char *msg);

#endif