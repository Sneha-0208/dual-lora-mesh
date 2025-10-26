# Dual LoRa Mesh – SF Conversion  
### System Documentation (`doc.md`)

---

## 1. Overview

This project demonstrates a **dual LoRa mesh network** capable of **cross-spreading factor (SF) communication**.  
It consists of three coordinated nodes:

1. **Transmitter (SF7)** — Sends periodic packets with incrementing message counters.  
2. **Relay Node (Dual LoRa)** — Receives packets at **SF7** via RFM1 and relays them at **SF8** via RFM2.  
3. **Receiver (SF8)** — Receives relayed packets and displays them on Serial Monitor.

This configuration acts as a **spreading factor conversion bridge**, enabling communication between LoRa nodes operating on different SF values.

---

## 2. Node Roles and Functions

| Node | Function | Description |
|------|-----------|-------------|
| **Transmitter** | Source | Sends “Hello” packets tagged with message count and prefix `TX1:` |
| **Relay Node** | Intermediate | Receives SF7 packets and retransmits them at SF8 |
| **Receiver** | Destination | Receives SF8 packets and prints them on Serial |

---

## 3. Hardware Setup

### Transmitter / Receiver

| Parameter | Value |
|------------|--------|
| **LoRa Module** | 1 (Default SPI) |
| **Pins Used** | `SS=10`, `RST=9`, `DIO0=2` |
| **Frequency** | 868 MHz |
| **Spreading Factor** | SF7 (Tx) for Transmitter, SF8 (Rx) for Receiver |

---

### Relay Node (Dual LoRa Setup)

| Module | SPI Bus | SCK | MISO | MOSI | SS | RST | DIO0 |
|---------|----------|-----|------|------|----|-----|------|
| **RFM1** | HSPI | 14 | 25 | 13 | 4 | 32 | 16 |
| **RFM2** | VSPI | 18 | 23 | 19 | 5 | 21 | 17 |

- **RFM1** operates as the **receiver** (SF7).  
- **RFM2** operates as the **transmitter** (SF8).  
- Both modules share the same frequency band (868 MHz).  
- Uses two separate `SPIClass` instances:
  ```cpp
  SPIClass hspi(HSPI);
  SPIClass vspi(VSPI);

  Example Transmission Path:

TX1 → Relay (RFM1:SF7) → RFM2(SF8) → Receiver

Relay Node

Dual LoRa modules (RFM1, RFM2)

Receives packets on SF7 (RFM1)

Transmits modified packets on SF8 (RFM2)

Uses printable character filter to ignore corrupted data

Adds " from RFM2" before relaying

Receiver

Single LoRa module

Listens on SF8

Uses optional CRC check (LoRa.enableCrc())

Prints received packets to Serial

9. Advantages of SF Conversion Design

Cross-SF Communication: Enables message transfer between devices operating at different SFs.

Dual-Radio Relay: Expands coverage and enables SF translation.

Modular Testing: Each node can be tested independently.

Enhanced Debugging: Clear logs for both RX and TX events.

Flexible Extension: Relay node can be expanded for dynamic SF detection or routing in mesh networks.
  SPIClass hspi(HSPI);
  SPIClass vspi(VSPI);
