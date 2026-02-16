// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "core/components/include/malloc_mascot.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/rand.h"

#include "apps/text_editor/include/text_editor.h"
#include "apps/virtual_keyboard/virtual_keyboard.h"
#include "core/components/include/hw_manager.h"
#include "core/components/include/sys_paths_manager.h"
#include "core/data_structures/include/string_list.h"
#include "core/graphics/include/bitmaps.h"
#include "core/hardware_drivers/include/joystick.h"
#include "core/hardware_drivers/include/ssd1306.h"
#include "core/tools/include/menus.h"
#include "core/tools/include/sha_256.h"
#include "core/utils/include/path.h"
#include "core/utils/include/utils.h"

malloc_memories *malloc_memories_inst = NULL;

void start_malloc_mascot_tutorial() {
  malloc_memories *memories = malloc_memories_init();
  malloc_greets_you();
  malloc_explains_you_joystick();
  malloc_explains_you_menu();
  malloc_explains_you_text_editor();
  malloc_asks_your_name();
  malloc_asks_for_password();
  malloc_explains_you_home_screen();
  malloc_generates_ulcp_address();
  malloc_says_goodbye();
}

malloc_memories *malloc_memories_init() {
  malloc_memories *memories = (malloc_memories *)malloc(
      sizeof(malloc_memories));
  memories->ulmp_addr = 0;
  malloc_memories_inst = memories;
  return memories;
}

void right_to_continue() {
  ssd1306_print(&(drivers->ssd1306), "Right to continue ->", 0, 7, 0);
  ssd1306_show(&(drivers->ssd1306));
  joystick_update(&(drivers->joystick));
  while (joystick_get_direction(&(drivers->joystick)) != E) {
    joystick_update(&(drivers->joystick));
    sleep_ms(10);
  }
}

void clear_text_area() {
  for (uint8_t i = 0; i < 8; i++)
    ssd1306_print(&(drivers->ssd1306), "                 ", 3, i, 0);
  ssd1306_print(&(drivers->ssd1306), "                     ", 0, 7, 0);
  ssd1306_show(&(drivers->ssd1306));
}

void clear_text_area_reduced() {
  for (uint8_t i = 2; i < 8; i++)
    ssd1306_print(&(drivers->ssd1306), "                 ", 3, i, 0);
  ssd1306_print(&(drivers->ssd1306), "                     ", 0, 7, 0);
  ssd1306_show(&(drivers->ssd1306));
}

void malloc_greets_you() {
  ssd1306_draw_bitmap(&(drivers->ssd1306), 0, 19, malloc_saying_hi, 26, 28, 0);
  ssd1306_print_gradually(&(drivers->ssd1306), "Ciao!", 4, 0, 0);
  sleep_ms(50);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Io sono Malloc e\n"
      "mi occupo di\n"
      "tutto qui dentro",
      4,
      1,
      0);
  right_to_continue();
  clear_text_area();
}

void malloc_explains_you_joystick() {
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      19,
      malloc_with_both_eyes,
      26,
      28,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Per iniziare\n"
      "impara ad usare\n"
      "il joystick\n",
      4,
      0,
      0);
  right_to_continue();
  clear_text_area();
}

void malloc_explains_you_menu() {
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      19,
      malloc_with_glasses,
      26,
      28,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Per interagire\n"
      "con i menu a\n"
      "lista, basta\n"
      "andare su e giu\n"
      "con la levetta.\n"
      "vai a destra\n"
      "per selezionare.",
      4,
      0,
      0);
  right_to_continue();
  clear_text_area();
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Oppure vai a\n"
      "sinistra per\n"
      "tornare alla\n"
      "pagina\n"
      "precedente.",
      4,
      0,
      0);
  right_to_continue();
  clear_text_area();
  sleep_ms(200);
  display_tutorial_menu();
  clear_text_area();
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      19,
      malloc_with_both_eyes,
      26,
      28,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Bene vedo che\n"
      "hai il pollice\n"
      "opponibile!",
      4,
      0,
      0);
  right_to_continue();
  clear_text_area();
}

void malloc_explains_keyboard_commands() {
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Sulla tastiera\n"
      "virtuale, oltre\n"
      "che a lettere\n"
      "numeri e simboli\n"
      "troverai alcuni\n"
      "tasti speciali.",
      4,
      0,
      0);
  right_to_continue();
  clear_text_area();
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      27,
      0,
      (uint8_t *)NEWLINE_PIXELS,
      8,
      8,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306), "-> Invio", 5, 0, 0);
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      27,
      8,
      (uint8_t *)BACKSPACE_PIXELS,
      8,
      8,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306), "-> Backspace", 5, 1, 0);
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      27,
      16,
      (uint8_t *)SHIFT_PIXELS,
      8,
      8,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306), "-> Shift", 5, 2, 0);
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      27,
      24,
      (uint8_t *)SPACE_PIXELS,
      8,
      8,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306), "-> Spazio", 5, 3, 0);
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      27,
      32,
      (uint8_t *)UPPERCASE_PIXELS,
      8,
      8,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306), "-> Maiuscolo", 5, 4, 0);
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      27,
      40,
      (uint8_t *)NAVIGATE_PIXELS,
      8,
      8,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306), "-> Naviga testo", 5, 5, 0);
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      27,
      48,
      (uint8_t *)END_INPUT_PIXELS,
      8,
      8,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306), "-> Salva e esci", 5, 6, 0);
  right_to_continue();
  clear_text_area();
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      19,
      malloc_with_both_eyes,
      26,
      28,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Se ti perdi\n"
      "e non sai come\n"
      "uscire, il\n"
      "simbolo per\n"
      "farlo si trova\n"
      "sempre IN BASSO\n"
      "A DESTRA.",
      4,
      0,
      0);
  right_to_continue();
  clear_text_area();
}

void malloc_explains_you_text_editor() {
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      19,
      malloc_with_glasses,
      26,
      28,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Abituati all'\n"
      "editor di testo\n"
      "adesso.",
      4,
      0,
      0);
  right_to_continue();
  clear_text_area();
  malloc_explains_keyboard_commands();
  text_editor *editor = text_editor_launch("# Try the text editor", true);
  char *text = text_editor_get_buf(editor);
  text_editor_kill(editor);
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      19,
      malloc_the_mascot,
      22,
      28,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306), "Sai scrivere!", 4, 0, 0);
  sleep_ms(50);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Bravo impari\n"
      "in fretta!",
      4,
      1,
      0);
  right_to_continue();
  clear_text_area();
}

void malloc_asks_your_name() {
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      19,
      malloc_with_pointy_eyes,
      26,
      28,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Ma veniamo al\n"
      "tuo nome:\n"
      "Come vuoi essere\n"
      "chiamato?",
      4,
      0,
      0);
  right_to_continue();
  clear_text_area();
  char *name;
  while (1) {
    text_editor *editor = text_editor_launch("# Type in your name", true);
    name = text_editor_get_buf(editor);
    if (strlen(name) <= 16) {
      text_editor_kill(editor);
      break;
    }
    text_editor_kill(editor);
    ssd1306_draw_bitmap(&(drivers->ssd1306),
        0,
        19,
        malloc_with_both_eyes,
        26,
        28,
        0);
    ssd1306_print_gradually(&(drivers->ssd1306),
        "Uoah! nome\n"
        "troppo lungo!,\n"
        "Rimani entro\n"
        "i 14 caratteri\n"
        "di lunghezza.",
        4,
        0,
        0);
    right_to_continue();
    clear_text_area();
  }
  strcpy(malloc_memories_inst->username, name);
  ssd1306_draw_bitmap(&(drivers->ssd1306), 0, 19, malloc_saying_hi, 26, 28, 0);
  ssd1306_print_gradually(&(drivers->ssd1306), "Ciao", 4, 0, 0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      malloc_memories_inst->username,
      4,
      1,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "!",
      strlen(malloc_memories_inst->username) + 4,
      1,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Sono felice di\n"
      "conoscerti.",
      4,
      2,
      0);
  char *username_no_lfd = string_remove_linefeed(
      malloc_memories_inst->username);
  char *username_no_spaces = string_replace(username_no_lfd, ' ', '_');
  strcpy(malloc_memories_inst->username, username_no_spaces);
  free(username_no_lfd);
  free(username_no_spaces);
  right_to_continue();
  clear_text_area();
}

void get_password() {
  text_editor *editor = text_editor_launch("# Type in your password", true);
  char *password = text_editor_get_buf(editor);
  text_editor_kill(editor);
  if (strlen(password) > 0 && strlen(password) <= 20) {
    strcpy(malloc_memories_inst->user_password, password);
    return;
  } else {
    ssd1306_draw_bitmap(&(drivers->ssd1306),
        0,
        19,
        malloc_with_both_eyes,
        26,
        28,
        0);
    ssd1306_print_gradually(&(drivers->ssd1306),
        "Uoah! password\n"
        "troppo lunga!\n"
        "Rimani entro\n"
        "i 19 caratteri\n"
        "di lunghezza.",
        4,
        0,
        0);
    right_to_continue();
    clear_text_area();
    get_password();
  }
}

void malloc_asks_for_password() {
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      19,
      malloc_with_pointy_eyes,
      26,
      28,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Ora ti chiedo\n"
      "di impostare una\n"
      "password per\n"
      "le operazioni\n"
      "importanti.",
      4,
      0,
      0);
  right_to_continue();
  clear_text_area();
  get_password();
}

void malloc_explains_you_home_screen() {
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      19,
      malloc_the_mascot,
      22,
      28,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306), "Perfetto !", 4, 0, 0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Ora ti spiego\n"
      "come funziona\n"
      "la tua\n"
      "home screen",
      4,
      2,
      0);
  right_to_continue();
  clear_text_area();
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      19,
      malloc_with_glasses,
      26,
      28,
      0);
  ssd1306_draw_bitmap(&(drivers->ssd1306), 0, 0, battery_4_bars, 21, 16, 0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "In alto a \n"
      "sinistra trovi\n"
      "lo stato di\n"
      "carica della\n"
      "batteria.",
      4,
      2,
      0);
  right_to_continue();
  clear_text_area_reduced();
  ssd1306_draw_bitmap(&(drivers->ssd1306), 21, 0, microsd_working, 21, 16, 0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Di fianco\n"
      "c'e' l'icona\n"
      "della microsd",
      4,
      2,
      0);
  right_to_continue();
  clear_text_area_reduced();
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Se la microsd\n"
      "e' inserita e\n"
      "funziona\n"
      "correttamente\n"
      "vedi questa",
      4,
      2,
      0);
  right_to_continue();
  clear_text_area_reduced();
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      21,
      0,
      microsd_not_working,
      21,
      16,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Se la microsd\n"
      "non e' inserita\n"
      "o non funziona\n"
      "correttamente\n"
      "vedi questa",
      4,
      2,
      0);
  right_to_continue();
  clear_text_area_reduced();
  ssd1306_draw_bitmap(&(drivers->ssd1306), 42, 0, lora_working, 21, 16, 0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Poi c'e' l'icona\n"
      "del modulo\n"
      "LoRa.",
      4,
      2,
      0);
  right_to_continue();
  clear_text_area_reduced();
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Se il modulo\n"
      "LoRa funziona\n"
      "correttamente\n"
      "vedi questo",
      4,
      2,
      0);
  right_to_continue();
  clear_text_area_reduced();
  ssd1306_draw_bitmap(&(drivers->ssd1306), 42, 0, lora_not_working, 21, 16, 0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Se il modulo\n"
      "LoRa non\n"
      "funziona\n"
      "correttamente\n"
      "vedi questo",
      4,
      2,
      0);
  right_to_continue();
  clear_text_area_reduced();
  ssd1306_draw_bitmap(&(drivers->ssd1306), 63, 0, ens160_working, 21, 16, 0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Poi c'e' l'icona\n"
      "del sensore di\n"
      "qualita' dell'\n"
      "aria ENS160.",
      4,
      2,
      0);
  right_to_continue();
  clear_text_area_reduced();
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Se il sensore\n"
      "ENS160 funziona\n"
      "correttamente\n"
      "vedi questo",
      4,
      2,
      0);
  right_to_continue();
  clear_text_area_reduced();
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      63,
      0,
      ens160_not_working,
      21,
      16,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Se il sensore\n"
      "ENS160 non\n"
      "funziona\n"
      "correttamente\n"
      "vedi questo",
      4,
      2,
      0);
  right_to_continue();
  clear_text_area_reduced();
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      84,
      0,
      num_msg_bitmaps[0],
      21,
      16,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Qui trovi\n"
      "l'icona delle\n"
      "notifiche.",
      4,
      2,
      0);
  right_to_continue();
  clear_text_area_reduced();
  ssd1306_draw_bitmap(&(drivers->ssd1306), 105, 0, alarm_enabled, 21, 16, 0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "In alto a\n"
      "destra trovi\n"
      "l'icona della\n"
      "sveglia",
      4,
      2,
      0);
  right_to_continue();
  clear_text_area_reduced();
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Se la sveglia\n"
      "e' attiva\n"
      "vedi questo",
      4,
      2,
      0);
  right_to_continue();
  clear_text_area_reduced();
  ssd1306_draw_bitmap(&(drivers->ssd1306), 105, 0, alarm_disabled, 21, 16, 0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Se la sveglia\n"
      "non e' attiva\n"
      "vedi questo",
      4,
      2,
      0);
  right_to_continue();
  clear_text_area_reduced();
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Dalla home puoi\n"
      "andare al menu\n"
      "principale\n"
      "andando a destra\n"
      "col joystick",
      4,
      2,
      0);
  right_to_continue();
  clear_text_area_reduced();
}

uint16_t malloc_get_ulcp_address() {
  if (malloc_memories_inst->ulmp_addr == 0) {
    malloc_memories_inst->ulmp_addr = MIN_ULMP_ADDR +
                                      (get_rand_32() %
                                          (MAX_ULMP_ADDR - MIN_ULMP_ADDR));
    return malloc_memories_inst->ulmp_addr;
  } else
    return malloc_memories_inst->ulmp_addr;
}

void malloc_generates_ulcp_address() {
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      19,
      malloc_the_mascot,
      22,
      28,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Bene, direi che\n"
      "per ora non ho\n"
      "altro da dirti",
      4,
      2,
      0);
  sleep_ms(100);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "...Ma ho un\n"
      "regalino per te!",
      4,
      5,
      0);
  right_to_continue();
  clear_text_area_reduced();
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      19,
      malloc_with_both_eyes,
      26,
      28,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Il tuo indirizzo\n"
      "ULMP!",
      4,
      2,
      0);
  sleep_ms(50);
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      19,
      malloc_with_pointy_eyes,
      26,
      28,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Fammi fare un po\n"
      "di spazio sullo \n"
      "schermo e spiego",
      4,
      4,
      0);
  right_to_continue();
  ssd1306_clear(&(drivers->ssd1306));
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      19,
      malloc_with_glasses,
      26,
      28,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Il tuo indirizzo\n"
      "ULMP e' un\n"
      "identificativo\n"
      "univoco che\n"
      "ti permette di\n"
      "inviare e\n"
      "ricevere",
      4,
      0,
      0);
  right_to_continue();
  clear_text_area();
  ssd1306_print_gradually(&(drivers->ssd1306),
      "messaggi a e da\n"
      "chiunque abbia\n"
      "un Watchdog_2040\n"
      "(o successivi)\n"
      "e si trovi nel\n"
      "raggio di\n"
      "trasmissione",
      4,
      0,
      0);
  right_to_continue();
  clear_text_area();
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Si tratta di un\n"
      "numero che va\n"
      "da 10000 a 65535\n"
      "e che viene\n"
      "generato in modo\n"
      "randomico.",
      4,
      0,
      0);
  right_to_continue();
  clear_text_area();
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      19,
      malloc_with_glasses,
      26,
      28,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Il protocollo di\n"
      "comunicazione\n"
      "(transport\n"
      "layer) che\n"
      "ho inventato non\n"
      "ha bisogno di\n"
      "internet",
      4,
      0,
      0);
  right_to_continue();
  clear_text_area();
  ssd1306_print_gradually(&(drivers->ssd1306), "e si chiama:", 4, 0, 0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "U(ncomplicated)\n"
      "L(oRa)\n"
      "M(messaging)\n"
      "P(rotocol)",
      4,
      2,
      0);
  right_to_continue();
  clear_text_area();
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      19,
      malloc_with_both_eyes,
      26,
      28,
      0);
  char *addr_str = malloc(6);
  sprintf(addr_str, "%d", malloc_get_ulcp_address());
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Ma basta parlare\n"
      "di me!, ecco il\n"
      "tuo indirizzo\n"
      "ULMP:",
      4,
      0,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306), addr_str, 9, 3, 0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "Rimarra' scritto\n"
      "In basso alla\n"
      "home screen",
      4,
      4,
      0);
  right_to_continue();
  clear_text_area();
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      19,
      malloc_with_both_eyes,
      26,
      28,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "in caso ne\n"
      "avessi bisogno",
      4,
      0,
      0);
  right_to_continue();
  clear_text_area();
}

void malloc_says_goodbye() {
  ssd1306_draw_bitmap(&(drivers->ssd1306),
      0,
      19,
      malloc_with_both_eyes,
      26,
      28,
      0);
  ssd1306_print_gradually(&(drivers->ssd1306),
      "E' stata una\n"
      "bella\n"
      "chiacchierata\n"
      "(o forse un\n"
      "monologo), se\n"
      "hai bisogno di\n"
      "me, non esitare",
      4,
      0,
      0);
  right_to_continue();
  clear_text_area();
  ssd1306_print_gradually(&(drivers->ssd1306),
      "a venirmi a\n"
      "trovare\n"
      "nel menu\n"
      "principale!",
      4,
      0,
      0);
  sleep_ms(100);
  ssd1306_draw_bitmap(&(drivers->ssd1306), 0, 19, malloc_saying_hi, 26, 28, 0);
  ssd1306_print_gradually(&(drivers->ssd1306), "Ciao! :)", 4, 4, 0);
  right_to_continue();
  ssd1306_clear(&(drivers->ssd1306));
  ssd1306_show(&(drivers->ssd1306));
}

void dump_malloc_memories_to_sd() {
  strcpy(malloc_memories_inst->user_folder,
      sys_paths->dirs->user_path->abs_path);
  path_key_value_dump(sys_paths->files->malloc_memories_file,
      'w',
      "username",
      malloc_memories_inst->username);
  char addr_str[6];
  sprintf(addr_str, "%u", malloc_memories_inst->ulmp_addr);
  path_key_value_dump(sys_paths->files->malloc_memories_file,
      'a',
      "ulmp_addr",
      addr_str);
  char *hashed = get_hash(malloc_memories_inst->user_password);
  path_key_value_dump(sys_paths->files->malloc_memories_file,
      'a',
      "user_password",
      hashed);
  volatile char *p = &malloc_memories_inst->user_password[0];
  size_t pass_len = strlen(malloc_memories_inst->user_password);
  while (pass_len--)
    *p++ = 0;
  free(hashed);
  path_key_value_dump(sys_paths->files->user_file,
      'w',
      "username",
      malloc_memories_inst->username);
}

malloc_memories *load_malloc_memories_from_sd() {
  /* This function gets called when sys_manager hasn't loaded system dirs
  and files yet, so it has to build the paths by itself. */
  malloc_memories *memories = malloc(sizeof(malloc_memories));

  /* username loading from ./username file*/
  path *user_file = path_init(USER_FILE);
  char *username = path_key_value_get(user_file, "username");
  char *username_no_lfd = string_remove_linefeed(username);
  char *username_no_spaces = string_replace(username_no_lfd, ' ', '_');
  strcpy(username, username_no_spaces);
  strcpy(memories->username, username);
  printf("Username loaded: %s\n", memories->username);
  free(username_no_lfd);
  free(username_no_spaces);
  path_free(user_file);

  /* loading of the user folder */
  char *user_folder_dir_str = string_add(HOME_DIR, "/");
  char *user_dir = string_add(user_folder_dir_str, username);
  strcpy(memories->user_folder, user_dir);
  free(user_folder_dir_str);
  free(user_dir);

  /* building of malloc_memories file path */
  path *user_path = path_init(memories->user_folder);
  path *malloc_path = path_init(MALLOC_MASCOT_DIR);
  path *malloc_memories_file = path_init(MALLOC_MEMORIES_FILE);
  path *user_malloc_path = path_concat(user_path, malloc_path);
  path *malloc_memories_filepath = path_concat(user_malloc_path,
      malloc_memories_file);

  /* loading of the ulmp_addr */
  char *ulmp_addr = path_key_value_get(malloc_memories_filepath, "ulmp_addr");
  strcpy(memories->ulmp_addr_str, ulmp_addr);
  uint32_t addr;
  sscanf(ulmp_addr, "%u", &addr);
  free(ulmp_addr);
  free(username);
  memories->ulmp_addr = (uint16_t)addr;

  /* loading the user password */
  char *user_password = path_key_value_get(malloc_memories_filepath,
      "user_password");
  char *user_password_no_lfd = string_remove_linefeed(user_password);
  free(user_password);
  free(user_password_no_lfd);
  strcpy(memories->user_password_hashed, user_password_no_lfd);
  malloc_memories_inst = memories;
  path_free(malloc_memories_filepath);
  path_free(user_malloc_path);
  path_free(malloc_memories_file);
  path_free(malloc_path);
  path_free(user_path);
  return memories;
}
