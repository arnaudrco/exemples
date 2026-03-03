#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(115200);
  
  // Init CC1101
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.setMHZ(433.92);  // Fréquence télécommandes FR
  ELECHOUSE_cc1101.setModulation(2); // MODULATION_2_FSK
  ELECHOUSE_cc1101.setDeviation(47.60); // Déviation FSK
  ELECHOUSE_cc1101.setChannel(0);
  ELECHOUSE_cc1101.setChsp(199.95);  // Espacement canal
  ELECHOUSE_cc1101.setRxBW(812.50);  // Bande passante réception
  ELECHOUSE_cc1101.setDRate(9.58);   // Débit 9.6kbps
  
  // Mode réception
  ELECHOUSE_cc1101.SetRx();
  
  // RCSwitch sur GDO0 (D1 GPIO5)
  mySwitch.enableReceive(D1); // D1
  
  Serial.println("🔍 Récepteur CC1101 + ESP8266 prêt");
  Serial.println("Appuie sur tes télécommandes 433MHz !");
}

void loop() {
  if (mySwitch.available()) {
    // Récupère le code reçu
    long code = mySwitch.getReceivedValue();
    int bits = mySwitch.getReceivedBitlength();
    int delay = mySwitch.getReceivedDelay();
    
    if (code == 0) {
      Serial.println("Signal inconnu");
    } else {
      Serial.print("📡 CODE: ");
      Serial.print(code);
      Serial.print(" | BITS: ");
      Serial.print(bits);
      Serial.print(" | PULSE: ");
      Serial.print(delay);
      Serial.println("µs");
      
      // Détection type télécommande
      if (bits == 24) Serial.println("  → Type PT2262 classique");
      if (bits == 12) Serial.println("  → Type simple 12 bits");
    }
    
    mySwitch.resetAvailable();
  }
  
  delay(50);
}
