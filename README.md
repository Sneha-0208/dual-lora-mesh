# dual-lora-mesh

## Project Overview  
This project implements communication between two LoRa end devices operating at different spreading factors (SF7 and SF8) using a bridge/relay node based on ESP32. The relay node is equipped with two LoRa chips, one configured for SF7 and the other for SF8, allowing it to receive messages on one SF, convert them, and forward them to the other SF.

LoRa nodes using different spreading factors cannot directly communicate because a single LoRa module can only demodulate packets sent at its configured SF. The relay node overcomes this limitation, enabling interoperability between nodes with different SFs and serving as a foundational step toward multi-SF LoRa mesh networks.

## Key Features  
- SF Conversion & Bridging: Enables communication between nodes on different spreading factors through a dual-LoRa relay node.

- Single Dual-LoRa Node Advantage: Forwarding data internally between two LoRa chips on the same microcontroller is faster than using two separate nodes connected via a serial link.

- Foundation for LoRa Mesh: The relay logic can be extended to act as a bridge between two separate LoRa meshes, facilitating multi-hop and multi-SF communication.

Scalable & Configurable: Node IDs, SFs, and routing parameters can be configured in firmware for different network setups.
## Tech Stack & Hardware  
- Firmware written in C++ (using the PlatformIO build system via `platformio.ini`).  
- Uses LoRa radio modules (e.g., SX127x series) for wireless communication.  
- Configured to work with microcontroller hardware (e.g., Arduino-compatible, ESP32, Heltec).  

## Installation & Setup  
1. Clone the repository:  
   ```bash
   git clone https://github.com/Sneha-0208/dual-lora-mesh.git
   cd dual-lora-mesh
Install PlatformIO and open the project in VS Code or your preferred IDE.

Connect your microcontroller board and configure platformio.ini with:

- Board type

- Upload port

- LoRa module settings (frequency, spreading factor, pins, etc.)

Upload the firmware to each node.

Configure node IDs and hardware pins in the firmware as required.
## Usage
Copy the TX, RX, and relay code located in the SF_conversion/ directory into main.cpp and configure the platformio.ini file accordingly.
- Power up the first end node and verify its LoRa module is operational.

- Power the second end node; the relay node will automatically forward packets between SF7 and SF8.

- Monitor serial output or on-board LEDs to check node connectivity and packet forwarding.

## Project Structure

├── .vscode/               # IDE / debug configuration  
├── lib/                   # External libraries or modules  
├── src/                   # Contains main.cpp
├── SF_conversion/         # Relay and End node logic 
├── platformio.ini         # Build configuration  
├── .gitignore             # Files to ignore in Git  
└── README.md              # (this file)  
└── Documentation.md       # Code-level-understanding 
Feel free to add additional folders (e.g., docs/, hardware/, tests/) as the project expands.
