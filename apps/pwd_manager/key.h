#ifndef KEY_H
#define KEY_H

#include <stdint.h>
#include <stdlib.h>

#define KEY_SIZE 16

void derive_key(uint8_t key[KEY_SIZE],
    const uint8_t *password,
    size_t pass_len,
    const uint8_t *salt,
    size_t salt_len);

#endif