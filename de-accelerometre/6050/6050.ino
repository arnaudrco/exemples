/* 
 *  A REICHART sortie sur un servo moteur de la donnée Y 
 *  Get tilt angles on X and Y, and rotation angle on Z
 * Angles are given in degrees
 * 
 * License: MIT
 */

#include "Wire.h"
#include <MPU6050_light.h>
#include "Servo.h"

MPU6050 mpu(Wire);
Servo servo1, servo2, servo3;
unsigned long timer = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
        servo1.attach(11);
        
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Done!\n");
}

void loop() {
  mpu.update();
  
  if((millis()-timer)>10){ // print data every 10ms
  Serial.print("X : ");
  Serial.print(mpu.getAngleX());
  Serial.print("\tY : ");
  Serial.print(mpu.getAngleY());
  Serial.print("\tZ : ");
  Serial.println(mpu.getAngleZ());

  servo1.write ((( 180 + mpu.getAngleY() )* 180)/360);

;  timer = millis();  
  }
}
