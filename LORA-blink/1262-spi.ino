// Balise réalisée avec SX1262

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
