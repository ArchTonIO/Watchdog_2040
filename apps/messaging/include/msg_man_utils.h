// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef MSG_MAN_UTILS_H
#define MSG_MAN_UTILS_H

#include <pico/types.h>
#include <sys/_intsup.h>

#include "core/data_structures/include/string_list.h"
#include "core/ulmp/include/ulmp.h"

#define MESSAGES_CHUNK_SIZE 10

void show_read_messages_menu();

#endif