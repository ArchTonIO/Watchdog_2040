<!-- markdownlint-disable MD024 -->
# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## leakybob [1.49.0]

### Added

- Password manager CLI (password manager can be used trough the serial CLI with the "pwd_man" command)
- Home screen averaged MCU temperature (10 samples)

### Fixed

- Updated tutorial (due to new location of RX continuous toggle)

## leakybob [1.48.0]

### Added

- Graphical system launcher
- System tray with fast settings and flashlight toggle
- Calendar app with events and remainders that gets pushed to the home page
- MCU temperature in the home page
- Horizontal scroll for entries longer than 21 characters in any list
- Tutorial: explanation of rxcontinuous mode

### Fixed

- bug 24, 25 - issues in the text editor when saving files containing linefeed
- bug 26 - auto brightness and haptics were disabled by default at first boot
- bug 27 - messy prints in the sys paths creation step at first boot
- "cat" terminal command accepts spaced file names now
- fixed memory leak when going back and forth between home page and main launcher

## leakybob [1.42.5]

### Added

- More alarms can be created and saved

### Fixed

- bug 20 - Alarm is now working properly
- bug 21 - Timer is now working properly
- bug 22 - Battery status screen is now working properly
- bug 23 - Joystick check screen x axis flipped

## leakybob [1.42.0]

### Added

- External RTC (DS3231) and CMOS battery, now time never gets lost at reboots
- Brightness of the oled screen is automatically adjusted by using a photoresistor
- Fast response for incoming messages
- Proper battery management with MCP73871, removed the necessity for an additional charging port and external charger, the battery charges via the USB-C port
- Led flashlight
- Reboot to bootsel directly into system app options
- Shortcut to enter serial CLI
- Settings to enable/disable auto screen brightness and haptic feedback, brightness can be manually set.

### Fixed

- System is more stable duo to less use of heap memory, malloc and free operations
- MicroSD baud rate cranked up so use of i/o heavy apps feels more fluid
- bug 18, bug 19 (password manager and text editor bugs, read buglist.txt for details)

### Changed

- Launchers are more neatly grouped, improving usability.
- ULMP app renamed to messaging
- Automatic online scan at messaging time removed due to long waiting time
- Online contacts scan option is back

## leakybob [1.34.0]

### Added

- Continuous rx mode can be enabled/disabled from an home screen shortcut

### Fixed

- Fixed bugs 14, 15, 16, 17 (all related to alarm and lora module)

### Changed

- Not fully implemented apps are removed from the main launcher
- Made mainloop more lightweight, when rx continuous is disabled more battery gets saved

## leakybob [1.33.0]

### Added

- Automatic power down of ens160 module when in standby-mode to save battery (dramatically)
- RSSI estimation at ping time - looking for online contacts is now automatically done when selecting the "write message" ULMP app entry, that is going to only show online contacts with their relative RSSI
- Password manager App, protected with master password that saves credentials to microSD with aes128-ctr encryption
- Todo App

### Fixed

- Battery percentage estimation is roughly better

### Changed

- Maxed out LoRa transmitting power
- Improved haptic feedback, it can also be turned-off by user.
- Joystick dead zones and axis orientation fitted for WD_2040 V1.0 REV3

## leakybob [1.26.0]

### Added

- Add note app
- Handle notification using core1
- MIT license
- Haptic feedback for long joystick press
- Centralization and enhancements of contacts management
- Change "serial" and "exit" terminal commands with "ser" and "esc"
- Add "reboot" and "bootsel" commands to terminal
- Standardize screen logs
- Joystick to the left in the home screen will lock screen
- Icons to launcher entries
- Flashlight
- Onboard notifications led support

### Fixed

- Fix:  memory leak when providing wrong password to enter terminal (was bug 11)
- Fix: if a message arrives when the recipient is displaying the notification screen from the previously arrived message, the new message will not get delivered (was bug 10)
- Fix: messages are not correctly delivered if receiving user is in one of the menu (is interacting with the device outside of home screen) (was bug 07)
- Fix: alarm-related crashes

### Changed

- Notification system is entirely managed by core 1

### Removed

- Messages are no longer received and stored when sent from unknown address

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
- Improve battery percentage estimation, moves to a 1 - 4 bar bitmaps (reading crude ADC from vsys does not gives enough accuracy for a real percentage estimation)
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

- Divide project in sub-folders

## nightlyeva [0.1.0] - 2025-02-26

### Added

- Make send_message really works
- add a function to send row bytes in sx1278
- Implement START END MSG of ULCP protocol
- make lora message received callback parametric
- implement hardware drivers
