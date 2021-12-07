#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Adafruit_NeoPixel.h>
//
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

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, 7, NEO_GRB + NEO_KHZ800);
// int delayval = 500; // delay for half a second

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
byte gammatable[256];

void setup(void) {
  Serial.begin(115200);
    pixels.begin(); // This initializes the NeoPixel library.
  rgb_sensor.begin();
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW); // @gremlins Bright light, bright light!

    for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;
      
    gammatable[i] = x;      
    Serial.println(gammatable[i]);
  }
    for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    //pixels.setPixelColor(i, pixels.Color(100,150,0)); // Moderately bright green color.
//  pixels.setPixelColor (i, (gammatable[(int)gg]),( gammatable[(int)rr]),(gammatable[(int)bb]));
 //   pixels.setPixelColor (i, (gammatable[(int)rr]),( gammatable[(int)gg]),(gammatable[(int)bb]));
   pixels.setPixelColor (i,0,128,0);// WS2811
    pixels.show(); // This sends the updated pixel color to the hardware.

  Serial.println();

//  delay(100);
  }
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

  // ----------------- NEOPIXEL-------------
     uint32_t rr, gg, bb, mm;
  rr = rgb_sensor.r_comp;
  gg = rgb_sensor.g_comp ;
  bb = rgb_sensor.b_comp ;
 //     Serial.println(rgb_sensor.c_comp);
    uint32_t mmm = rgb_sensor.r_comp ;
   mmm = max( mmm, gg);
  mmm = max( mmm, bb); 
  Serial.print(F(" Max:"));
  Serial.println(mmm);

  rr = (rr * 255 ) / mmm;
  gg = (gg * 255 )/ mmm;
  bb = (bb * 255 )/ mmm;

  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.

  for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    //pixels.setPixelColor(i, pixels.Color(100,150,0)); // Moderately bright green color.
//  pixels.setPixelColor (i, (gammatable[(int)gg]),( gammatable[(int)rr]),(gammatable[(int)bb]));
    pixels.setPixelColor (i, (gammatable[(int)rr]),( gammatable[(int)gg]),(gammatable[(int)bb]));
//   pixels.setPixelColor (i,gg,rr,bb);// WS2811
    pixels.show(); // This sends the updated pixel color to the hardware.

  Serial.println();

//  delay(100);
  }
}
