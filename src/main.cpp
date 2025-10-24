#include <Arduino.h>
#include <SPI.h>

#include <LoRaMesher.h>
#include <LoRa.h>

// ----- Node Type Selection via Build Flags -----
// In platformio.ini, you will define exactly one:
// -D NORMAL_NODE
// -D END_NODE
// -D BRIDGE_NODE

// ----- Common LoRa Pins -----
#ifndef RADIO_CS
#define RADIO_CS    10
#endif
#ifndef RADIO_RST
#define RADIO_RST   9
#endif
#ifndef RADIO_IRQ
#define RADIO_IRQ   2
#endif

#ifndef LORA_FREQ
#define LORA_FREQ 868E6
#endif

#ifdef NORMAL_NODE
// ----------- NORMAL NODE (Mesh Participant) -----------
LoraMesher &radio = LoraMesher::getInstance();

void setup() {
    Serial.begin(115200);
    while (!Serial);

    LoraMesher::LoraMesherConfig config;

    config.module = LoraMesher::LoraModules::SX1276_MOD;

    config.loraCs  = RADIO_CS;
    config.loraRst = RADIO_RST;
    config.loraIrq = RADIO_IRQ;
    config.sf = LORA_SF; 
    radio.begin(config);
    radio.start();

    Serial.println("NORMAL_NODE: LoraMesher started");
}

void loop() {
    static uint32_t counter = 0;
    counter++;

    struct { int type = 0; union { uint32_t counter; uint8_t data[4]; } data; } pkt;
    pkt.type = 0; pkt.data.counter = counter;

    radio.createPacketAndSend(BROADCAST_ADDR, &pkt, 1);
    Serial.print("NORMAL_NODE: Sent counter ");
    Serial.println(counter);
    delay(5000);
}
#endif // NORMAL_NODE

#ifdef END_NODE
// ----------- END NODE (Mesh Side) -----------
#include <LoRaMesher.h>
LoraMesher LM;
void sendRawToBridge(uint8_t *data, size_t len) {
    LoRa.beginPacket();
    LoRa.write(data, len);
    LoRa.endPacket();
}

void handleIncomingAppPacket(LM_Packet *pkt) {
#ifdef BRIDGE_NODE_ID
    if (pkt->destination == BRIDGE_NODE_ID) {
        uint8_t buffer[255];
        int size = pkt->toBytes(buffer);
        sendRawToBridge(buffer, size);
    }
#endif
}

void setup() {
    Serial.begin(115200);
    while (!Serial);

    // LoRaMesher
    LM.begin(LORA_FREQ, RADIO_CS, RADIO_RST, RADIO_IRQ);
    LM.onReceive(handleIncomingAppPacket);

    // LoRa for raw transmission
    LoRa.setPins(RADIO_CS, RADIO_RST, RADIO_IRQ);
    LoRa.begin(LORA_FREQ);
    LoRa.setSpreadingFactor(LORA_SF);

    Serial.println("END_NODE: Ready ");
}

void loop() {
    LM.loop(); // LoRaMesher routing
}
#endif // END_NODE

#ifdef BRIDGE_NODE

#include <SPI.h>
#include <LoRa.h>

// Create SPI classes for HSPI and VSPI
SPIClass hspi(HSPI);
SPIClass vspi(VSPI);

// LoRa instances for RFM1 and RFM2
LoRaClass LoRa1;
LoRaClass LoRa2;

bool lora1_ok = false;
bool lora2_ok = false;

void resetLoRa() {
  digitalWrite(RFM1_RST, LOW);  
  delay(10);
  digitalWrite(RFM1_RST, HIGH);
  delay(100);  // Wait for module to reset
 
  digitalWrite(RFM2_RST, LOW);  
  delay(10);
  digitalWrite(RFM2_RST, HIGH);
  delay(100);  // Wait for module to reset
}

void setup() {
  Serial.begin(115200);
  delay(2000);  // Small delay to stabilize serial communication
  while (Serial.available()) {
    Serial.read();  // Clear serial buffer
  }
  Serial.println("LoRa Relay System Initialized...");

  resetLoRa();  // Reset the LoRa modules before initialization

  // ----- RFM1 Setup (HSPI) -----
  hspi.begin(RFM1_SCK, RFM1_MISO, RFM1_MOSI, RFM1_SS);
  LoRa1.setSPI(hspi);
  LoRa1.setPins(RFM1_SS, RFM1_RST, RFM1_DIO0);
 
  if (LoRa1.begin(LORA_FREQ)) {
    Serial.println("[DEBUG] LoRa1 (RFM1) initialized successfully.");
    lora1_ok = true;
  } else {
    Serial.println("[ERROR] LoRa1 init failed!");
  }

  // ----- RFM2 Setup (VSPI) -----
  vspi.begin(RFM2_SCK, RFM2_MISO, RFM2_MOSI, RFM2_SS);
  LoRa2.setSPI(vspi);
  LoRa2.setPins(RFM2_SS, RFM2_RST, RFM2_DIO0);

  if (LoRa2.begin(LORA_FREQ)) {
    Serial.println("[DEBUG] LoRa2 (RFM2) initialized successfully.");
    lora2_ok = true;
  } else {
    Serial.println("[ERROR] LoRa2 init failed!");
  }

  if (!lora1_ok && !lora2_ok) {
    Serial.println("[ERROR] No LoRa modules detected. Halting...");
    while (true);
  }

  Serial.println("[DEBUG] LoRa modules initialized, ready to receive and relay.");
}

void loop() {
  // Check if RFM1 is ready
  if (lora1_ok) {
    int packetSize = LoRa1.parsePacket();
    if (packetSize > 0) {
      Serial.println("[DEBUG] Received a packet at RFM1.");

      String receivedMsg = "";

      // Read the received packet, checking for valid characters
      while (LoRa1.available()) {
        char c = (char)LoRa1.read();  // Read incoming byte
        if (isPrintable(c)) {  // Only keep printable characters
          receivedMsg += c;
        }
      }

      // If the message is valid, print it
      if (receivedMsg.length() > 0) {
        Serial.print("[RFM1 RX] Received Message: ");
        Serial.println(receivedMsg);  // Print received message
      } else {
        Serial.println("[DEBUG] Received invalid packet (non-printable data).");
      }

      // Remove the "TX1:" tag from the received message
      receivedMsg.replace("TX1:", "");

      // Now relay the message via RFM2
      if (lora2_ok) {
        // Explicitly set SF=8 each time before transmission
        LoRa2.setSpreadingFactor(8);  // SF=8 (longer range, lower data rate)
        Serial.println("[DEBUG] RFM2: Setting SF=8 before transmission.");

        // Add " from RFM2" to the message before relaying
        String relayMsg = receivedMsg + " from RFM2";  // Append " from RFM2" to the message
       
        // Debug message before sending
        Serial.println("[DEBUG] RFM2 Transmitting at SF=8...");
       
        LoRa2.beginPacket();
        LoRa2.print(relayMsg);
        LoRa2.endPacket();  // Send the relayed message at SF=8

        Serial.print("[RFM2 TX] Relayed Message: ");
        Serial.println(relayMsg);
      } else {
        Serial.println("[ERROR] RFM2 is not ready, cannot relay message.");
      }
    } else {
      // If no packet is received, print a debug message
      Serial.println("[DEBUG] No packet received at RFM1.");
    }
  }

  // Add small delay between checking for packets
  // delay(50);  // Adjusted delay to sync better with SF=8 timing
}
#endif // BRIDGE_NODE