/*
 * LoRa E220 ARNAUDRCO ajout d'un flash de 10 ms pour chaque reception d'un caractère 
 * 
 * Start device or reset to send a message
 * by Renzo Mischianti <https://mischianti.org>
 * https://mischianti.org/category/my-libraries/lora-e220-llcc68-devices/
 *
 * E220         ----- Wemos D1 mini
 * M0         ----- GND
 * M1         ----- GND
 * TX         ----- PIN D2 (PullUP)
 * RX         ----- PIN D3 (PullUP)
 * AUX        ----- Not connected
 * VCC        ----- 3.3v/5v
 * GND        ----- GND
 *
 */
#include "Arduino.h"
#include <SoftwareSerial.h>
 
SoftwareSerial mySerial(D2, D3); // WeMos RX --> e220 TX - WeMos TX --> e220 RX

// Temps entre 2 requêtes (ms)
unsigned long lastQuery = 0;
const unsigned long queryInterval = 2000;

void setup() {
  Serial.begin(9600);
  delay(500);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(LED_BUILTIN,HIGH );    // turn the LED off by making the voltage LOW

 
  Serial.println("Hi, I'm going to send message!");
 
  mySerial.begin(9600);
  mySerial.println("Hello, world?");
}
 
void loop() {
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
    if (millis() - lastQuery > queryInterval) {
    lastQuery = millis();
    mySerial.println("Hello, world?");
  }
}
