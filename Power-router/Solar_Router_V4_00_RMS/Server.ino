// ***************
// *  WEB SERVER *
// ***************
void Init_Server() {
  //Init Web Server on port 80
  server.on("/", handleRoot);
  server.on("/MainJS", handleMainJS);
  server.on("/Para", handlePara);
  server.on("/ParaJS", handleParaJS);
  server.on("/ParaRouteurJS", handleParaRouteurJS);
  server.on("/ParaAjax", handleParaAjax);
  server.on("/ParaRouteurAjax", handleParaRouteurAjax);
  server.on("/ParaUpdate", handleParaUpdate);
  server.on("/Actions", handleActions);
  server.on("/ActionsJS", handleActionsJS);
  server.on("/ActionsUpdate", handleActionsUpdate);
  server.on("/ActionsAjax", handleActionsAjax);
  server.on("/Brute", handleBrute);
  server.on("/BruteJS", handleBruteJS);
  server.on("/ajax_histo48h", handleAjaxHisto48h);
  server.on("/ajax_histo1an", handleAjaxHisto1an);
  server.on("/ajax_dataRMS", handleAjaxRMS);
  server.on("/ajax_dataESP32", handleAjaxESP32);
  server.on("/ajax_data", handleAjaxData);
  server.on("/ajax_data10mn", handleAjaxData10mn);
  server.on("/ajax_etatActions", handleAjax_etatActions);
  server.on("/restart", handleRestart);
  server.onNotFound(handleNotFound);
  server.begin();
  //DDD.println("HTTP server started");
}


void handleRoot() {                  //Pages principales
  if (WiFi.getMode() != WIFI_STA) {  // en AP mode
    String NewSsid = server.arg("ssid");
    NewSsid.trim();
    String NewPassword = server.arg("passe");
    NewPassword.trim();
    Serial.println(NewSsid);
    Serial.println(NewPassword);
    if (NewSsid.length() == 0) {
      server.send(200, "text/html", String(ConnectAP_Html));
    } else {
      String S = String(SwitchWifi_Html);
      S.replace("SSID", NewSsid);
      server.send(200, "text/html", S);
      ssid = NewSsid;
      password = NewPassword;
      EcritureEnROM();
      delay(5000);
      ESP.restart();
    }
  } else {  //Station Mode
    server.send(200, "text/html", String(MainHtml));
  }
}
void handleMainJS() {                             //Code Javascript
  server.send(200, "text/html", String(MainJS));  // Javascript code
}
void handleBrute() {  //Page données brutes
  server.send(200, "text/html", String(PageBrute));
}
void handleBruteJS() {                                 //Code Javascript
  server.send(200, "text/html", String(PageBruteJS));  // Javascript code
}

void handleAjaxRMS() {  // Envoi des dernières données  brutes reçues du RMS
  String S = "";
  String RMSExtDataB = "";
  int LastIdx = server.arg(0).toInt();
  if (Source == "Ext") {
    // Use WiFiClient class to create TCP connections
    WiFiClient clientESP_RMS;
    byte arr[4];
    arr[0] = RMSextIP & 0xFF;          // 0x78
    arr[1] = (RMSextIP >> 8) & 0xFF;   // 0x56
    arr[2] = (RMSextIP >> 16) & 0xFF;  // 0x34
    arr[3] = (RMSextIP >> 24) & 0xFF;  // 0x12

    String host = String(arr[3]) + "." + String(arr[2]) + "." + String(arr[1]) + "." + String(arr[0]);
    if (!clientESP_RMS.connect(host.c_str(), 80)) {
      Serial.println("connection to client ESP_RMS external failed (call from  handleAjaxRMS)");
      return;
    }
    String url = "/ajax_dataRMS?idx=" + String(LastIdx);
    clientESP_RMS.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (clientESP_RMS.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> clientESP_RMS Timeout !");
        clientESP_RMS.stop();
        return;
      }
    }
    // Lecture des données brutes distantes
    while (clientESP_RMS.available()) {
      RMSExtDataB += clientESP_RMS.readStringUntil('\r');
    }
    S = RMSExtDataB.substring(RMSExtDataB.indexOf("\n\n") + 2);
  } else {
    S = DATE + RS + Source_data;
    if (Source_data == "UxI") {
      S += RS + String(Tension_M) + RS + String(Intensite_M) + RS + String(PowerFactor_M) + GS;
      int i0 = 0;
      int i1 = 0;
      for (int i = 0; i < 100; i++) {
        i1 = (i + 1) % 100;
        if (voltM[i] <= 0 && voltM[i1] > 0) {
          i0 = i1;  //Point de départ tableau . Phase positive
          i = 100;
        }
      }
      for (int i = 0; i < 100; i++) {
        i1 = (i + i0) % 100;
        S += String(int(10 * voltM[i1])) + RS;  //Voltages*10. Increase dynamic
      }
      S += "0" + GS;
      for (int i = 0; i < 100; i++) {
        i1 = (i + i0) % 100;
        S += String(int(10 * ampM[i1])) + RS;  //Currents*10
      }
      S += "0";
    }
    if (Source_data == "UxIx2") {

      S += GS + String(Tension_M) + RS + String(Intensite_M) + RS + String(PuissanceS_M - PuissanceI_M) + RS + String(PowerFactor_M) + RS + String(Energie_M_Soutiree) + RS + String(Energie_M_Injectee);
      S += RS + String(Tension_T) + RS + String(Intensite_T) + RS + String(PuissanceS_T - PuissanceI_T) + RS + String(PowerFactor_T) + RS + String(Energie_T_Soutiree) + RS + String(Energie_T_Injectee);
      S += RS + String(Frequence);
    }
    if (Source_data == "Linky") {
      S += GS;
      while (LastIdx != IdxDataRawLinky) {
        S += String(DataRawLinky[LastIdx]);
        LastIdx = (1 + LastIdx) % 10000;
      }
      S += GS + String(IdxDataRawLinky);
    }
  }
  server.send(200, "text/html", S);
}
void handleAjaxHisto48h() {  // Envoi Historique de 50h (600points) toutes les 5mn
  String S = "";
  String T = "";
  int iS = IdxStockPW;
  for (int i = 0; i < 600; i++) {
    S += String(tabPw_Maison_5mn[iS]) + ",";
    T += String(tabPw_Triac_5mn[iS]) + ",";
    iS = (1 + iS) % 600;
  }
  server.send(200, "text/html", Source_data + GS + S + GS + T);
}
void handleAjaxESP32() {  // Envoi des dernières infos sur l'ESP32
  String S = "";
  int T = int(millis() / 36000);
  float H = float(T) / 100;
  String coeur0 = String(int(previousTimeRMSMin)) + ", " + String(int(previousTimeRMSMoy)) + ", " + String(int(previousTimeRMSMax));
  String coeur1 = String(int(previousLoopMin)) + ", " + String(int(previousLoopMoy)) + ", " + String(int(previousLoopMax));
  S += String(H) + RS + WiFi.RSSI() + RS + WiFi.BSSIDstr() + RS + WiFi.macAddress() + RS + ssid + RS + WiFi.localIP().toString() + RS + WiFi.gatewayIP().toString() + RS + WiFi.subnetMask().toString();
  S += RS + coeur0 + RS + coeur1 + RS + String(P_cent_EEPROM);
  server.send(200, "text/html", S);
}
void handleAjaxHisto1an() {  // Envoi Historique Energie quotiiienne sur 1 an 370 points
  server.send(200, "text/html", HistoriqueEnergie1An());
}
void handleAjaxData() {  //Données page d'accueil
  String S = "Deb" + RS + DATE + RS + Source_data;
  S += GS + String(PuissanceS_M) + RS + String(PuissanceI_M) + RS + String(PVAS_M) + RS + String(PVAI_M);
  S += RS + String(EnergieJour_M_Soutiree) + RS + String(EnergieJour_M_Injectee) + RS + String(Energie_M_Soutiree) + RS + String(Energie_M_Injectee);
  if (Source_data == "UxIx2") {
    S += GS + String(PuissanceS_T) + RS + String(PuissanceI_T) + RS + String(PVAS_T) + RS + String(PVAI_T);
    S += RS + String(EnergieJour_T_Soutiree) + RS + String(EnergieJour_T_Injectee) + RS + String(Energie_T_Soutiree) + RS + String(Energie_T_Injectee);
  }
  S += GS + "Fin";
  server.send(200, "text/html", S);
}
void handleAjax_etatActions() {
  int NbActifs = 0;
  String S = "";
  String On_;
  for (int i = 0; i < NbActions; i++) {
    if (LesActions[i].Actif == 1) {
      S += String(i) + RS + LesActions[i].Titre + RS;
      if (i == 0) {
        S += String(100 - retard) + RS;
      } else {
        On_ = "Off";
        if (LesActions[i].On) On_ = "On";
        S += On_ + RS;
      }
      S += GS;
      NbActifs++;
    }
  }
  S = String(temperature) + GS + String(Source_data) + GS + String(RMSextIP) + GS + NbActifs + GS + S;
  server.send(200, "text/html", S);
}
void handleRestart() {  // Eventuellement Reseter l'ESP32 à distance
  server.send(200, "text/plain", "OK Reset. Attendez.");
  delay(1000);
  ESP.restart();
}
void handleAjaxData10mn() {  // Envoi Historique de 10mn (300points)Energie Active Soutiré - Injecté
  String S = "";
  String T = "";
  int iS = IdxStock2s;
  for (int i = 0; i < 300; i++) {
    S += String(tabPw_Maison_2s[iS]) + ",";
    S += String(tabPva_Maison_2s[iS]) + ",";
    T += String(tabPw_Triac_2s[iS]) + ",";
    T += String(tabPva_Triac_2s[iS]) + ",";
    iS = (1 + iS) % 300;
  }
  server.send(200, "text/html", Source_data + GS + S + GS + T);
}
void handleActions() {
  server.send(200, "text/html", String(ActionsHtml));
}
void handleActionsJS() {
  server.send(200, "text/html", String(ActionsJS));
}
void handleActionsUpdate() {
  String s = server.arg("actions");
  String ligne = "";
  NbActions = 0;
  while (s.indexOf(GS) > 3 && NbActions < LesActionsLength) {
    ligne = s.substring(0, s.indexOf(GS));
    s = s.substring(s.indexOf(GS) + 1);
    LesActions[NbActions].Definir(ligne);
    NbActions++;
  }
  int adresse_max = EcritureEnROM();
  server.send(200, "text/plain", "OK" + String(adresse_max));
  InitGpioActions();
}
void handleActionsAjax() {
  String S = String(temperature) + GS;
  for (int i = 0; i < NbActions; i++) {
    S += LesActions[i].Lire();
  }
  server.send(200, "text/html", S);
}
void handlePara() {
  server.send(200, "text/html", String(ParaHtml));
}
void handleParaUpdate() {
  String Vp[20];
  String lesparas = server.arg("lesparas") + RS;
  Serial.print("lesparas:");
  Serial.println(lesparas);
  int idx = 0;
  while (lesparas.length() > 0) {
    Vp[idx] = lesparas.substring(0, lesparas.indexOf(RS));
    lesparas = lesparas.substring(lesparas.indexOf(RS) + 1);
    idx++;
  }
  dhcpOn = byte(Vp[0].toInt());
  IP_Fixe = strtoul(Vp[1].c_str(), NULL, 10);
  Gateway = strtoul(Vp[2].c_str(), NULL, 10);
  Source = Vp[3];
  RMSextIP = strtoul(Vp[4].c_str(), NULL, 10);
  DomoRepet = Vp[5].toInt();
  DomoIP = strtoul(Vp[6].c_str(), NULL, 10);  //4 bytes
  DomoPort = Vp[7].toInt();                   //int 2 bytes
  DomoIdx = Vp[8].toInt();                    // int 2 bytes
  MQTTRepet = Vp[9].toInt();
  MQTTIP = strtoul(Vp[10].c_str(), NULL, 10);
  MQTTPort = Vp[11].toInt();  //2 bytes
  MQTTUser = Vp[12];
  MQTTPwd = Vp[13];
  MQTTdeviceName = Vp[14];
  nomRouteur = Vp[15];
  nomSondeFixe = Vp[16];
  nomSondeMobile = Vp[17];
  CalibU = Vp[18].toInt();  //2 bytes
  CalibI = Vp[19].toInt();  //2 bytes
  int adresse_max = EcritureEnROM();
  if (Source != "Ext") {
    Source_data = Source;
  }
  server.send(200, "text/plain", "OK" + String(adresse_max));
}
void handleParaJS() {
  server.send(200, "text/html", String(ParaJS));
}
void handleParaRouteurJS() {
  server.send(200, "text/html", String(ParaRouteurJS));
}
void handleParaAjax() {
  String S = String(dhcpOn) + RS + String(IP_Fixe) + RS + String(Gateway) + RS + Source + RS + String(RMSextIP) + RS;
  S += String(DomoRepet) + RS + String(DomoIP) + RS + String(DomoPort) + RS + String(DomoIdx) + RS;
  S += String(MQTTRepet) + RS + String(MQTTIP) + RS + String(MQTTPort) + RS + MQTTUser + RS + MQTTPwd;
  S += RS + MQTTdeviceName + RS + nomRouteur + RS + nomSondeFixe + RS + nomSondeMobile;
  S += RS + String(CalibU) + RS + String(CalibI);
  server.send(200, "text/html", S);
}
void handleParaRouteurAjax() {
  String S = Source + RS + Source_data + RS + nomRouteur + RS + Version + RS + nomSondeFixe + RS + nomSondeMobile + RS +String(RMSextIP);
  server.send(200, "text/html", S);
}
void handleNotFound() {  //Page Web pas trouvé
  //DDD.println(F("Fichier non trouvé"));
  String message = "Fichier non trouvé\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
