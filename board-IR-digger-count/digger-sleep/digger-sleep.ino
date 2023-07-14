/*
 * Arnaud https://github.com/arnaudrco/exemples/blob/main/Telecommande-IR-servomoteur/IR-esp-mp3-board-sleep/IR-esp-mp3-board-sleep.ino
 *  rentrer les codes pour votre télécommande (emetteur IR)
 
 ajouter la bibliothèque IRremoteESP8266 dans outils > gérer les bibliothèque

6 boutons 

Après les essais de Maxence 5 ans, j'ai ajouté à sa demande un  boutons STOP pour arrêter le moteur et ARRET marche arrêt pour économiser les piles
 
**********************************************************
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
 *     
 *     
 *    
 *    
 *    
2
FF18E7
6
FF5AA5
8
FF4AB5
4
FF10EF 

FF629D haut
FFA857 bas

FFC23D droite 
FF22DD gauche

 */
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#include <Servo.h>



#define moteur1A D1
#define moteur1B D2
#define moteur2A D3 // rotation
#define moteur2B D4

#define haut 0x205D609F
#define droite 0x205D58A7

#define bas 0x205D40BF
#define gauche 0x205D708F

#define STOP  0x205DE01F

#define ARRET  0x205D38C7



#define GND D7


#define TEMPO 500 // temporisation 

int vitesse = 192;  // 0 à 255

// Le digger a un moteur pour les roues et une commande de direction pour tourner à droite ou à gauche

// le programme active le moteur ou les roues suivant une durée countM (moteur) ou count D (direction)


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
unsigned long mmm;
unsigned long dodo; // arret au bout de 5 minutes
int countM=0 , countD=0 ; // nb d'impulsions IR reçues

void ddd(){
  analogWrite(moteur2A, 0);
  analogWrite(moteur2B, 255);
}
void ggg(){
  analogWrite(moteur2A, 255);
  analogWrite(moteur2B, 0);
}


void setup() {
//  Test droite gauche
 ddd();   delay(500);  stopM() ;
 delay(500);
  ggg();   delay(500);  stopM() ;
   delay(500);

   
  bip();

  pinMode(GND, OUTPUT);      // board IR
  digitalWrite(GND, LOW);

  Serial.begin(115200);
  irrecv.enableIRIn();  // Start the receiver
  while (!Serial)  // Wait for the serial connection to be establised.
    delay(50);
  Serial.println();
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(kRecvPin);
  mmm = millis();
    dodo = millis();
}

void bip(){
   // AUTOTEST les deux moteurs tournent en marche avant,
  // à haute vitesse

  //moteur 1
  analogWrite(moteur1A, vitesse);
  analogWrite(moteur1B, 0);
  
  delay(200); // bip
    stopM();
  delay(500); // bip
        analogWrite(moteur1A, 0);
  analogWrite(moteur1B, vitesse);
  
  delay(200); // bip
      stopM();
}
void stopM() {

    analogWrite(moteur1A, 0);
  analogWrite(moteur1B, 0);

}
void stopD() {
  analogWrite(moteur2A, 0);
  analogWrite(moteur2B, 0);
}
void moteur( int x, int v){
    switch (x){
      case 1 :
            Serial.println("Moteur 1");

                      analogWrite(moteur1A, 0);
                      analogWrite(moteur1B, v);  
      
        break;
        case 2 :
            Serial.println("Moteur 2");
 
                      analogWrite(moteur1A, v);
                      analogWrite(moteur1B, 0);  

        break;
  }
}
void loop() {

    if ((millis()- dodo) > 300000){ 
            ESP.deepSleep(0);
    }
  if ((millis()- mmm) > TEMPO){ // 
    mmm= millis();
    if(countM > 0 ){
      countM --; 
    } else {
       stopM();
    }
  if(countD > 0 ){
      countD --; 
    } else {
       stopD();
    }
  }
  if (irrecv.decode(&results)) {
        mmm= millis();    dodo= millis();
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value, HEX);
    switch (results.value){
//-----------------------------   moteur 1 ou 2  ---------------------  <<  >>


      case haut : // 2
            moteur( 1,vitesse); countM++;
             break;
      case bas :
            moteur( 2,vitesse);countM++;
             break;
//-----------------------------   virage  ---------------------  +  -
       case droite : 
            ddd();countD++;
            break;
       case   gauche :
            ;ggg();countD++;
            break;
//---------------- STOP---------------
      case   STOP :
            stopD();stopM();
            break;
            
//---------------- ARRET---------------
      case   ARRET :
            ESP.deepSleep(0);
            break;



   }
    Serial.println("");
    irrecv.resume();  // Receive the next value
  }

}
