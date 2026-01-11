#include "apps/pwd_manager/key.h"

void derive_key(uint8_t key[KEY_SIZE],
    const uint8_t *password,
    size_t pass_len,
    const uint8_t *salt,
    size_t salt_len) {
  uint8_t buf[16] = {0};

  for (size_t i = 0; i < pass_len; i++)
    buf[i % 16] ^= password[i];

  for (size_t i = 0; i < salt_len; i++)
    buf[i % 16] ^= salt[i];

  for (int round = 0; round < 100000; round++) {
    for (int i = 0; i < 16; i++) {
      buf[i] ^= (buf[(i + 1) % 16] + round);
      buf[i] = (buf[i] << 1) | (buf[i] >> 7);
    }
  }

  for (int i = 0; i < 16; i++)
    key[i] = buf[i];
}
