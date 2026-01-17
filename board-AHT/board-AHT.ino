#include "Nokia_5110.h"
#include <Adafruit_BMP280.h>
#include <Wire.h>

Adafruit_BMP280 bmp;

/*
#define RST 2
#define CE 3
#define DC 4
#define DIN 5
#define CLK 6 */







#define CLK 4
#define DIN 5
#define DC 6
#define CE 7
#define RST 8

#define PIN_PLUS 3
#define PIN_BL 2
#define GND A6

Nokia_5110 lcd = Nokia_5110(RST, CE, DC, DIN, CLK);

//      SCK  - Pin 8 >> 4
//      MOSI - Pin 9 >> 5
//      DC   - Pin 10 >> 6
//      RST  - Pin 11 >> 8
//      CS   - Pin 12 >> 7

void setup() {
  Serial.begin(115200);
  pinMode(PIN_PLUS, OUTPUT); digitalWrite(PIN_PLUS, HIGH);
  pinMode(PIN_BL, OUTPUT);digitalWrite(PIN_BL, HIGH);
  pinMode(GND, OUTPUT); digitalWrite(GND, LOW);
  

 


    /**
     * Note: if instead of text being shown on the display, all the segments are on, you may need to decrease contrast value.
     */
    lcd.setContrast(55); // 60 is the default value set by the driver
    
    lcd.print("Please Wait ...");
    delay(100);
    lcd.clear();

    lcd.print("Hi there");
    lcd.println(":D");

    lcd.setCursor(0, 5);
    lcd.println("1 2 3 ...");

     if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
  Serial.println(F("Valid BMP280 sensor"));
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

}

void loop() {

 //   Serial.printf("Pressure: %.02f hPa\n", bmp.readPressure() );
     lcd.clear();

    lcd.print("Pression");
    lcd.println(":");

    lcd.setCursor(0, 5);
    lcd.println(bmp.readPressure());
        delay(1000);

  }
