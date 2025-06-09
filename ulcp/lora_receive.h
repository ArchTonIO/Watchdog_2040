#ifndef LORA_RECEIVE_H
#define LORA_RECEIVE_H

#include "hardware_drivers/sx1278.h"
#include "ulcp.h"

void on_recv(char *msg);

#endif