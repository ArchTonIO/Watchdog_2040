#ifndef MSG_RECORD_H
#define MSG_RECORD_H

#include <stdbool.h>
#include <stdint.h>

#include "apps/msg_manager/contacts_manager.h"

#define RECORD_UID_LENGTH 8
#define LFD_REPLACEMENT '`'

typedef struct {
  char *message;
  char *record_uid;
  char contact_name[MAX_CONTACT_NAME_LENGTH];
  uint8_t status; /*0: delivered, 1: sent, 2: not sent 3: "unread", 4: "read"*/
  char direction[12];  /*"transmitted" or "received"*/
  char status_str[10]; /*"delivered", "sent", "not sent", "unread", "read"*/
  char timestamp[20];  /*hh:mm:ss dd/mm/yyyy*/
} msg_record;

msg_record *
msg_record_init(uint16_t contact_addr, char *message, uint8_t status);
void msg_record_dump(msg_record *record);
msg_record *msg_record_load(char *stringified_record, char *record_uid);
bool msg_record_flag_as_read(const char *record_uid, char *contact_name);
void msg_record_free(msg_record *record);
void msg_record_print(msg_record *record);
#endif