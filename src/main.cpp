#include <Arduino.h>
#include "LoraMesher.h"
#include <SPI.h>

// Common LED definition
#define BOARD_LED 4
#define LED_ON LOW
#define LED_OFF HIGH

// Common data packet structure
struct DataPacket {
  int type;
  uint8_t payload[16];
};

// ========== MESH A NODE (SF7) ==========
#ifdef MeshANode

#define LORA_CS   5
#define LORA_RST  14
#define LORA_IRQ  2
#define LORA_IO1  33

LoraMesher& radio = LoraMesher::getInstance();

DataPacket outPkt;

void setupLoRaMeshA() {
  LoraMesher::LoraMesherConfig cfg;
  cfg.module = LoraMesher::LoraModules::SX1276_MOD;
  cfg.loraCs = LORA_CS;
  cfg.loraRst = LORA_RST;
  cfg.loraIrq = LORA_IRQ;
  cfg.loraIo1 = LORA_IO1;
  cfg.spreadingFactor = 7; // SF7
  SPI.begin(18, 19, 23, LORA_CS);
  cfg.spi = &SPI;

  radio.begin(cfg);
  radio.start();
  Serial.println("Mesh A Node (SF7) initialized");
}

void setup() {
  Serial.begin(115200);
  pinMode(BOARD_LED, OUTPUT);
  digitalWrite(BOARD_LED, LED_OFF);
  setupLoRaMeshA();
}

void loop() {
  outPkt.type = 0;
  for (int i = 0; i < 16; i++) outPkt.payload[i] = i;
  Serial.println("MeshA sending packet");
  radio.createPacketAndSend(BROADCAST_ADDR, &outPkt, 1);
  vTaskDelay(5000 / portTICK_PERIOD_MS);
}

#endif


// ========== MESH B NODE (SF8) ==========
#ifdef MeshBNode

#define LORA_CS   15
#define LORA_RST  4
#define LORA_IRQ  27
#define LORA_IO1  32

LoraMesher& radio = LoraMesher::getInstance();

void processPackets(void*) {
  for (;;) {
    ulTaskNotifyTake(pdPASS, portMAX_DELAY);
    while (radio.getReceivedQueueSize() > 0) {
      AppPacket<DataPacket>* pkt = radio.getNextAppPacket<DataPacket>();
      if (pkt) {
        Serial.println("MeshB received a packet:");
        for (int i = 0; i < pkt->payload->payload[0]; i++)
          Serial.printf("%d ", pkt->payload->payload[i]);
        Serial.println();
        radio.deletePacket(pkt);
      }
    }
  }
}

void setupLoRaMeshB() {
  LoraMesher::LoraMesherConfig cfg;
  cfg.module = LoraMesher::LoraModules::SX1276_MOD;
  cfg.loraCs = LORA_CS;
  cfg.loraRst = LORA_RST;
  cfg.loraIrq = LORA_IRQ;
  cfg.loraIo1 = LORA_IO1;
  cfg.spreadingFactor = 8; // SF8
  SPI.begin(18, 19, 23, LORA_CS);
  cfg.spi = &SPI;

  radio.begin(cfg);
  radio.start();

  TaskHandle_t rxTask;
  xTaskCreate(processPackets, "RX", 4096, NULL, 2, &rxTask);
  radio.setReceiveAppDataTaskHandle(rxTask);
  Serial.println("Mesh B Node (SF8) initialized");
}

void setup() {
  Serial.begin(115200);
  pinMode(BOARD_LED, OUTPUT);
  digitalWrite(BOARD_LED, LED_ON);
  setupLoRaMeshB();
}

void loop() {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}

#endif


// ========== BRIDGE NODE (SF7 ↔ SF8) ==========
#ifdef BridgeNode

#define LORA1_CS   5
#define LORA1_RST  14
#define LORA1_IRQ  2
#define LORA1_IO1  33

#define LORA2_CS   15
#define LORA2_RST  4
#define LORA2_IRQ  27
#define LORA2_IO1  32

LoraMesher& radioA = LoraMesher::getInstance(0);
LoraMesher& radioB = LoraMesher::getInstance(1);

TaskHandle_t taskA, taskB;

void processPacketsA(void*) {
  for (;;) {
    ulTaskNotifyTake(pdPASS, portMAX_DELAY);
    while (radioA.getReceivedQueueSize() > 0) {
      AppPacket<DataPacket>* pkt = radioA.getNextAppPacket<DataPacket>();
      if (pkt) {
        Serial.println("Bridge: forwarding from MeshA → MeshB");
        radioB.createPacketAndSend(BROADCAST_ADDR, pkt->payload, 1);
        radioA.deletePacket(pkt);
      }
    }
  }
}

void processPacketsB(void*) {
  for (;;) {
    ulTaskNotifyTake(pdPASS, portMAX_DELAY);
    while (radioB.getReceivedQueueSize() > 0) {
      AppPacket<DataPacket>* pkt = radioB.getNextAppPacket<DataPacket>();
      if (pkt) {
        Serial.println("Bridge: forwarding from MeshB → MeshA");
        radioA.createPacketAndSend(BROADCAST_ADDR, pkt->payload, 1);
        radioB.deletePacket(pkt);
      }
    }
  }
}

void setupLoRaModule(LoraMesher& r, int cs, int rst, int irq, int io1, int sf, SPIClass* spi) {
  LoraMesher::LoraMesherConfig cfg;
  cfg.module = LoraMesher::LoraModules::SX1276_MOD;
  cfg.loraCs = cs;
  cfg.loraRst = rst;
  cfg.loraIrq = irq;
  cfg.loraIo1 = io1;
  cfg.spreadingFactor = sf;
  cfg.spi = spi;
  r.begin(cfg);
  r.start();
}

void setup() {
  Serial.begin(115200);
  pinMode(BOARD_LED, OUTPUT);

  SPI.begin(18, 19, 23, LORA1_CS);
  SPIClass* vspi = new SPIClass(VSPI);
  vspi->begin(5, 19, 23, LORA2_CS);

  setupLoRaModule(radioA, LORA1_CS, LORA1_RST, LORA1_IRQ, LORA1_IO1, 7, &SPI);
  setupLoRaModule(radioB, LORA2_CS, LORA2_RST, LORA2_IRQ, LORA2_IO1, 8, vspi);

  xTaskCreate(processPacketsA, "ProcA", 4096, NULL, 2, &taskA);
  xTaskCreate(processPacketsB, "ProcB", 4096, NULL, 2, &taskB);
  radioA.setReceiveAppDataTaskHandle(taskA);
  radioB.setReceiveAppDataTaskHandle(taskB);

  Serial.println("Bridge Node active: relaying between MeshA(SF7) and MeshB(SF8)");
}

void loop() { vTaskDelay(1000 / portTICK_PERIOD_MS); }

#endif
