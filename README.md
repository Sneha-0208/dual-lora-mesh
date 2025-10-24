# dual-lora-mesh

## Project Overview  
The objective of this project is to design a bridge node equipped with two LoRa radios that enables bidirectional communication between two independent LoRa mesh networks operating with different spreading factors (SFs).

## Key Features  
- Mesh topology: nodes forward messages across the network, extending range beyond single hop.  
- Dual LoRa modules per node (hence “dual-LoRa”): one module for node-to-node communication, the other perhaps dedicated to gateway or long-haul links.  
- Low power: nodes are designed to run on minimal power and operate for extended durations.  
- Modular firmware structure (see `src/` and `lib/` folders) to allow easy customization or integration with sensors.

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

Connect your microcontroller board and configure platformio.ini with the correct board type, upload port, and LoRa module settings (frequency, spreading factor, etc.).

Upload the firmware to each node.

Configure node IDs, mesh parameters, and gateway configuration as required.

Usage
Power up your first node (gateway) and ensure its LoRa module is working.

Add additional nodes; they will join the mesh and start forwarding messages.

Use serial output or on-board indicators (LEDs) to monitor node connectivity and routing paths.

Test communication between nodes, verify packet routing, and adjust parameters (such as transmit power or mesh forwarding logic) for optimal performance.

Project Structure
bash
.
├── .vscode/               # IDE / debug configuration  
├── lib/                   # External libraries or modules  
├── src/                   # Main firmware source code  
├── platformio.ini         # Build configuration  
├── .gitignore             # Files to ignore in Git  
└── README.md              # (this file)  
Feel free to add additional folders (e.g., docs/, hardware/, tests/) as the project expands.
