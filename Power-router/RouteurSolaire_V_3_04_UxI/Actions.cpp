// ********************
// Gestion des Actions
// ********************
#include <Arduino.h>
#include "Actions.h"
#include "EEPROM.h"
#include <WiFiClient.h>


//Class Action
Action::Action() {
  valide = false;
}
Action::Action(int aIdx) {
  valide = true;  // si le n° de pin n'est pas valid, on ne fait rien
  Idx = aIdx;
  actionTimer = int(millis() / 1000);
  On = false;
}


void Action::Activer(float Pw, int Heure) {
  if (valide && Idx > 0) {
    if (Host == "localhost") {
      for (int i = 0; i < NbPeriode; i++) {
        if (Heure >= Hdeb[i] && Heure <= Hfin[i]) {
          switch (Type[i]) {  //NO,OFF,ON,PW,Triac
            case 1:           //OFF
              digitalWrite(GpioOff, OutOff);
              On = false;
              break;
            case 2:  //ON
              digitalWrite(GpioOn, OutOn);
              On = true;
              break;
            case 3:
              if (Pw < Vmin[i]) {
                digitalWrite(GpioOn, OutOn);
                On = true;
              }
              if (Pw > Vmax[i]) {
                digitalWrite(GpioOff, OutOff);
                On = false;
              }
              break;
          }
        }
      }
    } else {  //Ordre distant
      int tps = int(millis() / 1000);
      for (int i = 0; i < NbPeriode; i++) {
        if (Heure >= Hdeb[i] && Heure <= Hfin[i]) {
          switch (Type[i]) {  //NO,OFF,ON,PW,Triac
            case 1:           //OFF
              if (On) {
                CallExterne(Host, OrdreOff, Port);
                On = false;
                actionTimer = tps;
              }
              break;
            case 2:  //ON
              if (!On && Repet != 0) {
                CallExterne(Host, OrdreOn, Port);
                On = true;
                actionTimer = tps;
              }
              break;
            case 3:
              if (Pw < Vmin[i]) {
                if (!On && Repet != 0) {
                  CallExterne(Host, OrdreOn, Port);
                  On = true;
                  actionTimer = tps;
                }
              }
              if (Pw > Vmax[i]) {
                if (On) {
                  CallExterne(Host, OrdreOff, Port);
                  On = false;
                  actionTimer = tps;
                }
              }
              if ((tps - actionTimer) > Repet && Repet != 0) {
                if (On) {
                  CallExterne(Host, OrdreOn, Port);
                } else {
                  CallExterne(Host, OrdreOff, Port);
                }
                actionTimer = tps;
              }
              break;
          }
        }
      }
    }
  }
}


void Action::Definir(String ligne) {
  valide = true;
  Actif = byte(ligne.substring(0, ligne.indexOf(",")).toInt());
  ligne = ligne.substring(ligne.indexOf(",") + 1);
  Titre = DecodeString(ligne.substring(0, ligne.indexOf(",")));
  ligne = ligne.substring(ligne.indexOf(",") + 1);
  Host = DecodeString(ligne.substring(0, ligne.indexOf(",")));
  ligne = ligne.substring(ligne.indexOf(",") + 1);
  Port = ligne.substring(0, ligne.indexOf(",")).toInt();
  ligne = ligne.substring(ligne.indexOf(",") + 1);
  OrdreOn = DecodeString(ligne.substring(0, ligne.indexOf(",")));
  ligne = ligne.substring(ligne.indexOf(",") + 1);
  OrdreOff = DecodeString(ligne.substring(0, ligne.indexOf(",")));
  ligne = ligne.substring(ligne.indexOf(",") + 1);
  Repet = ligne.substring(0, ligne.indexOf(",")).toInt();
  Repet = min(Repet, 32000);
  Repet = max(0, Repet);
  ligne = ligne.substring(ligne.indexOf(",") + 1);
  NbPeriode = byte(ligne.substring(0, ligne.indexOf(",")).toInt());
  ligne = ligne.substring(ligne.indexOf(",") + 1);
  int Hdeb_ = 0;
  for (byte i = 0; i < NbPeriode; i++) {
    Type[i] = byte(ligne.substring(0, ligne.indexOf(",")).toInt());  //NO,OFF,ON,PW,Triac
    ligne = ligne.substring(ligne.indexOf(",") + 1);
    Hfin[i] = ligne.substring(0, ligne.indexOf(",")).toInt();
    Hdeb[i] = Hdeb_;
    Hdeb_ = Hfin[i];
    ligne = ligne.substring(ligne.indexOf(",") + 1);
    Vmin[i] = ligne.substring(0, ligne.indexOf(",")).toInt();
    ligne = ligne.substring(ligne.indexOf(",") + 1);
    Vmax[i] = ligne.substring(0, ligne.indexOf(",")).toInt();
    ligne = ligne.substring(ligne.indexOf(",") + 1);
  }
}
String Action::Lire() {
  String S;
  S += String(Actif) + ",";
  S += EncodeString(Titre) + ",";
  S += EncodeString(Host) + ",";
  S += String(Port) + ",";
  S += EncodeString(OrdreOn) + ",";
  S += EncodeString(OrdreOff) + ",";
  S += String(Repet) + ",";
  S += String(NbPeriode) + ",";
  for (byte i = 0; i < NbPeriode; i++) {
    S += String(Type[i]) + ",";
    S += String(Hfin[i]) + ",";
    S += String(Vmin[i]) + ",";
    S += String(Vmax[i]) + ",";
  }
  return S + "|";
}

String Action::DecodeString(String s) {
  s.trim();
  String S = "";
  while (s.length() > 1) {
    S += String((char)s.substring(0, s.indexOf("-")).toInt());
    s = s.substring(s.indexOf("-") + 1);
  }
  S.trim();
  return S;
}

String Action::EncodeString(String s) {
  String S = "";
  for (int i = 0; i < s.length(); i++) {
    S += String((int)s.charAt(i)) + "-";
  }
  return S;
}

byte Action::TypeEnCours(int Heure) {  //Retourne type d'action  active à cette heure
  byte S = 0;
  for (int i = 0; i < NbPeriode; i++) {
    if (Heure >= Hdeb[i] && Heure <= Hfin[i]) S = Type[i];
  }
  return S;
}
int Action::Valmin(int Heure) {  //Retourne la valeur Vmin (ex seuil Triac) à cette heure
  int S = 0;
  for (int i = 0; i < NbPeriode; i++) {
    if (Heure >= Hdeb[i] && Heure <= Hfin[i]) {
      S = Vmin[i];
      if (Type[i] == 2) S = 32000;  //Explicitement ON. Force Routage
    }
  }
  return S;
}
int Action::Valmax(int Heure) {  //Retourne la valeur Vmax (ex ouverture du Triac) à cette heure
  int S = 0;
  for (int i = 0; i < NbPeriode; i++) {
    if (Heure >= Hdeb[i] && Heure <= Hfin[i]) {
      S = Vmax[i];
      if (Type[i] == 2) S = 100;  //Explicitement ON. Ouverture 100%
    }
  }
  return S;
}

void Action::InitGpio() {  //Initialise les sorties GPIO pour des relais
  int p;
  int q;
  String S;
  // RAJOUTER UN TEST SUR LA VALIDITE DU GPIO sinon valide=false;
  if (Host == "localhost" && Idx > 0) {

    p = OrdreOn.indexOf("gpio=");
    if (p >= 0) {
      S = OrdreOn.substring(p + 5);
      q = S.indexOf("&");
      if (q == -1) q = 2;
      GpioOn = S.substring(0, q).toInt();
      pinMode(GpioOn, OUTPUT);
      OutOn = 1 + OrdreOn.indexOf("out=1");
      OutOn = min(OutOn, 1);
      if (OrdreOff.indexOf("init") >= 0) On = true;
      if (OrdreOn.indexOf("init=0") >= 0) digitalWrite(GpioOn, 0);
      if (OrdreOn.indexOf("init=1") >= 0) digitalWrite(GpioOn, 1);
    }
    p = OrdreOff.indexOf("gpio=");
    if (p >= 0) {
      S = OrdreOff.substring(p + 5);
      q = S.indexOf("&");
      if (q == -1) q = 2;
      GpioOff = S.substring(0, q).toInt();
      pinMode(GpioOff, OUTPUT);
      OutOff = 1 + OrdreOff.indexOf("out=1");
      OutOff = min(OutOff, 1);
      if (OrdreOff.indexOf("init") >= 0) On = false;
      if (OrdreOff.indexOf("init=0") >= 0) digitalWrite(GpioOff, 0);
      if (OrdreOff.indexOf("init=1") >= 0) digitalWrite(GpioOff, 1);
    }
    valide = true;
    if (GpioOff < 0 || GpioOff > 33 || GpioOn < 0 || GpioOn > 33) valide = false;
  }
}
void Action::CallExterne(String host, String url, int port) {

  // Use WiFiClient class to create TCP connections
  WiFiClient clientExt;
  char hostbuf[host.length() + 1];
  host.toCharArray(hostbuf, host.length() + 1);

  if (!clientExt.connect(hostbuf, port)) {
    Serial.println("connection to clientExt failed :" + host);
    return;
  }
  clientExt.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (clientExt.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> clientESP_Ext Timeout ! : " + host);
      clientExt.stop();
      return;
    }
  }

  // Read all the lines of the reply from server
  while (clientExt.available()) {
    String line = clientExt.readStringUntil('\r');
  }
}
