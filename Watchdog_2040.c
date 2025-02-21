#include <stdio.h>
#include "pico/stdlib.h"
#include "string_list.h"
#include "tests.h"

int main()
{
  stdio_init_all();
  test_all_hardware();
  return 0;
}
