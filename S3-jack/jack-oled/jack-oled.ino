
// version avec écran OLED 4 boutons

#include "Arduino.h"
#include "WiFi.h"
#include "Audio.h"
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <Adafruit_NeoPixel.h>

#define PIN        21 // Which pin on the Arduino is connected to the NeoPixels?
#define I2S_DOUT  2
#define I2S_BCLK 3
#define I2S_LRC  4
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#define OLED_VCC 7
#define OLED_SDA 9
#define OLED_SCL 8
#define BUTTON_VOL 10
#define BUTTON_PLAY 11
#define BUTTON_NEXT 12
#define BUTTON_PREV 13
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_NeoPixel pixels(1, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- STATIONS RADIO ---

const char* stations[] = {
"https://icecast.radiofrance.fr/monpetitfranceinter-midfi.aac",
"http://icecast.radiofrance.fr/fipjazz-midfi.mp3",
"http://icecast.radiofrance.fr/fipworld-midfi.mp3",
"http://icecast.radiofrance.fr/franceinfo-midfi.mp3"

};
const char* stationNames[] = {
  "PetitINTER",
  " JAZZ",
  " FIP",
  " INFO"
};
const int numStations = 4;
// --- AUDIO ---

Audio audio;
int currentStation = 0;
int currentVolume = 15;
bool displayConnected = false;
bool displayNeedsUpdate = false;

// --- BOUTONS ---
void setupButtons() {
   pinMode(OLED_VCC, OUTPUT); // board prise GND
 digitalWrite(OLED_VCC, HIGH);
  pinMode(BUTTON_PREV, INPUT_PULLUP);
  pinMode(BUTTON_PLAY, INPUT_PULLUP);
  pinMode(BUTTON_NEXT, INPUT_PULLUP);
  pinMode(BUTTON_VOL, INPUT_PULLUP);
  delay(300);
}
// ----------------- WIFI -----------------
//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
}
void connectWifi() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFiManager wm;
  wm.setAPCallback(configModeCallback);
  if (!wm.autoConnect()) {
    Serial.println("failed to connect and hit timeout");

    pixels.setPixelColor(0, pixels.Color(150, 0, 0)); // rouge
    pixels.show();   // Send the updated pixel colors to the hardware.   
  }
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
      pixels.setPixelColor(0, pixels.Color(0, 150, 0)); // vert
    pixels.show();   // Send the updated pixel colors to the hardware.
} 
// ═══════════════════════════════════════════════════════════════════════
// OLED DISPLAY FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(1);
  display.setCursor(0, 0);
  display.println("INTERNET RADIO");
  display.drawLine(0, 10, 128, 10, 1);
  // Station Name (Large)
  display.setTextSize(2);
  display.setCursor(5, 18);
  display.println( stationNames[currentStation]);
  // Full Station Name
  display.setTextSize(1);
  display.setCursor(0, 38);
  display.println(stations[currentStation]);
  
  // Volume Display
  display.setCursor(0, 52);
  display.print("VOL:");
  display.print(currentVolume);
  
  // Volume Bar Graphic
  int barWidth = map(currentVolume, 0, 21, 0, 80);
  display.drawRect(40, 52, 82, 10, 1);
  display.fillRect(41, 53, barWidth, 8, 1);
  
  // CRITICAL: Update the physical display!
  display.display();
}
void setup() {
  Serial.begin(115200);
  setupButtons();
  // OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED échec"));
    for (;;);
  }
  displayConnected = true;
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Initialisation...");
  display.display();
     pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
     pixels.setPixelColor(0, pixels.Color(0, 0, 150)); // bleu
    pixels.show(); 
    delay(1000);
    pixels.setPixelColor(0, pixels.Color(150, 0, 0)); // rouge
    pixels.show();   // Send the updated pixel colors to the hardware.
  Serial.begin(115200);
 connectWifi();
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(6); // 0...21
  audio.setConnectionTimeout(2000, 3000);
  audio.connecttohost("https://icecast.radiofrance.fr/monpetitfranceinter-midfi.aac");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Connecté!");
  display.display();
}
void loop() {
    audio.loop();
  // Lecture des boutons
  if (digitalRead(BUTTON_PREV) == LOW) {
    delay(300); // Anti-rebond
    changeStation(-1); updateDisplay();
  }
  if (digitalRead(BUTTON_NEXT) == LOW) {
    delay(300);
    changeStation(1);updateDisplay();
  }
  if (digitalRead(BUTTON_PLAY) == LOW) {
    delay(300);
    adjustVolume(+1);updateDisplay();
  }
  if (digitalRead(BUTTON_VOL) == LOW) {
    delay(300);
    adjustVolume(-1);updateDisplay();
  }
   // Refresh display every 2 seconds (prevents stuck display)
  static unsigned long lastDisplayUpdate = 0;
  if (millis() - lastDisplayUpdate > 2000) {
    updateDisplay();
    lastDisplayUpdate = millis();
  }
  
  delay(50);
}
void changeStation(int dir) {
  currentStation += dir;
  if (currentStation < 0) currentStation = 0;
  if (currentStation >= sizeof(stations) / sizeof(stations[0])) {
    currentStation = sizeof(stations) / sizeof(stations[0]) - 1;
  }
  updateDisplay();
  playStation(stations[currentStation]);
}
void adjustVolume(int dir) {
 if (dir == 1 ) {
          // Clockwise - Volume UP
          if (currentVolume < 21) {
            currentVolume++;
            audio.setVolume(currentVolume);
            Serial.printf("Volume UP: %d/21\n", currentVolume);
  
          }
        } else {
          // Counter-clockwise - Volume DOWN
          if (currentVolume > 0) {
            currentVolume--;
            audio.setVolume(currentVolume);
            Serial.printf("Volume DOWN: %d/21\n", currentVolume);
          }
  Serial.printf("Volume ajusté de %d\n", dir);
  }
}
void playStation(const char* url) {
 audio.connecttohost(url);
  }
