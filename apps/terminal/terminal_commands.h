// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#ifndef TERMINAL_COMMANDS_H
#define TERMINAL_COMMANDS_H

#include <stddef.h>

#include "core/data_structures/string_list.h"

typedef struct terminal terminal;

typedef struct {
  terminal *term;
  str_list *args;
} command_params;

typedef struct {
  char name[20];
  char description[100];
  int8_t (*callback)(command_params params);
} command;

command create_command(char *name,
    char *description,
    int8_t (*callback)(command_params params));

/*Standard commands callbacks*/
int8_t __help__(command_params params);
int8_t __clear__(command_params params);
int8_t __echo__(command_params params);
int8_t __exit__(command_params params);
int8_t __cd__(command_params params);
int8_t __ls__(command_params params);
int8_t __pwd__(command_params params);
int8_t __mkdir__(command_params params);
int8_t __touch__(command_params params);
int8_t __rm__(command_params params);
int8_t __mv__(command_params params);
int8_t __cp__(command_params params);
int8_t __cat__(command_params params);
int8_t __head__(command_params params);
int8_t __tail__(command_params params);
int8_t __grep__(command_params params);
int8_t __whoami__(command_params params);
int8_t __ping__(command_params params);
int8_t __unano__(command_params params);
int8_t __info__(command_params params);
int8_t __history__(command_params params);
int8_t __serial__(command_params params);

#endif