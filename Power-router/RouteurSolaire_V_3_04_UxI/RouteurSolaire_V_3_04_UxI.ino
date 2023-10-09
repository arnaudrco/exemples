/*
  Routeur Solaire avec Transformateur et Sonde Ampèremétrique 
  ***********************************************************
  Version V_3.04_UxI

  Les détails sont disponibles sur / Details are available  :
  https://f1atb.fr  Section Domotique / Home Automation

  F1ATB Septembre 2023

  GNU General Public License v3.0
  La version  V_3.01_UxI rajoute des sorties MQTT : pws, pwi (P en W soutiré et Injecté) et toujours >=0
  La version  V_3.02_UxI corrige un bug affectant les IDE Arduino sous Linux
  La version  V_3.03_UxI corrige un bug. L'échange de la variable Pw entre les coeurs
  La version  V_3.04_UxI corrige un bug sur la lecture des mesures faites par un ESP extérieur. 
*/

#define HOSTNAME "UxI-ESP32-"    //Début de l'identifiant de l'ESP32 sur le réseau
#define CLE_Rom_Init 1234567892  //Valeur pour tester si ROM (mémoire) vierge ou pas. Un changement de valeur remet à zéro toutes les données.


//Librairies
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <ArduinoOTA.h>    //Modification On The Air
// #include <RemoteDebug.h>   //Debug via Wifi
#include <esp_task_wdt.h>  //Pour un Watchdog
#include <PubSubClient.h>  //Librairie pour la gestion Mqtt
#include "EEPROM.h"        //Librairie pour le stockage en EEPROM historique quotidien
#include "esp_sntp.h"
//Program routines
#include "pageHtmlBrute.h"
#include "pageHtmlMain.h"
#include "pageHtmlConnect.h"
#include "pageHtmlPara.h"
#include "pageHtmlActions.h"
#include "Actions.h"


//Watchdog de 120 secondes. Le systeme se Reset si pas de dialoque avec le UxI ou le UxI distant pendant 120s
#define WDT_TIMEOUT 120

//PINS - GPIO
const int AnalogIn0 = 35;
const int AnalogIn1 = 32;
const int AnalogIn2 = 33;  //Note: si GPIO 33 non disponible sur la carte ESP32, utilisez la 34. If GPIO 33 not available on the board replace by GPIO 34
#define LedYellow 18
#define LedGreen 19
#define pulseTriac 22
#define zeroCross 23


//Nombre Actions Max
#define LesActionsLength 20
//VARIABLES
const char *ap_default_ssid;        // Mode Access point  IP: 192.168.4.1
const char *ap_default_psk = NULL;  // Pas de mot de passe en AP/ No password in AP

//Paramètres dans l'odre de stockage en ROM apres les données du UxI
unsigned long Cle_ROM;
String ssid = "";
String password = "";
byte dhcpOn = 1;
unsigned long IP_Fixe = 0;
unsigned long Gateway = 0;
byte UxIExterne = 0;
unsigned long UxIExtIP = 0;
unsigned int DomoRepet = 0;
unsigned long DomoIP = 0;
unsigned int DomoPort = 0;
unsigned int DomoIdx = 0;
unsigned int MQTTRepet = 0;
unsigned long MQTTIP = 0;
unsigned int MQTTPort = 0;
String MQTTUser = "User";
String MQTTPwd = "password";
String MQTTPrefix = "UxI/";
unsigned int CalibU = 1000;
unsigned int CalibI = 1000;

String UxIExtData;

int WIFIbug = 0;

bool DATEvalid = false;
bool Blink = false;

int value0;
int volt[100];
int amp[100];


int IdxStockPW = 0;

float Uef;  //Voltage
float Ief;  //Current
float PVA;  //Power in VA
float PW;   //Power in Watt
float PowerFactor;
float KV = 0.2083;  //Calibration coefficient for the voltage. Value for CalibU=1000 at startup
float KI = 0.0642;  //Calibration coefficient for the current. Value for CalibI=1000 at startup
float kV = 0.2083;  //Calibration coefficient for the voltage. Corrected value
float kI = 0.0642;  //Calibration coefficient for the current. Corrected value
float voltM[100];   //Voltage Mean value
float ampM[100];    //Amps Mean value
float EASJ = 0;     //Wh du jour soutirée
float EAIJ = 0;     //Watt*hour injectée
float PW_moy = 0;

int tabPW5mn[600];   //Puissance Active toutes les 5mn
float tabPW2s[300];  //Puissance Active toutes les 2s
int tabPVA2s[300];   //Puissance Apparentetoutes les 2s
int IdxStock2s = 0;

Action LesActions[LesActionsLength];  //Liste des actions
int NbActions = 0;


//Internal Timers
unsigned long startMillis;
unsigned long previousWifiMillis;
unsigned long previousHistoryMillis;
unsigned long previousUxIMillis;
unsigned long previousTimer2sMillis;
unsigned long previousBlinkMillis;
unsigned long previousOverProdMillis;
unsigned long previousLoop;
float previousLoopMin = 1000;
float previousLoopMax = 0;
float previousLoopMoy = 0;
unsigned long previousTimeUxI;
float previousTimeUxIMin = 1000;
float previousTimeUxIMax = 0;
float previousTimeUxIMoy = 0;
unsigned long previousDomoMillis;
unsigned long previousMqttMillis;


String DATE = "";
int HeureCouranteDeci = 0;

//Triac
float retardF = 100;  //Floating value of retard(delay)
//Variables in RAM for interruptions
volatile unsigned long lastIT = 0;
volatile int delayTriac = 0;
volatile int retard = 100;
hw_timer_t *timer = NULL;
volatile bool phasePositive = true;
volatile int Idx0 = 0;  //Ref Storage index

WebServer server(80);  // Simple Web Server on port 80

#define MAX_SIZE_T 80
const char *ntpServer = "fr.pool.ntp.org";


//Debug via WIFI instead of Serial
//Connect a Telnet terminal on port 23
// RemoteDebug Debug;
WiFiClient MqttClient;
PubSubClient clientMQTT(MqttClient);

//Multicoeur - Processeur 0 - Collecte données UxI local ou distant
TaskHandle_t Task1;

//Interruptions, Current Zero Crossing from Triac device and Internal Timer
//*************************************************************************
void IRAM_ATTR currentNull() {
  if ((millis() - lastIT) > 2) {  // to avoid glitch detection during 2ms
    delayTriac = 0;               //Time synchro every 10ms
    lastIT = millis();
    digitalWrite(pulseTriac, LOW);  //Stop Triac
  }
}
// Interruption Timer
void IRAM_ATTR onTimer() {  //Interruption every 100 micro second
  delayTriac += 1;
  if (delayTriac > retard && retard < 98) {  //100 steps in 10 ms
    digitalWrite(pulseTriac, HIGH);          //Activate Triac
  }
}

// SETUP
//*******
void setup() {
  startMillis = millis();

  //Pin initialisation
  pinMode(LedYellow, OUTPUT);
  pinMode(LedGreen, OUTPUT);
  pinMode(zeroCross, INPUT);
  pinMode(pulseTriac, OUTPUT);
  digitalWrite(LedYellow, LOW);
  digitalWrite(LedGreen, LOW);
  digitalWrite(pulseTriac, LOW);  //Stop Triac

  //Watchdog initialisation
  esp_task_wdt_init(WDT_TIMEOUT, true);  //enable panic so ESP32 restarts

  //Ports Série
  Serial.begin(115200);
  Serial.println("Booting");

  for (int i = 0; i < LesActionsLength; i++) {
    LesActions[i] = Action(i);  //Creation objets
  }

  for (int i = 0; i < 100; i++) {  //Reset table measurements
    voltM[i] = 0;
    ampM[i] = 0;
  }

  esp_task_wdt_reset();

  // Configure WIFI
  // **************
  String hostname(HOSTNAME);
  uint32_t chipId = 0;
  for (int i = 0; i < 17; i = i + 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  hostname += String(chipId);  //Add chip ID to hostname
  WiFi.hostname(hostname);
  Serial.println(hostname);
  ap_default_ssid = (const char *)hostname.c_str();
  // Check WiFi connection
  // ... check mode
  if (WiFi.getMode() != WIFI_STA) {
    WiFi.mode(WIFI_STA);
    delay(10);
  }

  INIT_EEPROM();
  //Lecture Clé pour identifier si la ROM a déjà été initialisée
  Cle_ROM = CLE_Rom_Init;
  unsigned long Rcle = LectureCle();
  Serial.println("cle : " + String(Rcle));
  if (Rcle == Cle_ROM) {  // Programme déjà executé
    LectureEnROM();
    LectureConsoMatinJour();
    InitGpioActions();
  } else {
    RAZ_Histo_Conso();
  }
  Serial.println("SSID:" + ssid);
  Serial.println("Pass:" + password);
  if (ssid.length() > 0) {
    if (dhcpOn == 0) {  //Static IP
      byte arr[4];
      arr[0] = IP_Fixe & 0xFF;          // 0x78
      arr[1] = (IP_Fixe >> 8) & 0xFF;   // 0x56
      arr[2] = (IP_Fixe >> 16) & 0xFF;  // 0x34
      arr[3] = (IP_Fixe >> 24) & 0xFF;  // 0x12
      // Set your Static IP address
      IPAddress local_IP(arr[3], arr[2], arr[1], arr[0]);
      // Set your Gateway IP address
      arr[0] = Gateway & 0xFF;          // 0x78
      arr[1] = (Gateway >> 8) & 0xFF;   // 0x56
      arr[2] = (Gateway >> 16) & 0xFF;  // 0x34
      arr[3] = (Gateway >> 24) & 0xFF;  // 0x12
      IPAddress gateway(arr[3], arr[2], arr[1], arr[0]);
      IPAddress subnet(255, 255, 255, 0);
      IPAddress primaryDNS(arr[3], arr[2], arr[1], arr[0]);  //optional
      IPAddress secondaryDNS(8, 8, 4, 4);                    //optional
      if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
        Serial.println("WIFI STA Failed to configure");
      }
    }
    Serial.println("Wifi Begin : " + ssid);
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED && (millis() - startMillis < 15000)) {  // Attente connexion au Wifi
      Serial.write('.');
      digitalWrite(LedYellow, HIGH);
      digitalWrite(LedGreen, HIGH);
      Serial.print(WiFi.status());
      delay(300);
      digitalWrite(LedYellow, LOW);
      digitalWrite(LedGreen, LOW);
      delay(200);
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    // ... print IP Address
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Connected IP address: " + WiFi.localIP().toString() + " or <a href='http://" + hostname + ".local' >" + hostname + ".local</a>");
  } else {
    Serial.println("Can not connect to WiFi station. Go into AP mode.");
    // Go into software AP mode.
    WiFi.mode(WIFI_AP);
    delay(10);
    WiFi.softAP(ap_default_ssid, ap_default_psk);
    Serial.print("Access Point Mode. IP address: ");
    Serial.println(WiFi.softAPIP());
  }

  // init remote debug
//  Debug.begin("ESP32");  // Telnet on port 23
//  Debug.println("Ready");
//  Debug.print("IP address: ");
//  Debug.println(WiFi.localIP());

  //Init Web Server on port 80
  server.on("/", handleRoot);
  server.on("/MainJS", handleMainJS);
  server.on("/Para", handlePara);
  server.on("/ParaJS", handleParaJS);
  server.on("/ParaAjax", handleParaAjax);
  server.on("/ParaUpdate", handleParaUpdate);
  server.on("/Actions", handleActions);
  server.on("/ActionsJS", handleActionsJS);
  server.on("/ActionsUpdate", handleActionsUpdate);
  server.on("/ActionsAjax", handleActionsAjax);
  server.on("/Brute", handleBrute);
  server.on("/BruteJS", handleBruteJS);
  server.on("/ajax_histo48h", handleAjaxHisto48h);
  server.on("/ajax_histo1an", handleAjaxHisto1an);
  server.on("/ajax_dataUxI", handleAjaxUxI);
  server.on("/ajax_dataESP32", handleAjaxESP32);
  server.on("/ajax_data", handleAjaxData);
  server.on("/ajax_data10mn", handleAjaxData10mn);
  server.on("/ajax_etatActions", handleAjax_etatActions);
  server.on("/restart", handleRestart);
  server.onNotFound(handleNotFound);
  server.begin();
//  Debug.println("HTTP server started");



  //Interruptions du Triac et Timer interne
  attachInterrupt(zeroCross, currentNull, RISING);

  //Hardware timer
  timer = timerBegin(0, 80, true);  //Clock Divider, 1 micro second Tick
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 100, true);  //Interrupt every 100 Ticks or microsecond
  timerAlarmEnable(timer);


  // Modification du programme par le Wifi  - OTA(On The Air)
  //***************************************************
  ArduinoOTA.setHostname((const char *)hostname.c_str());
  ArduinoOTA.begin();  //Mandatory

  xTaskCreatePinnedToCore(  //Préparation Tâche Multi Coeur
    Task_LectureUxI,        /* Task function. */
    "Task_LectureUxI",      /* name of task. */
    10000,                  /* Stack size of task */
    NULL,                   /* parameter of the task */
    10,                     /* priority of the task */
    &Task1,                 /* Task handle to keep track of created task */
    0);                     /* pin task to core 0 */

  //Heure / Hour
  //External timer to obtain the Hour and reset Watt Hour every day at 0h
  sntp_set_time_sync_notification_cb(time_sync_notification);
  configTzTime("CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", ntpServer);  //Voir Time-Zone: https://sites.google.com/a/usapiens.com/opnode/time-zones



  //Timers
  previousWifiMillis = millis() + 300000;
  previousHistoryMillis = millis() - 290000;
  previousTimer2sMillis = millis();
  previousLoop = millis();
  previousTimeUxI = millis();
  previousDomoMillis = millis();
  previousMqttMillis = millis();
  previousBlinkMillis = millis();
  previousOverProdMillis = millis();
}

/* **********************
   * ****************** *
   * * Tâches Coeur 0 * *
   * ****************** *
   **********************
*/

void Task_LectureUxI(void *pvParameters) {
  esp_task_wdt_add(NULL);  //add current thread to WDT watch
  esp_task_wdt_reset();
  for (;;) {
    unsigned long tps = millis();
    float deltaT = float(tps - previousTimeUxI);
    previousTimeUxI = tps;
    previousTimeUxIMin = min(previousTimeUxIMin, deltaT);
    previousTimeUxIMin = previousTimeUxIMin + 0.001;
    previousTimeUxIMax = max(previousTimeUxIMax, deltaT);
    previousTimeUxIMax = previousTimeUxIMax * 0.9999;
    previousTimeUxIMoy = deltaT * 0.001 + previousTimeUxIMoy * 0.999;


    //Recupération des données UxI
    //******************************
    if (UxIExterne == 1) {
      if (millis() - previousUxIMillis > 2000) {
        previousUxIMillis = millis();
        CallESP32_UxI();
      }
    } else {
      if (millis() - previousUxIMillis >= 40) {
        previousUxIMillis = millis();
        MeasurePower();
        ComputePower();
      }
      delay(2);
    }
  }
}
void MeasurePower() {  //Lecture Tension et courants pendant 20ms
  int iStore;
  value0 = analogRead(AnalogIn0);  //Mean value. Should be at 3.3v/2
  unsigned long MeasureMillis = millis();

  while (millis() - MeasureMillis < 21) {  //Read values in continuous during 20ms. One loop is around 150 micro seconds
    iStore = (micros() % 20000) / 200;     //We have more results that we need during 20ms to fill the tables of 100 samples
    volt[iStore] = analogRead(AnalogIn1) - value0;
    amp[iStore] = analogRead(AnalogIn2) - value0;
  }
}
void ComputePower() {
  float PWcal =0;   //Computation Power in Watt
  float V;
  float I;
  float Uef2 = 0;
  float Ief2 = 0;
  for (int i = 0; i < 100; i++) {
    voltM[i] = (19 * voltM[i] + float(volt[i])) / 20;  //Mean value. First Order Filter. Short Integration
    V = kV * voltM[i];
    Uef2 += sq(V);
    amp[i] = min(amp[i],2047); //Retrait glitch appel courant démarrage
    amp[i] = max(amp[i],-2047); //Retrait glitch appel courant démarrage
    ampM[i] = (19 * ampM[i] + float(amp[i])) / 20;  //Mean value. First Order Filter
    I = kI * ampM[i];
    Ief2 += sq(I);
    PWcal += V * I;
  }
  Uef2 = Uef2 / 100;  //square of voltage
  Uef = sqrt(Uef2);  //RMS voltage
  Ief2 = Ief2 / 100;  //square of current
  Ief = sqrt(Ief2);  // RMS current
  PWcal = PWcal / 100;
  PVA = floor(Uef * Ief);
  PowerFactor = floor(100 * PWcal / PVA) / 100;
  if (PWcal >= 0) {
    EASJ += PWcal / 90000;  // Watt Hour,Every 40ms. Soutirée
  } else {
    EAIJ += -PWcal / 90000;  // Watt Hour,Every 40ms . Injecté
  }

  PW = floor(PWcal);
  esp_task_wdt_reset();
}
// Client d'un autre ESP32 en charge de decoder le UxI
//****************
void CallESP32_UxI() {
  int p;

  // Use WiFiClient class to create TCP connections
  WiFiClient clientESP_UxI;
  byte arr[4];
  arr[0] = UxIExtIP & 0xFF;          // 0x78
  arr[1] = (UxIExtIP >> 8) & 0xFF;   // 0x56
  arr[2] = (UxIExtIP >> 16) & 0xFF;  // 0x34
  arr[3] = (UxIExtIP >> 24) & 0xFF;  // 0x12

  String host = String(arr[3]) + "." + String(arr[2]) + "." + String(arr[1]) + "." + String(arr[0]);
  if (!clientESP_UxI.connect(host.c_str(), 80)) {
    //  if (!clientESP_UxI.connect(hostbuf, 80)) {
    Serial.println("connection to clientESP_UxI failed");
    return;
  }
  String url = "/ajax_data";
  clientESP_UxI.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (clientESP_UxI.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> clientESP_UxI Timeout !");
      clientESP_UxI.stop();
      return;
    }
  }

  // Read all the lines of the reply from server
  while (clientESP_UxI.available()) {
    String line = clientESP_UxI.readStringUntil('\r');
    UxIExtData += line;
    p = UxIExtData.indexOf("Deb,");
    while (p >= 0) {
      UxIExtData = UxIExtData.substring(p + 4);
      p = UxIExtData.indexOf("Deb,");
    }
    if (UxIExtData.length() > 30 && UxIExtData.indexOf("Fin") > 0) {
      int idx = 0;
      String Sval = "";
      while (UxIExtData.indexOf(",") >= 0) {
        Sval = UxIExtData.substring(0, UxIExtData.indexOf(","));
        UxIExtData = UxIExtData.substring(UxIExtData.indexOf(",") + 1);
        switch (idx) {
          case 0:
            //DATE = Sval; On ne prend pas le temps ESP distant
            break;
          case 1:
            Uef = Sval.toFloat();
            break;
          case 2:
            Ief = Sval.toFloat();
            break;
          case 3:
            PW = Sval.toFloat();
            break;
          case 4:
            PVA = Sval.toFloat();
            break;
          case 5:
            PowerFactor = Sval.toFloat();
            break;
          case 6:
            EASJ = Sval.toFloat();
            break;
          case 7:
            EAIJ = Sval.toFloat();
            esp_task_wdt_reset();  //Reset du Watchdog à chaque trame reçue
            break;
        }
        idx++;
      }
    }
  }
  if (UxIExtData.length() > 300) UxIExtData = "";  //Pas de bonnes données
}

/* **********************
   * ****************** *
   * * Tâches Coeur 1 * *
   * ****************** *
   **********************
*/
void loop() {
  //Estimation charge coeur
  long tps = millis();
  float deltaT = float(tps - previousLoop);
  previousLoop = tps;
  previousLoopMin = min(previousLoopMin, deltaT);
  previousLoopMin = previousLoopMin + 0.001;
  previousLoopMax = max(previousLoopMax, deltaT);
  previousLoopMax = previousLoopMax * 0.9999;
  previousLoopMoy = deltaT * 0.001 + previousLoopMoy * 0.999;

  //Gestion des serveurs
  //********************
  ArduinoOTA.handle();
//  Debug.handle();
  server.handleClient();

  //Archivage et envois des mesures périodiquement
  //**********************************************
  if (millis() - previousBlinkMillis >= 2000) {
    Blink = !Blink;
    if (Blink) {
      previousBlinkMillis = millis() - 1950;
    } else {
      previousBlinkMillis = millis();
    }
    if (PW >= 0) {
      digitalWrite(LedYellow, Blink);
      digitalWrite(LedGreen, LOW);
    } else {
      digitalWrite(LedYellow, LOW);
      digitalWrite(LedGreen, Blink);
    }
  }
  if (tps - previousOverProdMillis >= 200) {
    previousOverProdMillis = tps;
    GestionOverproduction();
  }
  if (tps - previousTimer2sMillis >= 2000) {
    previousTimer2sMillis = tps;
    tabPW2s[IdxStock2s] = PW;
    tabPVA2s[IdxStock2s] = PVA;
    //Variation Energie Active
    IdxStock2s = (IdxStock2s + 1) % 300;
    PW_moy = (PW + 99 * PW_moy) / 100;  //Moyenne long terme
    EnvoiaDomoticz();
    envoiAuMQTT();
    JourHeureChange();
  }
  if (tps - previousHistoryMillis >= 300000) {  //Historique consommation par pas de 5mn
    previousHistoryMillis = tps;
    tabPW5mn[IdxStockPW] = floor(PW_moy);
    IdxStockPW = (IdxStockPW + 1) % 600;
  }
  //Vérification du WIFI
  //********************
  if (tps - previousWifiMillis > 30000) {  //Test présence WIFI toutes les 30s
    previousWifiMillis = tps;
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
//      Debug.println("Connection Failed! #" + String(WIFIbug));
      Serial.println("Connection Failed! #" + String(WIFIbug));
      WIFIbug++;
      if (WIFIbug > 20) {
        ESP.restart();
      }
    } else {
      WIFIbug = 0;
    }
    Serial.print("Niveau Signal WIFI:");
    Serial.println(WiFi.RSSI());
    Serial.print("WIFIbug:");
    Serial.println(WIFIbug);
//    Debug.print("Niveau Signal WIFI:");
//    Debug.println(WiFi.RSSI());
//    Debug.print("WIFIbug:");
//    Debug.println(WIFIbug);
    Serial.println("Charge Lecture UxI en ms - Min : " + String(int(previousTimeUxIMin)) + " Moy : " + String(int(previousTimeUxIMoy)) + "  Max : " + String(int(previousTimeUxIMax)));
//    Debug.println("Charge Lecture UxI en ms - Min : " + String(int(previousTimeUxIMin)) + " Moy : " + String(int(previousTimeUxIMoy)) + "  Max : " + String(int(previousTimeUxIMax)));
    Serial.println("Charge Boucle générale en ms - Min : " + String(int(previousLoopMin)) + " Moy : " + String(int(previousLoopMoy)) + "  Max : " + String(int(previousLoopMax)));
//    Debug.println("Charge Boucle générale en ms - Min : " + String(int(previousLoopMin)) + " Moy : " + String(int(previousLoopMoy)) + "  Max : " + String(int(previousLoopMax)));
    int T = int(millis() / 1000);
    float DureeOn = float(T) / 3600;
    Serial.println("ESP32 ON depuis : " + String(DureeOn) + " heures");
//    Debug.println("ESP32 ON depuis : " + String(DureeOn) + " heures");
  }
  if ((tps - startMillis) > 180000 && WiFi.getMode() != WIFI_STA) {  //Connecté en  Access Point depuis 3mn. Pas normal
    Serial.println("Pas connecté en WiFi mode Station. Redémarrage");
    delay(5000);
    ESP.restart();
  }
}

// ************
// *  ACTIONS *
// ************
void GestionOverproduction() {
  //Cas du Triac. Action 0
  if (LesActions[0].Actif == 1 && LesActions[0].TypeEnCours(HeureCouranteDeci) >= 2) {
    float TriacSeuilPw = LesActions[0].Valmin(HeureCouranteDeci);
    float MaxTriac = LesActions[0].Valmax(HeureCouranteDeci);
    float GainBoucle = LesActions[0].Port;                        //Valeur stockée dans Port
    retardF = retardF + 0.01;                                     //On ferme très légèrement si pas de message reçu
    retardF = retardF + (PW - TriacSeuilPw) * GainBoucle / 1000;  // Gain de boucle de l'asservissement
    if (retardF < 0) { retardF = 0; }
    if (retardF < 100 - MaxTriac) { retardF = 100 - MaxTriac; }
    if (retardF > 100) { retardF = 100; }
    retard = int(retardF);  //Valeure entiere pour piloter le Triac
  } else {
    retardF = 100;
    retard = 100;
  }
  //Cas des GPIOS
  for (int i = 1; i < NbActions; i++) {
    LesActions[i].Activer(PW, HeureCouranteDeci);
  }
}

void InitGpioActions() {
  for (int i = 1; i < NbActions; i++) {
    if (LesActions[i].Actif == 1) {
      LesActions[i].InitGpio();
    }
  }
}
// ***********
// *  SERVER *
// ***********
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

void handleAjaxUxI() {  // Envoi des tableaux I et U  du UxI
  String S = "";
  String UxIExtDataB = "";

  if (UxIExterne == 1) {
    // Use WiFiClient class to create TCP connections
    WiFiClient clientESP_UxI;
    byte arr[4];
    arr[0] = UxIExtIP & 0xFF;          // 0x78
    arr[1] = (UxIExtIP >> 8) & 0xFF;   // 0x56
    arr[2] = (UxIExtIP >> 16) & 0xFF;  // 0x34
    arr[3] = (UxIExtIP >> 24) & 0xFF;  // 0x12

    String host = String(arr[3]) + "." + String(arr[2]) + "." + String(arr[1]) + "." + String(arr[0]);
    if (!clientESP_UxI.connect(host.c_str(), 80)) {
      Serial.println("connection to clientESP_UxI failed");
      return;
    }
    String url = "/ajax_dataUxI";
    clientESP_UxI.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (clientESP_UxI.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> clientESP_UxI Timeout !");
        clientESP_UxI.stop();
        return;
      }
    }
    // Lecture des données brutes distantes
    while (clientESP_UxI.available()) {
      S += clientESP_UxI.readStringUntil('\r');
    }
    S = S.substring(S.indexOf("\n\n") + 2);
  } else {
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
      S += String(int(10 * voltM[i1])) + ",";  //Voltages*10. Increase dynamic
    }
    S += "0|";
    for (int i = 0; i < 100; i++) {
      i1 = (i + i0) % 100;
      S += String(int(10 * ampM[i1])) + ",";  //Currents*10
    }
    S += "0";
  }
  server.send(200, "text/html", S);
}
void handleAjaxHisto48h() {  // Envoi Historique de 50h (600points) toutes les 5mn
  String S = "";
  int iS = IdxStockPW;
  for (int i = 0; i < 600; i++) {
    S += String(tabPW5mn[iS]) + ",";
    iS = (1 + iS) % 600;
  }
  server.send(200, "text/html", S);
}
void handleAjaxESP32() {  // Envoi des dernières infos sur l'ESP32
  String S = "";
  int T = int(millis() / 36000);
  float H = float(T) / 100;
  String coeur0 = String(int(previousTimeUxIMin)) + ", " + String(int(previousTimeUxIMoy)) + ", " + String(int(previousTimeUxIMax));
  String coeur1 = String(int(previousLoopMin)) + ", " + String(int(previousLoopMoy)) + ", " + String(int(previousLoopMax));
  S += String(H) + "|" + WiFi.RSSI() + "|" + WiFi.BSSIDstr() + "|" + ssid + "|" + WiFi.localIP().toString() + "|" + WiFi.gatewayIP().toString() + "|" + WiFi.subnetMask().toString() + "|" + coeur0 + "|" + coeur1 + "|" + DATE;
  server.send(200, "text/html", S);
}
void handleAjaxHisto1an() {  // Envoi Historique Energie quotiiienne sur 1 an 370 points
  server.send(200, "text/html", HistoriqueEnergie1An());
}
void handleAjaxData() {

  String S = "Deb," + DATE + "," + String(Uef) + "," + String(Ief) + "," + String(PW) + "," + String(PVA) + "," + String(PowerFactor) + "," + String(EASJ) + "," + String(EAIJ) + ",Fin";
  server.send(200, "text/html", S);
}
void handleAjax_etatActions() {
  int NbActifs = 0;
  String S = "";
  String On_;
  for (int i = 0; i < NbActions; i++) {
    if (LesActions[i].Actif == 1) {
      S += String(i) + "," + LesActions[i].Titre + ",";
      if (i == 0) {
        S += String(100 - retard) + ",";
      } else {
        On_ = "Off";
        if (LesActions[i].On) On_ = "On";
        S += On_ + ",";
      }
      S += "|";
      NbActifs++;
    }
  }
  S = String(UxIExterne) + "|" + String(UxIExtIP) + "|" + NbActifs + "|" + S;
  server.send(200, "text/html", S);
}
void handleRestart() {  // Eventuellement Reseter l'ESP32 à distance
  server.send(200, "text/plain", "OK Reset. Attendez.");
  delay(1000);
  ESP.restart();
}
void handleAjaxData10mn() {  // Envoi Historique de 10mn (300points)Energie Active Soutiré - Injecté
  String S = "";
  int iS = IdxStock2s;
  for (int i = 0; i < 300; i++) {
    S += String(tabPW2s[iS]) + ",";
    S += String(tabPVA2s[iS]) + ",";
    iS = (1 + iS) % 300;
  }
  server.send(200, "text/html", S);
}
void handleActions() {
  server.send(200, "text/html", String(ActionsHtml));
}
void handleActionsJS() {
  server.send(200, "text/html", String(ActionsJS));
}
void handleActionsUpdate() {
  String s = server.arg("actions");
  Serial.println(s);
  String ligne = "";
  NbActions = 0;
  while (s.indexOf("|") >= 0 && NbActions < LesActionsLength) {
    ligne = s.substring(0, s.indexOf("|"));
    s = s.substring(s.indexOf("|") + 1);
    LesActions[NbActions].Definir(ligne);
    NbActions++;
  }
  int adresse_max = EcritureEnROM();
  server.send(200, "text/plain", "OK" + String(adresse_max));
  InitGpioActions();
}
void handleActionsAjax() {
  String S;
  for (int i = 0; i < NbActions; i++) {
    S += LesActions[i].Lire();
  }
  server.send(200, "text/html", S);
}
void handlePara() {
  server.send(200, "text/html", String(ParaHtml));
}
void handleParaUpdate() {
  dhcpOn = byte(server.arg("dhcp").toInt());
  IP_Fixe = strtoul(server.arg("adrIP").c_str(), NULL, 10);
  Gateway = strtoul(server.arg("gateway").c_str(), NULL, 10);
  UxIExterne = byte(server.arg("UxIExterne").toInt());
  UxIExtIP = strtoul(server.arg("UxIExtIP").c_str(), NULL, 10);
  DomoRepet = server.arg("DomoRepete").toInt();
  DomoIP = strtoul(server.arg("DomoIP").c_str(), NULL, 10);  //4 bytes
  DomoPort = server.arg("DomoPort").toInt();                 //int 2 bytes
  DomoIdx = server.arg("DomoIdx").toInt();                   // int 2 bytes
  MQTTRepet = server.arg("MQTTRepete").toInt();
  MQTTIP = strtoul(server.arg("MQTTIP").c_str(), NULL, 10);
  MQTTPort = server.arg("MQTTPort").toInt();  //2 bytes
  MQTTUser = DecodeString(server.arg("MQTTUser"));
  MQTTPwd = DecodeString(server.arg("MQTTpwd"));
  MQTTPrefix = DecodeString(server.arg("MQTTprefix"));
  CalibU = server.arg("CalibU").toInt();  //2 bytes
  CalibI = server.arg("CalibI").toInt();  //2 bytes
  int adresse_max = EcritureEnROM();
  server.send(200, "text/plain", "OK" + String(adresse_max));
}
void handleParaJS() {
  server.send(200, "text/html", String(ParaJS));
}
void handleParaAjax() {
  String S = String(dhcpOn) + "," + String(IP_Fixe) + "," + String(Gateway) + "," + String(UxIExterne) + "," + String(UxIExtIP) + ",";
  S += String(DomoRepet) + "," + String(DomoIP) + "," + String(DomoPort) + "," + String(DomoIdx) + ",";
  S += String(MQTTRepet) + "," + String(MQTTIP) + "," + String(MQTTPort) + "," + EncodeString(MQTTUser) + "," + EncodeString(MQTTPwd) + "," + EncodeString(MQTTPrefix) + ",";
  S += String(CalibU) + "," + String(CalibI);
  server.send(200, "text/html", S);
}
void handleNotFound() {  //Page Web pas trouvé
//  Debug.println(F("Fichier non trouvé"));
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

void time_sync_notification(struct timeval *tv) {
  Serial.println("Notification of a time synchronization event");
  DATEvalid = true;
}

// *******************
// * DOMOTICZ client * - Option pour transferer vers systeme Domoticz
// *******************
void EnvoiaDomoticz() {
  unsigned long tps = millis();
  if (int((tps - previousDomoMillis) / 1000) > DomoRepet && DomoRepet != 0) {
    previousDomoMillis = tps;
    byte arr[4];
    arr[0] = DomoIP & 0xFF;          // 0x78
    arr[1] = (DomoIP >> 8) & 0xFF;   // 0x56
    arr[2] = (DomoIP >> 16) & 0xFF;  // 0x34
    arr[3] = (DomoIP >> 24) & 0xFF;  // 0x12

    String host = String(arr[3]) + "." + String(arr[2]) + "." + String(arr[1]) + "." + String(arr[0]);
    WiFiClient DomoClient;
    if (!DomoClient.connect(host.c_str(), DomoPort)) {
      Serial.println("connection vers Domoticz plante");
      return;
    }
    String url;
    url = "/json.htm?type=command&param=udevice&idx=";
    url += String(DomoIdx);
    url += "&nvalue=0&svalue=";
    url += String(PW);
    Serial.println(url);
    // This will send the request to the Domoticz server
    DomoClient.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (DomoClient.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Domoticz Timeout !");
        DomoClient.stop();
        return;
      }
    }
    // Read all the lines of the reply from server and print them to Serial
    while (DomoClient.available()) {
      String line = DomoClient.readStringUntil('\r');
      //No interest of the response
    }
  }
}


// *********
// *  MQTT *
// *********
void envoiAuMQTT() {
  unsigned long tps = millis();
  float PWs = 0; //Soutirée
  float PWi = 0; // Injectée
  int etat = 0;
  if (int((tps - previousMqttMillis) / 1000) > MQTTRepet && MQTTRepet != 0) {
    previousMqttMillis = tps;
    byte arr[4];
    arr[0] = MQTTIP & 0xFF;          // 0x78
    arr[1] = (MQTTIP >> 8) & 0xFF;   // 0x56
    arr[2] = (MQTTIP >> 16) & 0xFF;  // 0x34
    arr[3] = (MQTTIP >> 24) & 0xFF;  // 0x12
    String host = String(arr[3]) + "." + String(arr[2]) + "." + String(arr[1]) + "." + String(arr[0]);
    clientMQTT.setServer(host.c_str(), MQTTPort);
    //clientMQTT.setServer(hostbuf, MQTTPort);
    clientMQTT.setCallback(callback);  //Déclaration de la fonction de souscription
    if (!clientMQTT.connected()) {
      Serial.println("Connection au serveur MQTT ...");
      if (clientMQTT.connect("UxIClient", MQTTUser.c_str(), MQTTPwd.c_str())) {
        Serial.println("MQTT connecté");
 //       Debug.println("MQTT connecté");
      } else {
        Serial.print("echec connexion au MQTT, code erreur= ");
        Serial.println(clientMQTT.state());
//        Debug.print("echec connexion au MQTT, code erreur= ");
//       Debug.println(clientMQTT.state());
        return;
      }
    }
    clientMQTT.loop();
    mqtt_publish(MQTTPrefix + "pva", PVA);
    mqtt_publish(MQTTPrefix + "easj", EASJ);
    mqtt_publish(MQTTPrefix + "eaij", EAIJ);
    mqtt_publish(MQTTPrefix + "pw", PW);
    PWs=0;
    PWi=0;
    if (PW<0) {
      PWi=-PW; //Injecté
    } else {
      PWs=PW;
    }
    mqtt_publish(MQTTPrefix + "pws", PWs);
    mqtt_publish(MQTTPrefix + "pwi", PWi);
    if (LesActions[0].Actif == 1) {  //CAs du Triac
      int triac = 100 - retard;
      mqtt_publish(MQTTPrefix + "triac", triac);
    }
    for (int i = 1; i < NbActions; i++) {
      if (LesActions[i].Actif == 1) {
        etat = 0;
        if (LesActions[i].On) { etat = 1; }
        mqtt_publish(MQTTPrefix + "action" + String(i), etat);  //Etat On Off des actions (relais)
      }
    }
    clientMQTT.loop();
  }
}
//Callback  pour souscrire a un topic et  prévoir une action
void callback(char *topic, byte *payload, unsigned int length) {
  Serial.println("-------Nouveau message du broker mqtt. Non utilisé-----");
}

//Fonction pour publier un float sur un topic
void mqtt_publish(String topic, float t) {
  String t_str = String(t);
  clientMQTT.publish(topic.c_str(), t_str.c_str());
}
// ******
// DIVERS
// ******
String DecodeString(String s) {
  String S = "";
  while (s.length() > 1) {
    S += String((char)s.substring(0, s.indexOf("-")).toInt());
    s = s.substring(s.indexOf("-") + 1);
  }
  return S;
}
String EncodeString(String s) {
  String S = "";
  for (int i = 0; i < s.length(); i++) {
    S += String((int)s.charAt(i)) + "-";
  }
  return S;
}
