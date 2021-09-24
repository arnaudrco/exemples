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

#define MQ7_input A5


#include <LCD5110_Basic.h>
#include <Wire.h>

float RS_gas = 0;
float ratio = 0;
float sensorValue = 0;
float sensor_volt = 0;
float R0 = 7200.0;
 
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
digitalWrite(PIN_BL, LOW);
/*
pinMode(MQ7_PLUS, OUTPUT);      // alimentation MQ7 
digitalWrite(MQ7_PLUS, HIGH);
*/

  myGLCD.InitLCD(); // initialisation LCD
  myGLCD.setContrast(60);
    Serial.begin(115200);
  Wire.begin();
}

void loop()
{

sensorValue = analogRead(MQ7_input);
   sensor_volt = sensorValue/1024*5.0;
   RS_gas = (5.0-sensor_volt)/sensor_volt;
   ratio = RS_gas/R0; //Replace R0 with the value found using the sketch above
   float x = 1538.46 * ratio;
   float ppm = pow(x,-1.709);
   Serial.print("PPM: ");
   Serial.println(ppm);
  
    myGLCD.clrScr();
  invertLCD++;

myGLCD.setFont(SmallFont);
      myGLCD.print("PPM",LEFT, 0);
   myGLCD.setFont(MediumNumbers);
    myGLCD.printNumF(ppm, 2, RIGHT, 0);

     myGLCD.setFont(SmallFont);
        myGLCD.print("MQ-7 CO et MH4",LEFT, 42);
   myGLCD.invert(invertLCD % 2);
 
  delay(500);
}
