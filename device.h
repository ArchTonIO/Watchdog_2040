#ifndef DEVICE_H
#define DEVICE_H

#define FIRST_BOOT_FILE "/.booted"
#define USER_FILE "/.user"

#define ROOT_DIR "/"
#define HOME_DIR "/home/"
#define MALLOC_MASCOT_DIR "/malloc/"
#define MESSAGES_DIR "/messages/"
#define CONTACTS_DIR "/contacts/"
#define LOGS_DIR "/logs/"
#define CONFIG_DIR "/config/"
#define NOTES_DIR "/notes/"
#define SENSORS_DIR "/sensors/"

#define MALLOC_MEMORIES_FILE MALLOC_MASCOT_DIR "malloc_memories"
#define CONTACTS_ADDR_FILE CONTACTS_DIR "contacts"
#define CONTACTS_NAMES_FILE CONTACTS_DIR "contact-names"
#define LOG_FILE LOGS_DIR "log"
#define CONFIG_FILE CONFIG_DIR "config"
#define ENS160_FILE SENSORS_DIR "ens160"
#define BATTERY_FILE SENSORS_DIR "battery"
#define MAX10102_FILE SENSORS_DIR "max10102"

#define FIRMWARE_VERSION "pre-alpha"
#define HARDWARE_VERSION "crap - 1"
#define DEVICE_NAME "Watchdog_2040"

#endif