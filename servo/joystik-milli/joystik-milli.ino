/* 
 *  version avec timer
 *  Joystick connecté sur A4 A5
 *  Servomoteurs sur pin 5 et 6
 */

#include <Servo.h>

#define pin1 5 
#define pin2 6 
  
int joyX = A5;
int joyY = A4;
  
int servoVal;
  
void setup() 
{

pinMode(pin1, OUTPUT);
   digitalWrite(pin1, LOW);
   pinMode(pin2, OUTPUT);
   digitalWrite(pin2, LOW);

}
  
void loop()
{

 unsigned long currentMillis = millis();

  servoVal = analogRead(joyX);
  servoVal = map(servoVal, 0, 1023, 500, 1500);

       digitalWrite(pin1, HIGH);
       delayMicroseconds(servoVal);
     digitalWrite(pin1, LOW);

  servoVal = analogRead(joyY);
  servoVal = map(servoVal, 0, 1023, 500, 1500);
         digitalWrite(pin2, HIGH);
       delayMicroseconds(servoVal);
     digitalWrite(pin2, LOW);
     
 while ( (millis()-currentMillis) < 20 ) ; // 20 ms pour une trame

}
