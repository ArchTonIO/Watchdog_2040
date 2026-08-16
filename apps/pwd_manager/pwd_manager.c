// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "apps/pwd_manager/include/pwd_manager.h"

#include <pico/rand.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/reent.h>

#include "pico/unique_id.h"

#include "apps/pwd_manager/include/aes_ctr.h"
#include "apps/pwd_manager/include/key.h"
#include "apps/text_editor/include/text_editor.h"
#include "core/components/include/sys_paths_manager.h"
#include "core/tools/include/crud_list.h"
#include "core/utils/include/path.h"
#include "core/utils/include/utils.h"

encrypt_fields encrypt;

void register_new_service(crud_list *list);
void edit_service_credentials(crud_list *list, const char *service_name);
int hex_string_to_bytes(const char *str, uint8_t *out, size_t max_len);
void encrypt_and_save_credentials(crud_list *list,
    const char *service_name,
    char *username,
    char *password);
void encrypt_and_save_credentials_cli(path *service_path,
    char *username,
    char *password);

void password_manager_launch() {
  crud_list list;
  list.name = "Password manager";
  list.items_category_name = "service";
  list.workdir = sys_paths->dirs->pwd_manager_path;
  list.create_as_dir = false;
  list.create_callback = register_new_service;
  list.read_update_callback = edit_service_credentials;
  list.delete_callback = delete_item_basic;
  list.flag_callback = NULL;
  init_encrypt_fields(false);
  launch_crud_list(&list);
}

bool init_encrypt_fields(bool cli) {
  char pwd_buf[30];
  if (cli) {
    char pwd_cli_buf[64];
    printf("Master password: ");
    fgets(pwd_cli_buf, sizeof(pwd_cli_buf), stdin);
    char *pwd_buf_nlfd = string_remove_linefeed(pwd_cli_buf);
    strcpy(pwd_buf, pwd_buf_nlfd);
    free(pwd_buf_nlfd);
    if (!is_password_ok(pwd_buf)) {
      printf("Wrong password !\n");
      return false;
    }
  } else if (!request_and_get_password(pwd_buf)) {
    return false;
  }
  encrypt.master_password = pwd_buf;
  uint8_t *master_password_as_uint = (uint8_t *)encrypt.master_password;
  pico_unique_board_id_t board_id;
  pico_get_unique_board_id(&board_id);
  encrypt.salt = (uint8_t *)&board_id;
  derive_key(encrypt.key,
      master_password_as_uint,
      strlen(encrypt.master_password),
      encrypt.salt,
      8);
  return true;
}

void edit_service_credentials_cli(const char *service_name) {
  path *temp_fpath = path_init(service_name);
  path *fpath = path_concat(sys_paths->dirs->pwd_manager_path, temp_fpath);
  str_list *file_content = path_fread(fpath);
  str_list *parts = string_split(str_list_get(file_content, 0), '|');

  char *username = str_list_get(parts, 0);
  char *encrypted_password = str_list_get(parts, 1);
  char *nonce = str_list_get(parts, 2);
  uint8_t encrypted_password_as_uint[MAX_PASSWORD_LENGTH];
  uint8_t nonce_as_uint[KEY_SIZE];

  size_t password_len = (strlen(encrypted_password) + 1) / 3;

  hex_string_to_bytes(encrypted_password,
      encrypted_password_as_uint,
      password_len);
  hex_string_to_bytes(nonce, nonce_as_uint, KEY_SIZE);

  uint8_t round_keys[176];
  aes128_key_expand(encrypt.key, round_keys);
  aes128_ctr_crypt(encrypted_password_as_uint,
      password_len,
      round_keys,
      nonce_as_uint);

  encrypted_password_as_uint[password_len] = '\0';

  size_t text_len = (strlen(username) + password_len +
                     strlen("Username: Password: ") + 4);
  char text[text_len];
  snprintf(text,
      text_len,
      "Username: %s\n\nPassword: %s",
      username,
      (char *)encrypted_password_as_uint);
  printf("%s", text);
  memset(encrypted_password_as_uint, 0, password_len);
  path_free(temp_fpath);
  path_free(fpath);
  str_list_free(file_content);
  str_list_free(parts);

  return;
}

void update_service_credentials_cli(const char *service_name) {
  path *temp_fpath = path_init(service_name);
  path *fpath = path_concat(sys_paths->dirs->pwd_manager_path, temp_fpath);
  if (!path_exists(fpath)) {
    printf("Service not found: %s\n", service_name);
    path_free(temp_fpath);
    path_free(fpath);
    return;
  }

  char buf[64];
  printf("New username for %s:", service_name);
  fgets(buf, sizeof(buf), stdin);
  char *username = strdup(buf);
  char *username_nlfd = string_remove_linefeed(username);

  printf("New password for %s:", service_name);
  fgets(buf, sizeof(buf), stdin);
  char *password = strdup(buf);
  char *password_nlfd = string_remove_linefeed(password);

  if (strlen(username_nlfd) > MAX_USERNAME_LENGTH) {
    printf("Username too long! Max length is %d chars\n", MAX_USERNAME_LENGTH);
    free(username);
    free(password);
    free(username_nlfd);
    free(password_nlfd);
    path_free(temp_fpath);
    path_free(fpath);
    return;
  }

  if (strlen(password_nlfd) > MAX_PASSWORD_LENGTH) {
    printf("Password too long! Max length is %d chars\n", MAX_PASSWORD_LENGTH);
    free(username);
    free(password);
    free(username_nlfd);
    free(password_nlfd);
    path_free(temp_fpath);
    path_free(fpath);
    return;
  }

  if (strstr(password_nlfd, " ")) {
    printf("Blank spaces not allowed!\n");
    free(username);
    free(password);
    free(username_nlfd);
    free(password_nlfd);
    path_free(temp_fpath);
    path_free(fpath);
    return;
  }

  encrypt_and_save_credentials_cli(fpath, username_nlfd, password_nlfd);
  printf("Credentials updated for %s\n", service_name);

  free(username);
  free(password);
  free(username_nlfd);
  free(password_nlfd);
  path_free(temp_fpath);
  path_free(fpath);
}

void edit_service_credentials(crud_list *list, const char *service_name) {
  path *service_path = get_item_path(list, service_name);
  str_list *file_content = path_fread(service_path);
  str_list *parts = string_split(str_list_get(file_content, 0), '|');

  char *username = str_list_get(parts, 0);
  char *encrypted_password = str_list_get(parts, 1);
  char *nonce = str_list_get(parts, 2);
  uint8_t encrypted_password_as_uint[MAX_PASSWORD_LENGTH];
  uint8_t nonce_as_uint[KEY_SIZE];

  size_t password_len = (strlen(encrypted_password) + 1) / 3;

  hex_string_to_bytes(encrypted_password,
      encrypted_password_as_uint,
      password_len);
  hex_string_to_bytes(nonce, nonce_as_uint, KEY_SIZE);

  uint8_t round_keys[176];
  aes128_key_expand(encrypt.key, round_keys);
  aes128_ctr_crypt(encrypted_password_as_uint,
      password_len,
      round_keys,
      nonce_as_uint);

  encrypted_password_as_uint[password_len] = '\0';

  size_t text_len = (strlen(username) + password_len +
                     strlen("Username: Password: ") + 4);
  char text[text_len];
  snprintf(text,
      text_len,
      "Username: %s\n\nPassword: %s",
      username,
      (char *)encrypted_password_as_uint);
  text_editor *credentials_editor = text_editor_launch(text, false);
  char *buf = text_editor_get_buf(credentials_editor);

  str_list *editor_buf = string_split(buf, '\n');
  str_list *username_buf = string_split(str_list_get(editor_buf, 0), ' ');
  str_list *password_buf = string_split(str_list_get(editor_buf, 1), ' ');

  char *new_username = str_list_get(username_buf, 1);
  char *new_password = str_list_get(password_buf, 1);

  if (strcmp(new_username, username) != 0 ||
      strcmp(new_password, (char *)encrypted_password_as_uint) != 0)
    encrypt_and_save_credentials(list,
        service_name,
        new_username,
        new_password);

  str_list_free(editor_buf);
  str_list_free(username_buf);
  str_list_free(password_buf);

  memset(encrypted_password_as_uint, 0, password_len);
  path_free(service_path);
  str_list_free(file_content);
  str_list_free(parts);
  text_editor_kill(credentials_editor);
  free(buf);
  return;
}

void delete_service_credentials(char *service_name) {
  path *service_path = path_init(service_name);
  path *full_service_path = path_concat(sys_paths->dirs->pwd_manager_path,
      service_path);
  path_fdelete(full_service_path);
  path_free(service_path);
  path_free(full_service_path);
  printf("Successfully deleted service %s credentials", service_name);
}

void populate_nonce(encrypt_fields *encrypt) {
  for (int i = 0; i < KEY_SIZE; i++)
    encrypt->nonce[i] = get_rand_32() % 256;
}

static const char hex[] = "0123456789ABCDEF";

void bytes_to_hex_string(const uint8_t *data, size_t len, char *out) {
  for (size_t i = 0; i < len; i++) {
    out[i * 3 + 0] = hex[data[i] >> 4];
    out[i * 3 + 1] = hex[data[i] & 0x0F];
    out[i * 3 + 2] = ' ';
  }
  out[len * 3 - 1] = '\0';
}

static uint8_t hex_to_nibble(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  return 0xFF;
}

int hex_string_to_bytes(const char *str, uint8_t *out, size_t max_len) {
  size_t count = 0;

  while (*str && count < max_len) {
    if (*str == ' ') {
      str++;
      continue;
    }

    uint8_t hi = hex_to_nibble(*str++);
    uint8_t lo = hex_to_nibble(*str++);

    if (hi == 0xFF || lo == 0xFF)
      return -1;

    out[count++] = (hi << 4) | lo;
  }

  return count;
}

void register_new_service_cli() {
  char buf[64];

  printf("Service name: ");
  fgets(buf, sizeof(buf), stdin);
  char *service_name = strdup(buf);
  char *service_name_nlfd = string_remove_linefeed(service_name);
  path *temp_fpath = path_init(service_name_nlfd);
  path *fpath = path_concat(sys_paths->dirs->pwd_manager_path, temp_fpath);
  if (path_exists(fpath)) {
    printf("A service with this name already exists!");
    path_free(temp_fpath);
    path_free(fpath);
    free(service_name);
    free(service_name_nlfd);
    return;
  }

  printf("Username for %s:", service_name_nlfd);
  fgets(buf, sizeof(buf), stdin);
  char *username = strdup(buf);
  char *username_nlfd = string_remove_linefeed(username);

  printf("Password for %s:", service_name_nlfd);
  fgets(buf, sizeof(buf), stdin);
  char *password = strdup(buf);
  char *password_nlfd = string_remove_linefeed(password);
  if (strstr(password_nlfd, " ")) {
    printf("Blank spaces not allowed!");
    free(service_name);
    free(username);
    free(password);
    free(service_name_nlfd);
    free(username_nlfd);
    free(password_nlfd);
    path_free(temp_fpath);
    path_free(fpath);
    return;
  }

  encrypt_and_save_credentials_cli(fpath, username_nlfd, password_nlfd);
  printf("Username and ecnrypted password saved for %s\n", service_name_nlfd);
  free(service_name);
  free(username);
  free(password);
  free(service_name_nlfd);
  free(username_nlfd);
  free(password_nlfd);
  path_free(temp_fpath);
  path_free(fpath);
}

void get_all_services_cli() {
  str_list *services = path_listdir(sys_paths->dirs->pwd_manager_path);
  for (size_t i = 0; i < services->len; i++)
    printf("Service: %s\n", str_list_get(services, i));
}

void register_new_service(crud_list *list) {

  text_editor *service_name_editor = text_editor_launch(
      "# Insert the service name",
      true);
  char *service_name = text_editor_get_buf(service_name_editor);
  text_editor_kill(service_name_editor);

  if (item_exists(list, service_name)) {
    free(service_name);
    print_usr_error("A service with this name\nalready exists!");
    return;
  }

  text_editor *username_editor = text_editor_launch(
      "# Insert the username for this service",
      true);
  char *username = text_editor_get_buf(username_editor);
  text_editor_kill(username_editor);

  if (strlen(username) > MAX_USERNAME_LENGTH) {
    free(service_name);
    free(username);
    print_usr_error("Username too long!\nMax length is 32 char");
    return;
  }

  text_editor *password_editor = text_editor_launch(
      "# Insert the password for this service",
      true);
  char *password = text_editor_get_buf(password_editor);
  text_editor_kill(password_editor);

  if (strlen(password) > MAX_PASSWORD_LENGTH) {
    free(service_name);
    free(username);
    free(password);
    print_usr_error("Password too long!\nMax length is 32 char");
    return;
  }

  if (strstr(password, " ")) {
    free(service_name);
    free(username);
    free(password);
    print_usr_error("Blank spaces \nnot allowed !");
    return;
  }

  encrypt_and_save_credentials(list, service_name, username, password);
  free(service_name);
  free(username);
  free(password);
}

void encrypt_and_save_credentials_cli(path *service_path,
    char *username,
    char *password) {
  populate_nonce(&encrypt);
  uint8_t round_keys[176];
  aes128_key_expand(encrypt.key, round_keys);
  aes128_ctr_crypt((uint8_t *)password,
      strlen(password),
      round_keys,
      encrypt.nonce);

  char chipher_text[MAX_PASSWORD_LENGTH * 3 + 1];
  bytes_to_hex_string((uint8_t *)password, strlen(password), chipher_text);

  char nonce_text[KEY_SIZE * 3 + 1];
  bytes_to_hex_string(encrypt.nonce, sizeof(encrypt.nonce), nonce_text);

  size_t buf_len = strlen(username) + strlen(chipher_text) +
                   strlen(nonce_text) + 3;
  char buf[buf_len];
  snprintf(buf, buf_len, "%s|%s|%s", username, chipher_text, nonce_text);
  path_fwrite(service_path, buf, 'w');
}

void encrypt_and_save_credentials(crud_list *list,
    const char *service_name,
    char *username,
    char *password) {
  print_loading("Encrypting and saving\ncredentials...");
  populate_nonce(&encrypt);
  uint8_t round_keys[176];
  aes128_key_expand(encrypt.key, round_keys);
  aes128_ctr_crypt((uint8_t *)password,
      strlen(password),
      round_keys,
      encrypt.nonce);

  char chipher_text[MAX_PASSWORD_LENGTH * 3 + 1];
  bytes_to_hex_string((uint8_t *)password, strlen(password), chipher_text);

  char nonce_text[KEY_SIZE * 3 + 1];
  bytes_to_hex_string(encrypt.nonce, sizeof(encrypt.nonce), nonce_text);

  size_t buf_len = strlen(username) + strlen(chipher_text) +
                   strlen(nonce_text) + 3;
  char buf[buf_len];
  snprintf(buf, buf_len, "%s|%s|%s", username, chipher_text, nonce_text);

  create_or_overwrite_item(list, service_name, buf);
}
