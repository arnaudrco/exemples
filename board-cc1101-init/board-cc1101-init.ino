/**************************************************************************
 CC1101

 **************************************************************************/

#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <cc1101.h>

using namespace CC1101;
Radio radio(/* cs pin */ 10);

#define PIN_PLUS 2
#define TFT_SCLK   3
#define TFT_MOSI   4
#define TFT_RST    5  // you can also connect this to the Arduino reset
#define TFT_DC   6
#define TFT_CS 7
#define PIN_BL 8
#define PIN_GPS A7

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup(void) {
  Serial.begin(115200);

  pinMode(PIN_PLUS, OUTPUT);      // sets the digital pin as output
  pinMode(PIN_BL, OUTPUT); 
  digitalWrite(PIN_PLUS, HIGH);
  digitalWrite(PIN_BL, HIGH);

  // OR use this initializer (uncomment) if using a 1.3" or 1.54" 240x240 TFT:
  //tft.init(240, 240);           // Init ST7789 240x240
  // OR use this initializer (uncomment) if using a 2.0" 320x240 TFT:
  tft.init(240, 320);           // Init ST7789 320x240
  // OR use this initializer (uncomment) if using a 1.14" 240x135 TFT:
  //tft.init(135, 240);           // Init ST7789 240x135
  
  Serial.println(F("Initialized"));

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
 tft.fillScreen(ST77XX_BLACK);

     tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.println("GC1101");
  delay(1000);

  while (radio.begin() == STATUS_CHIP_NOT_FOUND) {
    Serial.println(F("Chip not found!"));
     tft.setTextSize(1);
  tft.println("Chip");
    delay(1000); 
        time = millis() - time; Serial.println(time, DEC);
  }
       tft.setTextSize(3);
  tft.println("Chip OK");
  Serial.println(time, DEC);
    delay(1000);
    
    radio.setModulation(MOD_ASK_OOK);
  radio.setFrequency(433.8);
  radio.setDataRate(10);
  radio.setOutputPower(10);

  radio.setPacketLengthMode(PKT_LEN_MODE_VARIABLE);
  radio.setAddressFilteringMode(ADDR_FILTER_MODE_NONE);
  radio.setPreambleLength(64);
  radio.setSyncWord(0x1234);
  radio.setSyncMode(SYNC_MODE_16_16);
  radio.setCrc(true);
  radio.setDataWhitening(true);
  radio.setManchester(false);
  radio.setFEC(false);

}

void loop() {
   int y=30;// décalage titre
  // tft.fillRect(0, y , tft.width(), tft.height()/2 + y, ST77XX_MAGENTA);
 
}
