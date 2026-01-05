#include <stdio.h>
#include <string.h>

#include "pico/unique_id.h"

#include "apps/pwd_manager/aes_ctr.h"
#include "apps/pwd_manager/key.h"

uint8_t key[16] = {};
uint8_t nonce[16] = {0x12,
    0x34,
    0x56,
    0x78,
    0x9A,
    0xBC,
    0xDE,
    0xF0, // device ID
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x01};

void test_encrypt() {
  uint8_t *master_password = (uint8_t *)"masterpassword";
  uint8_t secret[] = "my_password_123";
  pico_unique_board_id_t salt;
  pico_get_unique_board_id(&salt);
  uint8_t *salt_as_uint = (uint8_t *)&salt;
  derive_key(key,
      master_password,
      sizeof("masterpassword") - 1,
      salt_as_uint,
      8);
  aes128_ctr_crypt(secret, sizeof(secret) - 1, key, nonce);
  aes128_ctr_crypt(secret, sizeof(secret) - 1, key, nonce);
}