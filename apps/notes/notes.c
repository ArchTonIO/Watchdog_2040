// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Antonio Del Cogliano

#include "apps/notes/notes.h"

#include "apps/text_editor/text_editor.h"
#include "core/components/sys_paths_manager.h"
#include "core/data_structures/string_list.h"
#include "core/tools/crud_list.h"
#include "core/utils/path.h"
#include "core/utils/utils.h"

void take_note(crud_list *list);
void open_existing_note(crud_list *list, const char *note_name);

void notes_launch() {
  crud_list list;
  list.name = "Notes";
  list.items_category_name = "note";
  list.workdir = sys_paths->dirs->notes_path;
  list.create_as_dir = false;
  list.create_callback = take_note;
  list.read_update_callback = open_existing_note;
  list.delete_callback = delete_item_basic;
  list.flag_callback = NULL;
  launch_crud_list(&list);
}

void take_note(crud_list *list) {
  text_editor *name_editor = text_editor_launch("# Type in the note name",
      true);
  char *name_buf = text_editor_get_buf(name_editor);
  text_editor_kill(name_editor);
  if (item_exists(list, name_buf)) {
    free(name_buf);
    print_usr_error("A note with this name\nalready exists!");
    return;
  }
  text_editor *note_editor = text_editor_launch("# Type in the note content",
      true);
  char *note_buf = text_editor_get_buf(note_editor);
  text_editor_kill(note_editor);
  print_loading("Saving note...");
  create_or_overwrite_item(list, name_buf, note_buf);
  free(name_buf);
  free(note_buf);
}

void open_existing_note(crud_list *list, const char *note_name) {
  path *note_path = get_item_path(list, note_name);
  str_list *note_content = path_fread(note_path);
  char *note_content_str = str_list_concat(note_content, '\n');
  text_editor *note_editor = text_editor_launch(note_content_str, false);
  char *buf = text_editor_get_buf(note_editor);
  text_editor_kill(note_editor);
  path_fwrite(note_path, buf, 'w');
  path_free(note_path);
  str_list_free(note_content);
  free(note_content_str);
  free(buf);
}
