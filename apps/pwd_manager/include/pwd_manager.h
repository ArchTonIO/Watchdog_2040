// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef PWD_MANAGER_H
#define PWD_MANAGER_H

#include "apps/pwd_manager/include/key.h"
#define MAX_PASSWORD_ENTRIES 50
#define MAX_USERNAME_LENGTH 32
#define MAX_PASSWORD_LENGTH 32

#include <stdbool.h>
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

bool init_encrypt_fields(bool cli);
void register_new_service_cli();
void edit_service_credentials_cli(const char *service_name);
void update_service_credentials_cli(const char *service_name);
void get_all_services_cli();
void delete_service_credentials(char *service_name);

void password_manager_launch();

#endif