// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef SHA_256_H
#define SHA_256_H

#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint8_t data[64];
  uint32_t datalen;
  uint64_t bitlen;
  uint32_t state[8];
} SHA256_CTX;

static void sha256_init(SHA256_CTX *ctx);
static void sha256_update(SHA256_CTX *ctx, const uint8_t data[], size_t len);
static void sha256_final(SHA256_CTX *ctx, uint8_t hash[]);

char *get_hash(const char *password);

#endif