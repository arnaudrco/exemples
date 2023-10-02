/*********************************************************************
This is an example sketch for our Monochrome Nokia 5110 LCD Displays

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/338

These displays use SPI to communicate, 4 or 5 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#define PIN_PLUS 3 // alimentation Nokia
#define PIN_BL 2 // fond ecran blanc
// pin 7 - Serial clock out (SCLK)>> 4
// pin 6 - Serial data out (DIN) >>  5
// pin 5 - Data/Command select (D/C) >> 6
// pin 4 - LCD chip select (CS)>> 7
// pin 3 - LCD reset (RST)>> 8

//      SCK  - Pin 8 >> 4
//      MOSI - Pin 9 >> 5
//      DC   - Pin 10 >> 6
//      RST  - Pin 11 >> 8
//      CS   - Pin 12 >> 7

// Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

Adafruit_PCD8544 display = Adafruit_PCD8544(4,5,6,7,8);


// chargeur de pile alcaline
// VERSION EVB GREEN-LOGIC puce lgt8f328p ATTENTION numérisation sur 2048 
// charge 100 mA 
// RCO mise à jour Arnaud 2023


#define  TIME_READ  10000
#define  TIME_LOAD  800
// #define  CMD_LOAD   PB1
#define  CMD_LOAD   11
//#define  CMD_DRAIN  PB2
#define  CMD_DRAIN  12


#define  LOAD_ON      digitalWrite(CMD_LOAD,  LOW); 
#define  LOAD_OFF     digitalWrite(CMD_LOAD,  HIGH); 
#define  DRAIN_ON     digitalWrite(CMD_DRAIN, HIGH); 
#define  DRAIN_OFF    digitalWrite(CMD_DRAIN, LOW); 
#define  PRT          Serial.print
#define  PRTL         Serial.println

#define  REF_EXT  3.3 // référence voltage donnée par arduino
#define  EVB  5.0 / 2048
#define  MAX_VAL  1.750  // correspond à 700 pour ValAdc
#define  MAX_ADC  720
#define  RESISTANCE 24   /// resistance de drain = 24 ohm 
//#define  INC_CHARGE  0.027778   // équivalent à 100 mA durant 1 seconde
#define  INC_CHARGE  0.555       // équivalent à 100 mA durant 20 seconde
#define  NBPASSMAX   10


// #define   USE_TELEPLOT
#define   USE_CONSOLE
#undef   USE_TRACEUR
#undef   USE_LOGVIEW

#define  REF_1V       6
#define  REF_256V     4
#define  REF_VCC      0
#define INC_TEMPS    5

//#include "Common.h"
// #include <SoftwareSerial.h>

// PINS    NANO 

//  External Ref ==> 3.3 V


// SoftwareSerial TinySerial(RX, TX);  // ( 0 , 3 )
char  Buf[24];
float Charge_Totale = 0.0;
int   NbPass    = 0;
int   ValAdc    = 0;    // tension pile en charge à  100 mA
float PileValue   = 0.0;  // tension pile en charge 100mA
int   Val_C0    = 0;    // tension pile à vide
int   Val_C1    = 0;    // tension pile en débit de 50 mA
float deltaV    = 0.0;
unsigned long time  = 0;
unsigned long Temps = 0;
unsigned long Tpx   = 0;


//************************************************************************
void setup() {
    Serial.begin(115200);

  pinMode(CMD_DRAIN,OUTPUT);
  pinMode(CMD_LOAD,OUTPUT);
  LOAD_OFF  // load coupée
  DRAIN_OFF // drain coupé

 // analogReference(EXTERNAL);  // utilise la tension du TL431 sur la pin 5 ===> 2.493 V 
  Charge_Totale = 0.0;
  Temps         = 0;
  NbPass        = 0;
  
    PRTL("Arnaud ");

        lcd_setup();
    test();

}
///********************************************************/
void loop() {
// on met en marche la charge de 100mA

  ValAdc = CourantPulse(32);
  #ifdef USE_CONSOLE
  PRT(" - "); PRT(ValAdc);

    display.setTextSize(2);
  display.setTextColor(BLACK);
  display.setCursor(30,30);
 display.println(ValAdc);
  display.display();
  
  #endif  
  if (NbPass > NBPASSMAX)  // 10 passages
  {
//    PileValue = ValAdc * (REF_EXT/1024.0);
    PileValue = ValAdc * EVB;
    LOAD_OFF 
    Val_C0 = analogRead(A2);  // Vpile à vide
    DRAIN_ON
    delay(5);   // delai stabilisation de Vpile
    Val_C1 = analogRead(A2);  // pile en charge
    DRAIN_OFF
    deltaV = (Val_C0 - Val_C1) * 1.0;
    deltaV = (deltaV / Val_C1) * RESISTANCE;   // calcul de la résistance interne ==> on reprend deltaV
    Temps  += 20; // temps écoulé depuis le départ

    LOAD_ON
    delay(1120);                //delai en + en charge pour ajuster à 20 secondes / tour
    LOAD_OFF  
    Charge_Totale = Charge_Totale + INC_CHARGE;  // charge == 20 secondes avec 0.1A

    NbPass = 0;

 display.clearDisplay();

  // text display tests
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
    display.print(">Vp:"); display.println(PileValue);
    display.print(">Ri:"); display.println(deltaV);
    display.print(">Qc:"); display.println(Charge_Totale);
  display.display();

    
    
#ifdef USE_CONSOLE
    PRTL(" ");
    PRT("Pile = ");       PRT(PileValue);
    PRT("\tR_int = ");    PRT(deltaV);PRT(" Ohm");
    PRT("\tQt = ");       PRT((int)Charge_Totale);PRT(" mAh");
    PRT("\tTemps = ");    CvTemp(Temps);PRTL(Buf);



#endif

#ifdef USE_TRACEUR
    PRT(PileValue); 
    PRT(" , "); 
    PRTL(deltaV);
#endif

#ifdef USE_TELEPLOT
    PRT(">Vp:"); PRTL(PileValue);
    PRT(">Ri:"); PRTL(deltaV);
    PRT(">Qc:"); PRTL(Charge_Totale);
#endif

#ifdef USE_LOGVIEW
    PRT("$1;1;; ");
    PRT(PileValue);PRT(";");
    PRT(deltaV);PRT(";");
    PRT((int)Charge_Totale);PRT(";");
    PRTL("0");
#endif
    
    if (ValAdc > MAX_ADC){   // MAX_ADC = 700  
      while (1){
        LOAD_OFF
        DRAIN_OFF
        delay(1000);
//        PileValue = analogRead(A2) * (REF_EXT/1024.0);
        PileValue = analogRead(A2) * EVB;
        PRT("Charge terminee Tension Pile = ");
        PRTL(PileValue);
        
        for (int x=60; x>1; x--)  // 2 mn entre chaque print 
        { 


          display.clearDisplay();

  // text display tests
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.println("Fin Charge");
    display.println(PileValue);
  display.display();

  
          delay(2000);
          PRT(".");
        }
        PRTL(".");
      }
    }
  }
  NbPass++;               // compteur de passage

}

///********************************************************/
///********************************************************/
int CourantPulse(int count)
{
    int adc ;
    int x;  
    for (adc=0, x=0; x<count; x++)
    {
      LOAD_ON
      delay(40);                // charge = 50ms
      adc += analogRead(A2);    // Vpile en charge
      delay(10);               
      LOAD_OFF
      DRAIN_ON
      delay(2);               // décharge = 5ms
      DRAIN_OFF
    }
  return (adc/count);   // moyenne sur count mesures
}

///********************************************************/
void CvTemp(unsigned long tx)
{
  int h,m,s = 0;
  unsigned long ti = tx;
  
  h = ti/3600;
  ti = ti - (h*3600);
  m = ti/60;
  s = ti - (m*60);
  sprintf(Buf,"%2d:%02d:%02d",h,m,s);
}
///********************************************************/


void lcd_setup()
{
 
//#define PIN_PLUS 3 // alimentation Nokia
//#define PIN_BL 2 // fond ecran blanc
pinMode(PIN_PLUS, OUTPUT);      
pinMode(PIN_BL, OUTPUT); 
digitalWrite(PIN_PLUS, HIGH);
digitalWrite(PIN_BL, HIGH);


  display.begin();
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(35); // ajuster suivant écran autour de 55 
  display.clearDisplay();   // clears the screen and buffer



}

void test()
{
  

 display.clearDisplay();

  // text display tests
  display.setTextSize(2);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.println("Arnaud");
  display.println("CHARGE");
  display.setTextSize(2);
  display.setTextColor(BLACK);
//  display.print("0x"); display.println(0xDEADBEEF, HEX);
  display.display();



}
