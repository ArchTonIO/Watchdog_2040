// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#ifndef CONTACTS_MANAGER_H
#define CONTACTS_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

#include "apps/msg_manager/msg_record.h"
#include "core/data_structures/string_list.h"

#define MAX_CONTACTS 50

typedef struct {
  char name[MAX_CONTACT_NAME_LENGTH];
  uint16_t address;
} contact;

typedef struct {
  contact contacts[MAX_CONTACTS];
  uint8_t contacts_count;
} contacts_list;

/*In-sram contacts list management*/
contacts_list *contacts_list_init();
void contacts_list_update(str_list *contacts_from_dir);

/*Submenus*/
void enter_contacts_submenu();
uint16_t select_contact();

/*Contacts accessors*/
str_list *get_all_contacts();
uint16_t get_contact_addr_by_name(char *name);
char *get_contact_name_by_addr(uint16_t addr);

/*Thread-safe contacts accessors*/
str_list *get_all_contacts_threadsafe();
uint16_t get_contact_addr_by_name_threadsafe(const char *name);
char *get_contact_name_by_addr_threadsafe(uint16_t addr);

#endif