// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Antonio Del Cogliano

#include "apps/notes/notes.h"

#include <string.h>

#include "apps/text_editor/text_editor.h"
#include "core/components/sys_paths_manager.h"
#include "core/data_structures/string_list.h"
#include "core/tools/options_gen.h"
#include "core/utils/path.h"
#include "core/utils/utils.h"

void take_note();
void edit_or_delete_note(const char *note_name);
void open_existing_note(const char *note_name);
bool note_exists(const char *note_name);

void enter_notes_submenu() {
  str_list *notes = str_list_init();
  str_list *existing_notes;
  str_list *options;
  options_page *notes_page;
  str_list_append(notes, "+ new note");
  while (true) {
    existing_notes = path_listdir(sys_paths->dirs->notes_path);
    options = str_list_extend(notes, existing_notes);
    notes_page = options_page_init("Notes menu", options);
    char *buf = options_page_launch(notes_page);
    if (strcmp(buf, "+ new note") == 0)
      take_note();
    else if (strcmp(buf, "") == 0)
      break;
    else
      edit_or_delete_note(buf);
    str_list_free(existing_notes);
    options_page_free(notes_page);
  }
  str_list_free(notes);
  str_list_free(existing_notes);
  options_page_free(notes_page);
}

void take_note() {
  text_editor *name_editor = text_editor_launch("# Type in the note name",
      true);
  char *title_buf = text_editor_get_buf(name_editor);
  text_editor_kill(name_editor);
  if (note_exists(title_buf)) {
    free(title_buf);
    print_usr_error("A note with this name\nalready exists!");
    return;
  }
  path *note_path = path_init(title_buf);
  path *note_full_path = path_concat(sys_paths->dirs->notes_path, note_path);
  text_editor *note_editor = text_editor_launch("# Type in the note content",
      true);
  char *note_buf = text_editor_get_buf(note_editor);
  text_editor_kill(note_editor);
  path_fwrite(note_full_path, note_buf, 'w');
  free(title_buf);
  free(note_buf);
  path_free(note_path);
  path_free(note_full_path);
}

void edit_or_delete_note(const char *note_name) {
  str_list *options = str_list_init();
  str_list_append(options, "edit");
  str_list_append(options, "delete");
  options_page *editordelete_page = options_page_init((char *)note_name,
      options);
  char *buf = options_page_launch(editordelete_page);
  if (strcmp(buf, "edit") == 0)
    open_existing_note(note_name);
  else if (strcmp(buf, "delete") == 0) {
    sleep_ms(200);
    str_list *yesno = str_list_init();
    str_list_append(yesno, "yes");
    str_list_append(yesno, "no");
    options_page *yesno_page = options_page_init("Are you sure?", yesno);
    char *yesno_buf = options_page_launch(yesno_page);
    if (strcmp(yesno_buf, "no") == 0 || strcmp(yesno_buf, "") == 0) {
      options_page_free(yesno_page);
      options_page_free(editordelete_page);
      return;
    }
    options_page_free(yesno_page);
    path *file_path = path_init(note_name);
    path *file_full_path = path_concat(sys_paths->dirs->notes_path, file_path);
    path_fdelete(file_full_path);
    path_free(file_path);
    path_free(file_full_path);
  }
  options_page_free(editordelete_page);
}

void open_existing_note(const char *note_name) {
  path *note_path = path_init(note_name);
  path *note_full_path = path_concat(sys_paths->dirs->notes_path, note_path);
  str_list *note_content = path_fread(note_full_path);
  char *note_content_str = str_list_concat(note_content, '\n');
  text_editor *note_editor = text_editor_launch(note_content_str, false);
  char *buf = text_editor_get_buf(note_editor);
  text_editor_kill(note_editor);
  path_fwrite(note_full_path, buf, 'w');
  path_free(note_path);
  path_free(note_full_path);
  str_list_free(note_content);
  free(note_content_str);
  free(buf);
}

bool note_exists(const char *note_name) {
  str_list *existing_notes = path_listdir(sys_paths->dirs->notes_path);
  for (size_t i = 0; i < existing_notes->len; i++)
    if (strcmp(str_list_get(existing_notes, i), note_name) == 0) {
      str_list_free(existing_notes);
      return true;
    }
  str_list_free(existing_notes);
  return false;
}
