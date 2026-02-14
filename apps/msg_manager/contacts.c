// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "apps/msg_manager/contacts.h"

#include <pico/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_intsup.h>

#include "apps/text_editor/text_editor.h"
#include "core/components/include/malloc_mascot.h"
#include "core/components/include/sys_paths_manager.h"
#include "core/data_structures/include/string_list.h"
#include "core/tools/include/options_gen.h"
#include "core/utils/include/path.h"
#include "core/utils/include/utils.h"

void add_contact();
bool address_is_valid(uint16_t addr);
bool name_is_valid(const char *name);
void edit_or_delete_contact(char *name);
void edit_contact(char *name);

str_list *get_all_contacts();
uint16_t get_contact_addr_by_name(char *name);
char *get_contact_name_by_addr(uint16_t addr);
path *get_contact_full_path(const char *name);

char *ask_for_contact_name();
uint16_t ask_for_contact_addr();
void edit_contact_name(char *name);
void edit_contact_addr(char *addr_str);

bool contact_exists(const char *name);
void save_contact(const char *name, uint16_t addr);
void delete_contact(const char *name);

static contacts_list *clist;

/**
 * @brief initializes the contacts_list global instance (clist),
 * thus effectively making a complete copy of the contacts from the
 * microsd and unlocking the feature to check for contacts in a thread-safe
 * way (avoiding directly interacting with the microsd interface)
 */
contacts_list *contacts_list_init() {
  contacts_list *self = malloc(sizeof(contacts_list));
  self->contacts_count = 0;
  clist = self;
  return self;
}

/**
 * @brief updates the current contacts_list instance (clist)
 * using the content of the microsd
 */
void contacts_list_update(str_list *contacts_from_dir) {
  clist->contacts_count = 0;
  if (contacts_from_dir->len > MAX_CONTACTS)
    print_sys_error("Too many contacts to\nhandle, UB may occur");
  for (size_t i = 0; i < contacts_from_dir->len; i++) {
    char *contact_name = str_list_get(contacts_from_dir, i);
    uint16_t contact_address = get_contact_addr_by_name(contact_name);
    strcpy(clist->contacts[clist->contacts_count].name, contact_name);
    clist->contacts[clist->contacts_count].address = contact_address;
    clist->contacts_count++;
  }
}

/**
 * @brief Enters the contacts submenu, allowing the user to add,
 * edit or delete contacts.
 */
void enter_contacts_submenu() {
  str_list *contacts = str_list_init();
  str_list *existing_contacts;
  str_list *options;
  options_page *contacts_page;
  bool contacts_modified = false;
  str_list_append(contacts, "+ add contact");
  while (true) {
    existing_contacts = get_all_contacts();
    options = str_list_extend(contacts, existing_contacts);
    contacts_page = options_page_init("Contacts", options);
    char *buf = options_page_launch(contacts_page);
    if (strcmp(buf, "+ add contact") == 0) {
      add_contact();
      contacts_modified = true;
    } else if (strcmp(buf, "") == 0)
      break;
    else {
      edit_or_delete_contact(buf);
      contacts_modified = true;
    }
    str_list_free(existing_contacts);
    options_page_free(contacts_page);
  }
  if (contacts_modified) {
    print_loading("Updating contacts\nsnapshot...");
    contacts_list_update(existing_contacts);
  }
  str_list_free(contacts);
  str_list_free(existing_contacts);
  options_page_free(contacts_page);
}

/**
 * @brief Add a new contact to the contact list.
 */
void add_contact() {
  char *name = ask_for_contact_name();
  if (!name_is_valid(name)) {
    free(name);
    return;
  }
  uint16_t addr = ask_for_contact_addr();
  if (!address_is_valid(addr)) {
    free(name);
    return;
  }
  print_loading("Saving contact...");
  save_contact(name, addr);
  free(name);
  print_info("Contact added\nsuccessfully");
}

bool name_is_valid(const char *name) {
  if (strlen(name) == 0 || contact_exists(name)) {
    print_usr_error("A contact with this\nname already exists");
    return false;
  }
  return true;
}

bool address_is_valid(uint16_t addr) {
  if (addr == 0) {
    print_usr_error("Invalid address");
    return false;
  }
  return true;
}

void edit_or_delete_contact(char *name) {
  str_list *options = str_list_init();
  str_list_append(options, "edit");
  str_list_append(options, "delete");
  options_page *editordelete_page = options_page_init(name, options);
  char *buf = options_page_launch(editordelete_page);
  if (strcmp(buf, "edit") == 0) {
    sleep_ms(200);
    edit_contact(name);
  } else if (strcmp(buf, "delete") == 0) {
    sleep_ms(200);
    str_list *options = str_list_init();
    str_list_append(options, "yes");
    str_list_append(options, "no");
    options_page *yesno_page = options_page_init("Are you sure?", options);
    char *yesno_buf = options_page_launch(yesno_page);
    if (strcmp(yesno_buf, "no") == 0 || strcmp(yesno_buf, "") == 0) {
      options_page_free(yesno_page);
      options_page_free(editordelete_page);
      return;
    }
    print_loading("Deleting contact...");
    options_page_free(yesno_page);
    delete_contact(name);
    print_info("Contact deleted\nsuccessfully");
  }
  options_page_free(editordelete_page);
}

void edit_contact(char *name) {
  str_list *options = str_list_init();
  str_list_append(options, "name");
  str_list_append(options, "address");
  options_page *name_or_address_page = options_page_init("Edit contact",
      options);
  char *buf = options_page_launch(name_or_address_page);
  if (strcmp(buf, "name") == 0) {
    edit_contact_name(name);
  } else if (strcmp(buf, "address") == 0)
    edit_contact_addr(name);
  options_page_free(name_or_address_page);
}

void edit_contact_name(char *name) {
  text_editor *editor = text_editor_launch(name, false);
  char *new_name = text_editor_get_buf(editor);
  text_editor_kill(editor);
  if (strcmp(new_name, name) == 0) {
    free(new_name);
    return;
  }
  print_loading("Updating contact...");
  path *old_full_path = get_contact_full_path(name);
  path *new_full_path = get_contact_full_path(new_name);
  path_rename(old_full_path, new_full_path);
  path_free(new_full_path);
  path_free(old_full_path);
  free(new_name);
  print_info("Contact name\nupdated successfully");
}

void edit_contact_addr(char *name) {
  uint16_t addr = get_contact_addr_by_name(name);
  char addr_str[6];
  snprintf(addr_str, 6, "%u", addr);
  text_editor *editor = text_editor_launch(addr_str, false);
  char *new_addr_str = text_editor_get_buf(editor);
  text_editor_kill(editor);
  if (!is_string_numeric(new_addr_str) || strlen(new_addr_str) != 5 ||
      strcmp(new_addr_str, malloc_memories_inst->ulmp_addr_str) == 0) {
    free(new_addr_str);
    print_usr_error("Invalid address");
    return;
  }
  if (strcmp(new_addr_str, addr_str) == 0) {
    free(new_addr_str);
    return;
  }
  print_loading("Updating contact...");
  path *full_path = get_contact_full_path(name);
  path_fwrite(full_path, new_addr_str, 'w');
  path_free(full_path);
  free(new_addr_str);
  print_info("Contact address\nupdated successfully");
}

path *get_contact_full_path(const char *name) {
  path *name_path = path_init(name);
  path *full_path = path_concat(sys_paths->dirs->contacts_path, name_path);
  path_free(name_path);
  return full_path;
}

/**
 * @brief Retrieves all contacts from the contacts directory in the microsd.
 * @note Do not call this function from core1, as it directly interacts
 * with the microsd interface, which is not thread-safe.
 *
 * @return A list of contact names.
 */
str_list *get_all_contacts() {
  return path_listdir(sys_paths->dirs->contacts_path);
}

/**
 * @brief Retrieves the address of a contact by name.
 * @note Do not call this function from core1, as it directly interacts
 * with the microsd interface, which is not thread-safe.
 *
 * @param name The name of the contact.
 * @return The address of the contact, or 0 if not found.
 */
uint16_t get_contact_addr_by_name(char *name) {
  path *full_path = get_contact_full_path(name);
  uint16_t addr;
  if (!path_exists(full_path)) {
    path_free(full_path);
    return 0;
  }
  str_list *fcontent = path_fread(full_path);
  char *addr_str = str_list_get(fcontent, 0);
  addr = atoi(addr_str);
  str_list_free(fcontent);
  path_free(full_path);
  return addr;
}

/**
 * @brief Retrieves the name of a contact by address.
 * @note Do not call this function from core1, as it directly interacts
 * with the microsd interface, which is not thread-safe.
 *
 * @param addr The address of the contact.
 * @return The name of the contact, or NULL if not found.
 */
char *get_contact_name_by_addr(uint16_t addr) {
  str_list *contacts = get_all_contacts();
  if (contacts->len == 0) {
    str_list_free(contacts);
    return NULL;
  }
  char *name = NULL;
  path *full_path = NULL;
  str_list *fcontent = NULL;
  for (size_t i = 0; i < contacts->len; i++) {
    name = str_list_get(contacts, i);
    full_path = get_contact_full_path(name);
    fcontent = path_fread(full_path);
    uint16_t found_addr;
    char *addr_str = str_list_get(fcontent, 0);
    found_addr = atoi(addr_str);
    if (found_addr == addr) {
      char *ret = strdup(name);
      str_list_free(fcontent);
      path_free(full_path);
      str_list_free(contacts);
      return ret;
    }
    str_list_free(fcontent);
    path_free(full_path);
  }
  str_list_free(contacts);
  return NULL;
}

/**
 * @brief Retrieves all contacts from the contacts list global instance
 * (clist), thread-safe.
 *
 * @return A list of contact names, if any.
 */
str_list *get_all_contacts_threadsafe() {
  str_list *contacts = str_list_init();
  for (size_t i = 0; i < clist->contacts_count; i++)
    str_list_append(contacts, clist->contacts[i].name);
  return contacts;
}

/**
 * @brief Retrieves the address of a contact by its name, thread-safe.
 *
 * @param name The name of the contact.
 * @return The address of the contact, or 0 if not found.
 */
uint16_t get_contact_addr_by_name_threadsafe(const char *name) {
  for (size_t i = 0; i < clist->contacts_count; i++)
    if (strcmp(name, clist->contacts[i].name) == 0)
      return clist->contacts[i].address;
  return 0;
}

/**
 * @brief Retrieves the name of a contact by its address, thread-safe.
 *
 * @param addr The address of the contact.
 * @return The name of the contact, or NULL if not found.
 */
char *get_contact_name_by_addr_threadsafe(uint16_t addr) {
  for (size_t i = 0; i < clist->contacts_count; i++)
    if (addr == clist->contacts[i].address)
      return strdup(clist->contacts[i].name);
  return NULL;
}

/**
 * @brief Makes the user select a contact from the contacts list.
 * @return The address of the selected contact, or 0 if none is selected.
 */
uint16_t select_contact() {
  str_list *contacts = get_all_contacts();
  options_page *page = options_page_init("Select a contact", contacts);
  char *name = options_page_launch(page);
  if (strcmp(name, "") == 0) {
    options_page_free(page);
    return 0;
  }
  uint16_t addr = get_contact_addr_by_name(name);
  options_page_free(page);
  return addr;
}

char *ask_for_contact_name() {
  text_editor *editor = text_editor_launch("# Type in the contact name", true);
  char *name = text_editor_get_buf(editor);
  text_editor_kill(editor);
  return name;
}

uint16_t ask_for_contact_addr() {
  text_editor *editor = text_editor_launch("# Type in the contact address",
      true);
  char *temp = text_editor_get_buf(editor);
  text_editor_kill(editor);
  if (!is_string_numeric(temp) || strlen(temp) != 5 ||
      strcmp(temp, malloc_memories_inst->ulmp_addr_str) == 0) {
    free(temp);
    return 0;
  }
  uint16_t addr = atoi(temp);
  free(temp);
  return addr;
}

bool contact_exists(const char *name) {
  str_list *contacts = get_all_contacts();
  for (size_t i = 0; i < contacts->len; i++)
    if (strcmp(str_list_get(contacts, i), name) == 0) {
      str_list_free(contacts);
      return true;
    }
  str_list_free(contacts);
  return false;
}

void save_contact(const char *name, uint16_t addr) {
  path *full_path = get_contact_full_path(name);
  char addr_str[6];
  snprintf(addr_str, 6, "%u", addr);
  path_fwrite(full_path, addr_str, 'w');
  path_free(full_path);
}

void delete_contact(const char *name) {
  path *full_path = get_contact_full_path(name);
  path_fdelete(full_path);
  path_free(full_path);
}