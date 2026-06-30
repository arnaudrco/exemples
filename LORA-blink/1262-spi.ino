/*
  Balise réalisée avec SX1262

  This example shows how to change all the properties of LoRa transmission.
  RadioLib currently supports the following settings:
  - pins (SPI slave select, DIO1, DIO2, BUSY pin)
  - carrier frequency
  - bandwidth
  - spreading factor
  - coding rate
  - sync word
  - output power during transmission
  - CRC
  - preamble length
  - TCXO voltage
  - DIO2 RF switch control

  Other modules from SX126x family can also be used.

  For default module settings, see the wiki page
  https://github.com/jgromes/RadioLib/wiki/Default-configuration#sx126x---lora-modem

  For full API reference, see the GitHub Pages
  https://jgromes.github.io/RadioLib/
*/

#include <Arduino.h>
#include <SPI.h>
#include <RadioLib.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

// Pins SX1262
#define NSS D0 // 15
#define DIO1 D1 // 5
#define RESET  D8 // 16
#define BUSY D2 // 4

// Pins OLED
#define OLED_SDA D4 // 2
#define OLED_SCL D3 // 14

// Création objets
SX1262 radio = new Module(NSS, DIO1, RESET, BUSY);
Adafruit_SSD1306 display(128, 64, &Wire, -1);

String freq = "868.0";
String lastMsg = "";
int rssi = 0;
bool txMode = false;

// Temps entre 2 requêtes (ms)
unsigned long lastQuery = 0;
const unsigned long queryInterval = 3000;
int comptage =0;

void setup() {
  setupPlus();
  Wire.begin(OLED_SDA, OLED_SCL);
     Serial.println(F("1262"));
  // Init OLED
     if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
 // if(!display.begin(SSD1306_I2C_ADDRESS, 0x3C)) {
    Serial.println(F("OLED KO"));
    for(;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  

  radio.setDio1Action(setFlag);
  radio.startReceive();
  
  display.println("LoRa 868MHz");
  display.display();
}

volatile bool flag = false;
ICACHE_RAM_ATTR void setFlag() { flag = true; }



void loop() {
  // RX
  if (flag) {
    flag = false;
    int state = radio.readData(lastMsg);
    if ( state == RADIOLIB_ERR_NONE ) {
      rssi = radio.getRSSI();
      txMode = false;
    }
  }
  
  // Affichage OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0,0);
  display.print("Freq: ");
  display.print(freq);
  display.print(" MHz");
  
  display.setCursor(0,20);
  display.print("RX: ");
  display.print(lastMsg);
  
  display.setCursor(0,40);
  display.print("RSSI: ");
  display.print(rssi);
  display.print(" dBm");
  
  display.setCursor(0,56);
  if (txMode) display.print("TX"); else display.print("RX");
  
  display.display();
  delay(100);

  // Test TX toutes les 3 secondes
   if (millis() - lastQuery > queryInterval) {
        if( comptage > 99 ) comptage = 0;// comptage 99 max  
        String message = String(comptage++);

   Serial.println(comptage);  
    radio.transmit(message);
    txMode = true;
    delay(100);
    
    lastQuery = millis();
  }
  
}

void setupPlus() {
  Serial.begin(9600);

  Serial.print(F("[SX1262] Initializing ... "));

    // Init SX1262

  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) { delay(10); }
  }

  // initialize the second LoRa instance with
  // non-default settings
  // this LoRa link will have high data rate,
  // but lower range
 

  // you can also change the settings at runtime
  // and check if the configuration was changed successfully

  // set carrier frequency to 868 MHz
  if (radio.setFrequency(868) == RADIOLIB_ERR_INVALID_FREQUENCY) {
    Serial.println(F("Selected frequency is invalid for this module!"));
    while (true) { delay(10); }
  }

  // set bandwidth to 250 kHz
  if (radio.setBandwidth(250.0) == RADIOLIB_ERR_INVALID_BANDWIDTH) {
    Serial.println(F("Selected bandwidth is invalid for this module!"));
  //  while (true) { delay(10); }
  }

  // set spreading factor to 10
  if (radio.setSpreadingFactor(12) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
    Serial.println(F("Selected spreading factor is invalid for this module!"));
 //   while (true) { delay(10); }
  }

  // set coding rate to 2
  if (radio.setCodingRate(5) == RADIOLIB_ERR_INVALID_CODING_RATE) {
    Serial.println(F("Selected coding rate is invalid for this module!"));
 //   while (true) { delay(10); }
  }

  // set LoRa sync word to 0xAB
  if (radio.setSyncWord(0xAB) != RADIOLIB_ERR_NONE) {
    Serial.println(F("Unable to set sync word!"));
    while (true) { delay(10); }
  }

  // set output power to 10 dBm (accepted range is -17 - 22 dBm)
  if (radio.setOutputPower(22) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
    Serial.println(F("Selected output power is invalid for this module!"));
    while (true) { delay(10); }
  }

  // set over current protection limit to 80 mA (accepted range is 45 - 240 mA)
  // NOTE: set value to 0 to disable overcurrent protection
  if (radio.setCurrentLimit(80) == RADIOLIB_ERR_INVALID_CURRENT_LIMIT) {
    Serial.println(F("Selected current limit is invalid for this module!"));
    while (true) { delay(10); }
  }

  // set LoRa preamble length to 15 symbols (accepted range is 0 - 65535)
  if (radio.setPreambleLength(15) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH) {
    Serial.println(F("Selected preamble length is invalid for this module!"));
    while (true) { delay(10); }
  }

  // disable CRC
  if (radio.setCRC(false) == RADIOLIB_ERR_INVALID_CRC_CONFIGURATION) {
    Serial.println(F("Selected CRC is invalid for this module!"));
    while (true) { delay(10); }
  }

  // Some SX126x modules have TCXO (temperature compensated crystal
  // oscillator). To configure TCXO reference voltage,
  // the following method can be used.
  if (radio.setTCXO(2.4) == RADIOLIB_ERR_INVALID_TCXO_VOLTAGE) {
    Serial.println(F("Selected TCXO voltage is invalid for this module!"));
    while (true) { delay(10); }
  }

  // Some SX126x modules use DIO2 as RF switch. To enable
  // this feature, the following method can be used.
  // NOTE: As long as DIO2 is configured to control RF switch,
  //       it can't be used as interrupt pin!
  if (radio.setDio2AsRfSwitch() != RADIOLIB_ERR_NONE) {
    Serial.println(F("Failed to set DIO2 as RF switch!"));
    // while (true) { delay(10); }
  }

  Serial.println(F("All settings succesfully changed!"));
}// Balise réalisée avec SX1262

#include <Arduino.h>
#include <SPI.h>
#include <RadioLib.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

// Pins SX1262
#define NSS D0 // 15
#define DIO1 D1 // 5
#define RESET  D8 // 16
#define BUSY D2 // 4

// Pins OLED
#define OLED_SDA D4 // 2
#define OLED_SCL D3 // 14

// Création objets
SX1262 radio = new Module(NSS, DIO1, RESET, BUSY);
Adafruit_SSD1306 display(128, 64, &Wire, -1);

String freq = "868.0";
String lastMsg = "";
int rssi = 0;
bool txMode = false;

// Temps entre 2 requêtes (ms)
unsigned long lastQuery = 0;
const unsigned long queryInterval = 3000;
int comptage =0;

void setup() {
  Serial.begin(115200);
  Wire.begin(OLED_SDA, OLED_SCL);
     Serial.println(F("1262"));
  // Init OLED
     if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
 // if(!display.begin(SSD1306_I2C_ADDRESS, 0x3C)) {
    Serial.println(F("OLED KO"));
    for(;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  
  // Init SX1262
  SPI.begin();
  int state = radio.begin(868.0, 125.0, 9, 5, 0x12, 10, 8, 1.6);
  Serial.println(state);
  while (radio.begin(868.0, 125.0, 9, 5, 0x12, 10, 8, 1.6) != RADIOLIB_ERR_NONE) {
    Serial.println(F("LoRa KO"));
  delay(1000);
  }
  radio.setDio1Action(setFlag);
  radio.startReceive();
  
  display.println("LoRa 868MHz");
  display.display();
}

volatile bool flag = false;
ICACHE_RAM_ATTR void setFlag() { flag = true; }



void loop() {
  // RX
  if (flag) {
    flag = false;
    int state = radio.readData(lastMsg);
    if ( state == RADIOLIB_ERR_NONE ) {
      rssi = radio.getRSSI();
      txMode = false;
    }
  }
  
  // Affichage OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0,0);
  display.print("Freq: ");
  display.print(freq);
  display.print(" MHz");
  
  display.setCursor(0,20);
  display.print("RX: ");
  display.print(lastMsg);
  
  display.setCursor(0,40);
  display.print("RSSI: ");
  display.print(rssi);
  display.print(" dBm");
  
  display.setCursor(0,56);
  if (txMode) display.print("TX"); else display.print("RX");
  
  display.display();
  delay(100);

  // Test TX toutes les 3 secondes
   if (millis() - lastQuery > queryInterval) {
        if( comptage > 99 ) comptage = 0;// comptage 99 max  
        String message = String(comptage++);

   Serial.println(comptage);  
    radio.transmit(message);
    txMode = true;
    delay(100);
    
    lastQuery = millis();
  }
  
}
