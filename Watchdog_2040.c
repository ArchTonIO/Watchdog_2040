#include <stdio.h>
#include "pico/stdlib.h"
#include "string_list.h"
#include "tests.h"
#include "hardware_config.h"
#include "sx1278.h"

void msg_callback(char *msg)
{
  printf("Message received: %s\n", msg);
}

int main()
{
  stdio_init_all();
  // test_all_hardware();
  wait_for_user_input();
  sx1278 *lora_radio = sx1278_init(
      SX1278_MOSI,
      SX1278_MISO,
      SX1278_SCK,
      SX1278_CS,
      SX1278_INTERRUPT,
      0,
      SX1278_SPI_PORT,
      SX1278_SPI_BAUDRATE,
      SX1278_TX_POWER,
      msg_callback);
  sx1278_set_mode_rx(lora_radio);
  while (1)
  {
    sleep_ms(1000);
  }
  return 0;
}
