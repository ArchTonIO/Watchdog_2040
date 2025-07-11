#include "components/msg_manager/msg_record.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_intsup.h>

#include "components/hw_manager.h"
#include "components/malloc_mascot.h"
#include "components/msg_manager/msg_manager.h"
#include "data_structures/string_list.h"
#include "device.h"
#include "utils/path.h"
#include "utils/utils.h"

msg_record *
msg_record_init(uint16_t contact_addr, char *message, uint8_t status) {
  msg_record *record = (msg_record *)malloc(sizeof(msg_record));
  record->message = (char *)malloc(strlen(message) + 1);
  char *no_lfd_message = string_replace(message, '\n', LFD_REPLACEMENT);
  strcpy(record->message, no_lfd_message);
  record->record_uid = gen_random_string(RECORD_UID_LENGTH);
  strncpy(record->contact_name,
      find_contact_name_by_addr(contact_addr),
      MAX_CONTACT_NAME_LENGTH);
  record->status = status;
  switch (status) {
  case 0:
    strcpy(record->status_str, "delivered");
    strcpy(record->direction, "transmitted");
    break;
  case 1:
    strcpy(record->status_str, "sent");
    strcpy(record->direction, "transmitted");
    break;
  case 2:
    strcpy(record->status_str, "not sent");
    strcpy(record->direction, "transmitted");
    break;
  case 3:
    strcpy(record->status_str, "unread");
    strcpy(record->direction, "received");
    break;
  case 4:
    strcpy(record->status_str, "read");
    strcpy(record->direction, "received");
    break;
  default:
    strcpy(record->status_str, "unknown");
    break;
  }
  sprintf(record->timestamp,
      "%02d:%02d:%02d %02d/%02d/%04d",
      drivers->rtc->internal_datetime.hour,
      drivers->rtc->internal_datetime.min,
      drivers->rtc->internal_datetime.sec,
      drivers->rtc->internal_datetime.day,
      drivers->rtc->internal_datetime.month,
      drivers->rtc->internal_datetime.year);
  return record;
}

void msg_record_dump(msg_record *record) {
  path *conversation_file = path_init(
      string_add(string_add(malloc_memories_inst->user_folder, MESSAGES_DIR),
          record->contact_name));
  path *keys_file = path_init(
      string_add(string_add(malloc_memories_inst->user_folder, MESSAGES_DIR),
          string_add(record->contact_name, ".keys")));
  path_ftouch(conversation_file);
  size_t payload_size = (strlen(record->contact_name) +
                         strlen(record->direction) +
                         strlen(record->status_str) +
                         strlen(record->timestamp) + strlen(record->message) +
                         strlen("||||") + 1);
  // <contact_name>|<direction>|<status str>|<timestamp|<message>
  char *payload = (char *)malloc(payload_size);
  snprintf(payload,
      payload_size,
      "%s|%s|%s|%s|%s",
      record->contact_name,
      record->direction,
      record->status_str,
      record->timestamp,
      record->message);
  path_fwrite(keys_file, string_add(record->record_uid, "\n"), 'a');
  path_key_value_dump(conversation_file, 'a', record->record_uid, payload);
  path_free(conversation_file);
  path_free(keys_file);
  free(payload);
}

msg_record *msg_record_load(char *stringified_record, char *record_uid) {
  msg_record *record = (msg_record *)malloc(sizeof(msg_record));
  char *contact_name = strtok(stringified_record, "|");
  char *direction = strtok(NULL, "|");
  char *status_str = strtok(NULL, "|");
  char *timestamp = strtok(NULL, "|");
  char *message = strtok(NULL, "|");
  char *message_with_lfd = string_replace(message, LFD_REPLACEMENT, '\n');
  uint8_t status = 5;
  if (strcmp(status_str, "delivered") == 0)
    status = 0;
  else if (strcmp(status_str, "sent") == 0)
    status = 1;
  else if (strcmp(status_str, "not sent") == 0)
    status = 2;
  else if (strcmp(status_str, "unread") == 0)
    status = 3;
  else if (strcmp(status_str, "read") == 0)
    status = 4;
  record->message = strdup(message_with_lfd);
  free(message_with_lfd);
  record->record_uid = strdup(record_uid);
  strncpy(record->contact_name,
      contact_name,
      sizeof(record->contact_name) - 1);
  record->contact_name[sizeof(record->contact_name) - 1] = '\0';
  strncpy(record->status_str, status_str, sizeof(record->status_str) - 1);
  record->status_str[sizeof(record->status_str) - 1] = '\0';
  strncpy(record->direction, direction, sizeof(record->direction) - 1);
  record->direction[sizeof(record->direction) - 1] = '\0';
  strncpy(record->timestamp, timestamp, sizeof(record->timestamp) - 1);
  record->timestamp[sizeof(record->timestamp) - 1] = '\0';
  record->status = status;
  return record;
}

/**
 * @brief Flags a message record as read.
 *
 * @param record_uid The unique identifier of the record.
 * @param contact_name The name of the contact associated with the record.
 * @return true if the record was successfully flagged as read, false
 * otherwise.
 */
bool msg_record_flag_as_read(const char *record_uid, char *contact_name) {
  path *conversation_file = path_init(
      string_add(string_add(malloc_memories_inst->user_folder, MESSAGES_DIR),
          contact_name));
  char *stringified_record = path_key_value_get(conversation_file, record_uid);
  char *read_flagged_record = string_substring_replace(stringified_record,
      "unread",
      "read");
  path_replace_value_at_key(conversation_file,
      record_uid,
      read_flagged_record);
  if (strcmp(read_flagged_record, stringified_record) == 0) {
    free(stringified_record);
    free(read_flagged_record);
    path_free(conversation_file);
    return false;
  }
  free(read_flagged_record);
  free(stringified_record);
  path_free(conversation_file);
  return true;
}

void msg_record_print(msg_record *record) {
  printf("Message: %s\n", record->message);
  printf("Record UID: %s\n", record->record_uid);
  printf("Contact Name: %s\n", record->contact_name);
  printf("Status: %d (%s)\n", record->status, record->status_str);
  printf("Direction: %s\n", record->direction);
  printf("Timestamp: %s\n", record->timestamp);
  printf("--------------------------------------------------\n");
}

void msg_record_free(msg_record *record) {
  if (record) {
    free(record->message);
    free(record->record_uid);
    free(record);
  }
}
