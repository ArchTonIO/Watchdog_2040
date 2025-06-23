
#include "components/msg_manager/msg_man_utils.h"

#include <pico/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_intsup.h>

#include "components/hw_manager.h"
#include "components/malloc_mascot.h"
#include "components/msg_manager/contacts_manager.h"
#include "components/msg_manager/msg_record.h"
#include "data_structures/string_list.h"
#include "device.h"
#include "hardware_drivers/ssd1306.h"
#include "tools/options_gen.h"
#include "tools/text_editor.h"
#include "ulcp/ulcp.h"
#include "utils/path.h"
#include "utils/utils.h"

void show_fetching_screen();
char *get_displayable_msg_by_uid(uint16_t contact_addr, char *msg_uid);
str_list *get_stored_msg_uids_by_user(uint16_t contact_addr);
str_list *get_chunks_by_msg_uids(str_list *msg_uids, uint8_t chunk_size);
str_list *get_msg_uids_by_chunk(str_list *msg_uids,
    uint8_t chunk_index,
    uint8_t chunk_size);
str_list *get_selectable_options_by_msg_uids(str_list *msg_uids,
    uint16_t contact_addr);

/*scary aah function*/
void show_read_messages_menu() {
  show_fetching_screen();

  // Build path to messages dir
  path *conversations_dir = path_init(
      string_add(malloc_memories_inst->user_folder, MESSAGES_DIR));

  // Get all files inside the messages dir
  str_list *all_files = path_listdir(conversations_dir);
  str_list *active_conversations = list_init();

  // Filter out "*.keys" files and collect active conversation names
  for (uint16_t i = 0; i < all_files->len; i++) {
    path *conv_file = path_init(
        string_add(string_add(malloc_memories_inst->user_folder, MESSAGES_DIR),
            get(all_files, i)));
    if (strcmp(conv_file->ext, "keys") != 0)
      list_append(active_conversations, conv_file->name);
    path_free(conv_file);
  }

  // MAIN MENU: CONTACTS
  while (true) {
    // Create contact menu (takes ownership of active_conversations)
    options_page *conversations = options_page_init("Active conversations:",
        active_conversations);

    char *selected_contact = options_page_launch(conversations);
    char *selected_contact_copy = strdup(selected_contact);

    if (strcmp(selected_contact, "") == 0) {
      // User pressed left at contact menu → Exit
      path_free(conversations_dir);
      list_free(all_files);
      options_page_free(conversations); // also frees active_conversations
      free(selected_contact_copy);
      break;
    }

    // Store contact name, free old conversations page
    active_conversations = list_copy(conversations->options_list);
    options_page_free(conversations);

    // CHUNKS MENU: Messages by 10
    while (true) {
      show_fetching_screen();
      uint16_t contact_addr = find_contact_addr_by_name(selected_contact_copy);
      str_list *message_uids = get_stored_msg_uids_by_user(contact_addr);
      str_list *chunks = get_chunks_by_msg_uids(message_uids, 10);

      options_page *chunks_page = options_page_init("1 chunk = 10 msg",
          chunks);
      char *selected_chunk = options_page_launch(chunks_page);
      options_page_free(chunks_page); // also frees `chunks`

      if (strcmp(selected_chunk, "") == 0) {
        // Go back to contact selection
        list_free(message_uids);
        free(selected_contact_copy);
        break;
      }

      // Extract chunk index (after "chunk: ")
      uint8_t chunk_index = atoi(selected_chunk + 7);
      str_list *message_uids_chunk = get_msg_uids_by_chunk(message_uids,
          chunk_index,
          10);

      // We don’t need full UID list anymore
      list_free(message_uids);

      // MESSAGE SELECTION
      while (true) {
        show_fetching_screen();
        str_list *selectable_options = get_selectable_options_by_msg_uids(
            message_uids_chunk,
            contact_addr);

        options_page *messages_entries = options_page_init(
            "Select a msg to read",
            selectable_options);
        char *selected_msg = options_page_launch(messages_entries);

        if (strcmp(selected_msg, "") == 0) {
          // Go back to chunk selection
          options_page_free(
              messages_entries); // also frees `selectable_options`
          list_free(message_uids_chunk);
          break;
        }

        show_fetching_screen();

        uint16_t selected_msg_index = list_index_of(selectable_options,
            selected_msg);
        char *msg_uid = get(message_uids_chunk, selected_msg_index);

        char *full_msg = get_displayable_msg_by_uid(contact_addr, msg_uid);

        text_editor *editor = text_editor_launch(full_msg, false);
        char *editor_buf = text_editor_get_buf(editor);
        text_editor_kill(editor);

        // Cleanup
        free(full_msg);
        free(editor_buf);
        options_page_free(messages_entries); // also frees `selectable_options`
      }
    }
  }
}

void show_fetching_screen() {
  ssd1306_clear(drivers->oled_screen);
  ssd1306_print(drivers->oled_screen, "Fetching...", 0, 0, false);
  ssd1306_show(drivers->oled_screen);
  ssd1306_clear(drivers->oled_screen);
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

str_list *get_chunks_by_msg_uids(str_list *msg_uids, uint8_t chunk_size) {
  uint64_t num_chunks = (msg_uids->len + chunk_size - 1) / chunk_size;
  str_list *chunks = list_init();
  for (uint64_t i = 0; i < num_chunks; i++) {
    char index_str[7];
    sprintf(index_str, "%lu", i);
    list_append(chunks, string_add("chunk: ", index_str));
  }
  return chunks;
}

str_list *get_msg_uids_by_chunk(str_list *msg_uids,
    uint8_t chunk_index,
    uint8_t chunk_size) {
  str_list *chunk = list_init();
  for (uint16_t i = chunk_index * chunk_size;
       i < (chunk_index + 1) * chunk_size && i < msg_uids->len;
       i++) {
    char *msg_uid = get(msg_uids, i);
    msg_uid[strlen(msg_uid) - 1] = '\0'; // Remove the newline character
    list_append(chunk, msg_uid);
  }
  return chunk;
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