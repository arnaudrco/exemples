// TEST  sur carte S2 MIni, le 1/1/2026 ArnaudRCO
//
// Source :  https://www.xtronical.com/i2sinternetradio/
// IDE Arduino 1.8.20
// Core  ESP32 d'Espressif en version 3.1.0  
// Bibliothèque : https://github.com/schreibfaul1/ESP32-audioI2S  ( en VERSION  3.0.12)
// Selectionner pour Partition Scheme  une option laissant au moins 2MB pour le code

#include "Arduino.h"
#include "WiFi.h"
#include "Audio.h"

// GPIOs utilisés ici pour l'ampli BF I2S  MAX98357A, autres choix possibles
#define I2S_DOUT      5  // DIN connection 
#define I2S_BCLK      7  // Bit clock 
#define I2S_LRC       9  // Left Right Clock

Audio audio;

String ssid =     "NUMERICABLE-63";
String password = "xxx";


void setup() {
  Serial.begin(115200);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED) delay(1500);
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(6); // 0...21

  audio.connecttohost("http://stream.antennethueringen.de/live/aac-64/stream.antennethueringen.de/");

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
