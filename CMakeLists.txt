# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
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

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()
add_subdirectory(lib/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)
# Add executable. Default name is the project name, version 0.1

add_executable(Watchdog_2040
Watchdog_2040.c
data_structures/string_list.c
hardware_drivers/battery.c
hardware_drivers/ssd1306.c
hardware_drivers/rtc_time.c
hardware_drivers/ens160.c
hardware_drivers/sx1278.c
hardware_drivers/sdcard.c
hardware_drivers/tests.c
ulcp/ulcp.c
ulcp/lora_send.c
ulcp/lora_receive.c
utils.c
)

pico_set_program_name(Watchdog_2040 "Watchdog_2040")
pico_set_program_version(Watchdog_2040 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Watchdog_2040 0)
pico_enable_stdio_usb(Watchdog_2040 1)

# Add the standard library to the build
target_link_libraries(Watchdog_2040
        pico_stdlib hardware_i2c hardware_rtc hardware_spi hardware_adc FatFs_SPI pico_rand)

# Add the standard include files to the build
target_include_directories(Watchdog_2040 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(Watchdog_2040)

