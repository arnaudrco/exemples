#include "Wire.h"
#include <MPU6050_light.h>

MPU6050 mpu(Wire);
long timer = 0;

#include <avr/pgmspace.h>
#include <LedControl.h>
 
const int numDevices = 1;      // number of MAX7219s used

#define CLK   12
#define CS 11
#define DIN   10
#define PIN_GND 9
#define PIN_VCC 8

// LedControl lc=LedControl(12,11,10,numDevices); 
LedControl lc=LedControl(DIN,CLK,CS,numDevices); //  

 //   lc.setRow(0,a,y); 


/* we always wait a bit between updates of the display */
unsigned long delaytime=20;
byte y=255;
int xx=0; // led de la carre de 30

/* 
 This time we have more than one device. 
 But all of them have to be initialized 
 individually.
 */
void setup() {
  

  //we have already set the number of devices when we created the LedControl
  int devices=lc.getDeviceCount();
  //we have to init all devices in a loop
  for(int address=0;address<devices;address++) {
    /*The MAX72XX is in power-saving mode on startup*/
    lc.shutdown(address,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(address,8);
    /* and clear the display */
    lc.clearDisplay(address);
  }
  // ligne allumée
  lc.setRow(0,0,y); 
  lc.setRow(0,7,y); 
  lc.setColumn(0,0,y); 
    lc.setColumn(0,7,y); 

  Serial.begin(115200);
  Wire.begin();
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  Serial.println(F("Calculating offsets, do not move MPU6050"));
 // delay(1000);
  roue();
  mpu.calcOffsets(true,true); // gyro and accelero
  Serial.println("Done!\n");
  
}
void allume(int x, int y){ 
        lc.setLed(0,x,y,true);
               delay(delaytime);
}
void eteint(int x, int y){ 
        lc.setLed(0,x,y,false);
               delay(delaytime);
}
void clignote(int x, int y){ // clignote 
       eteint(x,y);
           allume(x,y);
}
void led_allume(int x, int y, boolean allume){ // allume 
       lc.setLed(0,x,y,allume);
}
void carre(int x){ // allume led  x carre de 30 leds
  x=x %30 ;
  if (x<0) {
    x= x+30;
  }
 if (x<8){ // xxxxxxxxx bas
   clignote(x,0);
 } else { // yyyyyyyyy droite
   x = x - 8;
   if (x<8){ 
   clignote(7,x);
 }  else { // xxxxxxxxxx haut
   x = x - 8;
   if (x<8){
   clignote(7-x,7);
 } else { // yyyyyyyyy gauche
   x = x - 8;
   if (x<8){ 
   clignote(0,7-x);
 }
}}
}
}
void carre_allume(int x, boolean allume){ // allume led  x carre de 30 leds si allume 
  x=x %30 ;
  if (x<0) {
    x= x+30;
  }
 if (x<8){ // xxxxxxxxx bas
   led_allume(x,0,allume);
 } else { // yyyyyyyyy droite
   x = x - 8;
   if (x<8){ 
   led_allume(7,x,allume);
 }  else { // xxxxxxxxxx haut
   x = x - 8;
   if (x<8){
   led_allume(7-x,7,allume);
 } else { // yyyyyyyyy gauche
   x = x - 8;
   if (x<8){ 
   led_allume(0,7-x,allume);
 }
}}
}
}
int led(int x){
  // retourne 1 2 3 neutre 4 5 6 
  int plus=1;
  if(x<0){
    x = -x; plus = 0; // négatif
  }
  if (x>2) {
    if (plus) {
      return(6) ;
    } else return(1);
  }
  if (x>1) {
    if (plus) {
      return(5) ;
    } else return(2);
  }
  if (x>0) {
    if (plus) {
      return(4) ;
    } else return(3);
  }
  return(0);
}
void roue() { 
  for(int i=0;i<31;i++) {
       carre(i);
  }
}
void loop() { 
/*
      for(int i= -3;i< 4;i++) {
        int l=led(i); // led à allumer
        if (l){
           allume(l,4);
           eteint(l,4);
      }
      }*/
mpu.update();

  if(millis() - timer > 1000){ // print data every second
   
    Serial.print(F("TEMPERATURE: "));Serial.println(mpu.getTemp());
    Serial.print(F("ACCELERO  X: "));Serial.print(mpu.getAccX());
    Serial.print("\tY: ");Serial.print(mpu.getAccY());
    Serial.print("\tZ: ");Serial.println(mpu.getAccZ());
  
    Serial.print(F("GYRO      X: "));Serial.print(mpu.getGyroX());
    Serial.print("\tY: ");Serial.print(mpu.getGyroY());
    Serial.print("\tZ: ");Serial.println(mpu.getGyroZ());
  
    Serial.print(F("ACC ANGLE X: "));Serial.print(mpu.getAccAngleX());
    Serial.print("\tY: ");Serial.println(mpu.getAccAngleY());
    
    Serial.print(F("ANGLE     X: "));Serial.print(mpu.getAngleX());
    Serial.print("\tY: ");Serial.print(mpu.getAngleY());
    Serial.print("\tZ: ");Serial.println(mpu.getAngleZ());
    Serial.println(F("=====================================================\n"));
    timer = millis();
      }
      int ii= (( 180 + mpu.getAngleZ() )* 30)/360;
      if (xx != ii){
        carre_allume( xx , true   ); 
        carre_allume( ii, false   );// eteint la led de la carre
        xx = ii;
      }
      
    int lx=led(mpu.getGyroY()/16); // led à allumer
    int ly=led(mpu.getGyroX()/16); // led à allumer
    if (lx){
           if(ly){
            allume(lx,ly);
            eteint(lx,ly);
           } else {
            allume(lx,4);
            eteint(lx,4);
           }
           
      }else if( ly ){
        allume(4,ly);
        eteint(4,ly);
      }
}
