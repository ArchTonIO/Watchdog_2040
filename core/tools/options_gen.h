#ifndef OPTIONS_GEN_H
#define OPTIONS_GEN_H

#include <stdbool.h>
#include <stdint.h>

#include "core/components/hw_manager.h"
#include "core/data_structures/string_list.h"

#define MAX_OPTIONS 100
#define MAX_OPTIONS_ON_SCREEN 6
#define INTERAC_TIMEOUT 200

typedef struct {
  char *name;
  char *display_name;
  bool selected;
  void (*callback)(void);
} option;

typedef struct {
  str_list *options_list;
  option options[MAX_OPTIONS];
  uint8_t num_options;
  uint8_t selected_option;
  char *title;
  int8_t scroll;
} options_page;

options_page *options_page_init(char *title, str_list *options);
void attach_callback_to_option(options_page *page,
    uint8_t option_index,
    void (*callback)(void));
char *options_page_launch(options_page *page);
void options_page_free(options_page *page);

#endif