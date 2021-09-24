// LCD5110_NumberFonts 
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// This program is a demo of the included number-fonts,
// and how to use them.
//
// This program requires a Nokia 5110 LCD module
// and Arduino 1.5.2 (or higher).
//
// It is assumed that the LCD module is connected to
// the following pins.
//      SCK  - Pin 8 >> 4
//      MOSI - Pin 9 >> 5
//      DC   - Pin 10 >> 6
//      RST  - Pin 11 >> 8
//      CS   - Pin 12 >> 7
//
#define PIN_PLUS 3
#define PIN_BL 2

#define CSS_WAKE A3
#define CSS_GND A6
#define CSS_PLUS A7



#include <LCD5110_Basic.h>
#include <Wire.h>
#include "Adafruit_CCS811.h"
Adafruit_CCS811 ccs;



LCD5110 myGLCD(4,5,6,8,7);
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];

int invertLCD=0;

void setup()
{
  
pinMode(PIN_PLUS, OUTPUT);      // sets the digital pin as output
pinMode(PIN_BL, OUTPUT); 
digitalWrite(PIN_PLUS, HIGH);
digitalWrite(PIN_BL, HIGH);

pinMode(CSS_GND, OUTPUT);      // alimentation CSS 
pinMode(CSS_PLUS, OUTPUT);
pinMode(CSS_WAKE, OUTPUT); 
digitalWrite(CSS_PLUS, HIGH);
digitalWrite(CSS_GND, LOW);
digitalWrite(CSS_WAKE, LOW);

  myGLCD.InitLCD(); // initialisation LCD
  myGLCD.setContrast(60);
    Serial.begin(115200);
  Wire.begin();
  

  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
  }

  // Wait for the sensor to be ready
  while(!ccs.available());
}


void loop()
{
  char status;
    myGLCD.clrScr();
  invertLCD++;

  if(ccs.available()){
    if(!ccs.readData()){
      Serial.print("CO2: ");
      Serial.print(ccs.geteCO2());
      Serial.print("ppm, TVOC: ");
      Serial.println(ccs.getTVOC());
    }
    else{
      Serial.println("ERROR!");
      while(1);
    }
  }

   myGLCD.setFont(SmallFont);
      myGLCD.print("CO2",LEFT, 0);
   myGLCD.setFont(MediumNumbers);
    myGLCD.printNumF(ccs.geteCO2(), 0, RIGHT, 0);

     myGLCD.setFont(SmallFont);
        myGLCD.print("TVOC",LEFT, 42);
    myGLCD.setFont(BigNumbers);

    myGLCD.printNumI(ccs.getTVOC(), RIGHT, 24);
   myGLCD.invert(invertLCD % 2);
   
  delay(500);
}
