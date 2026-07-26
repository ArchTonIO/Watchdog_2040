
#include "core/graphics/include/layout.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/components/include/hw_manager.h"
#include "core/graphics/include/graphic_primitives.h"
#include "core/hardware_drivers/include/ssd1306.h"

layer *get_layer_by_name(layout *lo, char *layer_name) {
  for (size_t i = 0; i < lo->layers_count; i++) {
    if (strcmp(lo->layers[i]->name, layer_name) == 0)
      return lo->layers[i];
  }
  return NULL;
}

/**
 * @brief Creates a layer structure.
 * @return A pointer to the initialized layer structure.
 */
static layer *create_layer(char *layer_name) {
  layer *ly = (layer *)malloc(sizeof(layer));
  ly->name = layer_name;
  ly->visible = true;
  ly->points_count = 0;
  ly->lines_count = 0;
  ly->circles_count = 0;
  ly->rectangles_count = 0;
  ly->polylines_count = 0;
  ly->bitmap_defs_count = 0;
  ly->text_areas_count = 0;
  ly->buttons_count = 0;
  ly->toggle_buttons_count = 0;
  return ly;
}

/**
 * @brief Initializes a layout structure.
 * @param lo A newly created layout structure.
 * @return A pointer to the initialized layout structure.
 */
layout *layout_init() {
  layout *lo = (layout *)malloc(sizeof(layout));
  lo->layers_count = 0;
  return lo;
}

/**
 * @brief Frees the memory allocated for a layout.
 * @param ly The layout to free.
 */
void layout_free(layout *lo) {
  for (size_t i = 0; i < lo->layers_count; i++) {
    free(lo->layers[i]);
  }
  free(lo);
}

/**
 * @brief Add a new layer to a layout.
 * @param lo The layout to add the new layer to.
 * @param layer_name The name of the new layer.
 */
void layout_add_layer(layout *lo, char *layer_name) {
  if (get_layer_by_name(lo, layer_name) != NULL)
    return;
  lo->layers[lo->layers_count] = create_layer(layer_name);
  lo->layers_count++;
}

/**
 * @brief Writes all of the layer geometries to screen memory
 * @param The layout in which the layer is found.
 * @param layer_name The name of the layer you want to draw.
 */
inline void layout_draw_layer(layout *lo, char *layer_name) {
  layer *ly = get_layer_by_name(lo, layer_name);
  if (ly->visible)
    layer_draw_all(ly);
}

void layout_draw_all_layers(layout *lo) {
  for (size_t i = 0; i < lo->layers_count; i++)
    layout_draw_layer(lo, lo->layers[i]->name);
}

/**
  * @brief Removes from screen memory every entity (geometry) associated to
  that layer.
  * @param lo The layout in which the layer is found.
  * @param layer_name The name of the layer.
 */
void layout_remove_layer_entities(layout *lo, char *layer_name) {
  layer *ly = get_layer_by_name(lo, layer_name);
  layer_remove_all(ly);
}

/**
  * @brief Removes from screen memory every entity (geometry) associated to
  every layer.
  * @param lo The layout in which the layers are found.
 */
void layout_remove_all_layers_entities(layout *lo) {
  for (size_t i = 0; i < lo->layers_count; i++) {
    layer_remove_all(lo->layers[i]);
  }
}

/**
 * @brief Removes all of the layer geometries from screen memory
 * @param The layout in which the layer is found.
 * @param layer_name the name of the layer you want to clear.
 */
inline void layout_clear_layer(layout *lo, char *layer_name) {
  layer_clear_all(get_layer_by_name(lo, layer_name));
}

/**
 * @brief Changes the layer visibility
 * @param lo The layout in which the layer is found.
 * @param layer_name The name of the layer you want to change visibility to.
 * @param visible Either visible (true) or not (false).
 */
inline void
layout_set_layer_visibility(layout *lo, char *layer_name, bool visible) {
  get_layer_by_name(lo, layer_name)->visible = visible;
}

/**
 * @brief Adds a point to the layer.
 * @param ly The layer to which the point will be added.
 * @param ln The point to add.
 */
void layer_add_point(layer *ly, point p) {
  ly->points[ly->points_count] = p;
  ly->points_count++;
}

/**
 * @brief Adds a line to the layout.
 * @param ly The layout to which the line will be added.
 * @param ln The line to add.
 */
void layer_add_line(layer *ly, line ln) {
  ly->lines[ly->lines_count] = ln;
  ly->lines_count++;
}

/**
 * @brief Adds a circle to the layer.
 * @param ly The layer to which the circle will be added.
 * @param c The circle to add.
 */
void layer_add_circle(layer *ly, circle c) {
  ly->circles[ly->circles_count] = c;
  ly->circles_count++;
}

/**
 * @brief Adds a rectangle to the layer.
 * @param ly The layer to which the rectangle will be added.
 * @param r The rectangle to add.
 */
void layer_add_rectangle(layer *ly, rectangle r) {
  ly->rectangles[ly->rectangles_count] = r;
  ly->rectangles_count++;
}

/**
 * @brief Adds a polyline to the layer.
 * @param ly The layer to which the polyline will be added.
 * @param pl The polyline to add.
 */
void layer_add_polyline(layer *ly, polyline pl) {
  ly->polylines[ly->polylines_count] = pl;
  ly->polylines_count++;
}

/**
 * @brief Adds a bitmap definition to the layer.
 * @param ly The layer to which the bitmap definition will be added.
 * @param bd The bitmap definition to add.
 */
void layer_add_bitmap_definition(layer *ly, bitmap_definition bd) {
  ly->bitmap_defs[ly->bitmap_defs_count] = bd;
  ly->bitmap_defs_count++;
}

/**
 * @brief Adds a text area to the layer.
 * @param ly The layer to which the text area will be added.
 * @param ta The text area to add.
 */
void layer_add_text_area(layer *ly, text_area ta) {
  ly->text_areas[ly->text_areas_count] = ta;
  ly->text_areas_count++;
}

void layer_add_button(layer *ly, button_t btn) {
  ly->buttons[ly->buttons_count] = btn;
  ly->buttons_count++;
}

void layer_add_toggle_button(layer *ly, toggle_button_t btn) {
  ly->toggle_buttons[ly->toggle_buttons_count] = btn;
  ly->toggle_buttons_count++;
}

/**
 * @brief Draws all elements in the layer to the OLED screen memory.
 * @param ly The layer to draw.
 */
void layer_draw_all(layer *ly) {
  if (!ly->visible)
    return;
  for (uint8_t i = 0; i < ly->points_count; i++) {
    draw_point(ly->points[i]);
  }
  for (uint8_t i = 0; i < ly->lines_count; i++) {
    draw_line(ly->lines[i]);
  }
  for (uint8_t i = 0; i < ly->circles_count; i++) {
    draw_circle(ly->circles[i]);
  }
  for (uint8_t i = 0; i < ly->rectangles_count; i++) {
    draw_rectangle(ly->rectangles[i]);
  }
  for (uint8_t i = 0; i < ly->polylines_count; i++) {
    draw_polyline(ly->polylines[i]);
  }
  for (uint8_t i = 0; i < ly->bitmap_defs_count; i++) {
    ssd1306_draw_bitmap(&(drivers->ssd1306),
        ly->bitmap_defs[i].posx,
        ly->bitmap_defs[i].posy,
        ly->bitmap_defs[i].bitmap,
        ly->bitmap_defs[i].width,
        ly->bitmap_defs[i].height,
        ly->bitmap_defs[i].is_inverted);
  }
  for (uint8_t i = 0; i < ly->text_areas_count; i++) {
    ssd1306_print(&(drivers->ssd1306),
        ly->text_areas[i].text,
        ly->text_areas[i].posx,
        ly->text_areas[i].posy,
        ly->text_areas[i].is_inverted);
  }
  for (uint8_t i = 0; i < ly->buttons_count; i++) {
    ssd1306_draw_bitmap(&(drivers->ssd1306),
        ly->buttons[i].bd.posx,
        ly->buttons[i].bd.posy,
        ly->buttons[i].bd.bitmap,
        ly->buttons[i].bd.width,
        ly->buttons[i].bd.height,
        ly->buttons[i].selected);
  }
  for (uint8_t i = 0; i < ly->toggle_buttons_count; i++) {
    if (!ly->toggle_buttons[i].state)
      ssd1306_draw_bitmap(&(drivers->ssd1306),
          ly->toggle_buttons[i].bd_state_1.posx,
          ly->toggle_buttons[i].bd_state_1.posy,
          ly->toggle_buttons[i].bd_state_1.bitmap,
          ly->toggle_buttons[i].bd_state_1.width,
          ly->toggle_buttons[i].bd_state_1.height,
          ly->toggle_buttons[i].selected);
    else
      ssd1306_draw_bitmap(&(drivers->ssd1306),
          ly->toggle_buttons[i].bd_state_2.posx,
          ly->toggle_buttons[i].bd_state_2.posy,
          ly->toggle_buttons[i].bd_state_2.bitmap,
          ly->toggle_buttons[i].bd_state_2.width,
          ly->toggle_buttons[i].bd_state_2.height,
          ly->toggle_buttons[i].selected);
  }
}

/**
 * @brief Clears all elements in the layer from the OLED screen memory.
 * @param ly The layer to clear.
 */
void layer_clear_all(layer *ly) {
  for (uint8_t i = 0; i < ly->points_count; i++) {
    clear_point(ly->points[i]);
  }
  for (uint8_t i = 0; i < ly->lines_count; i++) {
    clear_line(ly->lines[i]);
  }
  for (uint8_t i = 0; i < ly->circles_count; i++) {
    clear_circle(ly->circles[i]);
  }
  for (uint8_t i = 0; i < ly->rectangles_count; i++) {
    clear_rectangle(ly->rectangles[i]);
  }
  for (uint8_t i = 0; i < ly->polylines_count; i++) {
    clear_polyline(ly->polylines[i]);
  }
}

void layer_clear_bitmap_definitions(layer *ly) {
  for (uint8_t i = 0; i < ly->bitmap_defs_count; i++) {
    uint8_t min_x = ly->bitmap_defs[i].posx;
    uint8_t min_y = ly->bitmap_defs[i].posy;
    uint8_t max_x = ly->bitmap_defs[i].posx + ly->bitmap_defs[i].width - 1;
    uint8_t max_y = ly->bitmap_defs[i].posy + ly->bitmap_defs[i].height - 1;
    for (uint8_t x = min_x; x <= max_x; x++)
      for (uint8_t y = min_y; y <= max_y; y++)
        ssd1306_draw_pixel(&(drivers->ssd1306), x, y, 0);
  }
}

/**
 * @brief Deletes all the points in the layer.
 * @param ly The layer to delete the points from.
 */
void layer_remove_points(layer *ly) { ly->points_count = 0; }

/**
 * @brief Deletes all the lines in the layer.
 * @param ly The layer to delete the lines from.
 */
void layer_remove_lines(layer *ly) { ly->lines_count = 0; }

/**
 * @brief Deletes all the circles in the layer.
 * @param ly The layer to delete the circles from.
 */
void layer_remove_circles(layer *ly) { ly->circles_count = 0; }

/**
 * @brief Deletes all the rectangles in the layer.
 * @param ly The layer to delete the rectangles from.
 */
void layer_remove_rectangles(layer *ly) { ly->rectangles_count = 0; }

/**
 * @brief Deletes all the polylines in the layer.
 * @param ly The layer to delete the polylines from.
 */
void layer_remove_polylines(layer *ly) { ly->polylines_count = 0; }

/**
 * @brief Deletes all the bitmap definitions in the layer.
 * @param ly The layer to delete the bitmap definitions from.
 */
void layer_remove_bitmap_definitions(layer *ly) { ly->bitmap_defs_count = 0; }

/**
 * @brief Deletes all the text areas in the layer.
 * @param ly The layer to delete the text areas from.
 */
void layer_remove_text_areas(layer *ly) { ly->text_areas_count = 0; }

/**
 * @brief Deletes all entities from a layer.
 * @param ly The layer to remove.
 */
void layer_remove_all(layer *ly) {
  ly->points_count = 0;
  ly->lines_count = 0;
  ly->circles_count = 0;
  ly->rectangles_count = 0;
  ly->polylines_count = 0;
  ly->bitmap_defs_count = 0;
  ly->text_areas_count = 0;
}
