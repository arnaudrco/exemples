/*
 * Arnaud https://github.com/arnaudrco/exemples/blob/main/Telecommande-IR-servomoteur/IR-esp-mp3-board-sleep/IR-esp-mp3-board-sleep.ino
 * ATTENTION rentrer les codes pour votre télécommande (emetteur IR)
code pour commander les 3 moteurs ( avance , tourne, soulève la benne)
FF22DD >=
FFE01F EQ

FF02FD <<
FFC23D >>

FF906F +
FFA857 -

A lampe 6B863ACC
A hp 306304AC
B lampe 7046F740
B hp 84D30638

 * IRremoteESP8266: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * This is very simple teaching code to show you how to use the library.
 * If you are trying to decode your Infra-Red remote(s) for later replay,
 * use the IRrecvDumpV2.ino (or later) example code instead of this.
 * An IR detector/demodulator must be connected to the input kRecvPin.
 * Copyright 2009 Ken Shirriff, http://arcfn.com
 * Example circuit diagram:
 *  https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-receiving
 * Changes:
 *   Version 0.2 June, 2017
 *     Changed GPIO pin to the same as other examples.
 *     Used our own method for printing a uint64_t.
 *     Changed the baud rate to 115200.
 *   Version 0.1 Sept, 2015
 *     Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009
 */
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#define moteur1A D1
#define moteur1B D2
#define moteur2A D3
#define moteur2B D4
#define moteur3A D5
#define moteur3B D0

#define GND D7

int vitesse = 128;  // 0 à 255

// An IR detector/demodulator is connected to GPIO pin 14(D5 on a NodeMCU
// board).
// Note: GPIO 16 won't work on the ESP8266 as it does not have interrupts.
// Note: GPIO 14 won't work on the ESP32-C3 as it causes the board to reboot.
#ifdef ARDUINO_ESP32C3_DEV
const uint16_t kRecvPin = 10;  // 14 on a ESP32-C3 causes a boot loop.
#else  // ARDUINO_ESP32C3_DEV
const uint16_t kRecvPin = 14;
#endif  // ARDUINO_ESP32C3_DEV


// kRecvPin 

IRrecv irrecv(D6);

decode_results results;
#define TEMPO 600000 // temporisation de 10 mn
unsigned long int mmm;
int pin=0; // pin précédente commande

void setup() {
//  ESP.lightSleep();
  mmm=millis();
  pinMode(GND, OUTPUT);      // board IR
digitalWrite(GND, LOW);

  Serial.begin(115200);
  irrecv.enableIRIn();  // Start the receiver
  while (!Serial)  // Wait for the serial connection to be establised.
    delay(50);
  Serial.println();
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(kRecvPin);
}
void stopM() {
  analogWrite(moteur2A, 0);
  analogWrite(moteur2B, 0);
    analogWrite(moteur1A, 0);
  analogWrite(moteur1B, 0);
      analogWrite(moteur3A, 0);
  analogWrite(moteur3B, 0);
}
void analogWriteRampe( int x, int v){ // si même commande et temps < 200 ms vitesse v
  if ( pin != x ){
        pin = x;
     for(int i = 0 ; i<256 ; i++){
                          analogWrite(x, i);  
                          delay ( 4 ) ;
    }
  } else {
    analogWrite(x, v);
    delay ( 1000 ) ;

  }
 
}
void moteur( int x, int v){
    switch (x){
      case 1 :
            Serial.println("Moteur 1");
            if(v>0){
                      analogWrite(moteur1A, 0);
                      analogWriteRampe(moteur1B, v);  
            } else {
              analogWrite(moteur1B, 0);
                analogWriteRampe(moteur1A, -v); 
            }
        break;
        case 2 :
            Serial.println("Moteur 2");
            if(v>0){
                      analogWrite(moteur2A, 0);
                      analogWriteRampe(moteur2B, v);  
            } else {
              analogWrite(moteur2B, 0);
                analogWriteRampe(moteur2A, -v);
                
            }

        break;
               case 3 :
            Serial.println("Moteur 3");
            if(v>0){
                      
                      analogWriteRampe(moteur3B, v);
                      analogWrite(moteur3A, 0);  
            } else {
              analogWrite(moteur3B, 0);
                analogWriteRampe(moteur3A, -v);
                
            }

        break;

  }
 //    delay(1000); // pendant 1 seconde
    stopM();
}
void loop() {
/*  if ((mmm - millis()) > TEMPO){ // sommeil profond
    ESP.deepSleep(0);
  }*/
  if (irrecv.decode(&results)) {
    mmm = millis();
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value, HEX);
    switch (results.value){
//-----------------------------   moteur 1  ---------------------  <<  >>


      case 0x6B863ACC :
      case 0xFF02FD :
            moteur( 1,128);
             break;
      case 0x306304AC :
            case 0xFFC23D :
            moteur( 1,-128);
             break;
//-----------------------------   moteur 2 ---------------------  +  -
      case 0x7046F740 :
            case 0xFF906F :
            moteur( 2,128);
            break;
      case 0x84D30638 :
                  case 0xFFA857 :
            moteur( 2,-128);
            break;
 
//----------------------  moteur 3 --------------------  
case 0x9D1F8A80  : // haut
case 0XB8D4BB2C  : 
           case 0xFF22DD : // mp3
            moteur( 3,128);
             break;
      case 0x985ECE0C  :
      case 0x4B400BF8 :
            case 0xFFE01F : // mp3
            moteur( 3,-128);
             break;
   }
    Serial.println("");
    irrecv.resume();  // Receive the next value
  }
  delay(100);
}
