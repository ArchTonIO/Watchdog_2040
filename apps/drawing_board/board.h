#ifndef DRAWING_BOARD_H
#define DRAWING_BOARD_H

#include <stdint.h>

#include "core/graphics/graphic_primitives.h"
#include "core/graphics/layout.h"

#define BOARD_BITMAP_H 13
#define BOARD_BITMAP_W 13
#define BOARD_BITMAP_NUM 9
#define BOARD_BITMAP_SPACING 1

#define TOOL_NAME_LEN 20
#define INPUT_INTERVAL_MS 100

#define SCREEN_KP_X_AXIS -0.048
#define SCREEN_KI_X_AXIS 155.1813
#define SCREEN_KP_Y_AXIS 0.024
#define SCREEN_KI_Y_AXIS -17.4034

#define POINTER_RADIUS 5

#define TOOLBAR_LAYER "toolbar"
#define TEXT_AREAS_LAYER "text_areas"
#define POINTER_LAYER "pointer"
#define DRAWING_AREA_LAYER "drawing_area"

typedef struct {
  line hl;
  line vl;
  uint8_t posx;
  uint8_t posy;
} pointer;

typedef struct {
  char name[TOOL_NAME_LEN];
  bitmap_definition *bitmap_def;
  bool active;
} tool;

typedef struct {
  tool *tools[10];
  layout *board_layout;
  uint8_t selected_tool_index;
  text_area *tool_name;
  text_area *select_or_draw;
  bool select_mode;
  bool should_quit;
  point points[100];
  uint8_t points_counter;
  pointer *board_pointer;
} drawing_board;

void drawing_board_launch();

#endif