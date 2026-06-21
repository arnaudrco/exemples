/*
 * 
 * BALISE avec affichage 126x64 en spi avec RA02 en mode spi

ST7567A
- D5 (GPIO14) | SCK
- D7 (GPIO13) | SDA (MOSI)
- D1 (GPIO5) | RST
- D2 (GPIO4) | CS
- D0 (GPIO16) | A0 (RS)
- 3V3 | VCC
- GND | GND

*/


#include "Arduino.h"
#include <SoftwareSerial.h>
#include <U8x8lib.h>
#define LIGNES  7

#include <LoRa.h>
#define NSS D0 // D0
#define RST D8
#define DIO0 D4

U8X8_ST7567_ENH_DG128064I_4W_SW_SPI  display( /* clock=*/ 14, /* data=*/ 13, /* cs=*/ D2, /* dc=*/ D6, /* reset=*/ D1);
// U8X8_ST7567_ENH_DG128064I_4W_HW_SPI display(/* cs=*/ D0, /* dc=*/ D6, /* reset=*/ D8); 

// Temps entre 2 requêtes (ms)
unsigned long lastQuery = 0;
const unsigned long queryInterval = 3000;
int comptage; // comptage du nombre de messages émis
int ligne ; // ligne pleine
#define characteres   120 // nombre de charactères max 

void setup() {
   Serial.begin(115200);
 display.begin();  delay(100); // initDisplay
  display.setPowerSave(0);
  display.setFont(u8x8_font_amstrad_cpc_extended_r);
display.setCursor(0, 0);
display.setInverseFont(1);
display.println("LORA 7567");  
    delay(500);
    display.setInverseFont(0);
  Serial.println("Hi, I'm going to send message!");
 
  LoRa.setPins(NSS, RST, DIO0);
  LoRa.setTxPower(20);     // Puissance 20dBm
LoRa.setSpreadingFactor(7);  // SF7 (équilibre vitesse/portée)
LoRa.setSignalBandwidth(125E3); // 125kHz
  while (!LoRa.begin(433E6)) {
    Serial.println("LoRa init échoué!");

  }
    Serial.println("");
  Serial.println("LoRa RÉCEPTEUR prêt");
       display.println("Balise OK");
  display.display();
  
  String message = "Arnaud";
  LoRa.beginPacket();
  LoRa.print(message);
  delay(1000);
}

void loop() { // ---------- reception de n caracteres -----------


  char c;
  int packetSize = LoRa.parsePacket();
  if (packetSize) {

      int sss= LoRa.packetRssi();
  Serial.println(sss);
         display.print(sss); 
         display.print(" "); ligne += 1;
  display.display();
  
    Serial.print("Reçu: ");
    while (LoRa.available()) {
      if(ligne++ > LIGNES){
  display.setCursor(0,0);
  ligne = 0;
  }
      Serial.print(c=LoRa.read());
       display.print(c);
  display.display();
    }
    Serial.println();
      display.print(" "); ligne++;
      display.display();
  }
      if (millis() - lastQuery > queryInterval) { // --------------- comptage balise
          lastQuery = millis();
          String message = String(comptage++);
          LoRa.beginPacket();
          LoRa.print(message);
        LoRa.endPacket();
        Serial.println(comptage);
        if( comptage > 100 ) comptage = 0;// comptage 99 max
  }

}
