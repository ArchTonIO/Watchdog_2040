// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <stddef.h>
#include <stdint.h>

#include "core/graphics/include/layout.h"

#define MAX_APPS 50

#define GUI_MAX_PAGES 8
#define GUI_APPS_PER_PAGE 6
#define GUI_APPS_PER_ROW 3
#define GUI_APPS_PER_COLUMN 2

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

typedef struct {
  layout *gui_layout;
  uint8_t selected_button;
  uint8_t selected_page;
} launcher_gui;

#define LAUNCHER_INIT(NAME)                                                   \
  {                                                                           \
    NAME, 0, { 0 }                                                            \
  }

void launcher_add_application(launcher *self,
    char *name,
    const uint8_t *icon,
    void (*entry_point)(void));
void launcher_start_tui(launcher *self);
void launcher_start_gui(launcher *self);

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
