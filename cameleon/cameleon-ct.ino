#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Adafruit_NeoPixel.h>
// Arnaud REICHART temperature de couleur
// An experimental wrapper class that implements the improved lux and color temperature from
// TAOS and a basic autorange mechanism.
//
// Written by ductsoup, public domain
//

// RGB Color Sensor with IR filter and White LED - TCS34725
// I2C 7-bit address 0x29, 8-bit address 0x52
// connect LED to digital 4 or GROUND for ambient light sensing
// connect SCL to analog 5
// connect SDA to analog 4
// connect Vin to 3.3-5V DC
// connect GROUND to common ground

// #define PIN            2 
#define NUMPIXELS      16
#define MMM      7 // nb de couleurs apprises

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, 7, NEO_GRB + NEO_KHZ800);
// int delayval = 500; // delay for half a second

  int ColorArray[3]; // algorithme plus proche voisin
  
// some magic numbers for this device from the DN40 application note
#define TCS34725_R_Coef 0.136
#define TCS34725_G_Coef 1.000
#define TCS34725_B_Coef -0.444
#define TCS34725_GA 1.0
#define TCS34725_DF 310.0
#define TCS34725_CT_Coef 3810.0
#define TCS34725_CT_Offset 1391.0

// Autorange class for TCS34725
class tcs34725 {
private:
  struct tcs_agc {
    tcs34725Gain_t ag;
    uint8_t at;
    uint16_t mincnt;
    uint16_t maxcnt;
  };
  static const tcs_agc agc_lst[];
  uint16_t agc_cur;

  void setGainTime(void);
  Adafruit_TCS34725 tcs;

public:
  tcs34725(void);

  boolean begin(void);
  void getData(void);

  boolean isAvailable, isSaturated;
  uint16_t againx, atime, atime_ms;
  uint16_t r, g, b, c;
  uint16_t ir;
  uint16_t r_comp, g_comp, b_comp, c_comp;
  uint16_t saturation, saturation75;
  float cratio, cpl, ct, lux, maxlux;
};
//
// Gain/time combinations to use and the min/max limits for hysteresis
// that avoid saturation. They should be in order from dim to bright.
//
// Also set the first min count and the last max count to 0 to indicate
// the start and end of the list.
//
const tcs34725::tcs_agc tcs34725::agc_lst[] = {
  { TCS34725_GAIN_60X, TCS34725_INTEGRATIONTIME_614MS,     0, 20000 },
  { TCS34725_GAIN_60X, TCS34725_INTEGRATIONTIME_154MS,  4990, 63000 },
  { TCS34725_GAIN_16X, TCS34725_INTEGRATIONTIME_154MS, 16790, 63000 },
  { TCS34725_GAIN_4X,  TCS34725_INTEGRATIONTIME_154MS, 15740, 63000 },
  { TCS34725_GAIN_1X,  TCS34725_INTEGRATIONTIME_154MS, 15740, 0 }
};
tcs34725::tcs34725() : agc_cur(0), isAvailable(0), isSaturated(0) {
}

// initialize the sensor
boolean tcs34725::begin(void) {
  tcs = Adafruit_TCS34725(agc_lst[agc_cur].at, agc_lst[agc_cur].ag);
  if ((isAvailable = tcs.begin()))
    setGainTime();
  return(isAvailable);
}

// Set the gain and integration time
void tcs34725::setGainTime(void) {
  tcs.setGain(agc_lst[agc_cur].ag);
  tcs.setIntegrationTime(agc_lst[agc_cur].at);
  atime = int(agc_lst[agc_cur].at);
  atime_ms = ((256 - atime) * 2.4);
  switch(agc_lst[agc_cur].ag) {
  case TCS34725_GAIN_1X:
    againx = 1;
    break;
  case TCS34725_GAIN_4X:
    againx = 4;
    break;
  case TCS34725_GAIN_16X:
    againx = 16;
    break;
  case TCS34725_GAIN_60X:
    againx = 60;
    break;
  }
}

// Retrieve data from the sensor and do the calculations
void tcs34725::getData(void) {
  // read the sensor and autorange if necessary
  tcs.getRawData(&r, &g, &b, &c);
  while(1) {
    if (agc_lst[agc_cur].maxcnt && c > agc_lst[agc_cur].maxcnt)
      agc_cur++;
    else if (agc_lst[agc_cur].mincnt && c < agc_lst[agc_cur].mincnt)
      agc_cur--;
    else break;

    setGainTime();
    delay((256 - atime) * 2.4 * 2); // shock absorber
    tcs.getRawData(&r, &g, &b, &c);
    break;
  }

  // DN40 calculations
  ir = (r + g + b > c) ? (r + g + b - c) / 2 : 0;
  r_comp = r - ir;
  g_comp = g - ir;
  b_comp = b - ir;
  c_comp = c - ir;
  cratio = float(ir) / float(c);

  saturation = ((256 - atime) > 63) ? 65535 : 1024 * (256 - atime);
  saturation75 = (atime_ms < 150) ? (saturation - saturation / 4) : saturation;
  isSaturated = (atime_ms < 150 && c > saturation75) ? 1 : 0;
  cpl = (atime_ms * againx) / (TCS34725_GA * TCS34725_DF);
  maxlux = 65535 / (cpl * 3);

  lux = (TCS34725_R_Coef * float(r_comp) + TCS34725_G_Coef * float(g_comp) + TCS34725_B_Coef * float(b_comp)) / cpl;
  ct = TCS34725_CT_Coef * float(b_comp) / float(r_comp) + TCS34725_CT_Offset;
}

tcs34725 rgb_sensor;
// byte gammatable[256];

void setup(void) {
  Serial.begin(115200);
    pixels.begin(); // This initializes the NeoPixel library.
  delay(500); setColor(255, 255, 255); // white 
  delay(500); setColor(255, 110, 0); // orange
  delay(500); setColor(255, 255, 0); // yellow
  delay(500); setColor(255, 0, 0); // red
  delay(500); setColor(0, 255, 0); // green
  delay(500); setColor(0, 0, 255); // blue
  
 delay(500); setColor(0, 255, 255); // indigo
 delay(500); setColor(255, 0, 255); // violet
    
  rgb_sensor.begin();
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW); // @gremlins Bright light, bright light!
}
void setColor(int red, int green, int blue) { // WS2811
   pixels.setPixelColor (0,green,red,blue);
    pixels.show(); // This sends the updated pixel color to the hardware.
}
void GetColor(float red, float green, float blue, float white) { //0=white, 1=orange, 2=yellow, 3=red, 4=green, 5=blue, 6=object out of range
long PercentageRed,PercentageGreen,PercentageBlue;
 
  PercentageRed= (red * 100)/white;
  PercentageGreen=(green * 100)/white; 
  PercentageBlue=(blue * 100)/white;

             Serial.print("RGB ");Serial.print(PercentageRed);  
           Serial.print(" "); Serial.print(PercentageGreen);  
           Serial.print(" "); Serial.println(PercentageBlue);
 
  //Learned blue, green, red percentage values of different colors
//  int SavedColorRed[] = {28,55,42,50,19,13}; 
//  int SavedColorGreen[] = {30,25,36,22,45,26};
//  int SavedColorBlue[] = {45,20,20,30,36,58};

    int SavedColorRed[] = {28,55,42,50,19,13,24}; 
  int SavedColorGreen[] = {30,25,36,22,45,26,30};
  int SavedColorBlue[] = {45,20,20,30,36,58,44};

  int i_color; 
  int ClosestColor;
  int MaxDiff;
  int MinDiff=300;

    for (i_color=0; i_color<MMM; i_color++) { //Find closest color
      ColorArray[0]=abs(SavedColorRed[i_color]-PercentageRed);
      ColorArray[1]=abs(SavedColorGreen[i_color]-PercentageGreen);
      ColorArray[2]=abs(SavedColorBlue[i_color]-PercentageBlue);
           Serial.print(" ");Serial.print(ColorArray[0]);  
           Serial.print(" "); Serial.print(ColorArray[1]);  
           Serial.print(" "); Serial.println(ColorArray[2]);
      MaxDiff=Maximum();
      if (MaxDiff<MinDiff) {
        MinDiff=MaxDiff;
        ClosestColor=i_color;
       }
   } 
     Serial.print(" Couleur : ");  
  Serial.println(ClosestColor);
   if(ClosestColor==0) setColor(0, 0, 255); // blue
     if(ClosestColor==1) setColor(255, 110, 0); // orange
     if(ClosestColor==2) setColor(255, 255, 0); // yellow
     if(ClosestColor==3) setColor(255, 0, 0); // red
     if(ClosestColor==4) setColor(0, 255, 0); // green
     if(ClosestColor==5) setColor(0, 0, 255); // blue
     if(ClosestColor==6) setColor(255, 0, 255); // violet
} 
int Temperature(float t){
   int ct = t ;
      Serial.print(" Couleur : ");  
  Serial.println(ct);
   if(ct > 12000) setColor(0, 0, 255); // blue
   else if (ct > 9000) setColor(0, 255, 0); // green
   else if (ct > 6000) setColor(255, 0, 0); // yellow
   else if (ct > 4000) setColor(255, 255, 0); // orange
   else if (ct > 3500) setColor(255, 0, 0); // red
   else setColor(0, 0, 0); // noir
}
int Maximum(){
  int rrr;
  rrr=max( ColorArray[0],ColorArray[1]);
  return(max(rrr, ColorArray[2]));
  }
void loop(void) {
  rgb_sensor.getData();
  Serial.print(F("Gain:"));
  Serial.print(rgb_sensor.againx);
  Serial.print(F("x "));
  Serial.print(F("Time:"));
  Serial.print(rgb_sensor.atime_ms);
  Serial.print(F("ms (0x"));
  Serial.print(rgb_sensor.atime, HEX);
  Serial.println(F(")"));

  Serial.print(F("Raw R:"));
  Serial.print(rgb_sensor.r);
  Serial.print(F(" G:"));
  Serial.print(rgb_sensor.g);
  Serial.print(F(" B:"));
  Serial.print(rgb_sensor.b);
  Serial.print(F(" C:"));
  Serial.println(rgb_sensor.c);

  Serial.print(F("IR:"));
  Serial.print(rgb_sensor.ir);
  Serial.print(F(" CRATIO:"));
  Serial.print(rgb_sensor.cratio);
  Serial.print(F(" Sat:"));
  Serial.print(rgb_sensor.saturation);
  Serial.print(F(" Sat75:"));
  Serial.print(rgb_sensor.saturation75);
  Serial.print(F(" "));
  Serial.println(rgb_sensor.isSaturated ? "*SATURATED*" : "");

  Serial.print(F("CPL:"));
  Serial.print(rgb_sensor.cpl);
  Serial.print(F(" Max lux:"));
  Serial.println(rgb_sensor.maxlux);

  Serial.print(F("Compensated R:"));
  Serial.print(rgb_sensor.r_comp);
  Serial.print(F(" G:"));
  Serial.print(rgb_sensor.g_comp);
  Serial.print(F(" B:"));
  Serial.print(rgb_sensor.b_comp);
  Serial.print(F(" C:"));
  Serial.println(rgb_sensor.c_comp);

  Serial.print(F("Lux:"));
  Serial.print(rgb_sensor.lux);
  Serial.print(F(" CT:"));
  Serial.print(rgb_sensor.ct);
  Serial.println(F("K"));
  Serial.println("");
// GetColor( rgb_sensor.r_comp,  rgb_sensor.g_comp,  rgb_sensor.b_comp,  rgb_sensor.c_comp);
   Temperature(rgb_sensor.ct);
   delay(500);
  }
