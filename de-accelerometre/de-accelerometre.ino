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
}
void clignote(int x, int y, int n){ // clignote n fois
    for(int i=0;i<n;i++) {
        lc.setLed(0,x,y,false);
        delay(delaytime);
        lc.setLed(0,x,y,true);
               delay(delaytime);
      }
}
void rampe(int x, int n){ // rampe de 28 leds
 if (x<8){ // xxxxxxxxx bas
   clignote(x,0,n);
 } else { // yyyyyyyyy droite
   x = x - 8;
   if (x<8){ 
   clignote(7,x,n);
 } else { // xxxxxxxxxx haut
   x = x - 8;
   if (x<8){
   clignote(7-x,7,n);
 } else { // yyyyyyyyy gauche
   x = x - 8;
   if (x<8){ 
   clignote(0,7-x,n);
 }
}
}
}
}
void loop() { 
  for(int i=0;i<28;i++) {
       rampe(i, 2);
      }
      /*
  //read the number cascaded devices
  int devices=lc.getDeviceCount();
  
  //we have to init all devices in a loop
  for(int row=0;row<8;row++) {
    for(int col=0;col<8;col++) {
      for(int address=0;address<devices;address++) {
        delay(delaytime);
        lc.setLed(address,row,col,true);
        delay(delaytime);
        lc.setLed(address,row,col,false);
      }
    }
  } */
}
