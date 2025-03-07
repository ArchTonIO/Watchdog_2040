#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware_drivers/tests.h"
#include "pico/rand.h"
#include "ulcp/ulcp.h"
#include "hardware_drivers/sdcard.h"
#include <stdlib.h>
#include <string.h>
#include "utils.h"

bool msg_arrived;
sdcard *sd;

void eventually_save_received_msg()
{
  if (!msg_arrived)
    return;
  char addr[6];
  sprintf(addr, "%u", this_lora->rx->must_send_ack_dest);
  char *to_write = string_add("New message received from: ", addr);
  to_write = string_add(to_write, "\n");
  to_write = string_add(to_write, this_lora->rx->recv_payloads_buf);
  sdcard_write_file(sd, "messages.txt", to_write, 'a');
  msg_arrived = false;
}

void notify(uint16_t src_address)
{
  printf("New message from %d: %s\n", src_address);
  msg_arrived = true;
}

void transmitter_demo()
{
  lora_init(0, notify);
  while (1)
  {
    wait_for_user_input();
    char *msg = "On the other hand, we denounce with righteous indignation and dislike men who are so beguiled and demoralized by the charms of pleasure of the moment, so blinded by desire, that they cannot foresee the pain and trouble that are bound to ensue; and equal blame belongs to those who fail in their duty through weakness of will, which is the same as saying through shrinking from toil and pain. These cases are perfectly simple and easy to distinguish. In a free hour, when our power of choice is untrammelled and when nothing prevents our being able to do what we like best, every pleasure is to be welcomed and every pain avoided. But in certain circumstances and owing to the claims of duty or the obligations of business it will frequently occur that pleasures have to be repudiated and annoyances accepted. The wise man therefore always holds in these matters to this principle of selection: he rejects pleasures to secure other greater pleasures, or else he endures pains to avoid worse pains.";
    printf("%d\n", lora_send_msg(1, msg));
  }
}

void receiver_demo()
{
  sd = sdcard_init();
  sdcard_mount(sd);
  lora_init(1, notify);
  msg_arrived = false;
  wait_for_user_input();
  lora_receive();
  while (1)
  {
    sleep_ms(10);
    lora_eventually_send_ack();
    eventually_save_received_msg();
  }
}

int main()
{
  stdio_init_all();
  // transmitter_demo();
  receiver_demo();
  return 0;
}