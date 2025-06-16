#ifndef CONTACTS_MANAGER_H
#define CONTACTS_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

#include "data_structures/string_list.h"
#include "utils/path.h"

#define MAX_CONTACT_NAME_LENGTH 30
#define MAX_CONTACTS 50

typedef struct {
  char name[MAX_CONTACT_NAME_LENGTH];
  uint16_t addr;
} contact;

typedef struct {
  contact contacts[MAX_CONTACTS];
  uint8_t contacts_count;
  path *contacts_addr_file;
  path *contacts_names_file;
} contacts_manager;

contacts_manager *contacts_manager_init();
void add_contact();
void remove_contact();
void dump_contacts_to_sd();

uint16_t find_contact_addr_by_name(char *name);
uint16_t select_contact();
char *find_contact_name_by_addr(uint16_t addr);
str_list *get_all_contacts();

extern contacts_manager *contacts_man_inst;

#endif