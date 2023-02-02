// MPPT
// arnaudrco https://github.com/arnaudrco/exemples/blob/main/README.md
// placer le niveau POWER sur l'entrée A0

// Define stepper motor connections and steps per revolution:
#define pwmPin 3
#define photoPin A0
#define SEUIL 300 // entrée divisée par 10 soit 15V / 5V * 1024 / 10 
#define MIN 0 // seuil bas
#define MAX 255 // seuil haut
int PWM=MIN;
   
void setup() {
  Serial.begin(115200);
    Serial.println("====================");
  pinMode(pwmPin, OUTPUT);
     
}
void loop() {
  if (Tension() < SEUIL ) {
    PWM--; 
  }
  else PWM++;
  if (PWM < 0 ) PWM = 0;
  if ( PWM > 255 ) PWM = 255;
    

}

int Tension(){ // lecture POWER
   analogWrite(pwmPin, PWM);
      delay(1);
      Serial.print(analogRead(photoPin) );
        Serial.print(";" );
          Serial.println(PWM );
   
   return ( analogRead(photoPin) );
}
