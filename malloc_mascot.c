#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "data_structures/string_list.h"
#include "hardware_drivers/ssd1306.h"
#include "hardware_drivers/joystick.h"
#include "text_editor.h"
#include "bitmaps.h"
#include "options_gen.h"
#include "hw_manager.h"
#include "pico/rand.h"
#include "virtual_keyboard.h"
#include "malloc_mascot.h"

malloc_memories *malloc_memories_inst = NULL;

void start_malloc_mascot_tutorial()
{
  malloc_memories *memories = malloc_memories_init();
  malloc_greets_you();
  // malloc_explains_you_joystick();
  // malloc_explains_you_menu();
  // malloc_explains_you_text_editor();
  // malloc_asks_your_name();
  // malloc_explains_you_home_screen();
  malloc_generates_ulcp_address();
  malloc_says_goodbye();
}

malloc_memories *malloc_memories_init()
{
  malloc_memories *memories = (malloc_memories *)malloc(sizeof(malloc_memories));
  memories->ulmp_addr = 0;
  malloc_memories_inst = memories;
  return memories;
}

void press_to_continue()
{
  ssd1306_print(drivers->oled_screen, "Press to continue ->", 0, 7, 0);
  ssd1306_show(drivers->oled_screen);
  joystick_update(drivers->joystick);
  while (drivers->joystick->button_pressed == false)
  {
    joystick_update(drivers->joystick);
    sleep_ms(10);
  }
}

void clear_text_area()
{
  for (uint8_t i = 0; i < 8; i++)
    ssd1306_print(drivers->oled_screen, "                 ", 3, i, 0);
  ssd1306_print(drivers->oled_screen, "                     ", 0, 7, 0);
  ssd1306_show(drivers->oled_screen);
}

void clear_text_area_reduced()
{
  for (uint8_t i = 2; i < 8; i++)
    ssd1306_print(drivers->oled_screen, "                 ", 3, i, 0);
  ssd1306_print(drivers->oled_screen, "                     ", 0, 7, 0);
  ssd1306_show(drivers->oled_screen);
}

void malloc_greets_you()
{
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_saying_hi, 26, 28, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Ciao!", 4, 0, 0, 0);
  sleep_ms(50);
  ssd1306_print_gradually(drivers->oled_screen, "Io sono Malloc e", 4, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "mi occupo di", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "tutto qui dentro.", 4, 3, 0, 0);
  press_to_continue();
  clear_text_area();
}

void malloc_explains_you_joystick()
{
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_with_both_eyes, 26, 28, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Usa il joystick", 4, 0, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "per navigare in", 4, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "tutte le ", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "direzioni e", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "premilo per", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "selezionare.", 4, 5, 0, 0);
  press_to_continue();
  clear_text_area();
}

void malloc_explains_you_menu()
{
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_with_glasses, 26, 28, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Per interagire", 4, 0, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "con i menu a", 4, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "lista, basta", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "andare su e giu", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "con la levetta.", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Premi il tasto", 4, 5, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "per selezionare.", 4, 6, 0, 0);
  press_to_continue();
  clear_text_area();
  sleep_ms(200);
  str_list *options = list();
  lstappend(options, "option_0");
  lstappend(options, "option_1");
  lstappend(options, "option_2");
  lstappend(options, "option_3");
  lstappend(options, "option_4");
  lstappend(options, "Ho capito!");
  options_page *tutorial = options_page_init("Tutorial", options);
  options_page_launch(tutorial);
  lstdel(options);
  free(tutorial);
  clear_text_area();
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_with_both_eyes, 26, 28, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Bene vedo che", 4, 0, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "hai il pollice", 4, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "opponibile!", 4, 2, 0, 0);
  press_to_continue();
  clear_text_area();
}

void malloc_explains_keyboard_commands()
{
  ssd1306_print_gradually(drivers->oled_screen, "Sulla tastiera", 4, 0, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "virtuale, oltre", 4, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "che a lettere", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "numeri e simboli", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "troverai alcuni", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "tasti speciali.", 4, 5, 0, 0);
  press_to_continue();
  clear_text_area();
  ssd1306_draw_bitmap(drivers->oled_screen, 27, 0, NEWLINE_PIXELS, 8, 8, 0);
  ssd1306_print_gradually(drivers->oled_screen, "-> Invio", 5, 0, 0, 0);
  ssd1306_draw_bitmap(drivers->oled_screen, 27, 8, BACSPACE_PIXELS, 8, 8, 0);
  ssd1306_print_gradually(drivers->oled_screen, "-> Backspace", 5, 1, 0, 0);
  ssd1306_draw_bitmap(drivers->oled_screen, 27, 16, SHIFT_PIXELS, 8, 8, 0);
  ssd1306_print_gradually(drivers->oled_screen, "-> Shift", 5, 2, 0, 0);
  ssd1306_draw_bitmap(drivers->oled_screen, 27, 24, SPACE_PIXELS, 8, 8, 0);
  ssd1306_print_gradually(drivers->oled_screen, "-> Spazio", 5, 3, 0, 0);
  ssd1306_draw_bitmap(drivers->oled_screen, 27, 32, UPPERCASE_PIXELS, 8, 8, 0);
  ssd1306_print_gradually(drivers->oled_screen, "-> Maiuscolo", 5, 4, 0, 0);
  ssd1306_draw_bitmap(drivers->oled_screen, 27, 40, NAVIGATE_PIXELS, 8, 8, 0);
  ssd1306_print_gradually(drivers->oled_screen, "-> Naviga testo", 5, 5, 0, 0);
  ssd1306_draw_bitmap(drivers->oled_screen, 27, 48, END_INPUT_PIXELS, 8, 8, 0);
  ssd1306_print_gradually(drivers->oled_screen, "-> Salva e esci", 5, 6, 0, 0);
  press_to_continue();
  clear_text_area();
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_with_both_eyes, 26, 28, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Se ti perdi", 4, 0, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "e non sai come", 4, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "uscire, il", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "simbolo per", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "farlo si trova", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "sempre IN BASSO", 4, 5, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "A DESTRA.", 4, 6, 0, 0);
  press_to_continue();
  clear_text_area();
}

void malloc_explains_you_text_editor()
{
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_with_glasses, 26, 28, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Abituati all'", 4, 0, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "editor di testo", 4, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "adesso.", 4, 2, 0, 0);
  press_to_continue();
  clear_text_area();
  malloc_explains_keyboard_commands();
  text_editor *editor = text_editor_launch("# Try the text editor");
  char *text = text_editor_get_buf(editor);
  text_editor_kill(editor);
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_the_mascot, 22, 28, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Sai scrivere!", 4, 0, 0, 0);
  sleep_ms(50);
  ssd1306_print_gradually(drivers->oled_screen, "Bravo impari", 4, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "in fretta!", 4, 2, 0, 0);
  press_to_continue();
  clear_text_area();
}

void malloc_asks_your_name()
{
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_with_pointy_eyes, 26, 28, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Ma veniamo al", 4, 0, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "tuo nome:", 4, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Come vuoi essere", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "chiamato?", 4, 3, 0, 0);
  press_to_continue();
  clear_text_area();
  char *name;
  while (1)
  {
    text_editor *editor = text_editor_launch("# Type in your name");
    name = text_editor_get_buf(editor);
    if (strlen(name) <= 16)
    {
      text_editor_kill(editor);
      break;
    }
    text_editor_kill(editor);
    ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_with_both_eyes, 26, 28, 0);
    ssd1306_print_gradually(drivers->oled_screen, "Uoah! nome", 4, 0, 0, 0);
    ssd1306_print_gradually(drivers->oled_screen, "troppo lungo!,", 4, 1, 0, 0);
    ssd1306_print_gradually(drivers->oled_screen, "Rimani entro", 4, 2, 0, 0);
    ssd1306_print_gradually(drivers->oled_screen, "i 16 caratteri", 4, 3, 0, 0);
    ssd1306_print_gradually(drivers->oled_screen, "di lunghezza.", 4, 4, 0, 0);
    press_to_continue();
    clear_text_area();
  }
  strcpy(malloc_memories_inst->username, name);
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_saying_hi, 26, 28, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Ciao", 4, 0, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, malloc_memories_inst->username, 4, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "!", strlen(malloc_memories_inst->username) + 1, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Sono felice di", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "conoscerti.", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Andiamo avanti", 4, 5, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "con lo spiegone", 4, 6, 0, 0);
  press_to_continue();
  clear_text_area();
}

void malloc_explains_you_home_screen()
{
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_the_mascot, 22, 28, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Ora ti spiego", 4, 0, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "come funziona", 4, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "la tua", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "home screen", 4, 3, 0, 0);
  press_to_continue();
  clear_text_area();
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_with_glasses, 26, 28, 0);
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 0, battery_level_bitmaps[9], 21, 16, 0);
  ssd1306_print_gradually(drivers->oled_screen, "In alto a ", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "sinistra trovi", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "la percentuale", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "di carica della", 4, 5, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "batteria.", 4, 6, 0, 0);
  press_to_continue();
  clear_text_area_reduced();
  ssd1306_draw_bitmap(drivers->oled_screen, 21, 0, microsd_working, 21, 16, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Di fianco", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "c'e' l'icona", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "della microsd", 4, 4, 0, 0);
  press_to_continue();
  clear_text_area_reduced();
  ssd1306_print_gradually(drivers->oled_screen, "Se la microsd", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "e' inserita e", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "funziona", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "correttamente", 4, 5, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "vedi questa", 4, 6, 0, 0);
  press_to_continue();
  clear_text_area_reduced();
  ssd1306_draw_bitmap(drivers->oled_screen, 21, 0, microsd_not_working, 21, 16, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Se la microsd", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "non e' inserita", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "o non funziona", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "correttamente", 4, 5, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "vedi questa", 4, 6, 0, 0);
  press_to_continue();
  clear_text_area_reduced();
  ssd1306_draw_bitmap(drivers->oled_screen, 42, 0, lora_working, 21, 16, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Poi c'e' l'icona", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "del modulo", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "LoRa.", 4, 4, 0, 0);
  press_to_continue();
  clear_text_area_reduced();
  ssd1306_print_gradually(drivers->oled_screen, "Se il modulo", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "LoRa funziona", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "correttamente", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "vedi questo", 4, 5, 0, 0);
  press_to_continue();
  clear_text_area_reduced();
  ssd1306_draw_bitmap(drivers->oled_screen, 42, 0, lora_not_working, 21, 16, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Se il modulo", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "LoRa non", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "funziona", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "correttamente", 4, 5, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "vedi questo", 4, 6, 0, 0);
  press_to_continue();
  clear_text_area_reduced();
  ssd1306_draw_bitmap(drivers->oled_screen, 63, 0, ens160_working, 21, 16, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Poi c'e' l'icona", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "del sensore di", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "qualita' dell'", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "aria ENS160.", 4, 5, 0, 0);
  press_to_continue();
  clear_text_area_reduced();
  ssd1306_print_gradually(drivers->oled_screen, "Se il sensore", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "ENS160 funziona", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "correttamente", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "vedi questo", 4, 5, 0, 0);
  press_to_continue();
  clear_text_area_reduced();
  ssd1306_draw_bitmap(drivers->oled_screen, 63, 0, ens160_not_working, 21, 16, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Se il sensore", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "ENS160 non", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "funziona", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "correttamente", 4, 5, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "vedi questo", 4, 6, 0, 0);
  press_to_continue();
  clear_text_area_reduced();
  ssd1306_draw_bitmap(drivers->oled_screen, 84, 0, alarm_enabled, 21, 16, 0);
  ssd1306_print_gradually(drivers->oled_screen, "In alto a", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "destra trovi", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "l'icona della", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "sveglia", 4, 5, 0, 0);
  press_to_continue();
  clear_text_area_reduced();
  ssd1306_print_gradually(drivers->oled_screen, "Se la sveglia", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "e' attiva", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "vedi questo", 4, 4, 0, 0);
  press_to_continue();
  clear_text_area_reduced();
  ssd1306_draw_bitmap(drivers->oled_screen, 84, 0, alarm_disabled, 21, 16, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Se la sveglia", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "non e' attiva", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "vedi questo", 4, 4, 0, 0);
  press_to_continue();
  clear_text_area_reduced();
  ssd1306_print_gradually(drivers->oled_screen, "Dalla home puoi", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "andare al menu", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "principale", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "premendo il", 4, 5, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "joystick", 4, 6, 0, 0);
  press_to_continue();
  clear_text_area_reduced();
}

uint16_t malloc_get_ulcp_address()
{
  if (malloc_memories_inst->ulmp_addr == 0)
  {
    malloc_memories_inst->ulmp_addr = get_rand_32() % 65536;
    return malloc_memories_inst->ulmp_addr;
  }
  else
    return malloc_memories_inst->ulmp_addr;
}

void malloc_generates_ulcp_address()
{
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_the_mascot, 22, 28, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Bene, direi che", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "per ora non ho", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "altro da dirti", 4, 4, 0, 0);
  sleep_ms(100);
  ssd1306_print_gradually(drivers->oled_screen, "...Ma ho un", 4, 5, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "regalino per te!", 4, 6, 0, 0);
  press_to_continue();
  clear_text_area_reduced();
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_with_both_eyes, 26, 28, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Il tuo indirizzo", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "ULMP!", 4, 3, 0, 0);
  sleep_ms(50);
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_with_pointy_eyes, 26, 28, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Fammi fare un po", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "di spazio sullo", 4, 5, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "schermo e spiego", 4, 6, 0, 0);
  press_to_continue();
  ssd1306_clear(drivers->oled_screen);
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_with_glasses, 26, 28, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Il tuo indirizzo", 4, 0, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "ULMP e' un", 4, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "identificativo", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "univoco che", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "ti permette di", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "inviare e", 4, 5, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "ricevere ", 4, 6, 0, 0);
  press_to_continue();
  clear_text_area();
  ssd1306_print_gradually(drivers->oled_screen, "messaggi a e da", 4, 0, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "chiunque abbia", 4, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "un Watchdog_2040", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "(o successivi)", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "e si trovi nel", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "raggio di", 4, 5, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "trasmissione", 4, 6, 0, 0);
  press_to_continue();
  clear_text_area();
  ssd1306_print_gradually(drivers->oled_screen, "Si tratta di un", 4, 0, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "numero che va", 4, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "da 0 a 65535", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "e che viene", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "generato in modo", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "randomico.", 4, 5, 0, 0);
  press_to_continue();
  clear_text_area();
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_with_glasses, 26, 28, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Il protocollo di", 4, 0, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "comunicazione", 4, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "(transport", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "layer) che", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "useremo non ha", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "bisogno di", 4, 5, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "internet", 4, 6, 0, 0);
  press_to_continue();
  clear_text_area();
  ssd1306_print_gradually(drivers->oled_screen, "e l'ho inventato", 4, 0, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "ieri mentre", 4, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "facevo colazione:", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "U(ncomplicated)", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "L(oRa)", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "M(messaging)", 4, 5, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "P(rotocol)", 4, 6, 0, 0);
  press_to_continue();
  clear_text_area();
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_with_both_eyes, 26, 28, 0);
  char *addr_str = malloc(6);
  sprintf(addr_str, "%d", malloc_get_ulcp_address());
  ssd1306_print_gradually(drivers->oled_screen, "Ma basta parlare", 4, 0, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "di me!, ecco il", 4, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "tuo indirizzo", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "ULMP:", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, addr_str, 9, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Rimarra' scritto", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "In basso alla", 4, 5, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "home screen", 4, 6, 0, 0);
  press_to_continue();
  clear_text_area();
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_with_both_eyes, 26, 28, 0);
  ssd1306_print_gradually(drivers->oled_screen, "in caso ne", 4, 0, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "avessi bisogno", 4, 1, 0, 0);
  press_to_continue();
  clear_text_area();
}

void malloc_says_goodbye()
{
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_with_both_eyes, 26, 28, 0);
  ssd1306_print_gradually(drivers->oled_screen, "E' stata una", 4, 0, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "bella", 4, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "chiacchierata", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "(o forse un", 4, 3, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "monologo), se", 4, 4, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "hai bisogno di", 4, 5, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "me, non esitare", 4, 6, 0, 0);
  press_to_continue();
  clear_text_area();
  ssd1306_print_gradually(drivers->oled_screen, "a venirmi a", 4, 0, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "trovare", 4, 1, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "nel menu", 4, 2, 0, 0);
  ssd1306_print_gradually(drivers->oled_screen, "principale!", 4, 3, 0, 0);
  sleep_ms(100);
  ssd1306_draw_bitmap(drivers->oled_screen, 0, 19, malloc_saying_hi, 26, 28, 0);
  ssd1306_print_gradually(drivers->oled_screen, "Ciao! :)", 4, 4, 0, 0);
  press_to_continue();
  ssd1306_clear(drivers->oled_screen);
  ssd1306_show(drivers->oled_screen);
}

void dump_malloc_memories_to_sd()
{
  sdcard_write_key_value_to_file(
      drivers->sd_card,
      ".malloc_memories",
      'w',
      "username",
      malloc_memories_inst->username);
  char addr_str[6];
  sprintf(addr_str, "%u", malloc_memories_inst->ulmp_addr);
  sdcard_write_key_value_to_file(
      drivers->sd_card,
      ".malloc_memories",
      'w',
      "ulmp_addr",
      addr_str);
}

malloc_memories *load_malloc_memories_from_sd()
{
  malloc_memories *memories = malloc(sizeof(malloc_memories));
  char *username = sdcard_read_value_from_file(
      drivers->sd_card,
      ".malloc_memories",
      "username");
  strcpy(memories->username, username);
  free(username);
  char *ulmp_addr = sdcard_read_value_from_file(
      drivers->sd_card,
      ".malloc_memories",
      "ulmp_addr");
  strcpy(memories->ulmp_addr_str, ulmp_addr);
  uint32_t addr;
  sscanf(ulmp_addr, "%u", &addr);
  free(ulmp_addr);
  memories->ulmp_addr = (uint16_t)addr;
  malloc_memories_inst = memories;
  return memories;
}