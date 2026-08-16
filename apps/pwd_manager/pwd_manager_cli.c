// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "apps/pwd_manager/include/pwd_manager_cli.h"

#include <pico/rand.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/reent.h>

#include "apps/pwd_manager/include/pwd_manager.h"
#include "core/utils/include/utils.h"

void print_help() {
  printf(
      "> add: add a new service\n> remove: remove an existing "
      "service by name\n> get: get an existing service by name\n> getall: "
      "get all "
      "services\n> update: update existing service credentials\n> esc: exit "
      "from password manager cli");
}

int8_t process_command(str_list *command) {
  char *cmd = str_list_get(command, 0);

  if (strcmp(cmd, "h") == 0 || strcmp(cmd, "help") == 0) {
    print_help();
    return 0;
  }
  if (strcmp(cmd, "add") == 0) {
    register_new_service_cli();
    return 0;
  }
  if (strcmp(cmd, "remove") == 0) {
    if (command->len > 1) {
      char *srv_name = str_list_get(command, 1);
      delete_service_credentials(srv_name);
    } else {
      printf("Name of the service required!");
      return 1;
    }
    return 0;
  }
  if (strcmp(cmd, "get") == 0) {
    if (command->len > 1) {
      char *srv_name = str_list_get(command, 1);
      printf("getting credentials for %s\n", srv_name);
      edit_service_credentials_cli(srv_name);
    } else {
      printf("Name of the service required!");
      return 1;
    }

    return 0;
  }
  if (strcmp(cmd, "getall") == 0) {
    get_all_services_cli();
    return 0;
  }
  if (strcmp(cmd, "update") == 0) {
    printf("'Update' not fully implemented yet!\n");
    return 1;
    if (command->len > 1) {
      char *srv_name = str_list_get(command, 1);
      update_service_credentials_cli(srv_name);
    } else {
      printf("Name of the service required!");
      return 1;
    }
    return 0;
  }
  if (strcmp(cmd, "esc") == 0)
    return -1;

  printf("Unknown command: %s", cmd);
  return 1;
}

void password_manager_launch_cli() {
  char buf[64];
  int8_t ret;
  printf("Password manager cli, send 'h' for help\n");
  if (!init_encrypt_fields(true))
    return;
  while (1) {
    fgets(buf, sizeof(buf), stdin);
    char *buf_no_lfd = string_remove_linefeed(buf);
    str_list *parts = string_split(buf_no_lfd, ' ');
    ret = process_command(parts);
    free(buf_no_lfd);
    str_list_free(parts);
    if (ret == -1)
      return;
  }
}
