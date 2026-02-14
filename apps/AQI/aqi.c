// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "apps/AQI/aqi.h"

#include "core/components/hw_manager.h"
#include "core/graphics/graphs.h"
#include "core/hardware_drivers/joystick.h"
#include "core/hardware_drivers/ssd1306.h"

void display_air_quality_indexes() {
  graph *g_aqi = graph_init("AQI", 48, 40, 0, 16, 0, 5);
  graph *g_co2 = graph_init("eCO2", 48, 40, 43, 16, 400, 2000);
  graph *g_tvoc = graph_init("TVOC", 48, 40, 86, 16, 0, 600);
  while (joystick_get_direction(&(drivers->joystick)) != W) {
    joystick_update(&(drivers->joystick));
    graph_push_value(g_aqi, ens160_read_aqi(&(drivers->ens160)));
    graph_push_value(g_co2, ens160_read_co2(&(drivers->ens160)));
    graph_push_value(g_tvoc, ens160_read_tvoc(&(drivers->ens160)));
    ssd1306_clear(&(drivers->ssd1306));
    graph_update(g_aqi);
    graph_update(g_co2);
    graph_update(g_tvoc);
    ssd1306_show(&(drivers->ssd1306));
    sleep_ms(500);
  }
  graph_free(g_aqi);
  graph_free(g_co2);
  graph_free(g_tvoc);
}
