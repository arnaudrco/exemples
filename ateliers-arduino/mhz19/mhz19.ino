#include <Arduino.h>
#include "MHZ19.h"                                        
#include <SoftwareSerial.h>                                // Remove if using HardwareSerial


#include <TM1637Display.h>

// Module connection pins (Digital Pins)
#define CLK 2
#define DIO 3
#define PIN_PLUS 4
#define GND 5


TM1637Display display(CLK, DIO);

#define RX_PIN A5                                          // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN A4  
//#define RX_PIN 10                                          // Rx pin which the MHZ19 Tx pin is attached to
//#define TX_PIN 11  // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)

MHZ19 myMHZ19;                                             // Constructor for library
SoftwareSerial mySerial(RX_PIN, TX_PIN);                   // (Uno example) create device to MH-Z19 serial

unsigned long getDataTimer = 0;

void setup()
{
  
    pinMode(PIN_PLUS, OUTPUT);      // sets the digital pin as output
pinMode(GND, OUTPUT); 
digitalWrite(PIN_PLUS, HIGH);
digitalWrite(GND, LOW);
    Serial.begin(9600);                                     // Device to serial monitor feedback
  display.setBrightness(0x0f);
  display.showNumberDec(0, true); // Expect: 0301
  
    mySerial.begin(BAUDRATE);                               // (Uno example) device to MH-Z19 serial start   
    myMHZ19.begin(mySerial);                                // *Serial(Stream) refence must be passed to library begin(). 

    myMHZ19.autoCalibration();   // Turn auto calibration ON (OFF autoCalibration(false))



}

void loop()
{
    if (millis() - getDataTimer >= 2000)
    {
        int CO2; 

        /* note: getCO2() default is command "CO2 Unlimited". This returns the correct CO2 reading even 
        if below background CO2 levels or above range (useful to validate sensor). You can use the 
        usual documented command with getCO2(false) */

        CO2 = myMHZ19.getCO2();                             // Request CO2 (as ppm)
        
        Serial.print("CO2 (ppm): ");                      
        Serial.println(CO2);  
                                      
  display.clear();
  display.showNumberDec(CO2, false); // Expect: 0301




        int8_t Temp;
        Temp = myMHZ19.getTemperature();                     // Request Temperature (as Celsius)
        Serial.print("Temperature (C): ");                  
        Serial.println(Temp);                               

        getDataTimer = millis();
    }
}
