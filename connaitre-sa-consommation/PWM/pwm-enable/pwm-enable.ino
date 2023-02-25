// MPPT
// arnaudrco https://github.com/arnaudrco/exemples/blob/main/README.md
// placer le niveau POWER sur l'entrée A0

#define chargePin 9 // charge L298
#define enablePin 10 // enable L298

#define LED  2 // LED clignotante par PERIODE secondes
#define GND 3 // LED gnd
#define LEDV  4 // LED clignotante verte par PERIODE secondes
#define LEDB  5 // LED clignotante bleue par PERIODE secondes
#define PERIODE 5

#define photoPin A0

#define SEUIL 500 // entrée divisée par 10 soit 25V / 5V * 1024 / 10 
#define DELTA 100 //  plage de réglage de 5V  
#define S1 SEUIL - DELTA

#define MIN 64 // seuil bas
#define MAX 128 // seuil haut

int charge=0;
int enable=0;
   
void setup() {
  Serial.begin(115200);
  setPwmFrequency(chargePin, 1); // 4 KHz = 32 KHz / 8
    Serial.println("====================");
  // Declare pins as output:
 
      pinMode(LED, OUTPUT); // 
      pinMode(LEDV, OUTPUT); // 
      pinMode(LEDB, OUTPUT); // 
      pinMode(GND, OUTPUT);
      analogWrite(GND, 60);// réglage de la luminosité
      // ----------- AUTOTEST LED ------------------
      digitalWrite(LEDV, HIGH);
      digitalWrite(LEDB, HIGH);
             digitalWrite(LED, LOW);delay(500); digitalWrite(LED, HIGH);
             digitalWrite(LEDV, LOW);  delay(500); digitalWrite(LEDV, HIGH);     
             digitalWrite(LEDB, LOW);delay(500); digitalWrite(LEDB, HIGH);
             
                  pinMode(chargePin, OUTPUT);
           pinMode(enablePin, OUTPUT); 
            digitalWrite(enablePin, LOW); 

}
void loop() {
        unsigned long currentMillis = millis();
  analogWrite(chargePin, charge);
    analogWrite(enablePin, enable);
    int ttt=Tension();
  if ( ttt > (SEUIL + DELTA) ) { // -----------bleu -----------------
             digitalWrite(LED, HIGH);
             digitalWrite(LEDV, HIGH);       
             digitalWrite(LEDB, LOW);
             enable = 255;
             charge = MAX;
      
  }
  else if (ttt > SEUIL ) { // ----------- vert double--------------------

        charge = map( ttt, SEUIL, SEUIL + DELTA, 0, MAX);
        enable = 255;
          digitalWrite(LEDV,  (((currentMillis/PERIODE) % 256) > charge ));  
             digitalWrite(LED, HIGH);     
             digitalWrite(LEDB, HIGH);
             
    // enable = MAX * ( ttt - S1 ) / (DELTA);
  }
  else if (ttt > S1 ) { // ------------ rouge simple ----------------------
    enable = map( ttt, S1, SEUIL, 0, 255);
    charge = 0;
          digitalWrite(LED,  (((currentMillis/PERIODE) % 256) > enable ));
             digitalWrite(LEDV, HIGH);       
             digitalWrite(LEDB, HIGH);

  }
  else {
            digitalWrite(LED, HIGH);
             digitalWrite(LEDV, HIGH);       
             digitalWrite(LEDB, HIGH);

    charge=0;
    enable=0;
  }

}
int Tension(){ // lecture POWER
      delay(1);
      Serial.print(analogRead(photoPin) );
        Serial.print(";" );
          Serial.println(charge );
   return ( analogRead(photoPin) );
}
void setPwmFrequency(int pin, int divisor) {
   byte mode;
   if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
      switch(divisor) {
         case 1: mode = 0x01; break;
         case 8: mode = 0x02; break;
         case 64: mode = 0x03; break;
         case 256: mode = 0x04; break;
         case 1024: mode = 0x05; break;
         default: return;
      }
      if(pin == 5 || pin == 6) {
         TCCR0B = TCCR0B & 0b11111000 | mode;
      } else {
         TCCR1B = TCCR1B & 0b11111000 | mode;
      }
   } else if(pin == 3 || pin == 11) {
      switch(divisor) {
         case 1: mode = 0x01; break;
         case 8: mode = 0x02; break;
         case 32: mode = 0x03; break;
         case 64: mode = 0x04; break;
         case 128: mode = 0x05; break;
         case 256: mode = 0x06; break;
         case 1024: mode = 0x7; break;
         default: return;
      }
      TCCR2B = TCCR2B & 0b11111000 | mode;
   }
}
