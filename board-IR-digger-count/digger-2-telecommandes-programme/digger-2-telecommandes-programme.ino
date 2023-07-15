/*
 * Arnaud https://github.com/arnaudrco/exemples/blob/main/Telecommande-IR-servomoteur/IR-esp-mp3-board-sleep/IR-esp-mp3-board-sleep.ino
 * ATTENTION rentrer les codes pour votre télécommande (emetteur IR)
 
 ajouter la bibliothèque IRremoteESP8266 dans outils > gérer les bibliothèque

MODELE PROGRAMMABLE
 
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
 */
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#define moteur1A D1
#define moteur1B D2
#define moteur2A D3 // rotation
#define moteur2B D4

#define haut 1
#define bas 2
#define droite 3
#define gauhec 4

#define HAUT 0x205D609F
#define DROITE 0x205D58A7
#define BAS 0x205D40BF
#define GAUCHE 0x205D708F
#define STOP  0x205DE01F
#define ARRET  0x205D38C7

// ------------- ajout d'un 2° télécommande
#define HAUT1 0xFF18E7
#define DROITE1 0xFF5AA5
#define BAS1 0xFF4AB5
#define GAUCHE1 0xFF10EF
#define STOP1  0xFF38C7
#define ARRET1  0xFFA25D
// ------------- direction
#define avant 1
#define arriere 2
#define droite 3
#define gauche 4
int lastDirection = 1;

#define GND D7
#define TEMPO 500 // temporisation 
// ---------------------------Forth-------------
#define MAX 100
int Index=0;
byte Forth[MAX]; // pile Forth
int vitesse = 192;  // 0 à 255
int programmation=1;

// Le digger a un moteur pour les roues et une commande de direction pour tourner à DROITE ou à GAUCHE

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
unsigned long mmm, dodo; // arret au bout de 5 minutes
int countM=0 , countD=0 ; // nb d'impulsions IR reçues

void empile( byte x ){
      Serial.print("empile ");
  Serial.println(Forth[Index]);
  if(Index < MAX ) Forth[Index++] = x;

}
byte depile(){
      Serial.print("depile ");
   
  if ( Index == 0 ) return(0);
  else {
         Serial.println(Forth[Index-1]);
    return ( Forth[--Index]   );
  }
}
void execute() {
  while ( int x=depile()){
        moteur(x,255);
          delay(100); // bip
    moteur(lastDirection,vitesse);
    moteur(x,vitesse);
  delay(400); // bip
      stopM();      stopD();
  delay(200); // bip   
  }
}
void setup() {
  pinMode(GND, OUTPUT);      // board IR
  digitalWrite(GND, LOW);
  Serial.begin(115200);
  // test FORTH
    empile(avant);
empile(arriere);
empile(droite);
empile(gauche);
execute();
  irrecv.enableIRIn();  // Start the receiver
  while (!Serial)  // Wait for the serial connection to be establised.
    delay(50);
  Serial.println();
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(kRecvPin);
  mmm = millis();    dodo = millis();
}
void stopM() { // avant arriere
    analogWrite(moteur1A, 0);
  analogWrite(moteur1B, 0);

}
void stopD() {// direction droite gauche
  analogWrite(moteur2A, 0);
  analogWrite(moteur2B, 0);
}
void stopA() { // stop all
stopM();
stopD();
}

void moteur( int x, int v){
    switch (x){
      if (lastDirection != avant) v=255; // accélération de départ
      case avant :
            Serial.println("Moteur avant");
                      lastDirection = avant;
                      analogWrite(moteur1A, 0);
                      analogWrite(moteur1B, v);  
      
        break;
        case arriere :
            Serial.println("Moteur arriere");
                       lastDirection = arriere;
                      analogWrite(moteur1A, v);
                      analogWrite(moteur1B, 0);  

        break;

              case droite :
            Serial.println("Moteur droite");
            analogWrite(moteur2A, 0);
            analogWrite(moteur2B, v);
 
      
        break;
        case gauche :
            Serial.println("Moteur gauche");
            analogWrite(moteur2A, v);
            analogWrite(moteur2B, 0);
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
      case HAUT : // 2
      case HAUT1 : // 2
           countM++; if(programmation)  empile( avant); else moteur(avant,vitesse);
             break;
      case BAS :
      case BAS1 :
           countM++; if(programmation) empile( arriere);else moteur(arriere,vitesse);
             break;
//-----------------------------   virage  ---------------------  +  -
       case DROITE : 
       case DROITE1 : 
            moteur(droite,255);countD++;if(programmation) empile( droite);
            break;
       case   GAUCHE :
       case   GAUCHE1 :
            moteur(gauche,255);countD++;if(programmation) empile( gauche);
            break;
//---------------- STOP---------------
      case   STOP :
      case   STOP1 :
            stopA();
            break;
            
//---------------- ARRET---------------
      case   ARRET :
      case   ARRET1 :
            // ESP.deepSleep(0);
            programmation = 1 - programmation;
               Serial.println(programmation);
            if(programmation) execute(); else  stopA(); ;// arret moteur
            break;
   }
    Serial.println("");
    irrecv.resume();  // Receive the next value
  }

}
