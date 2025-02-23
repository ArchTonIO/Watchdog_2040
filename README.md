
# Watchdog_2040

## The networking layer

The Networking layer created for this project is called **`ULCP`**: **Uncomplicated LoRa Communication Protocol.**

This communication protocol is built upon this project sx1278.c driver, a port of the micropython driver ulora.py.

The protocol is designed to be simple and easy to use,
it allows peer to peer or broadcast communication between
two or more sx1278 enabled devices on the same frequency.

### Packets

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

### Packets header
Let's see the header structure:
  |BYTES                   |FIELD           |DESCRIPTION
  |------------------------|----------------|-----------------------------------
  |**`BYTES 1 and 2`**     | src_address    | The address of device A.
  |**`BYTES 3 and 4`**     | dest_address   | The address of device B.
  |**`BYTES from 4 to 12`**| transaction_uid| A unique identifier for the whole transaction, a string of 8 bytes + '\0'.
  |**`BYTE  5`**           | packet_type   | (PING, PONG, START, END, MSG, ACK).
  |**`BYTES 6 and 7`**     | payload_len    | The length of the message in bytes.


### A transaction example

Device **`A`** sends a message to device **`B`**:

1. Device **`A`** sends a START packet to device **`B`**,

2. Device **`B`** receives the START packet and, if it is the intended recipient,
  it allocates the structures to store the message and waits for the next packets to arrive.

3. Device **`A`** sends one or more MSG packets to device **`B`**, each containing a part of the message.

4. Device **`B`** receives the MSG packets and, if it is the intended recipient,
  it reads the payload, storing it in the message structure.

5. Device **`A`** sends an END packet to device **`B`**, indicating that the message has been fully sent.

6. Device **`B`** receives the END packet and, if it is the intended recipient, it reads the message structure
  and processes the message.

7. Device **`B`** sends an ACK packet to device **`A`**, indicating that the message has been fully received.

8. Device **`A`** receives the ACK packet from device **`B`** and its' happy. 
