/* 
 *  version analogique
 *  Joystick connecté sur A4 A5
 *  Servomoteurs sur pin 5 et 6
 */

#include <Servo.h>
  
Servo servo1;
Servo servo2;
int joyX = A5;
int joyY = A4;
  
int servoVal;
  
void setup() 
{
  servo1.attach(6);
  servo2.attach(5);
}
  
void loop()
{
  
  servoVal = analogRead(joyX);
  servoVal = map(servoVal, 0, 1023, 0, 180);
  servo1.write(servoVal);

  servoVal = analogRead(joyY);
  servoVal = map(servoVal, 0, 1023, 0, 180);
  servo2.write(servoVal);
  delay(15);  
}
