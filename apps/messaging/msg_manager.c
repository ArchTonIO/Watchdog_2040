// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "apps/messaging/include/msg_manager.h"

#include <pico/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_intsup.h>

#include "apps/messaging/include/contacts.h"
#include "apps/messaging/include/msg_man_utils.h"
#include "apps/messaging/include/msg_record.h"
#include "apps/text_editor/include/text_editor.h"
#include "core/components/include/bitmaps.h"
#include "core/components/include/hw_manager.h"
#include "core/data_structures/include/string_list.h"
#include "core/hardware_drivers/include/haptics.h"
#include "core/hardware_drivers/include/joystick.h"
#include "core/hardware_drivers/include/onboard_led.h"
#include "core/hardware_drivers/include/ssd1306.h"
#include "core/tools/include/options_gen.h"
#include "core/ulmp/include/ulmp.h"
#include "core/utils/include/utils.h"

msg_manager *msg_man_inst;

void notify(uint16_t src_address);
char *compose_message();
void display_sent_message_status(uint8_t result, uint16_t dest_addr);
void update_conversation_file(uint16_t contact_addr,
    char *message,
    uint8_t status);
void display_received_message(char *name, uint16_t src_address);
str_list *get_stored_msg_uids_by_user(uint16_t contact_addr);
str_list *get_chunks_by_msg_uids(str_list *msg_uids, uint8_t chunk_size);
str_list *get_msg_uids_by_chunk(str_list *msg_uids,
    uint8_t chunk_index,
    uint8_t chunk_size);
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
  msg_man->received_msgs_count = 0;
  msg_man->ulmp_impl = lora_init(my_addr, drivers->sx1278);
  msg_man->conversation_updates_count = 0;
  lora_receive();
  contacts_list_init();
  str_list *contacts = get_all_contacts();
  contacts_list_update(contacts);
  str_list_free(contacts);
  msg_man_inst = msg_man;
  return msg_man;
}

/**
 * @brief Processes incoming messages and sends acknowledgments if necessary.
 */
void process_messages() {
  lora_send_ack(notify);
  lora_send_pong();
  if (msg_man_inst->received_msgs_count > 0)
    onboard_led_enable_blinking();
  else
    onboard_led_disable_blinking();
  sleep_ms(10);
}

/**
 *@brief push a new conversation update to the conversation_updates array, this
 *operation should be done by core1, in the hope that core0 will then update
 *the conversations file using the array later on
 *@param update The conversation update to push.
 */
void push_conversation_update(conversation_update update) {
  if (msg_man_inst->conversation_updates_count >= MAX_CONVERSATION_UPDATES) {
    msg_man_inst->conversation_updates_count = 0;
    return;
  }
  msg_man_inst->conversation_updates
      [msg_man_inst->conversation_updates_count] = update;
  msg_man_inst->conversation_updates_count++;
}

/**
 * @brief Updates the conversations based on the conversation_updates array.
 * This action should be done by core0 only as it has ownership over the
 * microsd interface
 */
void update_conversations() {
  bool should_clear = false;
  if (msg_man_inst->conversation_updates_count > 0) {
    print_loading("Updating\nconversations...");
    should_clear = true;
  }

  for (uint8_t i = 0; i < msg_man_inst->conversation_updates_count; i++) {
    update_conversation_file(
        msg_man_inst->conversation_updates[i].contact_addr,
        msg_man_inst->conversation_updates[i].message,
        msg_man_inst->conversation_updates[i].status);
    free(msg_man_inst->conversation_updates[i].message);
  }
  msg_man_inst->conversation_updates_count = 0;
  if (should_clear) {
    ssd1306_clear(&(drivers->ssd1306));
    ssd1306_show(&(drivers->ssd1306));
  }
}

void update_conversation_file(uint16_t contact_addr,
    char *message,
    uint8_t status) {
  msg_record *record = msg_record_init(contact_addr, message, status);
  msg_record_dump(record);
  msg_record_free(record);
}

void read_messages() { show_read_messages_menu(); }

void send_message_status_update_callback(uint8_t progress) {
  char progress_str[2];
  sprintf(progress_str, "%u", progress);
  ssd1306_print(&(drivers->ssd1306),
      "Message sent\n"
      "Waiting for ack...",
      0,
      0,
      false);
  char max_sending_attempts_buf[3];
  sprintf(max_sending_attempts_buf, "%u", MAX_SENDING_ATTEMPTS);
  ssd1306_print(&(drivers->ssd1306), "Attempts: ", 0, 4, false);
  ssd1306_print(&(drivers->ssd1306), progress_str, 9, 4, false);
  ssd1306_print(&(drivers->ssd1306), "/", 10, 4, false);
  ssd1306_print(&(drivers->ssd1306), max_sending_attempts_buf, 11, 4, false);
  ssd1306_show(&(drivers->ssd1306));
}

/**
 * @brief Sends a message to a selected contact.
 */
void send_message() {
  uint16_t dest_addr = choose_from_online_contacts();
  if (dest_addr == 0) {
    ssd1306_clear(&(drivers->ssd1306));
    ssd1306_show(&(drivers->ssd1306));
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

void notify(uint16_t src_address) {
  char *name = get_contact_name_by_addr_threadsafe(src_address);
  if (name == NULL) {
    free(name);
    return;
  }
  msg_man_inst->new_msg_arrived = true;
  msg_man_inst->received_msgs_count++;
  if (!msg_man_inst->should_notify) {
    free(name);
    return;
  }
  haptics_switch_performing_core();
  haptic_double_pulse();
  haptics_switch_performing_core();
  if (!ssd1306_was_mutex_support_enabled(&(drivers->ssd1306))) {
    push_conversation_update((conversation_update){.contact_addr = src_address,
        .message = strdup(msg_man_inst->ulmp_impl->rx->recv_payloads_buf),
        .status = 3});
    lora_reset_recv_buffer();
    free(name);
    return;
  }
  sleep_ms(2);
  ssd1306_get_mutex(&(drivers->ssd1306));
  display_received_message(name, src_address);
  ssd1306_release_mutex(&(drivers->ssd1306));
  free(name);
}

char *compose_message() {
  text_editor *editor = text_editor_launch("# Type in your message", true);
  char *message = text_editor_get_buf(editor);
  text_editor_kill(editor);
  return message;
}

void display_sent_message_status(uint8_t status, uint16_t dest_addr) {
  ssd1306_clear(&(drivers->ssd1306));
  char *name = get_contact_name_by_addr(dest_addr);
  ssd1306_print(&(drivers->ssd1306), "Message status:", 0, 0, false);
  ssd1306_print(&(drivers->ssd1306), "sent to", 4, 2, false);
  ssd1306_print(&(drivers->ssd1306), "recv by", 4, 4, false);
  ssd1306_print(&(drivers->ssd1306), name, 10, 2, false);
  ssd1306_print(&(drivers->ssd1306), name, 10, 4, false);
  if (status == 0) {
    ssd1306_print(&(drivers->ssd1306), "[OK]", 0, 2, false);
    ssd1306_print(&(drivers->ssd1306), "[OK]", 0, 4, false);
  } else if (status == 1) {
    ssd1306_print(&(drivers->ssd1306), "[OK]", 0, 2, false);
    ssd1306_print(&(drivers->ssd1306), "[NO]", 0, 4, false);
  } else if (status == 2) {
    ssd1306_print(&(drivers->ssd1306), "[NO]", 0, 2, false);
    ssd1306_print(&(drivers->ssd1306), "[NO]", 0, 4, false);
  }
  ssd1306_show(&(drivers->ssd1306));
  free(name);
  sleep_ms(INFO_PAGES_TIMEOUT);
}

void display_received_message(char *name, uint16_t src_address) {
  uint8_t persistency = 10;
  uint32_t start;
  start = to_us_since_boot(get_absolute_time()) / 1000000;
  ssd1306_clear(&(drivers->ssd1306));
  ssd1306_print(&(drivers->ssd1306), "New message from:", 0, 0, false);
  ssd1306_print(&(drivers->ssd1306), name, 0, 1, false);
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      22,
      22,
      message_received,
      28,
      20,
      0);
  ssd1306_draw_bitmap(&(drivers->ssd1306), 50, 24, easyarrow, 28, 20, 0);
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      78,
      22,
      message_received_open,
      28,
      20,
      0);
  ssd1306_print(&(drivers->ssd1306), "Right to read -> ", 0, 7, false);
  ssd1306_show(&(drivers->ssd1306));
  while (true) {
    if ((to_us_since_boot(get_absolute_time()) / 1000000) - start >
        persistency) {
      ssd1306_clear(&(drivers->ssd1306));
      ssd1306_show(&(drivers->ssd1306));
      push_conversation_update((conversation_update){
          .contact_addr = src_address,
          .message = strdup(msg_man_inst->ulmp_impl->rx->recv_payloads_buf),
          .status = 3});
      lora_reset_recv_buffer();
      return;
    }
    joystick_update(&(drivers->joystick));
    sleep_ms(100);
    if (joystick_get_direction(&(drivers->joystick)) == E) {
      text_editor *editor = text_editor_launch(
          msg_man_inst->ulmp_impl->rx->recv_payloads_buf,
          false);
      char *text = text_editor_get_buf(editor);
      text_editor_kill(editor);
      push_conversation_update((conversation_update){
          .contact_addr = src_address,
          .message = strdup(msg_man_inst->ulmp_impl->rx->recv_payloads_buf),
          .status = 4});
      msg_man_inst->received_msgs_count--;
      free(text);
      lora_reset_recv_buffer();
      return;
    }
  }
}

void make_bar(uint8_t value, char *out) {
  for (uint8_t i = 0; i < 5; i++)
    out[i] = (i < value) ? '*' : '-';
  out[5] = '\0';
}

/**
 * @brief Allows the user to choose from online contacts by pinging them.
 * @return The address of the selected contact, or 0 if none is selected.
 */
uint16_t choose_from_online_contacts() {
  str_list *contacts = get_all_contacts();
  str_list *results = str_list_init();
  uint16_t addr;
  print_loading("Scanning nearby\ncontacts...");
  for (uint8_t i = 0; i < contacts->len; i++) {
    addr = get_contact_addr_by_name(str_list_get(contacts, i));
    char print_str[40];
    sprintf(print_str,
        "Scanning nearby \ncontacts...[%u/%u]",
        i + 1,
        contacts->len);
    print_loading(print_str);
    uint8_t signal_strength = lora_ping(addr);
    if (signal_strength != 0) {
      char str_buf[10];
      char bar_buf[6];
      make_bar(signal_strength, bar_buf);
      sprintf(str_buf, "[%s] ", bar_buf);
      char *option = string_add(str_buf, str_list_get(contacts, i));
      str_list_append(results, option);
      free(option);
    }
    sleep_ms(10);
  }
  if (results->len == 0)
    str_list_append(results, "no one is reachable");
  options_page *page = options_page_init("Signal| Name         ", results);
  char *res = options_page_launch(page);
  str_list *res_split = string_split(res, ' ');
  char *name = str_list_get(res_split, 1);
  uint16_t address = 0;
  if (name != NULL)
    address = get_contact_addr_by_name(name);
  options_page_free(page);
  str_list_free(contacts);
  str_list_free(res_split);
  return address;
}

void enable_message_notifications() {
  msg_man_inst->should_notify = true;
  ssd1306_clear(&(drivers->ssd1306));
  ssd1306_print(&(drivers->ssd1306),
      "Notifications\n"
      "enabled",
      0,
      0,
      false);
  ssd1306_show(&(drivers->ssd1306));
  sleep_ms(INFO_PAGES_TIMEOUT);
  ssd1306_clear(&(drivers->ssd1306));
  ssd1306_show(&(drivers->ssd1306));
}

void disable_message_notifications() {
  msg_man_inst->should_notify = false;
  ssd1306_clear(&(drivers->ssd1306));
  ssd1306_print(&(drivers->ssd1306),
      "Notifications\n"
      "disabled",
      0,
      0,
      false);
  ssd1306_show(&(drivers->ssd1306));
  sleep_ms(INFO_PAGES_TIMEOUT);
  ssd1306_clear(&(drivers->ssd1306));
  ssd1306_show(&(drivers->ssd1306));
}
