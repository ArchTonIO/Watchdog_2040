#include "components/msg_manager/contacts_manager.h"

#include <pico/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_intsup.h>

#include "components/hw_manager.h"
#include "components/malloc_mascot.h"
#include "components/msg_manager/msg_manager.h"
#include "components/sys_paths_manager.h"
#include "data_structures/string_list.h"
#include "device.h"
#include "graphics/bitmaps.h"
#include "hardware_drivers/ssd1306.h"
#include "tools/options_gen.h"
#include "tools/text_editor.h"
#include "utils/path.h"
#include "utils/utils.h"

contacts_manager *contacts_man_inst;

bool name_exists(char *name);
void delete_contact(uint16_t addr);

void load_contacts_from_sd();
void save_contact(char *name, uint16_t addr);
char *ask_for_contact_name();
uint16_t ask_for_contact_addr();

contacts_manager *contacts_manager_init() {
  contacts_manager *contacts_man = (contacts_manager *)malloc(
      sizeof(contacts_manager));
  contacts_man->contacts_count = 0;
  contacts_man->contacts_addr_file = sys_paths->files->contacts_addr_file;
  contacts_man->contacts_names_file = sys_paths->files->contacts_names_file;
  contacts_man_inst = contacts_man;
  if (path_exists(contacts_man->contacts_addr_file) &&
      path_exists(contacts_man->contacts_names_file))
    load_contacts_from_sd();
  else
    printf("[CONTACTS MANAGER] (INFO): no contacts found, creating empty "
           "contacts list\n");
  return contacts_man;
}

/**
 * @brief Add a new contact to the contact list.
 */
void add_contact() {
  char *name = ask_for_contact_name();
  if (strlen(name) == 0 || name_exists(name)) {
    ssd1306_clear(drivers->oled_screen);
    ssd1306_print(drivers->oled_screen, "[ERR] name exists", 0, 0, false);
    ssd1306_show(drivers->oled_screen);
    sleep_ms(INFO_PAGES_TIMEOUT);
    ssd1306_clear(drivers->oled_screen);
    ssd1306_show(drivers->oled_screen);
    free(name);
    return;
  }
  uint16_t addr = ask_for_contact_addr();
  if (addr == 0) {
    ssd1306_clear(drivers->oled_screen);
    ssd1306_print(drivers->oled_screen, "[ERR] invalid addr", 0, 0, false);
    ssd1306_show(drivers->oled_screen);
    sleep_ms(INFO_PAGES_TIMEOUT);
    ssd1306_clear(drivers->oled_screen);
    ssd1306_show(drivers->oled_screen);
    free(name); // Fix memory leak
    return;
  }
  save_contact(name, addr);
  char addr_str[8];
  sprintf(addr_str, "%u", addr);
  ssd1306_print(drivers->oled_screen, "Contact saved !", 2, 0, false);
  ssd1306_print(drivers->oled_screen, name, 0, 7, false);
  ssd1306_print(drivers->oled_screen, ":", strlen(name) - 1, 7, false);
  ssd1306_print(drivers->oled_screen, addr_str, strlen(name), 7, false);
  ssd1306_draw_bitmap(drivers->oled_screen, 50, 22, contact_saved, 28, 20, 0);
  ssd1306_show(drivers->oled_screen);
  free(name);
  sleep_ms(INFO_PAGES_TIMEOUT);
  ssd1306_clear(drivers->oled_screen);
  ssd1306_show(drivers->oled_screen);
}

/**
 * @brief Removes a contact from the contact list.
 */
void remove_contact() {
  uint16_t to_remove = select_contact();
  str_list *options = list_init();
  list_append(options, "Yes");
  list_append(options, "No");
  options_page *yesno_page = options_page_init("You sure?", options);
  char *sure = options_page_launch(yesno_page);
  if (strcmp(sure, "Yes") == 0) {
    delete_contact(to_remove);
    ssd1306_clear(drivers->oled_screen);
    ssd1306_print(drivers->oled_screen, "Contact deleted !", 0, 0, false);
    ssd1306_draw_bitmap(drivers->oled_screen,
        50,
        22,
        contact_deleted,
        28,
        20,
        0);
    ssd1306_show(drivers->oled_screen);
    sleep_ms(INFO_PAGES_TIMEOUT);
    ssd1306_clear(drivers->oled_screen);
    ssd1306_show(drivers->oled_screen);
  }
  options_page_free(yesno_page);
}

void save_contact(char *name, uint16_t addr) {
  strncpy(contacts_man_inst->contacts[contacts_man_inst->contacts_count].name,
      name,
      strlen(name) + 1);
  contacts_man_inst->contacts[contacts_man_inst->contacts_count].addr = addr;
  contacts_man_inst->contacts_count++;
}

void delete_contact(uint16_t addr) {
  for (uint16_t i = 0; i < contacts_man_inst->contacts_count; i++) {
    if (contacts_man_inst->contacts[i].addr == addr) {
      for (uint16_t j = i; j < contacts_man_inst->contacts_count - 1; j++) {
        contacts_man_inst->contacts[j] = contacts_man_inst->contacts[j + 1];
      }
      contacts_man_inst->contacts_count--;
      break;
    }
  }
}

bool name_exists(char *name) {
  str_list *contacts = get_all_contacts();
  for (uint16_t i = 0; i < contacts->len; i++) {
    if (strcmp(get(contacts, i), name) == 0) {
      list_free(contacts);
      return true;
    }
  }
  list_free(contacts);
  return false;
}

str_list *get_all_contacts() {
  str_list *contacts = list_init();
  for (uint16_t i = 0; i < contacts_man_inst->contacts_count; i++) {
    list_append(contacts, contacts_man_inst->contacts[i].name);
  }
  return contacts;
}

uint16_t find_contact_addr_by_name(char *name) {
  for (uint16_t i = 0; i < contacts_man_inst->contacts_count; i++) {
    if (strcmp(contacts_man_inst->contacts[i].name, name) == 0)
      return contacts_man_inst->contacts[i].addr;
  }
  return 0;
}

char *find_contact_name_by_addr(uint16_t addr) {
  for (uint16_t i = 0; i < contacts_man_inst->contacts_count; i++) {
    if (contacts_man_inst->contacts[i].addr == addr)
      return contacts_man_inst->contacts[i].name;
  }
  return NULL;
}

uint16_t select_contact() {
  str_list *contacts = get_all_contacts();
  options_page *page = options_page_init("Select a contact", contacts);
  char *name = options_page_launch(page);
  if (strcmp(name, "") == 0) {
    options_page_free(page);
    return 0;
  }
  uint16_t addr = find_contact_addr_by_name(name);
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
  if (!is_string_numeric(temp) || strlen(temp) > 5 || strlen(temp) == 0 ||
      strcmp(temp, malloc_memories_inst->ulmp_addr_str) == 0) {
    free(temp);
    return 0;
  }
  uint16_t addr = 0;
  if (sscanf(temp, "%hu", &addr) == 1) {
    free(temp);
    return addr;
  }
  free(temp);
  return 0;
}

void dump_contacts_to_sd() {
  ssd1306_clear(drivers->oled_screen);
  ssd1306_print(drivers->oled_screen, "Dumping contacts", 0, 0, false);
  ssd1306_print(drivers->oled_screen, "to MicroSD...", 0, 1, false);
  ssd1306_show(drivers->oled_screen);
  str_list *contacts = get_all_contacts();
  path_fwrite(contacts_man_inst->contacts_names_file, "", 'w');
  path_fwrite(contacts_man_inst->contacts_names_file, "", 'w');
  for (uint16_t i = 0; i < contacts->len; i++) {
    char i_to_str[4];
    sprintf(i_to_str, "%u", i + 1);
    ssd1306_print(drivers->oled_screen, i_to_str, 0, 2, false);
    ssd1306_print(drivers->oled_screen, "/", 2, 2, false);
    char total[4];
    sprintf(total, "%u", contacts->len);
    ssd1306_print(drivers->oled_screen, total, 3, 2, false);
    ssd1306_show(drivers->oled_screen);
    char *name = get(contacts, i);
    uint16_t addr = find_contact_addr_by_name(name);
    char addr_str[8];
    sprintf(addr_str, "%u", addr);
    path_key_value_dump(contacts_man_inst->contacts_addr_file,
        'a',
        name,
        addr_str);
    path_fwrite(contacts_man_inst->contacts_names_file, name, 'a');
    path_fwrite(contacts_man_inst->contacts_names_file, "\n", 'a');
  }
  list_free(contacts);
  ssd1306_print(drivers->oled_screen, "Contacts dumped !", 0, 3, false);
  ssd1306_show(drivers->oled_screen);
  sleep_ms(1000);
  ssd1306_clear(drivers->oled_screen);
  ssd1306_show(drivers->oled_screen);
}

void load_contacts_from_sd() {
  str_list *contact_names = path_fread(contacts_man_inst->contacts_names_file);
  for (uint8_t i = 0; i < contact_names->len; i++) {
    char *name = get(contact_names, i);
    size_t name_len = strlen(name);
    if (name[name_len - 1] == '\n')
      name[name_len - 1] = '\0';
    char *addr_str = path_key_value_get(contacts_man_inst->contacts_addr_file,
        name);
    uint16_t addr;
    sscanf(addr_str, "%hu", &addr);
    save_contact(name, addr);
    free(addr_str);
  }
  list_free(contact_names);
}