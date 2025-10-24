# dual-lora-mesh

## 🎯 Project Overview  
This project implements a dual-LoRa mesh network using LoRa (Long Range) radio modules to enable low-power, wide-area communication between nodes without relying on traditional infrastructure (e.g., WiFi or cellular).  
It’s designed for applications such as remote sensing, IoT networks in rural areas, or fallback communication when other networks fail.

## 🧩 Key Features  
- Mesh topology: nodes forward messages across the network, extending range beyond single-hop.  
- Dual LoRa modules per node (hence “dual-LoRa”): one module for node-to-node communication, the other perhaps dedicated to gateway or long-haul links.  
- Low power: nodes are designed to run on minimal power and operate for extended durations.  
- Modular firmware structure (see `src/` and `lib/` folders) to allow easy customization or integration with sensors.

## 📦 Tech Stack & Hardware  
- Firmware written in C++ (using the PlatformIO build system via `platformio.ini`).  
- Uses LoRa radio modules (e.g., SX127x series) for wireless communication.  
- Likely built on microcontroller hardware (e.g., Arduino-compatible, ESP32, or similar).  
- Code organised into `src/`, `lib/`, and other folders for clean architecture.

## 🛠 Installation & Setup  
1. Clone the repository:  
   ```bash
   git clone https://github.com/Sneha-0208/dual-lora-mesh.git
   cd dual-lora-mesh
Install PlatformIO and open the project in VS Code or your preferred IDE.

Connect your microcontroller board and configure platformio.ini with the correct board type, upload port, and LoRa module settings (frequency, spreading factor, etc.).

Upload the firmware to each node.

Configure node IDs, mesh parameters and gateway configuration as required.

🚀 Usage
Power up your first node (gateway) and ensure its LoRa module is working.

Add additional nodes; they will join the mesh and start forwarding messages.

Use serial output or on-board indicators (LEDs) to monitor node connectivity and routing paths.

Test communication between nodes, verify packet routing, and adjust parameters (such as transmit power or mesh forwarding logic) for optimal performance.

🔍 Project Structure
bash
Copy code
.
├── .vscode/               # IDE / debug configuration  
├── lib/                   # External libraries or modules  
├── src/                   # Main firmware source code  
├── platformio.ini         # Build configuration  
├── .gitignore             # Files to ignore in Git  
└── README.md              # (this file)  
Feel free to add additional folders (e.g., docs/, hardware/, tests/) as the project expands.

👥 Contributing
Contributions are welcome! Whether you’d like to fix bugs, add new features (e.g., sensor modules, advanced routing algorithms), or expand documentation, here’s how to get started:

Fork this repository.

Create a feature branch: git checkout -b feature/my-new-feature

Commit your changes and push: git push origin feature/my-new-feature

Open a Pull Request describing what you’ve done.

Ensure code adheres to existing style and includes comments if needed.
