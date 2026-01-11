#include "apps/pwd_manager/pwd_manager.h"

#include <pico/rand.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/reent.h>

#include "pico/unique_id.h"

#include "apps/pwd_manager/aes_ctr.h"
#include "apps/pwd_manager/key.h"
#include "apps/text_editor/text_editor.h"
#include "core/components/sys_paths_manager.h"
#include "core/tools/options_gen.h"
#include "core/utils/path.h"
#include "core/utils/utils.h"

void init_encrypt_fields(encrypt_fields *encrypt);
void show_services_page(encrypt_fields *encrypt);
void register_new_service(encrypt_fields *encrypt);
void edit_or_delete_service(char *service_name, encrypt_fields *encrypt);
int hex_string_to_bytes(const char *str, uint8_t *out, size_t max_len);
void encrypt_and_save_credentials(char *service_name,
    char *username,
    char *password,
    encrypt_fields *encrypt);
void delete_service_credentials(char *service_name);

void password_manager_launch() {
  encrypt_fields encrypt;
  init_encrypt_fields(&encrypt);
}

void init_encrypt_fields(encrypt_fields *encrypt) {
  char pwd_buf[30];
  if (!request_and_get_password(
          "Enter your password to access the password manager",
          pwd_buf)) {
    return;
  }
  encrypt->master_password = pwd_buf;
  uint8_t *master_password_as_uint = (uint8_t *)encrypt->master_password;
  pico_unique_board_id_t board_id;
  pico_get_unique_board_id(&board_id);
  encrypt->salt = (uint8_t *)&board_id;
  derive_key(encrypt->key,
      master_password_as_uint,
      strlen(encrypt->master_password),
      encrypt->salt,
      8);

  show_services_page(encrypt);
}

void show_services_page(encrypt_fields *encrypt) {
  str_list *services = str_list_init();
  str_list *existing_services;
  str_list *options;
  options_page *services_page;
  str_list_append(services, "+ new service");
  while (true) {
    existing_services = path_listdir(sys_paths->dirs->pwd_manager_path);
    options = str_list_extend(services, existing_services);
    services_page = options_page_init("Services", options);
    char *buf = options_page_launch(services_page);
    if (strcmp(buf, "+ new service") == 0)
      register_new_service(encrypt);
    else if (strcmp(buf, "") == 0)
      break;
    else
      edit_or_delete_service(buf, encrypt);
    str_list_free(existing_services);
    options_page_free(services_page);
  }
  str_list_free(services);
  str_list_free(existing_services);
  options_page_free(services_page);
}

void edit_service_credentials(char *service_name, encrypt_fields *encrypt) {
  path *service_path = path_init(service_name);
  path *service_full_path = path_concat(sys_paths->dirs->pwd_manager_path,
      service_path);
  str_list *file_content = path_fread(service_full_path);
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
  aes128_key_expand(encrypt->key, round_keys);
  aes128_ctr_crypt(encrypted_password_as_uint,
      password_len,
      round_keys,
      nonce_as_uint);

  encrypted_password_as_uint[password_len] = '\0';

  size_t text_len = (strlen(username) + password_len +
                     strlen("Username: Password: ") + 3);
  char text[text_len];
  snprintf(text,
      text_len,
      "Username: %s\nPassword: %s",
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
    encrypt_and_save_credentials(service_name,
        new_username,
        new_password,
        encrypt);

  str_list_free(editor_buf);
  str_list_free(username_buf);
  str_list_free(password_buf);

  memset(encrypted_password_as_uint, 0, password_len);
  path_free(service_path);
  path_free(service_full_path);
  str_list_free(file_content);
  str_list_free(parts);
  text_editor_kill(credentials_editor);
  free(buf);
  return;
}

void edit_or_delete_service(char *service_name, encrypt_fields *encrypt) {
  str_list *options = str_list_init();
  str_list_append(options, "view/edit");
  str_list_append(options, "delete");
  options_page *editordelete_page = options_page_init(service_name, options);
  char *buf = options_page_launch(editordelete_page);
  if (strcmp(buf, "view/edit") == 0)
    edit_service_credentials(service_name, encrypt);
  else if (strcmp(buf, "delete") == 0) {
    sleep_ms(200);
    str_list *yesno = str_list_init();
    str_list_append(yesno, "yes");
    str_list_append(yesno, "no");
    options_page *yesno_page = options_page_init("Are you sure?", yesno);
    char *yesno_buf = options_page_launch(yesno_page);
    if (strcmp(yesno_buf, "no") == 0 || strcmp(yesno_buf, "") == 0) {
      options_page_free(yesno_page);
      options_page_free(editordelete_page);
      return;
    }
    options_page_free(yesno_page);
    delete_service_credentials(service_name);
  }
  options_page_free(editordelete_page);
}

void delete_service_credentials(char *service_name) {
  path *service_path = path_init(service_name);
  path *full_service_path = path_concat(sys_paths->dirs->pwd_manager_path,
      service_path);
  path_fdelete(full_service_path);
  path_free(service_path);
  path_free(full_service_path);
}

void populate_nonce(encrypt_fields *encrypt) {
  for (int i = 0; i < KEY_SIZE; i++)
    encrypt->nonce[i] = get_rand_32() % 256;
}

bool service_exists(char *service_name) {
  str_list *existing_services = path_listdir(
      sys_paths->dirs->pwd_manager_path);
  for (size_t i = 0; i < existing_services->len; i++) {
    if (strcmp(str_list_get(existing_services, i), service_name) == 0) {
      str_list_free(existing_services);
      return true;
    }
  }
  str_list_free(existing_services);
  return false;
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

void register_new_service(encrypt_fields *encrypt) {

  text_editor *service_name_editor = text_editor_launch(
      "# Insert the service name",
      true);
  char *service_name = text_editor_get_buf(service_name_editor);
  text_editor_kill(service_name_editor);

  if (service_exists(service_name)) {
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
  encrypt_and_save_credentials(service_name, username, password, encrypt);
  free(service_name);
  free(username);
  free(password);
}

void encrypt_and_save_credentials(char *service_name,
    char *username,
    char *password,
    encrypt_fields *encrypt) {
  print_loading("Encrypting and saving\ncredentials...");
  populate_nonce(encrypt);
  uint8_t round_keys[176];
  aes128_key_expand(encrypt->key, round_keys);
  aes128_ctr_crypt((uint8_t *)password,
      strlen(password),
      round_keys,
      encrypt->nonce);

  char chipher_text[MAX_PASSWORD_LENGTH * 3 + 1];
  bytes_to_hex_string((uint8_t *)password, strlen(password), chipher_text);

  char nonce_text[KEY_SIZE * 3 + 1];
  bytes_to_hex_string(encrypt->nonce, sizeof(encrypt->nonce), nonce_text);

  size_t buf_len = strlen(username) + strlen(chipher_text) +
                   strlen(nonce_text) + 3;
  char buf[buf_len];
  snprintf(buf, buf_len, "%s|%s|%s", username, chipher_text, nonce_text);

  path *service_path = path_init(service_name);
  path *full_service_path = path_concat(sys_paths->dirs->pwd_manager_path,
      service_path);
  path_fwrite(full_service_path, buf, 'w');

  path_free(service_path);
  path_free(full_service_path);
}
