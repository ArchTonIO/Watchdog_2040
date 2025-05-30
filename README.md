
# Watchdog_2040

## The Transport Layer

The LoRa transport layer I built for this project was named **`ULMP`**: **Uncomplicated LoRa Messaging Protocol.**

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
  |**`BYTES from 5 to 12`**| transaction_uid| A unique identifier for the whole transaction, a string of 8 bytes.
  |**`BYTE  13`**           | packet_type   | (PING, PONG, START, END, MSG, ACK).
  |**`BYTES 14 and 15`**     | payload_len    | The length of the message in bytes.

### A transaction example

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

**`\x00\x00\x01\x00QPYQPYQA\x02\x00\x00`**
This is a `START` packet as we can see by the packet type byte **0x02**:

- the source address is **`0x0000`** (device **A** wich has address 0 (decimal)),
- the destination address is **`0x0100`** (device **B** wich has address 1 (decimal)),
- the transaction_uid is **`QPYQPYQA`**,
- the payload length is **`0x0000`** (0 bytes), as this is a START packet.

**`\x00\x00\x01\x00QPYQPYQA\x04\xd7\x00`** On the other hand, we denounce with righteous indignation and dislike men who are so beguiled and demoralized by the charms of pleasure of the moment, so blinded by desire, that they cannot foresee the pain and trou

>This is a `MSG` packet as we can see by the packet type byte **0x04**:

- the source address is **`0x0000`** (device **A** wich has address 0 (decimal)),
- the destination address is **`0x0100`** (device **B** wich has address 1 (decimal)),
- the transaction_uid is **`QPYQPYQA`**,
- the payload length is **`0x00d7`** (215 bytes),
- the payload is the message.

>The next 3 packets are MSG packets with the same transaction_uid and the same source and destination addresses, but with different payloads (and thus different payload lengths):

**`\x00\x00\x01\x00QPYQPYQA\x04\xd7\x00`** ble that are bound to ensue; and equal blame belongs to those who fail in their duty through weakness of will, which is the same as saying through shrinking from toil and pain. These cases are perfectly simple and e'

**`\x00\x00\x01\x00QPYQPYQA\x04\xd7\x00`** asy to distinguish. In a free hour, when our power of choice is untrammelled and when nothing prevents our being able to do what we like best, every pleasure is to be welcomed and every pain avoided. But in certain

**`\x00\x00\x01\x00QPYQPYQA\x04\xd7\x00`** circumstances and owing to the claims of duty or the obligations of business it will frequently occur that pleasures have to be repudiated and annoyances accepted. The wise man therefore always holds in these matter

**`\x00\x00\x01\x00QPYQPYQA\x04\x88\x00`** s to this principle of selection: he rejects pleasures to secure other greater pleasures, or else he endures pains to avoid worse pains.

>This is an `END` packet as we can see by the packet type byte **0x03**:

**`\x00\x00\x01\x00QPYQPYQA\x03\x00\x00`**

Here our transaction ends, the message has been fully sent.
