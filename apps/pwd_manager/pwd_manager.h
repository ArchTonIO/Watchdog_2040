#ifndef PWD_MANAGER_H
#define PWD_MANAGER_H

#include "apps/pwd_manager/key.h"
#define MAX_PASSWORD_ENTRIES 50
#define MAX_USERNAME_LENGTH 32
#define MAX_PASSWORD_LENGTH 32

#include <stdint.h>

typedef struct {
  char *service_name;
  char *username;
  char *encrypted_password;
  uint8_t nonce[KEY_SIZE]; // nonce size and key size are equal
} service_entry;

typedef struct {
  char *master_password;
  uint8_t *salt;
  uint8_t key[KEY_SIZE];
  uint8_t nonce[KEY_SIZE];
} encrypt_fields;

void password_manager_launch();
#endif