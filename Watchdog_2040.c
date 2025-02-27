#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware_drivers/tests.h"
#include "pico/rand.h"
#include "ulcp/ulcp.h"
#include <stdlib.h>
#include <string.h>
int main()
{
  stdio_init_all();
  wait_for_user_input();
  lora_init(0);
  for (int i = 0; i < 10; i++)
  {
    lora_send_msg(1, "On the other hand, we denounce with righteous indignation and dislike men who are so beguiled and demoralized by the charms of pleasure of the moment, so blinded by desire, that they cannot foresee the pain and trouble that are bound to ensue; and equal blame belongs to those who fail in their duty through weakness of will, which is the same as saying through shrinking from toil and pain. These cases are perfectly simple and easy to distinguish. In a free hour, when our power of choice is untrammelled and when nothing prevents our being able to do what we like best, every pleasure is to be welcomed and every pain avoided. But in certain circumstances and owing to the claims of duty or the obligations of business it will frequently occur that pleasures have to be repudiated and annoyances accepted. The wise man therefore always holds in these matters to this principle of selection: he rejects pleasures to secure other greater pleasures, or else he endures pains to avoid worse pains.");
    sleep_ms(10000);
  }
}
