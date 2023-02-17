// MPPT
// arnaudrco https://github.com/arnaudrco/exemples/blob/main/README.md
// placer le niveau POWER sur l'entrée A0
// LED placée entre pin 5 et 4
// Define stepper motor connections and steps per revolution:
#define pwmPin 3
#define pwmLed  4 // LED clignotante par PERIODE secondes
#define GND  5 // LED gnd
#define PERIODE 5
#define photoPin A0
#define SEUIL 650 // entrée divisée par 10 soit 15V / 5V * 1024 / 10 
#define S1 500 // entrée divisée par 10 soit 15V / 5V * 1024 / 10 
#define MIN 16 // seuil bas
#define MAX 255 // seuil haut
int PWM=MIN;
   
void setup() {
  Serial.begin(115200);
    Serial.println("====================");
  // Declare pins as output:
  pinMode(pwmPin, OUTPUT);
      pinMode(pwmLed, OUTPUT);
          pinMode(GND, OUTPUT);
                  analogWrite(GND, 230);// réglage de la luminosité
}
void loop() {
  int ttt=Tension();
  if ( ttt > SEUIL ) {
    PWM = MAX;
  }
  else if (ttt > S1 ) {
    PWM = MAX * ( ttt - S1 ) / (SEUIL - S1);
  }
  else {
    PWM=0;
  }
  if (PWM < 0 ) PWM = 0;
  if ( PWM > 255 ) PWM = 255;
           Serial.print(";" );
          Serial.println(PWM );

}

int Tension(){ // lecture POWER

  
      unsigned long currentMillis = millis();
      digitalWrite(pwmLed,  (((currentMillis/PERIODE) % 256) < PWM ));

     
   analogWrite(pwmPin, PWM);
      delay(1);
      Serial.print(analogRead(photoPin) );
 
   
   return ( analogRead(photoPin) );
}
