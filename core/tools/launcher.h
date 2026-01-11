// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <stdint.h>

#define MAX_APPS 50

typedef struct {
  char *name;
  const uint8_t *icon;
  void (*entry_point)(void);
} application;

typedef struct {
  char *name;
  uint8_t apps_count;
  application applications[MAX_APPS];
} launcher;

#define LAUNCHER_INIT(NAME)                                                   \
  {                                                                           \
    NAME, 0, { 0 }                                                            \
  }

void launcher_add_application(launcher *self,
    char *name,
    const uint8_t *icon,
    void (*entry_point)(void));
void launcher_start(launcher *self);

#define DEFINE_LAUNCHER(VAR_NAME, TITLE, ...)                                 \
  launcher VAR_NAME = LAUNCHER_INIT(TITLE);                                   \
  static void __init_##VAR_NAME(void) {                                       \
    application temp_apps[] = {__VA_ARGS__};                                  \
    for (size_t i = 0; i < sizeof(temp_apps) / sizeof(temp_apps[0]); i++) {   \
      launcher_add_application(&VAR_NAME,                                     \
          temp_apps[i].name,                                                  \
          temp_apps[i].icon,                                                  \
          temp_apps[i].entry_point);                                          \
    }                                                                         \
  }                                                                           \
  __attribute__((constructor)) static void __ctor_##VAR_NAME(void) {          \
    __init_##VAR_NAME();                                                      \
  }

#endif
