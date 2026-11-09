// TEST  sur carte S3 Zero, le 1/1/2026 ArnaudRCO
// connectez vous à 192.168.4.1 pour rentrer la connexion de votre BOX 
// METTRE A JOUR LE CODE WIFI EN SE CONNECTANT ESP32_XXXX adresse 192.168.4.1
//  options outils > partition > HUGE APP et SPRAM > QSPI
// SORTIE HP 
// Source :  https://www.xtronical.com/i2sinternetradio/
// IDE Arduino 2.3.4
// Core  ESP32 d'Espressif en version 3.1.0  
// Bibliothèque : https://github.com/schreibfaul1/ESP32-audioI2S  ( en VERSION  3.0.12)
// Selectionner pour Partition Scheme  une option laissant au moins 2MB pour le code

#include "Arduino.h"
#include "WiFi.h"
#include "Audio.h"
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <Adafruit_NeoPixel.h>

#define PIN        21 // Which pin on the Arduino is connected to the NeoPixels?
Adafruit_NeoPixel pixels(1, PIN, NEO_GRB + NEO_KHZ800);

// GPIOs utilisés ici pour l'ampli BF I2S  MAX98357A, autres choix possibles
#define I2S_DOUT  2
#define I2S_BCLK 3
#define I2S_LRC  4

// Rotary Encoder Pins
#define ENCODER_CLK 7
#define ENCODER_DT  8
#define GND  9
#define ENCODER_SW  10

Audio audio;
int currentStation = 0;
int currentVolume = 15;
bool displayConnected = false;
bool displayNeedsUpdate = false;
int lastCLK = HIGH;



// ═══════════════════════════════════════════════════════════════════════
// CONFIGURATION
// ═══════════════════════════════════════════════════════════════════════


// Radio Stations
const char* stations[] = {
  "http://listen.openstream.co/4635/audio",                              // Radio Mirchi
  "https://air.pc.cdn.bitgravity.com/air/live/pbaudio173/playlist.m3u8" // All India Radio
};

const char* stationNames[] = {
  "Radio Mango",
  "All India Radio"
};

const int numStations = 2;

// ═══════════════════════════════════════════════════════════════════════
// ROTARY ENCODER FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════

void readRotaryEncoder() {
  static unsigned long lastRotationTime = 0;
  
  // 10ms debounce - fast enough for any human turn speed,
  // long enough for KY-040 electrical bounce to settle (typically 1-5ms)
  if (millis() - lastRotationTime < 10) {
    return;
  }
  
  // Read CLK and DT at the exact same instant from GPIO register
  uint32_t gpioIn = REG_READ(GPIO_IN_REG);
  int clkState = (gpioIn >> ENCODER_CLK) & 1;
  int dtState  = (gpioIn >> ENCODER_DT) & 1;
  
  if (clkState != lastCLK) {
    if (clkState == LOW) {
      // Make sure button is NOT pressed during rotation
      if (digitalRead(ENCODER_SW) == HIGH) {
        // if (dtState == clkState) 
        if (dtState != clkState) {
          // Clockwise - Volume UP
          if (currentVolume < 21) {
            currentVolume++;
            audio.setVolume(currentVolume);
            Serial.printf("Volume UP: %d/21\n", currentVolume);
            displayNeedsUpdate = true;
            lastRotationTime = millis();
          }
        } else {
          // Counter-clockwise - Volume DOWN
          if (currentVolume > 0) {
            currentVolume--;
            audio.setVolume(currentVolume);
            Serial.printf("Volume DOWN: %d/21\n", currentVolume);
            displayNeedsUpdate = true;
            lastRotationTime = millis();
          }
        }
      }
    }
    lastCLK = clkState;
  }
}

void readButton() {
  static int stableButtonState = HIGH;
  static unsigned long buttonDebounceTime = 0;
  const unsigned long buttonDebounceDelay = 250;
  
  int currentButtonState = digitalRead(ENCODER_SW);
  
  if (currentButtonState == LOW && stableButtonState == HIGH) {
    if (millis() - buttonDebounceTime > buttonDebounceDelay) {
      // Switch station
      currentStation = (currentStation + 1) % numStations;
      
audio.stopSong();
  delay(300);
// audio.connecttohost("http://icecast.radiofrance.fr/franceinfo-midfi.mp3");
 audio.connecttohost("https://stream.radiofrance.fr/franceinter/franceinter.m3u8");
     // audio.connecttohost("http://tsfjazz.ice.infomaniak.ch/tsfjazz-high.mp3");
   pixels.setPixelColor(0, pixels.Color(150, 150, 150)); // blanc
    pixels.show();   // Send the updated pixel colors to the hardware.   
  
      
      Serial.printf("\nSwitching to: %s\n", stationNames[currentStation]);
      
//      showLoadingStation();
      playStation(currentStation);
      
      buttonDebounceTime = millis();
      stableButtonState = LOW;
      delay(500);
    }
  } else if (currentButtonState == HIGH && stableButtonState == LOW) {
    stableButtonState = HIGH;
    buttonDebounceTime = millis();
  }
}

// ═══════════════════════════════════════════════════════════════════════
// RADIO FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════

void playStation(int index) {
  Serial.println("\n--------------------------------------");
  Serial.printf("NOW PLAYING: %s\n", stationNames[index]);
  Serial.printf("    Station: %d/%d\n", index + 1, numStations);
  Serial.printf("    Volume: %d/21\n", currentVolume);
  Serial.println("--------------------------------------");
  
  audio.stopSong();
  delay(300);
  
  if (strstr(stations[index], ".m3u8") != NULL) {
    Serial.println("HLS stream - connecting...");
  } else {
    Serial.println("MP3 stream - connecting...");
  }
  
  audio.connecttohost(stations[index]);
  Serial.println("Connection initiated");
  
  // Update display after a short delay
  delay(500);
//  updateDisplay();
}

// ═══════════════════════════════════════════════════════════════════════
// AUDIO CALLBACK FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════
/*
void audio_info(const char *info) {
  Serial.print("Info: ");
  Serial.println(info);
}

void audio_showstation(const char *info) {
  Serial.print("Station: ");
  Serial.println(info);
  displayNeedsUpdate = true;
}

void audio_showstreamtitle(const char *info) {
  Serial.print("Now Playing: ");
  Serial.println(info);
}

void audio_bitrate(const char *info) {
  Serial.print("Bitrate: ");
  Serial.println(info);
}

void audio_eof_stream(const char *info) {
  Serial.println("Stream ended. Reconnecting...");
  delay(1000);
  playStation(currentStation);
}
*/
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


void setup() {
  
 pinMode(GND, OUTPUT); // board prise GND
 digitalWrite(GND, LOW);
  
  // ─────────────────────────────────────────────────────────────────────
  // STEP 1: Setup Rotary Encoder
  // ─────────────────────────────────────────────────────────────────────
  Serial.println("\n[1/4] Setting up Rotary Encoder...");
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  
  lastCLK = digitalRead(ENCODER_CLK);
  Serial.println("Rotary Encoder ready");
  Serial.printf("   Pins: CLK=%d, DT=%d, SW=%d\n", ENCODER_CLK, ENCODER_DT, ENCODER_SW);
  

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
// audio.connecttohost("http://icecast.radiofrance.fr/franceinfo-midfi.mp3");

  //audio.connecttohost("http://icecast.radiofrance.fr/fipjazz-midfi.mp3");
  //audio.connecttohost("http://icecast.radiofrance.fr/fipworld-midfi.mp3");
  //audio.connecttohost("http://icecast.radiofrance.fr/fip-midfi.mp3");
  //audio.connecttohost("http://icecast.radiofrance.fr/franceinfo-midfi.mp3");
  //audio.connecttohost("http://tsfjazz.ice.infomaniak.ch/tsfjazz-high.mp3");
}


void loop()
{
  audio.loop();
    // Read encoder every iteration for fastest response
  readRotaryEncoder();
   
  // Read button every 10ms
  static unsigned long lastButtonRead = 0;
  if (millis() - lastButtonRead > 10) {
    readButton();
    lastButtonRead = millis();
  }
}
