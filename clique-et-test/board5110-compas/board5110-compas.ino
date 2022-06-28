// LCD5110_NumberFonts
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// This program is a demo of the included number-fonts,
// and how to use them.
/**********************************************************
   Boussole constituée d'un Arduino, d'un magnétomètre
   HMC5883 et d'un écran LCD Nokia.
   
   Plus d'infos:
   https://electroniqueamateur.blogspot.com/2019/06/fabrication-dune-boussole-avec-le.html
***********************************************************/

#include <Wire.h>  // i2c pour le capteur HMC5883
#define adresse 0x1E // Adresse i2c du capteur HMC5883

// bibliothèques pour l'écran LCD Nokia
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// valeurs requises pour calibrer nos mesures (spécifique à votre module):
const int xmin = -564;
const int xmax = -62;
const int ymin = -277;
const int ymax = 254;

// Pour pointer vers le nord géographique plutôt que vers
// le nord magnétique, on additionne la déclinaison, qui dépend
// de l'endroit où vous êtes:
// http://www.magnetic-declination.com/

float declinaison = 0.22;  // en radians

//
// It is assumed that the LCD module is connected to
// the following pins.
//      SCK  - Pin 8 >> 4
//      MOSI - Pin 9 >> 5
//      DC   - Pin 10 >> 6
//      CS   - Pin 12 >> 7
//      RST  - Pin 11 >> 8

// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)


//
#define PIN_PLUS 3
#define PIN_BL 2

//PCD8544(SCLK, DIN/MOSI, D/C, SCE/CS, RST); 
Adafruit_PCD8544 display = Adafruit_PCD8544(4,5,6,7,8);

// D/C broche 5,CSE ou CS broche 4, RST broche 3
// LCD5110 myGLCD(4,5,6,8,7);
const int rayon = 20; //rayon du cercle en pixels, sur l'afficheur LCD

void setup(void)
{



   Serial.begin(115200);
   Serial.println("HMC5883 Magnetometer Test"); 

     pinMode(PIN_PLUS, OUTPUT);      // sets the digital pin as output
pinMode(PIN_BL, OUTPUT); 
digitalWrite(PIN_PLUS, HIGH);
digitalWrite(PIN_BL, HIGH);

  // initialiation du capteur HMC5883
  Wire.begin();
  Wire.beginTransmission(adresse);
  Wire.write(0x02);
  Wire.write(0x00);
  Wire.endTransmission();

  // initialisation de l'afficheur
  display.begin();
  display.setContrast(60); // réglage du contraste (40-60) variable selon votre écran
  display.clearDisplay();
}

void loop(void)
{

  int xbrut, ybrut, zbrut, xcalibree, ycalibree;

  // On demande les informations au capteur HMC5883
  Wire.beginTransmission(adresse);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();

  //Lecture des composantes du champ magnétique
  Wire.requestFrom(adresse, 6);
  if (6 <= Wire.available()) {
    xbrut = Wire.read() << 8; //X msb
    xbrut |= Wire.read(); //X lsb
    zbrut = Wire.read() << 8; //Z msb
    zbrut |= Wire.read(); //Z lsb
    ybrut = Wire.read() << 8; //Y msb
    ybrut |= Wire.read(); //Y lsb
  }

  // valeurs calibrées:
  xcalibree = map(xbrut, xmin, xmax, -1000, 1000);
  ycalibree = map(ybrut, ymin, ymax, -1000, 1000);

  // Calcul de l'angle entre le nord magnétique et l'axe x du capteur
  float orientationNord = atan2(ycalibree, xcalibree);

  // Pour pointer vers le nord géographique plutôt que vers
  // le nord magnétique, on additionne la déclinaison

  orientationNord += declinaison;

  Serial.println(orientationNord); 

  // on dessine la flèche à l'angle approprié sur l'afficheur LCD
  display.clearDisplay();
  display.drawCircle(display.width() / 2, display.height() / 2, rayon, BLACK); // contour d'un cercle
  display.drawLine(display.width() / 2, display.height() / 2, display.width() / 2 + rayon * cos(orientationNord) , display.height() / 2 - rayon * sin(orientationNord), BLACK);
  display.display();

  delay(50);
}
