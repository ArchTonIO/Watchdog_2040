#ifndef LAYOUT_H
#define LAYOUT_H

#define MAX_ENTITIES_BY_TYPE 100
#define MAX_LAYERS 10

#include <stdbool.h>
#include <stdint.h>

#include "core/graphics/include/graphic_primitives.h"

typedef struct {
  char *name;
  bool visible;
  point points[MAX_ENTITIES_BY_TYPE];
  line lines[MAX_ENTITIES_BY_TYPE];
  circle circles[MAX_ENTITIES_BY_TYPE];
  rectangle rectangles[MAX_ENTITIES_BY_TYPE];
  polyline polylines[MAX_ENTITIES_BY_TYPE];
  bitmap_definition bitmap_defs[MAX_ENTITIES_BY_TYPE];
  text_area text_areas[MAX_ENTITIES_BY_TYPE];
  button_t buttons[MAX_ENTITIES_BY_TYPE];
  toggle_button_t toggle_buttons[MAX_ENTITIES_BY_TYPE];
  uint8_t points_count;
  uint8_t lines_count;
  uint8_t circles_count;
  uint8_t rectangles_count;
  uint8_t polylines_count;
  uint8_t bitmap_defs_count;
  uint8_t text_areas_count;
  uint8_t buttons_count;
  uint8_t toggle_buttons_count;
} layer;

typedef struct {
  layer *layers[MAX_LAYERS];
  uint8_t layers_count;
} layout;

/* layout management */
layout *layout_init();
void layout_free(layout *lo);
void layout_add_layer(layout *lo, char *layer_name);

void layout_draw_layer(layout *lo, char *layer_name);
void layout_draw_all_layers(layout *lo);
void layout_remove_layer_entities(layout *lo, char *layer_name);
void layout_remove_all_layers_entities(layout *lo);

void layout_clear_layer(layout *lo, char *layer_name);
void layout_clear_all_layers(layout *lo, char *layer_name);

void layout_set_layer_visibility(layout *lo, char *layer_name, bool visible);

layer *get_layer_by_name(layout *lo, char *layer_name);

/*
layer_add functions

Whenever called a new geometry by type is added to the layer.
It will be shown on the screen at the next "layer_draw" call.
*/
void layer_add_point(layer *ly, point p);
void layer_add_line(layer *ly, line ln);
void layer_add_circle(layer *ly, circle c);
void layer_add_rectangle(layer *ly, rectangle r);
void layer_add_polyline(layer *ly, polyline p);
void layer_add_bitmap_definition(layer *ly, bitmap_definition bd);
void layer_add_button(layer *ly, button_t btn);
void layer_add_toggle_button(layer *ly, toggle_button_t btn);
void layer_add_text_area(layer *ly, text_area ta);
/*
layer l yer_remove functions

Symmetric to layer_add functions,
whenever called all geometry by the specified type (or all)
will be removed from the layer.
Changes will be shown on the screen at the next "layer_draw" call.
*/
void layer_remove_points(layer *ly);
void layer_remove_lines(layer *ly);
void layer_remove_circles(layer *ly);
void layer_remove_rectangles(layer *ly);
void layer_remove_polylines(layer *ly);
void layer_remove_bitmap_definitions(layer *ly);
void layer_remove_text_areas(layer *ly);
void layer_remove_buttons(layer *ly);
void layer_remove_toggle_buttons(layer *ly);
void layer_remove_all(layer *ly);

/*
layer layer_draw functions

Whenever called all geometry by the specified type (or all)
will be wrote to screen memory.
Be aware that only a final call to ssd1306_show(&(drivers->ssd1306))
will make them actually visible (when to call it is your responsibility,
it is good practice to call it only once in your loop)
*/
void layer_draw_points(layer *ly);
void layer_draw_lines(layer *ly);
void layer_draw_circles(layer *ly);
void layer_draw_rectangles(layer *ly);
void layer_draw_polylines(layer *ly);
void layer_draw_bitmap_definitions(layer *ly);
void layer_draw_text_areas(layer *ly);
void layer_draw_buttons(layer *ly);
void layer_draw_toggle_buttons(layer *ly);
void layer_draw_all(layer *ly);

/*
layer  ayer_clear functions

Symmetric to geometry_draw functions,
whenever called all geometry by the specified type (or all)
will be cleared from the screen (screen memory of the pixels
belonging to the geometry will be zeroed)
Be aware that only a final call to ssd1306_show(&(drivers->ssd1306))
will make them actually go away (when to call it is your responsibility,
it is good practice to call it only once in your loop).
Be aware that this will not remove the geometries from the layer.
*/
void layer_clear_points(layer *ly);
void layer_clear_lines(layer *ly);
void layer_clear_circles(layer *ly);
void layer_clear_rectangles(layer *ly);
void layer_clear_polylines(layer *ly);
void layer_clear_bitmap_definitions(layer *ly);
void layer_clear_text_areas(layer *ly);
void layer_clear_buttons(layer *ly);
void layer_clear_toggle_buttons(layer *ly);
void layer_clear_all(layer *ly);

#endif