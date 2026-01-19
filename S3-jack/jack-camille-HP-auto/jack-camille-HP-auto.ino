// TEST  sur carte S3 Zero, le 1/1/2026 ArnaudRCO
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


/* GPIOs utilisés ici pour l'ampli BF I2S  MAX98357A, autres choix possibles
#define I2S_DOUT      10  // DIN connection 
#define I2S_BCLK      11  // Bit clock 
#define I2S_LRC       12  // Left Right Clock

#define GND  1
#define I2S_LRC  2
#define I2S_DOUT  3
#define I2S_BCLK 4
#define SCK  5 */

#define I2S_DOUT  2
#define I2S_BCLK 3
#define I2S_LRC  4

Audio audio;



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
          delay(1000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
  }
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
} 



void setup() {

 /* 
  pinMode(GND, OUTPUT); // board prise jack" "PCM5100" GND
  digitalWrite(GND, LOW);
  pinMode(SCK, OUTPUT); // board prise jack" "PCM5100" GND
  digitalWrite(SCK, LOW);
  delay(100);*/
  
  Serial.begin(115200);
   connectWifi();
  
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(6); // 0...21

  audio.connecttohost("https://icecast.radiofrance.fr/monpetitfranceinter-midfi.aac");

  //audio.connecttohost("http://icecast.radiofrance.fr/fipjazz-midfi.mp3");
  //audio.connecttohost("http://icecast.radiofrance.fr/fipworld-midfi.mp3");
  //audio.connecttohost("http://icecast.radiofrance.fr/fip-midfi.mp3");
  //audio.connecttohost("http://icecast.radiofrance.fr/franceinfo-midfi.mp3");
  //audio.connecttohost("http://tsfjazz.ice.infomaniak.ch/tsfjazz-high.mp3");
}


void loop()
{
  audio.loop();
}

// optional
void audio_info(const char *info) {
  Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info) { //id3 metadata
  Serial.print("id3data     "); Serial.println(info);
}
void audio_eof_mp3(const char *info) { //end of file
  Serial.print("eof_mp3     "); Serial.println(info);
}
void audio_showstation(const char *info) {
  Serial.print("station     "); Serial.println(info);
}
void audio_showstreaminfo(const char *info) {
  Serial.print("streaminfo  "); Serial.println(info);
}
void audio_showstreamtitle(const char *info) {
  Serial.print("streamtitle "); Serial.println(info);
}
void audio_bitrate(const char *info) {
  Serial.print("bitrate     "); Serial.println(info);
}
void audio_commercial(const char *info) { //duration in sec
  Serial.print("commercial  "); Serial.println(info);
}
void audio_icyurl(const char *info) { //homepage
  Serial.print("icyurl      "); Serial.println(info);
}
void audio_lasthost(const char *info) { //stream URL played
  Serial.print("lasthost    "); Serial.println(info);
}
void audio_eof_speech(const char *info) {
  Serial.print("eof_speech  "); Serial.println(info);
}
