# Generated Cmake Pico project file
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Watchdog_2040 C CXX ASM)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()
add_subdirectory(core/lib/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)
# Add executable. Default name is the project name, version 0.1

add_executable(Watchdog_2040
########################################################################
############################# APPS SECTION #############################
########################################################################

## AQI ##
apps/AQI/aqi.c
apps/AQI/bitmaps.c

## drawing board ##
apps/drawing_board/bitmaps.c
apps/drawing_board/board.c

## flashlight ##
apps/flashlight/bitmaps.c
apps/flashlight/flashlight.c

## messaging ##
apps/messaging/bitmaps.c
apps/messaging/contacts.c
apps/messaging/messaging.c
apps/messaging/msg_man_utils.c
apps/messaging/msg_manager.c
apps/messaging/msg_record.c

## notes ##
apps/notes/bitmaps.c
apps/notes/notes.c

## password manager ##
apps/pwd_manager/aes_ctr.c
apps/pwd_manager/bitmaps.c
apps/pwd_manager/key.c
apps/pwd_manager/pwd_manager.c

## system app ##
apps/system_app/bitmaps.c
apps/system_app/system_app.c

## terminal ##
apps/terminal/bitmaps.c
apps/terminal/terminal_commands.c
apps/terminal/terminal.c

## text editor ##
apps/text_editor/text_editor.c

## time ##
apps/time/bitmaps.c
apps/time/set_alarm.c
apps/time/set_date.c
apps/time/set_time.c
apps/time/stopwatch.c
apps/time/time_utils.c
apps/time/time.c
apps/time/timer.c

## todo ##
apps/todo/bitmaps.c
apps/todo/todo.c

## virtual keyboard ##
apps/virtual_keyboard/virtual_keyboard.c

########################################################################
############################# SYSTEM SECTION ###########################
########################################################################

## boot ##
core/boot/bootup.c

## components ##
core/components/bitmaps.c
core/components/home_page.c
core/components/hw_manager.c
core/components/malloc_mascot.c
core/components/sys_paths_manager.c

## data structures ##
core/data_structures/string_list.c

## graphics ##
core/graphics/graphic_primitives.c
core/graphics/graphs.c
core/graphics/layout.c

## hardware drivers ##
core/hardware_drivers/battery.c
core/hardware_drivers/core1.c
core/hardware_drivers/ds3231.c
core/hardware_drivers/ens160.c
core/hardware_drivers/haptics.c
core/hardware_drivers/joystick.c
core/hardware_drivers/onboard_led.c
core/hardware_drivers/rtc_time.c
core/hardware_drivers/sdcard.c
core/hardware_drivers/ssd1306.c
core/hardware_drivers/sx1278.c

## tools ##
core/tools/crud_list.c
core/tools/launcher.c
core/tools/menus.c
core/tools/options_gen.c
core/tools/sha_256.c

## ulmp ##
core/ulmp/lora_receive.c
core/ulmp/lora_send.c
core/ulmp/ulmp.c

## utils ##
core/utils/path.c
core/utils/utils.c

## entry point ##
Watchdog_2040.c
)

pico_set_program_name(Watchdog_2040 "Watchdog_2040")
pico_set_program_version(Watchdog_2040 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Watchdog_2040 0)
pico_enable_stdio_usb(Watchdog_2040 1)

# Add the standard library to the build
target_link_libraries(Watchdog_2040
        pico_stdlib hardware_i2c hardware_rtc hardware_spi hardware_adc FatFs_SPI pico_rand pico_multicore hardware_pwm)

# Add the standard include files to the build
target_include_directories(Watchdog_2040 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(Watchdog_2040)

