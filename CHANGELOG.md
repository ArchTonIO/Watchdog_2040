# Changelog

## leakybob

### leakybob [1.5.2] - 11/08/25

feat:

- Makes loading screen display firmware and hardware version
- Makes loading screen on until the system is fully initialized

### leakybob [1.5.1] - 11/08/25

feat:

- Enhances tutorial
- Adds notification icon explanation in tutorial
- Makes sssd1306_print_gradually() remember the x cursor so that it keeps the column when linefeed
- Adds more entries in the tutorial menu

fix:

- Fixes typo in virtual_keyboard BACKSPACE_PIXELS

### leakybob [1.5.0] - 11/08/25

refactor:

- Changes directory structure
- Moves all non-app files in the 'core' directory

### leakybob [1.4.0] - 10/08/25

refactor:

- Creates 'app' folder to clearly distinguish the code between
the 'core' and the applications that use the core stuff to make things.

### leakybob [1.3.0] - 05/08/25

feat:

- Implements terminal with basic unix commands

### leakybob [1.2.1] - 27/07/25

feat:

- Removes collector
- Introduces sys_paths_manager
- Makes all path concatenation use path_concat()
- Introduces path_rename()
- Implements kinda working cli feature

fix:

- Solves some lingering memory leaks

### leakybob [1.2.0] - 14/07/25

feat:

- Improves battery percentage estimation, moves to a 1 - 4 bar bitmaps (reading crude ADC from vsys does not gives enought accuracy for a real percentage estimation)

### leakybob [1.1.1] - 12/07/25

feat:

- Allows the user to exit from the timer before the time has elapsed

### leakybob [1.1.0] - 11/07/25

feat:

- Makes ping and pong stuff work
- Adds a way to escape from timer

chore:

- Removes some comments and prints

docs:

- Adds docs to lot of functions

### leakybob [1.0.0] - 06/07/25

fix:

- Solves the greatest bug of them all, so no more random crashes during idle time, first stable version

## prealphajane

### prealphajane [0.25.0] - 24/06/25

feat:

- Makes notifications number effectively increase and decrease based on the status of the received messages
- Adds a function to flag a message as read (msg_record_flag_as_read())
- Adds a function to replace value at key in a key~value file inside sdcard.c (needed to flag messages records as read)
- Extends value at key replacement as a function to path.c
- Adds string_substring_replace() function to utils
- Added a led blink every sys_mainloop 10 iterations to have an immediate visual cue to see if the system is still alive

fix:

- Solves lingering bugs in message_manager
- Makes Update conversations file stops freeing outer msg to avoid segfaulting in certain cases (ie when the user sends another message after a first one)
- Improves ULMP internals since they were causing crashes in the system
- Improves memory management, this work was mainly done to solve the most terrible of all bugs (bug01) that was causing random crashes during runtime, this changes are now put to test to ensure they have solved the problem

refactor:

- Renames ulmp protocol from ulcp

### prealphajane [0.24.0] - 23/06/25

feat:

- Adds msg_man_utils to handle reading messages jobs,

fix:

- Adds count_time in Watchdog_2040.c to inspect latent crashes

### prealphajane [0.23.2] - 22/06/25

feat:

- Adds chunks in message reading to make fetching time non-infinite in case of a large number of message records in storage

### prealphajane [0.23.1] - 21/06/25

fix:

- Solves:message with a linefeed will cause just the last line to be read

### prealphajane [0.23.0] - 21/06/25

feat:

- Adds haptics

### prealphajane [0.22.1] - 19/06/25

fix:

- Solves annoying memory leak in read_messages()

### prealphajane [0.22.0] - 16/06/25

refactor:

- Creates contacts_manager as msg_manager child to have a more clear separation of responsibilities

### prealphajane [0.21.0] - 16/06/25

fix:

- Adds collector for string_add string pointers, now reading messages no longer causes memory leaks

### prealphajane [0.20.0] - 14/06/25

feat:

- Enables messages to be read using the read messages ULMP submenu

### prealphajane [0.19.1] - 09/06/25

fix:

- Fixes and orders all includes

### prealphajane [0.19.0] - 09/06/25

refactor:

- Splits project in multiple subdir

### prealphajane [0.18.0] - 09/06/25

feat:

- Implements paths

### prealphajane [0.17.0] - 01/06/25

feat:

- bitmaps.c: adds easyarrow bitmap to draw message notification screen
- hw_manager.c: removes include of hardware_drivers/test.h
- malloc_mascot.c: adds placeholder text parameter in calls to text_editor_launch
- menus.c: adds placeholder text parameter in calls to text_editor_launch
- msg_manager.c: adds new struct field ulmp_impl to keep received messages buffers
- msg_manager.c: now notify callback is passed directly into lora_send_ack()
- msg_manager.c: adds display_received_message function, with the option to directly read the message
- msg_manager.c: adds placeholder text parameter in calls to text_editor_launch
- terminal.c: just a starting point to build a working simple terminal
- text_editor.c: modified placeholder text so that now any text can be loaded into the editor at launch time, if is_text_placeholder param is keep on false, the text will not be treates as placeholer but kept
- utils.c: adds wait_for_user_input function
- Watchdog_2040.c: moves write_first_startup_file call below so that if the tutorial is not completed will re- start at next startup, avoid missing generation of ULMP address and related problems
- lora_receive.c: on_transac_ended_callback does not exists anymore cause callback (notify) is called at lora_send_ack() time
- ulcp.c: removes on_transac_ended_callback
- ulcp.c: calls notify inside lora_send_ack
- ulcp.c renamed lora_eventually_send_ack in lora_send_ack

### prealphajane [0.16.2] - 30/05/25

feat:

- Adds take note submenu

fix:

- Solves bugs 00 and 04

### prealphajane [0.16.1] - 30/05/25

fix:

- Fixes bug 03

### prealphajane [0.16.0] - 30/05/25

feat:

- Adds device.h for general configurations
- Adds utility functions to hardware manager
- Adds scrolling for options_gen
- Adds new submenus

### prealphajane [0.15.0] - 25/05/25

feat:

- Implements crazy graphs for AQI

### prealphajane [0.14.2] - 10/05/25

feat:

- Changes str_list functions names
- Adds tutorial menus

### prealphajane [0.14.1] - 10/05/25

fix:

- Fixes huge memory leak when freeing a list

### prealphajane [0.14.0] - 09/05/25

feat:

- Added menus

### prealphajane [0.13.0] - 05/05/25

feat:

- Kinda-stable version

### prealphajane [0.12.1] - 25/04/25

feat:

- Completes malloc the mascot tutorial

### prealphajane [0.12.0] - 25/04/25

feat:

- Adds gli spiegoni of malloc the mascot
- Resizes icons bitmaps to fit yellow part of the screen (first two rows for a total of 16 pixels)

### prealphajane [0.11.0] - 24/04/25

feat:

- Goes crazy with bootup things

### prealphajane [0.10.0] - 21/04/25

feat:

- Adds remove contact function to message manager
- Adds title in options_gen so that option pages can have a title

refactor:

- Removes commented code

### prealphajane [0.9.0] - 21/04/25

feat:

- Adds option to rotate joystick axis for different mounting configurations

fix:

- Fixes options gen
- Removes magic numbers from joystick_get_direction

### prealphajane [0.8.0] - 11/04/25

feat:

- Completes text editor

### prealphajane [0.7.4] - 08/04/25

feat:

- Makes text editor basic functionalities all work well

### prealphajane [0.7.3] - 04/04/25

fix:

- Fixes text editor bugs

### prealphajane [0.7.2] - 04/04/25

feat:

- Improves keyboard look by using custom symbols

fix:

- Fixes up screen draw_bitmap function

### prealphajane [0.7.1] - 28/03/25

feat:

- Makes downscrolling and scrollbar fully functional

### prealphajane [0.7.0] - 28/03/25

refactor:

- Separates virtual keyboard from text editor

### prealphajane [0.6.2] - 26/03/25

feat:

- Adds proper caps lock and shift

### prealphajane [0.6.1] - 26/03/25

feat:

- Adds diagonal selection

### prealphajane [0.6.0] - 26/03/25

feat:

- Makes virtual keyboard work nicely

### prealphajane [0.5.3] - 26/03/25

feat:

- Adds uppercase and lowercase toggling, backspace, flashing cursor, space, and multiline support

### prealphajane [0.5.2] - 25/03/25

feat:

- Works on virtual keyboard (wip)

### prealphajane [0.5.1] - 22/03/25

fix:

- Fixes joystick button

### prealphajane [0.5.0] - 22/03/25

feat:

- Implements joystick

### prealphajane [0.4.1] - 21/03/25

feat:

- Does lot of tests, seems that msg_manager kinda works but input functions using serial are screwing things up

### prealphajane [0.4.0] - 07/03/25

feat:

- Makes lora and ulcp fully functional

### prealphajane [0.3.0] - 03/03/25

feat:

- Makes ulcp transaction fully work (with acks)

### prealphajane [0.2.0] - 27/02/25

refactor:

- Divides in subfolders

### prealphajane [0.1.4] - 27/02/25

feat:

- Modifies ssd1306_print call

### prealphajane [0.1.3] - 27/02/25

feat:

- adds inverted mode to visualize characters in  black fg on withe bg
- adds MAX_X CHAR and MAX_Y_CHAR

docs:

- adds some doc

### prealphajane [0.1.2] - 27/02/25

docs:

- adds some doc

### prealphajane [0.1.1] - 26/02/25

feat:

- Makes send_message really works
- adds a function to send row bytes in sx1278

### prealphajane [0.1.0] - 23/02/25

feat:

- Implements START END MSG of ULCP protocol

### prealphajane [0.0.2] - 21/02/25

feat:

- makes lora message received callback parametric

### prealphajane [0.0.1] - 21/02/25

feat:

- first commit
