<!-- markdownlint-disable MD024 -->
# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Add note app
- Handle notification using core1

### Fixed

- Fix memory leak when providing wrong password to enter terminal

## leakybob [1.14.0] - 2025-08-13

### Added

- Further improvements of the path api
- Implement serial CLI with the 'serial' command
- Implement full-working system reset
- Print out command with whole description when
  using 'help' command in serial CLI
- Require password to enter terminal
- Make loading screen display firmware and hardware version
- Make loading screen on until the system is fully initialized
- Tutorial enhancements

### Fixed

- Make early call to end_loading_screen() when first boot
  to avoid race condition for the screen between the two cores

### Changed

- Create 'app' folder to clearly distinguish the code between
the 'core' and the applications that use the core stuff to make things.

## leakybob [1.6.0] - 2025-08-05

### Added

- Implement terminal with basic unix commands

## leakybob [1.5.1] - 2025-07-25

### Added

- Introduce sys_paths_manager to better manage fixed system paths
- Improved path api
- Improve battery percentage estimation, moves to a 1 - 4 bar bitmaps (reading crude ADC from vsys does not gives enought accuracy for a real percentage estimation)
- Make ping and pong stuff work
- Allow the user to exit from the timer before the time has elapsed

### Removed

- Remove collector for string_add strings

### Fixed

- Solve some lingering memory leaks

## leakybob [1.0.0] - 2025-07-06

### Fixed

- Solve the greatest bug of them all, so no more random crashes during idle time, first stable version

## nightlyeva [0.24.0] - 2025-06-24

### Added

- Make notifications number effectively increase and decrease based on the status of the received messages
- Add a function to flag a message as read (msg_record_flag_as_read())
- Add a function to replace value at key in a key~value file inside sdcard.c (needed to flag messages records as read)
- Extend value at key replacement as a function to path.c
- Add string_substring_replace() function to utils
- Add a led blink every sys_mainloop 10 iterations to have an immediate visual cue to see if the system is still alive

### Fixed

- Solve lingering bugs in message_manager
- Make Update conversations file stops freeing outer msg to avoid segfaulting in certain cases (ie when the user sends another message after a first one)
- Improve ULMP internals since they were causing crashes in the system
- Improve memory management, this work was mainly done to solve the most terrible of all bugs (bug01) that was causing random crashes during runtime, this changes are now put to test to ensure they have solved the problem

### Changed

- Rename ulmp protocol from ulcp

## nightlyeva [0.18.0] - 2025-06-23

### Added

- Implement paths
- Add chunks in message reading to make fetching time non-infinite in case of a large number of message records in storage
- Add haptics
- Enable messages to be read using the read messages ULMP submenu

### Fixed

- Add count_time in Watchdog_2040.c to inspect latent crashes
- Solve: message with a linefeed will cause just the last line to be read
- Solve annoying memory leak in read_messages()
- Add collector for string_add string pointers, now reading messages no longer causes memory leaks

### Changed

- Split project in multiple subdir

## nightlyeva [0.14.0] - 2025-06-01

### Added

- Improve message handling: Incoming messages can now be displayed directly, and message acknowledgments trigger notifications immediately.
- Enhance text editor: You can preload any text at launch, not just placeholders.
- New terminal foundation: Added a basic terminal framework for future development.
- User input & watchdog improvements: Added wait_for_user_input() and improved startup flow to ensure tutorial completion and proper ULMP address generation.
- LoRa communication streamline: Acknowledgments are now sent and handled more cleanly via lora_send_ack().

## nightlyeva [0.9.0] - 2025-05-30

### Added

- Add device.h for general configurations
- Add scrolling for options_gen (menus)
- Implement graphs for AQI
- Add Malloc the mascot tutorial
- Resizes icons bitmaps to fit yellow part of the screen (first two rows for a total of 16 pixels)
- Add remove contact function to message manager
- Add title in options_gen so that option pages (menus) can have a title
- Add option to rotate joystick axis for different mounting configurations
- Add Bootup screen

### Fixed

- Solve bugs 00
- Fix bug 03
- Fix bug 04
- Fix huge memory leak when freeing a string list
- Fix options generation

## nightlyeva [0.4.0] - 2025-04-11

### Added

- Add virtual keyboard
- Add text editor
- Implement joystick
- Make lora and ulcp fully functional (with acks)

### Fixed

- Fix joystick button

### Changed

- Divide project in subfolders

## nightlyeva [0.1.0] - 2025-02-26

### Added

- Make send_message really works
- add a function to send row bytes in sx1278
- Implement START END MSG of ULCP protocol
- make lora message received callback parametric
- implement hardware drivers
