#ifndef AES_CTR_H
#define AES_CTR_H

#include <stdint.h>

#define AES_BLOCK_SIZE 16
#define AES_KEY_SIZE 16
#define AES_ROUNDS 10

void aes128_ctr_crypt(uint8_t *data,
    uint32_t length,
    const uint8_t round_keys[176],
    uint8_t nonce[16]);
void aes128_key_expand(const uint8_t *key, uint8_t *round_keys);

#endif
