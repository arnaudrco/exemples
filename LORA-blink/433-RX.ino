#include <SPI.h>
#include <LoRa.h>

// #define NSS 15
#define NSS D0
#define RST 4
#define DIO0 5

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  delay(500);
  
  LoRa.setPins(NSS, RST, DIO0);
  
  while (!LoRa.begin(433E6)) {
    Serial.println("LoRa init échoué!");

  }
    Serial.println("");
  Serial.println("LoRa RÉCEPTEUR prêt");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Reçu: ");
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }
    Serial.println();
  }
}
