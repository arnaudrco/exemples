/* 
 *  version avec 9 servomoteurs au maximum 
 *  Joystick connecté sur A4 A5
 *  Servomoteurs sur pin 5 et 6
 *  Ligne de commande servomoteurs sur pin 7
 */

#include <Servo.h>

#define pin1 5 
#define pin2 6 
#define pin3 7

#define NB_SERVO 4

int joyX = A5;
int joyY = A4;
  
int servoVal;
int trame[NB_SERVO]; // 9 servomoteurs au maximum
  
void setup() 
{

pinMode(pin1, OUTPUT);
   digitalWrite(pin1, LOW);
   pinMode(pin2, OUTPUT);
   digitalWrite(pin2, LOW);
      pinMode(pin3, OUTPUT);
   digitalWrite(pin3, HIGH);// niveau haut au repos


}
void init_trame(){ // initialise les valeurs de commande de trame
  trame[0] = map(analogRead(joyX), 0, 1023, 500, 2000);
  trame[1] = map(analogRead(joyY), 0, 1023, 500, 2000);
  for(int i=2; i<NB_SERVO ;i++){
    trame[i] = 500;
  } 
}
void loop_trame()
{
 unsigned long currentMillis = millis();
    init_trame();
       digitalWrite(pin1, HIGH);
       delayMicroseconds(trame[0]);
     digitalWrite(pin1, LOW);
     
         digitalWrite(pin2, HIGH);
       delayMicroseconds(trame[1]);
     digitalWrite(pin2, LOW);
// impulsion de départ
    digitalWrite(pin3, LOW);
     delayMicroseconds(200);
     digitalWrite(pin3, HIGH);
     
       for(int i=0; i<NB_SERVO ;i++){ 
 
       delayMicroseconds(trame[i]);
     digitalWrite(pin3, LOW);
     delayMicroseconds(200);
              digitalWrite(pin3, HIGH);
  } 
 
 while ( (millis()-currentMillis) < 20 ) ; // 20 ms pour une trame

} 
void loop()
{
   loop_trame();
}
