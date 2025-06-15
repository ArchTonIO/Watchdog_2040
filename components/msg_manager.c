#include "components/msg_manager.h"

#include <pico/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_intsup.h>

#include "components/hw_manager.h"
#include "components/malloc_mascot.h"
#include "components/msg_record.h"
#include "data_structures/string_list.h"
#include "device.h"
#include "graphics/bitmaps.h"
#include "hardware_drivers/ssd1306.h"
#include "tools/options_gen.h"
#include "tools/text_editor.h"
#include "ulcp/ulcp.h"
#include "utils/path.h"
#include "utils/utils.h"

msg_manager *msg_man_inst;

void notify(uint16_t src_address);
char *compose_message();
uint16_t select_contact();
void delete_contact(uint16_t addr);
char *ask_for_contact_name();
uint16_t ask_for_contact_addr();
void display_sent_message_status(uint8_t result, uint16_t dest_addr);
void save_contact(char *name, uint16_t addr);
void update_conversation_file(uint16_t contact_addr,
    char *message,
    uint8_t status);
uint16_t find_contact_addr_by_name(char *name);
char *find_contact_name_by_addr(uint16_t addr);
void display_received_message(uint16_t src_address);
void load_contacts_from_sd();
void dump_contacts_to_sd();
str_list *get_stored_msg_uids_by_user(uint16_t contact_addr);
str_list *get_selectable_options_by_msg_uids(str_list *msg_uids,
    uint16_t contact_addr);
char *get_displayable_msg_by_uid(uint16_t contact_addr, char *msg_uid);

/**
 * @brief Initializes the message manager with the given address.
 *
 * @param my_addr The address of the device.
 * @return A pointer to the initialized message manager.
 */
msg_manager *msg_manager_init(uint16_t my_addr) {
  if (my_addr == 0) {
    printf("[MSG MANAGER] (ERR): the ulcp address of a device cannot be 0\n");
    return NULL;
  }
  msg_manager *msg_man = (msg_manager *)malloc(sizeof(msg_manager));
  msg_man->new_msg_arrived = false;
  msg_man->should_notify = true;
  msg_man->contacts_count = 0;
  msg_man->received_msgs_count = 0;
  msg_man->ulmp_impl = lora_init(my_addr, drivers->lora_module);
  msg_man->contacts_addr_file = path_init(
      string_add(malloc_memories_inst->user_folder, CONTACTS_ADDR_FILE));
  msg_man->contacts_names_file = path_init(
      string_add(malloc_memories_inst->user_folder, CONTACTS_NAMES_FILE));
  lora_receive();
  msg_man_inst = msg_man;
  if (path_exists(msg_man->contacts_addr_file) &&
      path_exists(msg_man->contacts_names_file))
    load_contacts_from_sd();
  else
    printf("[MSG MANAGER] (INFO): no contacts found, creating empty contacts "
           "list\n");
  return msg_man;
}

/**
 * @brief Processes incoming messages and sends acknowledgments if necessary.
 */
void process_messages() {
  lora_send_ack(notify);
  sleep_ms(10);
}

void show_fetching_screen() {
  ssd1306_clear(drivers->oled_screen);
  ssd1306_print(drivers->oled_screen, "Fetching...", 0, 0, false);
  ssd1306_show(drivers->oled_screen);
  ssd1306_clear(drivers->oled_screen);
}

void read_messages() {
  /*step 1: fetching screen*/
  show_fetching_screen();

  /*step 2: found active conversations by parsing files in the messages
   * folder and showing them with options_gen*/
  path *conversations_dir = path_init(
      string_add(malloc_memories_inst->user_folder, MESSAGES_DIR));
  str_list *all_files = path_listdir(conversations_dir);
  str_list *active_conversations = list_init();
  for (uint16_t i = 0; i < all_files->len; i++) {
    path *conv_file = path_init(
        string_add(string_add(malloc_memories_inst->user_folder, MESSAGES_DIR),
            get(all_files, i)));
    if (strcmp(conv_file->ext, "keys") != 0)
      list_append(active_conversations, conv_file->name);
    path_free(conv_file);
  }

  /*step 3: select a conversation, collect all message_uids for that
   * conversation*/
  options_page *conversations = options_page_init("Active conversations:",
      active_conversations);
  char *selected_contact = options_page_launch(conversations);
  if (strcmp(selected_contact, "") == 0) {
    path_free(conversations_dir);
    list_free(all_files);
    options_page_free(conversations);
    return;
  }
  show_fetching_screen();
  uint16_t contact_addr = find_contact_addr_by_name(selected_contact);
  str_list *message_uids = get_stored_msg_uids_by_user(contact_addr);
  /*step 4: convert all those message_uids into usable entries for the user to
   * select*/
  str_list *selectable_options = get_selectable_options_by_msg_uids(
      message_uids,
      contact_addr);
  options_page *messages_entries = options_page_init("Select a msg to read",
      selectable_options);
  char *selected_msg = options_page_launch(messages_entries);
  /*step 5: since the selectable_options and the message_uids keeps the same
   * indexing and order, the second can be used to understand which message uid
   * has been selected*/
  uint16_t selected_msg_index = list_index_of(selectable_options,
      selected_msg);
  char *msg_uid = get(message_uids, selected_msg_index);
  char *full_msg = get_displayable_msg_by_uid(contact_addr, msg_uid);
  show_fetching_screen();
  text_editor *editor = text_editor_launch(full_msg, false);
  char *editor_buf = text_editor_get_buf(editor);
  text_editor_kill(editor);
  path_free(conversations_dir);
  options_page_free(messages_entries);
  options_page_free(conversations);
  list_free(message_uids);
  list_free(all_files);
  free(full_msg);
  free(editor_buf);
}

void send_message_status_update_callback(uint8_t progress) {
  char progress_str[2];
  sprintf(progress_str, "%u", progress);
  ssd1306_print(drivers->oled_screen, "Message sent", 0, 0, false);
  ssd1306_print(drivers->oled_screen, "Waiting for ack...", 0, 2, false);
  ssd1306_print(drivers->oled_screen, "Attempts: ", 0, 4, false);
  ssd1306_print(drivers->oled_screen, progress_str, 9, 4, false);
  ssd1306_print(drivers->oled_screen, "/", 10, 4, false);
  ssd1306_print(drivers->oled_screen, "5", 11, 4, false);
  ssd1306_show(drivers->oled_screen);
}

/**
 * @brief Sends a message to a selected contact.
 */
void send_message() {
  uint16_t dest_addr = select_contact();
  if (dest_addr == 0) {
    ssd1306_clear(drivers->oled_screen);
    ssd1306_show(drivers->oled_screen);
    return;
  }
  char *msg = compose_message();
  uint8_t result = lora_send_msg(dest_addr,
      msg,
      send_message_status_update_callback);
  display_sent_message_status(result, dest_addr);
  update_conversation_file(dest_addr, msg, result);
  free(msg);
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

void update_conversation_file(uint16_t contact_addr,
    char *message,
    uint8_t status) {
  msg_record *record = msg_record_init(contact_addr, message, status);
  msg_record_dump(record);
  msg_record_free(record);
}

char *get_displayable_msg_by_uid(uint16_t contact_addr, char *msg_uid) {
  char *contact_name = find_contact_name_by_addr(contact_addr);
  path *conversation_file = path_init(
      string_add(string_add(malloc_memories_inst->user_folder, MESSAGES_DIR),
          contact_name));
  char *message = path_key_value_get(conversation_file, msg_uid);
  msg_record *record = msg_record_load(message, msg_uid);
  char *displayable_msg = (char *)malloc(
      strlen(record->message) + strlen(record->timestamp) + 1);
  snprintf(displayable_msg,
      strlen(record->message) + strlen(record->timestamp) + 1,
      "%s\n%s",
      record->timestamp,
      record->message);
  path_free(conversation_file);
  msg_record_free(record);
  return displayable_msg;
}

str_list *get_stored_msg_uids_by_user(uint16_t contact_addr) {
  char *contact_name = find_contact_name_by_addr(contact_addr);
  path *conversation_file = path_init(
      string_add(string_add(malloc_memories_inst->user_folder, MESSAGES_DIR),
          contact_name));
  path *keys_file = path_init(
      string_add(string_add(malloc_memories_inst->user_folder, MESSAGES_DIR),
          string_add(contact_name, ".keys")));
  str_list *keys = path_fread(keys_file);
  str_list *keys_reversed = list_reverse(keys);
  path_free(keys_file);
  path_free(conversation_file);
  list_free(keys);
  return keys_reversed;
}

str_list *get_selectable_options_by_msg_uids(str_list *msg_uids,
    uint16_t contact_addr) {
  //[<"->" or "<-"> <status str>]<first_n_chars_of_msg>
  char *contact_name = find_contact_name_by_addr(contact_addr);
  path *conversation_file = path_init(
      string_add(string_add(malloc_memories_inst->user_folder, MESSAGES_DIR),
          contact_name));
  str_list *options = list_init();
  for (uint16_t i = 0; i < msg_uids->len; i++) {
    char *msg_uid = get(msg_uids, i);
    msg_uid[strlen(msg_uid) - 1] = '\0';
    char *message = path_key_value_get(conversation_file, msg_uid);
    msg_record *record = msg_record_load(message, msg_uid);
    char *option = (char *)malloc(strlen(record->status_str) +
                                  strlen(record->message) + strlen("[ ]") + 3);
    snprintf(option,
        strlen(record->status_str) + strlen(record->message) + strlen("[ ]") +
            3,
        "[%s %s]%s",
        strcmp(record->direction, "transmitted") == 0 ? "->" : "<-",
        record->status_str,
        record->message);
    option[strlen(option) - 1] = '\0';
    list_append(options, option);
    free(option);
    free(message);
    msg_record_free(record);
  }
  path_free(conversation_file);
  return options;
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

void notify(uint16_t src_address) {
  msg_man_inst->new_msg_arrived = true;
  msg_man_inst->received_msgs_count++;
  update_conversation_file(src_address,
      msg_man_inst->ulmp_impl->rx->recv_payloads_buf,
      3);
  if (!msg_man_inst->should_notify)
    return;
  display_received_message(src_address);
}

void save_contact(char *name, uint16_t addr) {
  strncpy(msg_man_inst->contacts[msg_man_inst->contacts_count].name,
      name,
      strlen(name) + 1);
  msg_man_inst->contacts[msg_man_inst->contacts_count].addr = addr;
  msg_man_inst->contacts_count++;
}

void delete_contact(uint16_t addr) {
  for (uint16_t i = 0; i < msg_man_inst->contacts_count; i++) {
    if (msg_man_inst->contacts[i].addr == addr) {
      for (uint16_t j = i; j < msg_man_inst->contacts_count - 1; j++) {
        msg_man_inst->contacts[j] = msg_man_inst->contacts[j + 1];
      }
      msg_man_inst->contacts_count--;
      break;
    }
  }
}

str_list *get_all_contacts() {
  str_list *contacts = list_init();
  for (uint16_t i = 0; i < msg_man_inst->contacts_count; i++) {
    list_append(contacts, msg_man_inst->contacts[i].name);
  }
  return contacts;
}

uint16_t find_contact_addr_by_name(char *name) {
  for (uint16_t i = 0; i < msg_man_inst->contacts_count; i++) {
    if (strcmp(msg_man_inst->contacts[i].name, name) == 0)
      return msg_man_inst->contacts[i].addr;
  }
  return 0;
}

char *find_contact_name_by_addr(uint16_t addr) {
  for (uint16_t i = 0; i < msg_man_inst->contacts_count; i++) {
    if (msg_man_inst->contacts[i].addr == addr)
      return msg_man_inst->contacts[i].name;
  }
  return NULL;
}

char *compose_message() {
  text_editor *editor = text_editor_launch("# Type in your message", true);
  char *message = text_editor_get_buf(editor);
  text_editor_kill(editor);
  return message;
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
  if (!is_string_numeric(temp) || strlen(temp) > 5 || strlen(temp) == 0)
    return 0;
  uint16_t addr = 0;
  if (sscanf(temp, "%hu", &addr) == 1) {
    free(temp);
    return addr;
  }
}

void display_sent_message_status(uint8_t status, uint16_t dest_addr) {
  ssd1306_clear(drivers->oled_screen);
  char *name = find_contact_name_by_addr(dest_addr);
  ssd1306_print(drivers->oled_screen, "Message status:", 0, 0, false);
  ssd1306_print(drivers->oled_screen, "sent to", 4, 2, false);
  ssd1306_print(drivers->oled_screen, "recv by", 4, 4, false);
  ssd1306_print(drivers->oled_screen, name, 10, 2, false);
  ssd1306_print(drivers->oled_screen, name, 10, 4, false);
  if (status == 0) {
    ssd1306_print(drivers->oled_screen, "[OK]", 0, 2, false);
    ssd1306_print(drivers->oled_screen, "[OK]", 0, 4, false);
  } else if (status == 1) {
    ssd1306_print(drivers->oled_screen, "[OK]", 0, 2, false);
    ssd1306_print(drivers->oled_screen, "[NO]", 0, 4, false);
  } else if (status == 2) {
    ssd1306_print(drivers->oled_screen, "[NO]", 0, 2, false);
    ssd1306_print(drivers->oled_screen, "[NO]", 0, 4, false);
  }
  ssd1306_show(drivers->oled_screen);
  sleep_ms(INFO_PAGES_TIMEOUT);
}

void display_received_message(uint16_t src_address) {
  char *name = find_contact_name_by_addr(src_address);
  bool to_free = false;
  if (name == NULL) {
    name = (char *)malloc(20 * sizeof(char));
    sprintf(name, "Unknown (%u)", src_address);
    to_free = true;
  }
  uint8_t persistency = 10;
  uint32_t start;
  start = to_us_since_boot(get_absolute_time()) / 1000000;
  ssd1306_clear(drivers->oled_screen);
  ssd1306_print(drivers->oled_screen, "New message from:", 0, 0, false);
  ssd1306_print(drivers->oled_screen, name, 0, 1, false);
  ssd1306_draw_bitmap(drivers->oled_screen,
      22,
      22,
      message_received,
      28,
      20,
      0);
  ssd1306_draw_bitmap(drivers->oled_screen, 50, 24, easyarrow, 28, 20, 0);
  ssd1306_draw_bitmap(drivers->oled_screen,
      78,
      22,
      message_received_open,
      28,
      20,
      0);
  ssd1306_print(drivers->oled_screen, "Right to read -> ", 0, 7, false);
  ssd1306_show(drivers->oled_screen);
  while (true) {
    if ((to_us_since_boot(get_absolute_time()) / 1000000) - start >
        persistency) {
      ssd1306_clear(drivers->oled_screen);
      ssd1306_show(drivers->oled_screen);
      if (to_free)
        free(name);
      return;
    }
    joystick_update(drivers->joystick);
    if (joystick_get_direction(drivers->joystick) == E) {
      text_editor *editor = text_editor_launch(
          msg_man_inst->ulmp_impl->rx->recv_payloads_buf,
          false);
      char *text = text_editor_get_buf(editor);
      text_editor_kill(editor);
      free(text);
      if (to_free)
        free(name);
      return;
    }
  }
}

void scan_online_contacts() {
  str_list *contacts = get_all_contacts();
  str_list *results = list_init();
  uint8_t addr;
  for (uint8_t i = 0; i < contacts->len; i++) {
    addr = find_contact_addr_by_name(get(contacts, i));
    if (lora_ping(addr) == 0)
      list_append(results, get(contacts, i));
    sleep_ms(10);
  }
  options_page *page = options_page_init("Online contacts", results);
  char *name = options_page_launch(page);
  options_page_free(page);
  list_free(contacts);
}

void dump_contacts_to_sd() {
  ssd1306_clear(drivers->oled_screen);
  ssd1306_print(drivers->oled_screen, "Dumping contacts", 0, 0, false);
  ssd1306_print(drivers->oled_screen, "to MicroSD...", 0, 1, false);
  ssd1306_show(drivers->oled_screen);
  str_list *contacts = get_all_contacts();
  path_fwrite(msg_man_inst->contacts_names_file, "", 'w');
  path_fwrite(msg_man_inst->contacts_names_file, "", 'w');
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
    path_key_value_dump(msg_man_inst->contacts_addr_file, 'a', name, addr_str);
    path_fwrite(msg_man_inst->contacts_names_file, name, 'a');
    path_fwrite(msg_man_inst->contacts_names_file, "\n", 'a');
  }
  list_free(contacts);
  ssd1306_print(drivers->oled_screen, "Contacts dumped !", 0, 3, false);
  ssd1306_show(drivers->oled_screen);
  sleep_ms(1000);
  ssd1306_clear(drivers->oled_screen);
  ssd1306_show(drivers->oled_screen);
}

void load_contacts_from_sd() {
  str_list *contact_names = path_fread(msg_man_inst->contacts_names_file);
  for (uint8_t i = 0; i < contact_names->len; i++) {
    char *name = get(contact_names, i);
    size_t name_len = strlen(name);
    if (name[name_len - 1] == '\n')
      name[name_len - 1] = '\0';
    char *addr_str = path_key_value_get(msg_man_inst->contacts_addr_file,
        name);
    uint16_t addr;
    sscanf(addr_str, "%hu", &addr);
    save_contact(name, addr);
    free(addr_str);
  }
  list_free(contact_names);
}

void enable_message_notifications() {
  msg_man_inst->should_notify = true;
  ssd1306_clear(drivers->oled_screen);
  ssd1306_print(drivers->oled_screen, "Notifications", 0, 0, false);
  ssd1306_print(drivers->oled_screen, "enabled !", 0, 2, false);
  ssd1306_show(drivers->oled_screen);
  sleep_ms(INFO_PAGES_TIMEOUT);
  ssd1306_clear(drivers->oled_screen);
  ssd1306_show(drivers->oled_screen);
}

void disable_message_notifications() {
  msg_man_inst->should_notify = false;
  ssd1306_clear(drivers->oled_screen);
  ssd1306_print(drivers->oled_screen, "Notifications", 0, 0, false);
  ssd1306_print(drivers->oled_screen, "disabled !", 0, 2, false);
  ssd1306_show(drivers->oled_screen);
  sleep_ms(INFO_PAGES_TIMEOUT);
  ssd1306_clear(drivers->oled_screen);
  ssd1306_show(drivers->oled_screen);
}
