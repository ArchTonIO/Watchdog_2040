#include "apps/AQI/aqi.h"

#include "core/components/hw_manager.h"
#include "core/graphics/graphs.h"
#include "core/hardware_drivers/joystick.h"
#include "core/hardware_drivers/ssd1306.h"

void display_air_quality_indexes() {
  graph *g_aqi = graph_init("AQI", 48, 40, 0, 16, 0, 5);
  graph *g_co2 = graph_init("eCO2", 48, 40, 43, 16, 400, 2000);
  graph *g_tvoc = graph_init("TVOC", 48, 40, 86, 16, 0, 600);
  while (joystick_get_direction(drivers->joystick) != W) {
    joystick_update(drivers->joystick);
    graph_push_value(g_aqi, ens160_read_aqi(drivers->air_quality_sensor));
    graph_push_value(g_co2, ens160_read_co2(drivers->air_quality_sensor));
    graph_push_value(g_tvoc, ens160_read_tvoc(drivers->air_quality_sensor));
    ssd1306_clear(drivers->oled_screen);
    graph_update(g_aqi);
    graph_update(g_co2);
    graph_update(g_tvoc);
    ssd1306_show(drivers->oled_screen);
    sleep_ms(500);
  }
  graph_free(g_aqi);
  graph_free(g_co2);
  graph_free(g_tvoc);
}
