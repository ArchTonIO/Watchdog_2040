
#include "terminal.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"

#include "components/hw_manager.h"
#include "data_structures/string_list.h"
#include "hardware_drivers/joystick.h"
#include "hardware_drivers/ssd1306.h"
#include "utils/utils.h"

void help();
void clear();
void echo();
void exit();
void cd();
void ls();
void pwd();
void mkdir();
void touch();
void rm();
void mv();
void cp();
void cat();
void head();
void tail();
void grep();
void top();
void whoami();
void ping();
void nano();

void generate_commands() {
  command help_cmd;
  strcpy(help_cmd.name, "help");
  help_cmd.callback = help;
  command clear_cmd;
  strcpy(clear_cmd.name, "clear");
  clear_cmd.callback = clear;
  command echo_cmd;
  strcpy(echo_cmd.name, "echo");
  echo_cmd.callback = echo;
  command exit_cmd;
  strcpy(exit_cmd.name, "exit");
  exit_cmd.callback = exit;
  command cd_cmd;
  strcpy(cd_cmd.name, "cd");
  cd_cmd.callback = cd;
  command ls_cmd;
  strcpy(ls_cmd.name, "ls");
  ls_cmd.callback = ls;
  command pwd_cmd;
  strcpy(pwd_cmd.name, "pwd");
  pwd_cmd.callback = pwd;
  command mkdir_cmd;
  strcpy(mkdir_cmd.name, "mkdir");
  mkdir_cmd.callback = mkdir;
  command touch_cmd;
  strcpy(touch_cmd.name, "touch");
  touch_cmd.callback = touch;
  command rm_cmd;
  strcpy(rm_cmd.name, "rm");
  rm_cmd.callback = rm;
  command mv_cmd;
  strcpy(mv_cmd.name, "mv");
  mv_cmd.callback = mv;
  command cp_cmd;
  strcpy(cp_cmd.name, "cp");
  cp_cmd.callback = cp;
  command cat_cmd;
  strcpy(cat_cmd.name, "cat");
  cat_cmd.callback = cat;
  command head_cmd;
  strcpy(head_cmd.name, "head");
  head_cmd.callback = head;
  command tail_cmd;
  strcpy(tail_cmd.name, "tail");
  tail_cmd.callback = tail;
  command grep_cmd;
  strcpy(grep_cmd.name, "grep");
  grep_cmd.callback = grep;
  command top_cmd;
  strcpy(top_cmd.name, "top");
  top_cmd.callback = top;
  command whoami_cmd;
  strcpy(whoami_cmd.name, "whoami");
  whoami_cmd.callback = whoami;
  command ping_cmd;
  strcpy(ping_cmd.name, "ping");
  ping_cmd.callback = ping;
  command nano_cmd;
  strcpy(nano_cmd.name, "nano");
  nano_cmd.callback = nano;
}

terminal *terminal_init() {
  terminal *term = (terminal *)malloc(sizeof(terminal));
  term->history = list_init();
  strcpy(term->current_command, "");
  term->keyboard = virtual_keyboard_init();
  return term;
}

void terminal_launch(terminal *term) {
  ssd1306_clear(drivers->oled_screen);
  ssd1306_show(drivers->oled_screen);
  draw_keyboard(term->keyboard);
  while (true) {
    char last_char = virtual_keyboard_read(term->keyboard);
    if (last_char == END)
      break;
    else if (last_char == BCK) {
      if (strlen(term->current_command) > 0)
        term->current_command[strlen(term->current_command) - 1] = '\0';
    } else if (last_char != NOW && last_char != NSK) {
      strncat(term->current_command, &last_char, 1);
    }
    ssd1306_clear(drivers->oled_screen);
    ssd1306_set_cursor(0, 0);
    ssd1306_print(drivers->oled_screen,
        term->current_command,
        FONT_SIZE_1,
        true);
    ssd1306_show(drivers->oled_screen);
  }
}
