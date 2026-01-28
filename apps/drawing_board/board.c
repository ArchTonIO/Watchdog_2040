#include "apps/drawing_board/board.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/components/hw_manager.h"
#include "core/graphics/bitmaps.h"
#include "core/graphics/graphic_primitives.h"
#include "core/graphics/layout.h"
#include "core/hardware_drivers/config.h"
#include "core/hardware_drivers/haptics.h"
#include "core/hardware_drivers/joystick.h"
#include "core/hardware_drivers/ssd1306.h"

void board_run(drawing_board *board);

void drawing_board_launch() {
  drawing_board board;
  board.board_layout = layout_init();
  layout_add_layer(board.board_layout, TOOLBAR_LAYER);
  layout_add_layer(board.board_layout, TEXT_AREAS_LAYER);
  layout_add_layer(board.board_layout, POINTER_LAYER);
  layout_add_layer(board.board_layout, DRAWING_AREA_LAYER);
  board.selected_tool_index = 0;
  board.should_quit = false;
  board.select_mode = true;
  board.points_counter = 0;

  text_area tool_name;
  text_area select_or_draw;
  tool_name.text = "[TOOL] pencil";
  tool_name.posx = 0;
  tool_name.posy = 3;
  tool_name.is_inverted = false;
  select_or_draw.text = "[MODE] select";
  select_or_draw.posx = 0;
  select_or_draw.posy = 5;
  select_or_draw.is_inverted = false;

  board.tool_name = &tool_name;
  board.select_or_draw = &select_or_draw;

  pointer p;

  p.posx = SSD1306_WIDTH / 2;
  p.posy = SSD1306_HEIGHT / 2;

  line hl = create_line(create_point(p.posx - POINTER_RADIUS, p.posy),
      create_point(p.posx + POINTER_RADIUS, p.posy));
  line vl = create_line(create_point(p.posx, p.posy - POINTER_RADIUS),
      create_point(p.posx, p.posy + POINTER_RADIUS));

  p.hl = hl;
  p.vl = vl;

  board.board_pointer = &p;

  bitmap_definition pencil_bd;
  bitmap_definition eraser_bd;
  bitmap_definition line_bd;
  bitmap_definition square_bd;
  bitmap_definition triangle_bd;
  bitmap_definition circle_bd;
  bitmap_definition thickness_bd;
  bitmap_definition undo_bd;
  bitmap_definition redo_bd;
  bitmap_definition quit_bd;

  pencil_bd.bitmap = pencil;
  eraser_bd.bitmap = eraser;
  line_bd.bitmap = b_line;
  square_bd.bitmap = square;
  triangle_bd.bitmap = triangle;
  circle_bd.bitmap = b_circle;
  thickness_bd.bitmap = thickness;
  undo_bd.bitmap = undo;
  redo_bd.bitmap = redo;
  quit_bd.bitmap = quit;

  bitmap_definition *bitmaps_defs[BOARD_BITMAP_NUM] = {&pencil_bd,
      &eraser_bd,
      &line_bd,
      &square_bd,
      &triangle_bd,
      &circle_bd,
      &thickness_bd,
      &undo_bd,
      &redo_bd};

  uint8_t xoff = 0;
  for (size_t i = 0; i < BOARD_BITMAP_NUM; i++) {
    bitmaps_defs[i]->width = BOARD_BITMAP_W;
    bitmaps_defs[i]->height = BOARD_BITMAP_H;
    bitmaps_defs[i]->posx = xoff;
    bitmaps_defs[i]->posy = 0;
    bitmaps_defs[i]->is_inverted = false;
    xoff = xoff + BOARD_BITMAP_W + BOARD_BITMAP_SPACING;
  }

  quit_bd.width = BOARD_BITMAP_W;
  quit_bd.height = BOARD_BITMAP_H;
  quit_bd.posx = SSD1306_WIDTH - BOARD_BITMAP_W;
  quit_bd.posy = SSD1306_HEIGHT - BOARD_BITMAP_H;
  quit_bd.is_inverted = false;

  tool pencil = {.name = "[TOOL] pencil   ",
      .bitmap_def = &pencil_bd,
      .active = false};
  tool eraser = {.name = "[TOOL] eraser   ",
      .bitmap_def = &eraser_bd,
      .active = false};
  tool line = {.name = "[TOOL] line     ",
      .bitmap_def = &line_bd,
      .active = false};
  tool square = {.name = "[TOOL] square   ",
      .bitmap_def = &square_bd,
      .active = false};
  tool triangle = {.name = "[TOOL] triangle ",
      .bitmap_def = &triangle_bd,
      .active = false};
  tool circle = {.name = "[TOOL] circle   ",
      .bitmap_def = &circle_bd,
      .active = false};
  tool thickness = {.name = "[TOOL] thickness",
      .bitmap_def = &thickness_bd,
      .active = false};
  tool undo = {.name = "[TOOL] undo     ",
      .bitmap_def = &undo_bd,
      .active = false};
  tool redo = {.name = "[TOOL] redo     ",
      .bitmap_def = &redo_bd,
      .active = false};
  tool quit = {.name = "[TOOL] quit     ",
      .bitmap_def = &quit_bd,
      .active = false};

  tool *tools[10] = {&pencil,
      &eraser,
      &line,
      &square,
      &triangle,
      &circle,
      &thickness,
      &undo,
      &redo,
      &quit};

  for (size_t i = 0; i < 10; i++)
    board.tools[i] = tools[i];

  board_run(&board);
}

void draw(drawing_board *board) {}

void select_tool(drawing_board *board) {
  joystick_update(drivers->joystick);
  uint8_t direction = joystick_get_direction(drivers->joystick);
  if (direction == E && board->selected_tool_index < BOARD_BITMAP_NUM) {
    haptic_auto_pulse();
    board->tools[board->selected_tool_index]->bitmap_def->is_inverted = false;
    board->selected_tool_index++;
    sleep_ms(INPUT_INTERVAL_MS);
  } else if (direction == W && board->selected_tool_index > 0) {
    haptic_auto_pulse();
    board->tools[board->selected_tool_index]->bitmap_def->is_inverted = false;
    board->selected_tool_index--;
    sleep_ms(INPUT_INTERVAL_MS);
  } else if (drivers->joystick->button_pressed) {
    haptic_auto_pulse();
    ssd1306_clear(drivers->oled_screen);
    board->select_mode = false;
    board->tools[board->selected_tool_index]->active = true;
    if (board->tools[9]->active)
      board->should_quit = true;
    sleep_ms(INPUT_INTERVAL_MS);
  }
  board->tools[board->selected_tool_index]->bitmap_def->is_inverted = true;
  board->tool_name->text = board->tools[board->selected_tool_index]->name;
}

void draw_pointer(drawing_board *board) {
  pointer p = *(board->board_pointer);
  layer *pointer = get_layer_by_name(board->board_layout, POINTER_LAYER);
  layer_remove_lines(pointer);
  line hl = create_line(create_point(p.posx - 5, p.posy),
      create_point(p.posx + 5, p.posy));
  line vl = create_line(create_point(p.posx, p.posy - 5),
      create_point(p.posx, p.posy + 5));
  p.hl = hl;
  p.vl = vl;
  layer_add_line(pointer, hl);
  layer_add_line(pointer, vl);
}

void draw_loop(drawing_board *board) {
  joystick_update(drivers->joystick);
  board->board_pointer->posx = SCREEN_KP_X_AXIS * drivers->joystick->x_value +
                               SCREEN_KI_X_AXIS;
  board->board_pointer->posy = SCREEN_KP_Y_AXIS * drivers->joystick->y_value +
                               SCREEN_KI_Y_AXIS;
  if (board->board_pointer->posx < POINTER_RADIUS)
    board->board_pointer->posx = POINTER_RADIUS;
  if (board->board_pointer->posx > SSD1306_WIDTH - POINTER_RADIUS)
    board->board_pointer->posx = SSD1306_WIDTH - POINTER_RADIUS;
  if (board->board_pointer->posy < POINTER_RADIUS)
    board->board_pointer->posy = POINTER_RADIUS;
  if (board->board_pointer->posy > SSD1306_HEIGHT - POINTER_RADIUS)
    board->board_pointer->posy = SSD1306_HEIGHT - POINTER_RADIUS;
  draw_pointer(board);
  layer *drawing_area = get_layer_by_name(board->board_layout,
      DRAWING_AREA_LAYER);
  if (drivers->joystick->button_pressed) {
    point p = create_point(board->board_pointer->posx,
        board->board_pointer->posy);
    layer_add_point(drawing_area, p);
    board->points[board->points_counter] = p;
    board->points_counter++;
  }
}

void render_board(drawing_board *board) {
  layer *toolbar = get_layer_by_name(board->board_layout, TOOLBAR_LAYER);
  layer *text_areas = get_layer_by_name(board->board_layout, TEXT_AREAS_LAYER);
  layer *pointer = get_layer_by_name(board->board_layout, POINTER_LAYER);
  layer *drawing_area = get_layer_by_name(board->board_layout,
      DRAWING_AREA_LAYER);
  layer_remove_bitmap_definitions(toolbar);
  layer_remove_text_areas(text_areas);
  // layout_flush_lines(board->board_layout);
  // layout_clear(board->board_layout);
  layer_remove_lines(pointer);
  clear_line(board->board_pointer->hl);
  clear_line(board->board_pointer->vl);
  if (!board->select_mode) {
    draw_loop(board);
  } else {
    for (size_t i = 0; i < 10; i++) {
      layer_add_bitmap_definition(toolbar, *(board->tools[i]->bitmap_def));
    }
    layer_add_text_area(text_areas, *board->tool_name);
    layer_add_text_area(text_areas, *board->select_or_draw);
  }
  layout_draw_all_layers(board->board_layout);
  ssd1306_show(drivers->oled_screen);
}

void board_run(drawing_board *board) {
  ssd1306_clear(drivers->oled_screen);
  render_board(board);
  while (!board->should_quit) {
    if (board->select_mode) {
      select_tool(board);
    }
    render_board(board);
  }
  layout_free(board->board_layout);
}