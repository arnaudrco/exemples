// MPPT
// arnaudrco https://github.com/arnaudrco/exemples/blob/main/README.md
// placer le niveau POWER sur l'entrée A0

// Define stepper motor connections and steps per revolution:
#define pwmPin D2

#define delai 10
#define stepsPerScan 20 // nombre de pas par itération
#define seuil1 5 // nombre de pas pour le seuil bas
#define seuil2 15 // seuil haut
#define MIN 0 // nombre de pas pour le seuil bas
#define MAX 1000 // seuil haut
#define NN 1000 // itérations pour readPower


#define photoPin A0

int PWM=MIN;
float rrr=0;
       
void setup() {
  Serial.begin(115200);
    Serial.println("====================");
  // Declare pins as output:
  pinMode(pwmPin, OUTPUT);
     analogWrite(pwmPin, MIN);
     
}

void loop() {
  float nnn;
  nnn= readPower();
  if (nnn >= rrr ) {
    PWM++; 
  }
  else PWM--;
  rrr = nnn ; 
  if (PWM < 0 ) PWM = 0;
  if ( PWM > MAX ) PWM = MAX;
   Serial.print(nnn );Serial.print(";" );
             Serial.println(PWM );

}
               

float readPower(){ // lecture POWER V² PWM
  long int ppp;
  long int mmm=0; // max
   analogWrite(pwmPin, PWM);
   for (int i=NN; i; i--) {
      ppp= analogRead(photoPin);
      if (ppp > mmm) mmm=ppp; 
   }

   return (mmm * mmm * PWM / MAX);

}
