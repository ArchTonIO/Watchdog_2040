#ifndef KEY_H
#define KEY_H

#include <stdint.h>
#include <stdlib.h>

void derive_key(uint8_t key[16],
    const uint8_t *password,
    size_t pass_len,
    const uint8_t *salt,
    size_t salt_len);

#endif