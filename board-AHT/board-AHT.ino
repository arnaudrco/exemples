// affichage sur un écran NOKIA de la pression
// relier A4 et A7, et GND de ATH20 sur GND arduino

#include "Nokia_5110.h"
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include <AHT20.h>

Adafruit_BMP280 bmp;
AHT20 aht20;

#define CLK 4
#define DIN 5
#define DC 6
#define CE 7
#define RST 8

#define PIN_PLUS 3
#define PIN_BL 2
#define GND A6

Nokia_5110 lcd = Nokia_5110(RST, CE, DC, DIN, CLK);

void setup() {
  Serial.begin(115200);
  pinMode(PIN_PLUS, OUTPUT); digitalWrite(PIN_PLUS, HIGH); // programmation pour éviter un fil vers la borne 5V
  pinMode(PIN_BL, OUTPUT);digitalWrite(PIN_BL, HIGH);
  pinMode(GND, OUTPUT); digitalWrite(GND, LOW);
  

  Wire.begin();
  if (aht20.begin() == false)
  {
    Serial.println("AHT20 not detected. Please check wiring. Freezing.");
  }


    /**
     * Note: if instead of text being shown on the display, all the segments are on, you may need to decrease contrast value.
     */
    lcd.setContrast(60); // 60 is the default value set by the driver
    
    lcd.print("Please Wait ...");
    delay(100);
    lcd.clear();
    lcd.setCursor(0, 5);
    lcd.println("1 2 3 ...");

     if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
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
  float temperature = aht20.getTemperature();
  float humidity = aht20.getHumidity();
  
    Serial.print("T: ");
  Serial.println(temperature, 2);
  Serial.print("H: ");
  Serial.print(humidity, 2);
  Serial.println("% RH");

 //   Serial.printf("Pressure: %.02f hPa\n", bmp.readPressure() );
     lcd.clear();
 lcd.println("ATH20");
    lcd.print("T: ");
  lcd.println(temperature, 2);
  lcd.print("H: ");
  lcd.print(humidity, 2);
  lcd.println("% RH");
  
    lcd.print("Pression");
    lcd.println(":");

    lcd.setCursor(0, 5);
    lcd.println(bmp.readPressure());
        delay(1000);

  }
