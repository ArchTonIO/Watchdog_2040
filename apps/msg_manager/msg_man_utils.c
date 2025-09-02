// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include "apps/msg_manager/msg_man_utils.h"

#include <pico/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_intsup.h>

#include "apps/msg_manager/contacts.h"
#include "apps/msg_manager/msg_manager.h"
#include "apps/msg_manager/msg_record.h"
#include "apps/text_editor/text_editor.h"
#include "core/components/sys_paths_manager.h"
#include "core/data_structures/string_list.h"
#include "core/tools/options_gen.h"
#include "core/ulmp/ulmp.h"
#include "core/utils/path.h"
#include "core/utils/utils.h"

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
  str_list *all_files = path_listdir(sys_paths->dirs->messages_path);
  str_list *active_conversations = str_list_init();
  for (uint16_t i = 0; i < all_files->len; i++) {
    path *conv_file_temp = path_init(str_list_get(all_files, i));
    path *conv_file = path_concat(sys_paths->dirs->messages_path,
        conv_file_temp);
    path_free(conv_file_temp); // Fix: Use path_free instead of free
    if (strcmp(conv_file->ext, "keys") != 0)
      str_list_append(active_conversations, conv_file->name);
    path_free(conv_file);
  }
  while (true) {
    options_page *conversations = options_page_init("Active conversations:",
        active_conversations);
    char *selected_contact = options_page_launch(conversations);
    char *selected_contact_copy = strdup(selected_contact);
    if (strcmp(selected_contact, "") == 0) {
      str_list_free(all_files);
      options_page_free(conversations);
      free(selected_contact_copy);
      break;
    }
    active_conversations = str_list_copy(conversations->options_list);
    options_page_free(conversations);
    while (true) {
      show_fetching_screen();
      uint16_t contact_addr = get_contact_addr_by_name_threadsafe(
          selected_contact_copy);
      str_list *message_uids = get_stored_msg_uids_by_user(contact_addr);
      str_list *chunks = get_chunks_by_msg_uids(message_uids,
          MESSAGES_CHUNK_SIZE);
      options_page *chunks_page = options_page_init("1 chunk = 10 msg",
          chunks);
      char *selected_chunk = options_page_launch(chunks_page);
      char *selected_chunk_copy = strdup(selected_chunk);
      options_page_free(chunks_page);
      if (strcmp(selected_chunk, "") == 0) {
        str_list_free(message_uids);
        free(selected_contact_copy);
        free(selected_chunk_copy);
        break;
      }
      uint8_t chunk_index = atoi(selected_chunk_copy + 7);
      free(selected_chunk_copy);
      str_list *message_uids_chunk = get_msg_uids_by_chunk(message_uids,
          chunk_index,
          MESSAGES_CHUNK_SIZE);
      str_list_free(message_uids);
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
          options_page_free(messages_entries);
          str_list_free(message_uids_chunk);
          break;
        }
        show_fetching_screen();
        uint16_t selected_msg_index = str_list_index_of(selectable_options,
            selected_msg);
        char *msg_uid = str_list_get(message_uids_chunk, selected_msg_index);
        char *full_msg = get_displayable_msg_by_uid(contact_addr, msg_uid);
        text_editor *editor = text_editor_launch(full_msg, false);
        char *editor_buf = text_editor_get_buf(editor);
        text_editor_kill(editor);
        if (msg_record_flag_as_read(msg_uid, selected_contact_copy))
          msg_man_inst->received_msgs_count--;
        free(full_msg);
        free(editor_buf);
        options_page_free(messages_entries);
      }
    }
  }
}

void show_fetching_screen() { print_loading("Fetching data..."); }

char *get_displayable_msg_by_uid(uint16_t contact_addr, char *msg_uid) {
  char *contact_name = get_contact_name_by_addr_threadsafe(contact_addr);
  path *contact_name_file = path_init(contact_name);
  path *conversation_file = path_concat(sys_paths->dirs->messages_path,
      contact_name_file);
  path_free(contact_name_file);
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
  free(message);
  free(contact_name);
  return displayable_msg;
}

str_list *get_stored_msg_uids_by_user(uint16_t contact_addr) {
  char *contact_name = get_contact_name_by_addr_threadsafe(contact_addr);
  path *contact_name_file = path_init(contact_name);
  path *conversation_file = path_concat(sys_paths->dirs->messages_path,
      contact_name_file);
  path_free(contact_name_file);
  char *keys_file_str = string_add(contact_name, ".keys");
  path *keys_file_temp = path_init(keys_file_str);
  path *keys_file = path_concat(sys_paths->dirs->messages_path,
      keys_file_temp);
  free(keys_file_str);
  path_free(keys_file_temp);
  str_list *keys = path_fread(keys_file);
  str_list *keys_reversed = str_list_reverse(keys);
  path_free(keys_file);
  path_free(conversation_file);
  str_list_free(keys);
  free(contact_name);
  return keys_reversed;
}

str_list *get_chunks_by_msg_uids(str_list *msg_uids, uint8_t chunk_size) {
  uint64_t num_chunks = (msg_uids->len + chunk_size - 1) / chunk_size;
  str_list *chunks = str_list_init();
  for (uint64_t i = 0; i < num_chunks; i++) {
    char index_str[7];
    sprintf(index_str, "%llu", i);
    char *chunk_index_str = string_add("chunk: ", index_str);
    str_list_append(chunks, chunk_index_str);
    free(chunk_index_str);
  }
  return chunks;
}

str_list *get_msg_uids_by_chunk(str_list *msg_uids,
    uint8_t chunk_index,
    uint8_t chunk_size) {
  str_list *chunk = str_list_init();
  for (uint16_t i = chunk_index * chunk_size;
       i < (chunk_index + 1) * chunk_size && i < msg_uids->len;
       i++) {
    char *msg_uid = str_list_get(msg_uids, i);
    msg_uid[strlen(msg_uid) - 1] = '\0'; // Remove the newline character
    str_list_append(chunk, msg_uid);
  }
  return chunk;
}

str_list *get_selectable_options_by_msg_uids(str_list *msg_uids,
    uint16_t contact_addr) {
  //[<"->" or "<-"> <status str>]<first_n_chars_of_msg>
  char *contact_name = get_contact_name_by_addr_threadsafe(contact_addr);
  path *contact_name_file = path_init(contact_name);
  path *conversation_file = path_concat(sys_paths->dirs->messages_path,
      contact_name_file);
  path_free(contact_name_file);
  str_list *options = str_list_init();
  for (uint16_t i = 0; i < msg_uids->len; i++) {
    char *msg_uid = str_list_get(msg_uids, i);
    char *message = path_key_value_get(conversation_file, msg_uid);
    msg_record *record = msg_record_load(message, msg_uid);
    char *option = (char *)malloc(strlen(record->status_str) +
                                  strlen(record->message) + strlen("[ ]:") +
                                  3);
    snprintf(option,
        strlen(record->status_str) + strlen(record->message) + strlen("[ ]:") +
            3,
        "[%s]%s: %s",
        strcmp(record->direction, "transmitted") == 0 ? "->" : "<-",
        record->status_str,
        record->message);
    option[strlen(option) - 1] = '\0';
    str_list_append(options, option);
    free(option);
    free(message);
    msg_record_free(record);
  }
  path_free(conversation_file);
  free(contact_name);
  return options;
}