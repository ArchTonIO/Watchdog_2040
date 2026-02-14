<!-- markdownlint-disable MD024 -->
# Watchdog_2040

## Aims

Building a **functional** piece of wearable technology, featuring:

1. ⌚ Time-related functionalities
2. 💬 LoRa text messages transmission
3. 📈 Air quality measurement capabilities
4. 🔧 Easily extendible firmware for future hardware components and capabilities
5. 🚀 A core API system to manage the hardware and provide basic UI elements, for fast app developing

**This project is not, and does not aim to be a smartwatch in the usual sense, for this reason:**

- There is no need to connect it to a smartphone, you already have enough notifications on that.
- Has nothing to do with your fitness routine or exercise tracking.
- Does not have a touch screen, your fingers are too big for that.
- It is not meant to be a fashion accessory, it is a tool (although a cool one).
- It is not meant to be a gaming device, although you can develop some simple games on it (just read Developing apps for the Watchdog_2040 section).

**Other fundamental principles are:**

- Totally **open source hardware and software**.
- Designed to be **easily hackable and extendible**.
- Easy to **fix and maintain**.

## Hardware

photos here

## Firmware

The firmware is written in C and is based on the [Pico SDK](https://github.com/raspberrypi/pico-sdk).

> From here on, when referring to a **`"module"`** (that is not specifically an hardware physical module), it is meant a **`C file with its header file, that provides a specific functionality`**.

Almost all the hardware-related modules are written from scratch, with the exception of the spi SD card driver, that is just a copy of: [https://github.com/carlk3/no-OS-FatFS-SD-SPI-RPi-Pico](https://github.com/carlk3/no-OS-FatFS-SD-SPI-RPi-Pico) embedded in the project under the [core/lib/no-OS-FatFS-SD-SPI-RPi-Pico](core/lib/no-OS-FatFS-SD-SPI-RPi-Pico) directory, this great library makes all the heavy lifting for the SD card read/write operations and the FAT filesystem management.

The sx1278 LoRa driver is just a C port I made from the [ulora micropython library](https://github.com/armanghobadi/ulora)

The code is divided into two mains sub-folders: `core` and `apps`, the first one contains all the necessary modules to make the hardware work and provides a set of APIs to create basic user interfaces, the second one contains the applications that run on top of the core modules.

### Project overview

The following diagram shows the firmware architecture by layers:

```mermaid
---
config:
  theme: dark
---
block-beta
    columns 3
    block:group0:3
        columns 3
        Apps
    end
    block:group1:3
        columns 3
        Tools
        Components
        DS["Data structures"]
        Utils
        ULMP
        Graphics

    end
    block:group2:3
        columns 3
        H["Hardware drivers"]

        M["Hardware manager"]
    end
    A["Pico C SDK"]:3
    Hardware:3
```

The Layers will be named like this:

- **`Core`**:
  - **layer 0**: the hardware drivers and their management (the third block from the top of the diagram)
  - **layer 1**: the core API and the ULMP stack (the second block from the top of the diagram)
- **`Apps`** (the top block of the diagram)

### Core - layer 0: the hardware drivers and their management

#### Hardware drivers

All the hardware drivers are located in the `core/hardware_drivers` folder, here is a list of the available drivers:

- **`battery`** for battery management and monitoring, available including [`core/hardware_drivers/battery.h`](core/hardware_drivers/battery.h).
- **`core1`** for core1 management, available including [`core/hardware_drivers/core1.h`](core/hardware_drivers/core1.h).
- **`ens160`** for the ENS160 air quality sensor, available including [`core/hardware_drivers/ens160.h`](core/hardware_drivers/ens160.h).
- **`haptics`** for haptic feedback control, available including [`core/hardware_drivers/haptics.h`](core/hardware_drivers/haptics.h).
- **`joystick`** for joystick control, available including [`core/hardware_drivers/joystick.h`](core/hardware_drivers/joystick.h).
- **`onboard_led`** for onboard LED control, available including [`core/hardware_drivers/onboard_led.h`](core/hardware_drivers/onboard_led.h).
- **`rtc_time`** for RTC time management, available including [`core/hardware_drivers/rtc_time.h`](core/hardware_drivers/rtc_time.h).
- **`sdcard`** for SD card management (built on top of [https://github.com/carlk3/no-OS-FatFS-SD-SPI-RPi-Pico](https://github.com/carlk3/no-OS-FatFS-SD-SPI-RPi-Pico)), available including [`core/hardware_drivers/sdcard.h`](core/hardware_drivers/sdcard.h).
- **`ssd1306`** for SSD1306 OLED display control, available including [`core/hardware_drivers/ssd1306.h`](core/hardware_drivers/ssd1306.h).
- **`sx1278`** for SX1278 LoRa transceiver control, available including [`core/hardware_drivers/sx1278.h`](core/hardware_drivers/sx1278.h).

Almost all hardware drivers are structured like this:

- A `<peripheral_name>_init(<args>)` function to initialize the hardware, returning a pointer to the hardware struct.
- One or more `<peripheral_name>_<action>(<peripheral_struct_pointer>)` functions to perform actions on the hardware.

This was done to provide a simple and extensible hardware abstraction layer, where you can plug, let's say, a second joystick or screen by just initializing a new one and storing it in the [**`drivers`** struct](#drivers-struct-and-hw_manager).

> Exception was made for `haptics` and `onboard_led` drivers, due to their simplicity, and for `core1` driver, as it is used to manage the second core of the RP2040 (not a peripheral).

#### Drivers struct and hw_manager

The drivers struct is defined inside [`core/components/hw_manager.h`](core/components/hw_manager.h) and is used to store all the initialized hardware drivers, so they can be easily accessed from anywhere in the code, here is a code snippet showing the drivers present right now for this project:

```c
#include "core/components/include/hw_manager.h"

&(drivers->ens160) // pointer to the air quality sensor driver struct
&(drivers->ssd1306)   // pointer to the screen driver struct
drivers->sx1278 // pointer to the lora module driver struct
drivers->battery // pointer to the battery driver struct
&(drivers->joystick) // pointer to the joystick driver struct
&(drivers->sd_card) // pointer to the sd card driver struct
&(drivers->internal_rtc) // pointer to the rtc driver struct
```

> *drivers* is just the name of the global pointer to the singleton instance of *hw_manager* struct

The hardware drivers are initialized when the [**`sys_setup`**](Watchdog_2040.c) function is called in the firmware entry point ([Watchdog_2040.c](Watchdog_2040.c)), through the call of the [**`hardware_drivers_init`**](core/components/hw_manager.h) function
, that initializes all the hardware drivers and stores their pointers in the `drivers` struct.

> The hardware manager also exposes important functions to get system stats (free heap memory, temperature of the chip etc...), and performs an hardware check at startup, visible on the screen when the device is powered on.

### Core - layer 1: the core API and the ULMP stack

On top of the hardware-related code, there is a set of modules

- **`components`** contains "singleton" struct that are initialized at startup and are used during runtime:
  - **home_page** to manage the device home page, with all needed functionalities, available including [`core/components/home_page.h`](core/components/home_page.h).
  - **hw_manager** to manage all the hardware drivers, available including [`core/components/hw_manager.h`](core/components/hw_manager.h).
  - **malloc_mascot** to manage tutorial-related functions and most importantly dump and load data such as username, hashed password and ULMP address (aka "malloc_memories") to and from the SD card, available including [`core/components/malloc_mascot.h`](core/components/malloc_mascot.h).
  - **sys_paths_manager** to manage all the filesystem paths used in the project, available including [`core/components/sys_paths_manager.h`](core/components/sys_paths_manager.h).

- **`tools`** contains important modules to build the whole UI and manage the system, as the hardware drivers all this tools follows the same convention:

  - A `<tool_name>_init(<args>)` function to initialize the tool, returning a pointer to the tool struct.
  - One or more `<tool_name>_<action>(<tool_struct_pointer>)` functions to perform actions on the tool.

  The available tools are:
  - **launcher** to create one or more launchers for applications, available including [`core/tools/launcher.h`](core/tools/launcher.h).
  - **menu** to manage the system launchers and its functionalities, available including [`core/tools/menu.h`](core/tools/menu.h).
  - **options_gen** the building block of launchers, to generate list of options selectable on the screen, that triggers a callback when selected, available including [`core/tools/options_gen.h`](core/tools/options_gen.h).
  - **sha_256** to hash strings using the SHA-256 algorithm, available including [`core/tools/sha256.h`](core/tools/sha256.h).

- **`utils`** for utility functions used all around the project, available including [`core/utils/utils.h`](core/utils/utils.h):
  - **path** to manipulate filesystem paths in some sort of "object oriented" way (like python pathlib does), available including [`core/utils/path.h`](core/utils/path.h).
  - **utils** general purpose utility functions for strings, arrays and logging, available including [`core/utils/utils.h`](core/utils/utils.h).

- **`data_structures`** contains some useful data structures used all around the project, available including [`core/data_structures/data_structures.h`](core/data_structures/data_structures.h):
  - **string_list** a simple doubly linked list to store strings, inspired to python lists, available including [`core/data_structures/string_list.h`](core/data_structures/string_list.h).

To better understand how this modules work together and how they are used to build the UI, please refer to the [Developing apps for the Watchdog_2040](#developing-apps-for-the-watchdog_2040) section.

### The ULMP stack

**`ULMP (Uncomplicated LoRa Messaging Protocol)`** is the transport layer built on top of the sx1278 driver to provide a simple and reliable way to send and receive messages over LoRa.

For "ULMP stack" is meant the software pieces that works together to provide lora messaging functionalities, this includes:

- The sx1278 driver, located in [`hardware_drivers`](core/hardware_drivers)
- The ULMP protocol implementation, located in [`core/ulmp`](core/ulmp)
- The messages manager located in [`apps/msg_manager`](apps/msg_manager)

#### Overview of a message transaction (receive)

--------------------------------

```mermaid
---
config:
  theme: redux-dark-color
---

sequenceDiagram
    participant setup as sys_setup()
    participant msg_man as messages manager (UI)
    participant core1 as core1 scheduler
    participant ulmp as ulmp (transport layer)
    participant radio as sx1278 radio module


    setup->>msg_man: calls msg_manager_init()
    setup->>core1: calls core1_scheduler_add_callback(process_messages)
    setup->>core1: calls core1_scheduler_set_start_flag(true)
    
    loop on process_messages()
        core1->>msg_man: Calls process_messages()
    end
    
    msg_man->>ulmp: calls lora_receive()
    ulmp->>radio: calls sx1278_set_mode_rx()

    loop on rx interrupt
        radio->>ulmp: Calls on_recv() providing package content
    end

    loop on storing received message
        ulmp->>ulmp: Waits for an END package to finish storing the payload
    end

    ulmp->>radio: ACK package is sent
    ulmp->>msg_man: calls notify()

    msg_man->>msg_man: display_message_notification



```

#### Overview of a message transaction (send)

--------------------------------

The diagram below shows the flow of a message transaction when sending a message:

```mermaid
---
config:
  theme: redux-dark-color
---

sequenceDiagram
    participant msg_man as messages manager (UI)
    participant ulmp as ulmp (transport layer)
    participant radio as sx1278 radio module

    msg_man->>msg_man: user selects contact and compose message
    msg_man->>ulmp: calls lora_send_msg()
    
    loop sending attempts
        ulmp->>ulmp: builds the message packets

        ulmp->>radio: send packets
        
        alt the ACK packet is received
            radio->>ulmp: ACK
            ulmp->>msg_man: show message status as: delivered
        else the ACK packet is not received
            ulmp->>msg_man: show message status as: sent
            ulmp->>ulmp: loop repeats until MAX_SENDING_ATTEMPTS is reached
        end
    end
```

#### The Transport Layer

--------------------------------

Let's see, more in detail, how the transport layer works.

The protocol is designed to be simple and easy to use,
it allows peer to peer or broadcast communication between
two or more sx1278 enabled devices on the same frequency.

##### Packets

There are 6 types of packets that can be sent over the network:

  |NAME       |HEX  |DESCRIPTION
  |-----------|-----|-----------------------------------
  |**`PING`** |0x00 |Used to **check** if a device is still connected to the network.
  |**`PONG`** |0x01 |Used to **respond** to a ping packet.
  |**`START`**|0x02 |Used to **start** a message transaction.
  |**`END`**  |0x03 |Used to **end** a message transaction.
  |**`MSG`**  |0x04 |Used to **send** a string message from one device to another.
  |**`ACK`**  |0x05 |Used to **acknowledge** the receipt of a message.

> All packets have a header and a payload, but only the MSG packet has a non-empty payload.

##### Packets header

```mermaid
---
config:
  theme: dark
---

packet-beta
title ULMP packet header
0-15: "src_address (bytes 1-2) - Address of device A"
16-31: "dest_address (bytes 3-4) - Address of device B"
32-63: "transaction_uid (bytes 5-12) - 8-byte ASCII string unique transaction ID"
64-71: "packet_type (byte 13)"
72-87: "payload length (bytes 14 and 15)"

```

Let's see the header structure:

  |BYTES                   |FIELD           |DESCRIPTION
  |------------------------|----------------|-----------------------------------
  |**`BYTES 1 and 2`**     | src_address    | The address of device A.
  |**`BYTES 3 and 4`**     | dest_address   | The address of device B.
  |**`BYTES from 5 to 12`**| transaction_uid| A unique identifier for the whole transaction, a string of 8 bytes.
  |**`BYTE  13`**           | packet_type   | (PING, PONG, START, END, MSG, ACK).
  |**`BYTES 14 and 15`**     | payload_len    | The length of the message in bytes.

##### A transaction example

Device **A** sends a message to device **B**:

1. Device **A** sends a START packet to device **B**,

2. Device **B** receives the START packet and, if it is the intended recipient,
  it allocates the structures to store the message and waits for the next packets to arrive.

3. Device **A** sends one or more MSG packets to device **B**, each containing a part of the message.

4. Device **B** receives the MSG packets and, if it is the intended recipient,
  it reads the payload, storing it in the message structure.

5. Device **A** sends an END packet to device **B**, indicating that the message has been fully sent.

6. Device **B** receives the END packet and, if it is the intended recipient, it reads the message structure
  and processes the message.

7. Device **B** sends an ACK packet to device **A**, indicating that the message has been fully received.

8. Device **A** receives the ACK packet from device **B** and its' happy.

>Let's see the entire transaction in decoded bytes when device **A** sends a long text message to device **B**.

**`\x2B\x33\xF3\xBDQPYQPYQA\x02\x00\x00`**
This is a `START` packet as we can see by the packet type byte **0x02**:

- the source address is **`0x332B`** (device **A** which has address 13099 (decimal)),
- the destination address is **`0xBDF3`** (device **B** which has address 48627 (decimal)),
- the transaction_uid is **`QPYQPYQA`**,
- the payload length is **`0x0000`** (0 bytes), as this is a START packet.

**`\x2B\x33\xF3\xBDQPYQPYQA\x04\xd7\x00`** On the other hand, we denounce with righteous indignation and dislike men who are so beguiled and demoralized by the charms of pleasure of the moment, so blinded by desire, that they cannot foresee the pain and trou

>This is a `MSG` packet as we can see by the packet type byte **0x04**:

- the source address is **`0x332B`** (device **A** which has address 13099 (decimal)),
- the destination address is **`0xBDF3`** (device **B** which has address 48627 (decimal)),
- the transaction_uid is **`QPYQPYQA`**,
- the payload length is **`0x00d7`** (215 bytes),
- the payload is the message.

>The next 3 packets are MSG packets with the same transaction_uid and the same source and destination addresses, but with different payloads (and thus different payload lengths):

**`\x2B\x33\xF3\xBDQPYQPYQA\x04\xd7\x00`** ble that are bound to ensue; and equal blame belongs to those who fail in their duty through weakness of will, which is the same as saying through shrinking from toil and pain. These cases are perfectly simple and e'

**`\x2B\x33\xF3\xBDQPYQPYQA\x04\xd7\x00`** asy to distinguish. In a free hour, when our power of choice is untrammelled and when nothing prevents our being able to do what we like best, every pleasure is to be welcomed and every pain avoided. But in certain

**`\x2B\x33\xF3\xBDQPYQPYQA\x04\xd7\x00`** circumstances and owing to the claims of duty or the obligations of business it will frequently occur that pleasures have to be repudiated and annoyances accepted. The wise man therefore always holds in these matter

**`\x2B\x33\xF3\xBDQPYQPYQA\x04\x88\x00`** s to this principle of selection: he rejects pleasures to secure other greater pleasures, or else he endures pains to avoid worse pains.

>This is an `END` packet as we can see by the packet type byte **0x03**:

**`\x2B\x33\xF3\xBDQPYQPYQA\x03\x00\x00`**

Here our transaction ends, the message has been fully sent.

## Apps

![alt text](doc_images/apps.gif)

The apps "shipped" with the last version of the firmware are:

- **[`AQI`](#aqi)**
- **[`Flashlight`](#flashlight)**
- **[`Message manager`](#message-manager)**
- **[`Notes`](#notes)**
- **[`System submenus`](#system-submenus)**
- **[`Terminal`](#terminal)**
- **[`Text editor`](#text-editor)**
- **[`Time submenus`](#time-submenus)**
- **[`Virtual keyboard`](#virtual-keyboard)**

### AQI

It reads the air quality data from the ENS160 sensor and displays it on the screen, both in the home page and in a more detailed view when launched trough graphs.

#### Dependencies

Depends on the following core hardware modules:

- [**ENS160 sensor driver**](core/hardware_drivers/ens160.h)
- [**SSD1306 driver**](core/hardware_drivers/ssd1306.h)
- [**joystick driver**](core/hardware_drivers/joystick.h)

Depends on the following core modules:

- [**hardware manager component**](core/components/hw_manager.h)
- [**graphs module**](core/graphics/graphs.h)

No app depends on it.

> header: [`apps/aqi/aqi.h`](apps/aqi/aqi.h)

### Flashlight

An app that turns the screen into a soft flashlight, useful when you need a bit of light in the dark, but don't want to blind yourself or others with a bright light, it's just that simple.

#### Dependencies

Depends on the following core hardware modules:

- [**SSD1306 driver**](core/hardware_drivers/ssd1306.h)
- [**joystick driver**](core/hardware_drivers/joystick.h)

Depends on the following core modules:

- [**hardware manager component**](core/components/hw_manager.h)

No app depends on it.

> header: [`apps/flashlight/flashlight.h`](apps/flashlight/flashlight.h)

### Message manager

Sending a message

![alt text](doc_images/ulmp.gif)

Adding a contact

![alt text](doc_images/contacts.gif)

The message manager app is the most complex so its functionalities deserve a list:

1. Send and receive text messages over LoRa.
2. Manage contacts (add, remove, edit).
3. View message history with a contact, with the automatic update of message status (sent, delivered, read).
4. View message details (timestamp, status, content).
5. Scan for nearby online contacts.
6. Manage notifications (enable/disable).

It is not a single module, but a set of modules all found in the [`apps/msg_manager`](apps/msg_manager) folder, for further explanation just read the code, it's well documented, I promise.

#### Dependencies

Depends on the following core hardware modules:

- [**SSD1306 driver**](core/hardware_drivers/ssd1306.h)
- [**joystick driver**](core/hardware_drivers/joystick.h)
- [**haptic feedback driver**](core/hardware_drivers/haptics.h)
- [**onboard_led driver**](core/hardware_drivers/onboard_led.h)

Depends on the following core modules:

- [**hardware manager component**](core/components/hw_manager.h)
- [**ULMP transport layer**](core/ulmp/ulmp.h)
- [**malloc_mascot component**](core/components/malloc_mascot.h)
- [**sys_paths_manager component**](core/components/sys_paths_manager.h)
- [**options_gen tool**](core/tools/options_gen.h)
- [**path utils module**](core/utils/path.h)
- [**utils module**](core/utils/utils.h)
- [**string_list data structure**](core/data_structures/string_list.h)

Depends on the following apps:

- [**text editor app**](apps/text_editor/text_editor.h)

No app depends on it.

> header: [`apps/msg_manager/msg_manager.h`](apps/msg_manager/msg_manager.h)

### Notes

![alt text](doc_images/notes.gif)

A simple notes app that allows to create, edit and delete text files on the SD card.

#### Dependencies

Depends on the following core hardware modules:

- [**SSD1306 driver**](core/hardware_drivers/ssd1306.h)
- [**joystick driver**](core/hardware_drivers/joystick.h)

Depends on the following core modules:

- [**hardware manager component**](core/components/hw_manager.h)
- [**sys_paths_manager component**](core/components/sys_paths_manager.h)
- [**options_gen tool**](core/tools/options_gen.h)
- [**path utils module**](core/utils/path.h)
- [**utils module**](core/utils/utils.h)
- [**string_list data structure**](core/data_structures/string_list.h)

Depends on the following apps:

- [**text editor app**](apps/text_editor/text_editor.h)

No app depends on it.

> header: [`apps/notes/notes.h`](apps/notes/notes.h)

### System submenus

![alt text](doc_images/sys.gif)

A set of submenus to display system information and perform system actions, like:

1. View system information (battery status, free memory, temperature).
2. Reset the system (erase all user data and settings).
3. Display battery status (percentage and voltage).
4. Perform a manual joystick check (to test if it is working properly).

#### Dependencies

Depends on the following core hardware modules:

- [**battery driver**](core/hardware_drivers/battery.h)
- [**joystick driver**](core/hardware_drivers/joystick.h)
- [**SSD1306 driver**](core/hardware_drivers/ssd1306.h)
- [**haptic feedback driver**](core/hardware_drivers/haptics.h)

Depends on the following core modules:

- [**hardware manager component**](core/components/hw_manager.h)
- [**sys_paths_manager component**](core/components/sys_paths_manager.h)
- [**options_gen tool**](core/tools/options_gen.h)
- [**graphics primitives module**](core/graphics/graphics_primitives.h)
- [**utils module**](core/utils/utils.h)
- [**path utils module**](core/utils/path.h)
- [**string_list data structure**](core/data_structures/string_list.h)
- [**device config module**](device.h)

Apps that depend on it:

- [**terminal**](apps/terminal/terminal.h)

> header: [`apps/system_submenus/system_submenus.h`](apps/system_submenus/system_submenus.h)

### Terminal

![alt text](doc_images/terminal.gif)

A simple terminal app that allows to execute shell commands on the device, it has two main mode of operation:

1. **Local mode**, where the commands are executed on the device itself, using a combination of text editors for inputs and outputs.
2. **Serial mode**, where the commands are executed on a connected computer via USB serial, using a serial terminal program.

A commands list is always available by typing `help` in the terminal, All basic linux commands are supported both in local and serial mode, like `ls`, `cat`, `rm`, `mv`, `cp` etc..., thus making it a valid tool for file management on the device and bug fixing.

Since the terminal is so powerful, the user is asked to insert the password (set during the first setup) to access it (there is no sudo or root).

#### Dependencies

Depends on the following core modules:

- [**malloc mascot component**](core/components/malloc_mascot.h)
- [**sys_paths_manager component**](core/components/sys_paths_manager.h)
- [**ULMP transport layer**](core/ulmp/ulmp.h)
- [**string list data structure**](core/data_structures/string_list.h)
- [**path utils module**](core/utils/path.h)
- [**utils module**](core/utils/utils.h)
- [**device config module**](device.h)

Depends on the following apps:

- [**text editor app**](apps/text_editor/text_editor.h)
- [**system submenus app**](apps/system_submenus/system_submenus.h)

No app depends on it.

> header: [`apps/terminal/terminal.h`](apps/terminal/terminal.h)

### Text editor

A simple text editor that allows to create, edit and save text files on the SD card, it is used all around the firmware as a way to input and output text.

#### Dependencies

Depends on the following core hardware modules:

- [**SSD1306 driver**](core/hardware_drivers/ssd1306.h)
- [**joystick driver**](core/hardware_drivers/joystick.h)
- [**hardware config module**](core/hardware_drivers/config.h)

Depends on the following core modules:

- [**hardware manager component**](core/components/hw_manager.h)
- [**string list data structure**](core/data_structures/string_list.h)

Depends on the following apps:

- [**virtual keyboard app**](apps/virtual_keyboard/virtual_keyboard.h)

Apps that depend on it:

- [**message manager app**](apps/msg_manager/msg_manager.h)
- [**notes app**](apps/notes/notes.h)
- [**terminal app**](apps/terminal/terminal.h)
- [**time submenus app**](apps/time_submenus/time_submenus.h)

> header: [`apps/text_editor/text_editor.h`](apps/text_editor/text_editor.h)

### Time submenus

All the sub menus related to time management, like:

1. Setting the time and date (using the RTC module).
2. Set an alarm (with haptic feedback).
3. Use a stopwatch.
4. Use a countdown timer (with haptic feedback).

#### Dependencies

Depends on the following core hardware modules:

- [**rtc_time driver**](core/hardware_drivers/rtc_time.h)
- [**haptic feedback driver**](core/hardware_drivers/haptics.h)
- [**SSD1306 driver**](core/hardware_drivers/ssd1306.h)
- [**joystick driver**](core/hardware_drivers/joystick.h)

Depends on the following core modules:

- [**hardware manager component**](core/components/hw_manager.h)
- [**bitmaps module**](core/graphics/bitmaps.h)
- [**graphics primitives module**](core/graphics/graphics_primitives.h)
- [**string list data structure**](core/data_structures/string_list.h)

Depends on the following apps:

- [**text editor app**](apps/text_editor/text_editor.h)

No app depends on it.

### Virtual keyboard

A simple virtual keyboard that allows to input text using the joystick, it is used all around the firmware as a way to input text.

#### Dependencies

Depends on the following core hardware modules:

- [**SSD1306 driver**](core/hardware_drivers/ssd1306.h)
- [**joystick driver**](core/hardware_drivers/joystick.h)
- [**hardware config module**](core/hardware_drivers/config.h)

Depends on the following core modules:

- [**hardware manager component**](core/components/hw_manager.h)

Apps that depend on it:

- [**text editor app**](apps/text_editor/text_editor.h)

## Developing apps for the Watchdog_2040

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
