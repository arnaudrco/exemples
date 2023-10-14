/*
  PV Router / Routeur Solaire 
  ****************************************
  Version V_4.00_RMS

  RMS=Routeur Multi Sources

  Choix de 3 sources différentes pour lire la consommation électrique en entrée de maison
  - lecture de la tension avec un transformateur et du courant avec une sonde ampèremétrique (UxI)
  - lecture des données du Linky (Linky)
  - module (JSY-MK-194T) intégrant une mesure de tension secteur et 2 sondes ampèmétriques (UxIx2)
  
  En option une mesure de température (DS18B20) est possible.
  
  Les détails sont disponibles sur / Details are available here:
  https://f1atb.fr  Section Domotique / Home Automation

  F1ATB Octobre  2023 

  GNU General Public License v3.0

  ArnaudRCO : suppression de RemoteDebug

  modification de 9600 >> 1200 bauds 


*/
#define Version "4.00_RMS"
#define HOSTNAME "RMS-ESP32-"
#define CLE_Rom_Init 4004567894  //Valeur pour tester si ROM vierge ou pas. Un changement de valeur remet à zéro toutes les données. / Value to test whether blank ROM or not.


//Librairies
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ArduinoOTA.h>    //Modification On The Air
// #include <Remote//DDD.h>   //Debug via Wifi
#include <esp_task_wdt.h>  //Pour un Watchdog
#include <PubSubClient.h>  //Librairie pour la gestion Mqtt
#include "EEPROM.h"        //Librairie pour le stockage en EEPROM historique quotidien
#include "esp_sntp.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include <ArduinoJson.h>

//Program routines
#include "pageHtmlBrute.h"
#include "pageHtmlMain.h"
#include "pageHtmlConnect.h"
#include "pageHtmlPara.h"
#include "pageHtmlActions.h"
#include "Actions.h"


//Watchdog de 120 secondes. Le systeme se Reset si pas de dialoque avec le LINKY ou JSY-MK-194T pendant 120s
//Watchdog for 120 seconds. The system resets if no dialogue with the Linky or  JSY-MK-194T for 120s 
#define WDT_TIMEOUT 120

//PINS - GPIO

const int AnalogIn0 = 35;  //Pour Routeur Uxi
const int AnalogIn1 = 32;
const int AnalogIn2 = 33;  //Note: si GPIO 33 non disponible sur la carte ESP32, utilisez la 34. If GPIO 33 not available on the board replace by GPIO 34
// #define RXD2 26            //Pour Routeur Linky ou UxIx2
#define RXD2 12            //Pour Routeur Linky ou UxIx2
#define PLUS 14            // alimentation opto coupleur
#define TXD2 27
#define LedYellow 18
#define LedGreen 19
#define pulseTriac 22
#define zeroCross 23
#define pinTemp 13  //Capteur température


//Nombre Actions Max
#define LesActionsLength 20
//VARIABLES
const char *ap_default_ssid;        // Mode Access point  IP: 192.168.4.1
const char *ap_default_psk = NULL;  // Pas de mot de passe en AP,

//Paramètres dans l'odre de stockage en ROM apres les données du RMS
unsigned long Cle_ROM;

String ssid = "NUMERICABLE-63";
String password = "xxxx";
String Source = "UxI";
//String Source = "Linky";
String Source_data = "UxI";
//String Source_data = "Linky";
byte dhcpOn = 1;
unsigned long IP_Fixe = 0;
unsigned long Gateway = 0;
unsigned long RMSextIP = 0;
unsigned int DomoRepet = 0;
unsigned long DomoIP = 0;
unsigned int DomoPort = 0;
unsigned int DomoIdx = 0;
unsigned int MQTTRepet = 0;
unsigned long MQTTIP = 0;
unsigned int MQTTPort = 0;
String MQTTUser = "User";
String MQTTPwd = "password";
String MQTTdeviceName = "routeur_rms";
String nomRouteur = "Routeur - RMS";
String nomSondeFixe = "Données Triac";
String nomSondeMobile = "Données Maison";
String GS = String((char)29);  //Group Separator
String RS = String((char)30);  //Record Separator
int P_cent_EEPROM;
int cptLEDyellow = 0;
int cptLEDgreen = 0;

unsigned int CalibU = 1000;  //Calibration Routeur UxI
unsigned int CalibI = 1000;
int value0;
int volt[100];
int amp[100];
//float KV = 0.2083;  //Calibration coefficient for the voltage. Value for CalibU=1000 at startup
//float KI = 0.0642;  //Calibration coefficient for the current. Value for CalibI=1000 at startup
float kV = 0.2083;  //Calibration coefficient for the voltage. Corrected value
float kI = 0.0642;  //Calibration coefficient for the current. Corrected value

float KV = 0.2083 * 234 / 264 ;  // réel et relevé web
float KI = 0.0642 * 3 / 47 ;  // réel et relevé web

float voltM[100];   //Voltage Mean value
float ampM[100];

bool EnergieActiveValide = false;
long EAS_T_J0 = 0;
long EAI_T_J0 = 0;
long EAS_M_J0 = 0;  //Debut du jour energie active
long EAI_M_J0 = 0;


int adr_debut_para = 0;  //Adresses Para après le Wifi


//Paramètres électriques
float Tension_T, Intensite_T, PowerFactor_T, Frequence;
float Tension_M, Intensite_M, PowerFactor_M;
long Energie_T_Soutiree = 0;
long Energie_T_Injectee = 0;
long Energie_M_Soutiree = 0;
long Energie_M_Injectee = 0;
long EnergieJour_T_Injectee = 0;
long EnergieJour_M_Injectee = 0;
long EnergieJour_T_Soutiree = 0;
long EnergieJour_M_Soutiree = 0;
int PuissanceS_T, PuissanceS_M, PuissanceI_T, PuissanceI_M;
int PVAS_T, PVAS_M, PVAI_T, PVAI_M;
int tabPw_Maison_5mn[600];  //Puissance Active:Soutiré-Injecté toutes les 5mn
int tabPw_Triac_5mn[600];
int tabPw_Maison_2s[300];   //Puissance Active: toutes les 2s
int tabPw_Triac_2s[300];    //Puissance Triac: toutes les 2s
int tabPva_Maison_2s[300];  //Puissance Active: toutes les 2s
int tabPva_Triac_2s[300];
int IdxStock2s = 0;
int IdxStockPW = 0;

//Parameters for JSY-MK-194T module
byte ByteArray[130];
long LesDatas[14];
int Sens_1, Sens_2;

//Parameters for Linky
bool LFon = false;
bool EASTvalid = false;
bool EAITvalid = false;
int IdxDataRawLinky = 0;
int IdxBufDecodLinky = 0;
char DataRawLinky[10000];  //Buffer entrée données Linky
float moyPWS = 0;
float moyPWI = 0;
float moyPVAS = 0;
float moyPVAI = 0;
float COSphiS = 1;
float COSphiI = 1;
long TlastEASTvalide = 0;
long TlastEAITvalide = 0;
String LTARF = ""; //Option tarifaire


//Actions
Action LesActions[LesActionsLength];  //Liste des actions
int NbActions = 0;



//Internal Timers
unsigned long startMillis;
unsigned long previousWifiMillis;
unsigned long previousHistoryMillis;
unsigned long previousWsMillis;
unsigned long previousWiMillis;
unsigned long nextRMS_Millis;
unsigned long previousTimer2sMillis;
unsigned long previousOverProdMillis;
unsigned long previousLEDsMillis;
unsigned long previousLoop;
unsigned long previousETX;

float previousLoopMin = 1000;
float previousLoopMax = 0;
float previousLoopMoy = 0;
unsigned long previousTimeRMS;
float previousTimeRMSMin = 1000;
float previousTimeRMSMax = 0;
float previousTimeRMSMoy = 0;
unsigned long previousDomoMillis;
unsigned long previousMqttMillis;

//Triac
float retardF = 100;  //Floating value of retard(delay)
//Variables in RAM for interruptions
volatile unsigned long lastIT = 0;
volatile int delayTriac = 0;
volatile int retard = 100;
hw_timer_t *timer = NULL;

WebServer server(80);  // Simple Web Server on port 80

// Heure et Date
#define MAX_SIZE_T 80
const char *ntpServer = "fr.pool.ntp.org";
String DATE = "";
String DateCeJour = "";
bool DATEvalid = false;
int HeureCouranteDeci = 0;
int idxPromDuJour = 0;

//Température Capteur DS18B20
OneWire oneWire(pinTemp);
DallasTemperature ds18b20(&oneWire);
float temperature = 0;  // La valeur vaut -127 quand la sonde DS18B20 n'est pas présente

//Debug via WIFI instead of Serial
//Connect a Telnet terminal on port 23
// RemoteDebug Debug;
WiFiClient MqttClient;
PubSubClient clientMQTT(MqttClient);
int WIFIbug = 0;

//Multicoeur - Processeur 0 - Collecte données RMS local ou distant
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
  previousLEDsMillis = startMillis;

  //Pin initialisation
  pinMode(LedYellow, OUTPUT);
  pinMode(LedGreen, OUTPUT);
  pinMode(zeroCross, INPUT);
  pinMode(pulseTriac, OUTPUT);
  digitalWrite(LedYellow, LOW);
  digitalWrite(LedGreen, LOW);
  digitalWrite(pulseTriac, LOW);  //Stop Triac

  pinMode(PLUS, OUTPUT);
  digitalWrite(PLUS, HIGH);
  //Watchdog initialisation
  esp_task_wdt_init(WDT_TIMEOUT, true);  //enable panic so ESP32 restarts

  //Ports Série ESP
  Serial.begin(115200);
  Serial.println("Booting");

  for (int i = 0; i < LesActionsLength; i++) {
    LesActions[i] = Action(i);  //Creation objets
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
      Gestion_LEDs();
      Serial.print(WiFi.status());
      delay(300);
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
  //DDD.begin("ESP32");
  //DDD.println("Ready");
  //DDD.print("IP address: ");
  //DDD.println(WiFi.localIP());

  Init_Server();

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

  //Adaptation à la Source
  Serial.println("Source : " + Source);

  if (Source == "UxIx2") {
    Setup_UxIx2();
  }
  if (Source == "UxI") {
    Setup_UxI();
  }
  if (Source == "Linky") {
    Setup_Linky();
  }
  if (Source == "Ext") {
  } else {
    Source_data = Source;
  }

  xTaskCreatePinnedToCore(  //Préparation Tâche Multi Coeur
    Task_LectureRMS,        /* Task function. */
    "Task_LectureRMS",      /* name of task. */
    10000,                  /* Stack size of task */
    NULL,                   /* parameter of the task */
    10,                     /* priority of the task */
    &Task1,                 /* Task handle to keep track of created task */
    0);                     /* pin task to core 0 */


  //Heure / Hour
  //External timer to obtain the Hour and reset Watt Hour every day at 0h
  sntp_set_time_sync_notification_cb(time_sync_notification);
  configTzTime("CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", ntpServer);  //Voir Time-Zone: https://sites.google.com/a/usapiens.com/opnode/time-zones

  //Temperature
  ds18b20.begin();
  LectureTemperature();

  //Timers
  previousWifiMillis = millis() + 300000;
  previousHistoryMillis = millis() - 290000;
  previousTimer2sMillis = millis();
  previousLoop = millis();
  previousTimeRMS = millis();
  previousDomoMillis = millis();
  previousMqttMillis = millis() - 5000;
  previousETX = millis();
  previousOverProdMillis = millis();
  nextRMS_Millis = millis() + 500;
}

/* **********************
   * ****************** *
   * * Tâches Coeur 0 * *
   * ****************** *
   **********************
*/

void Task_LectureRMS(void *pvParameters) {
  esp_task_wdt_add(NULL);  //add current thread to WDT watch
  esp_task_wdt_reset();
  for (;;) {
    unsigned long tps = millis();
    float deltaT = float(tps - previousTimeRMS);
    previousTimeRMS = tps;
    previousTimeRMSMin = min(previousTimeRMSMin, deltaT);
    previousTimeRMSMin = previousTimeRMSMin + 0.001;
    previousTimeRMSMax = max(previousTimeRMSMax, deltaT);
    previousTimeRMSMax = previousTimeRMSMax * 0.9999;
    previousTimeRMSMoy = deltaT * 0.001 + previousTimeRMSMoy * 0.999;


    //Recupération des données RMS
    //******************************

    if (tps >= nextRMS_Millis) {

      if (Source == "UxI") {
        nextRMS_Millis = tps + 40;
        LectureUxI();
      }
      if (Source == "UxIx2") {
        nextRMS_Millis = tps + 400;
        LectureUxIx2();
      }
      if (Source == "Linky") {
        nextRMS_Millis = tps;
        LectureLinky();
      }
      if (Source == "Ext") {       
        CallESP32_Externe();
        nextRMS_Millis = millis() + 2000; //Après pour ne pas surchargé Wifi
      }
    }
    delay(2);
  }
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
  //DDD.handle();
  server.handleClient();

  //Archivage et envois des mesures périodiquement
  //**********************************************
  if (EnergieActiveValide) {
    if (tps - previousHistoryMillis >= 300000) {  //Historique consommation par pas de 5mn
      previousHistoryMillis = tps;
      previousHistoryMillis = tps;
      tabPw_Maison_5mn[IdxStockPW] = PuissanceS_M - PuissanceI_M;
      tabPw_Triac_5mn[IdxStockPW] = PuissanceS_T - PuissanceI_T;
      IdxStockPW = (IdxStockPW + 1) % 600;
    }

    if (tps - previousTimer2sMillis >= 2000) {
      previousTimer2sMillis = tps;
      tabPw_Maison_2s[IdxStock2s] = PuissanceS_M - PuissanceI_M;
      tabPw_Triac_2s[IdxStock2s] = PuissanceS_T - PuissanceI_T;
      tabPva_Maison_2s[IdxStock2s] = PVAS_M - PVAI_M;
      tabPva_Triac_2s[IdxStock2s] = PVAS_T - PVAI_T;
      IdxStock2s = (IdxStock2s + 1) % 300;
      EnvoiaDomoticz(PuissanceS_M - PuissanceI_M);
      envoiAuMQTT();
      JourHeureChange();
    }

    if (tps - previousOverProdMillis >= 200) {
      previousOverProdMillis = tps;
      GestionOverproduction();
    }
  }
  if (tps - previousLEDsMillis >= 50) {
    previousLEDsMillis = tps;
    Gestion_LEDs();
  }
  //Vérification du WIFI
  //********************
  if (tps - previousWifiMillis > 30000) {  //Test présence WIFI toutes les 30s
    previousWifiMillis = tps;
    if (WiFi.waitForConnectResult(10000) != WL_CONNECTED) {
      Serial.println("Connection Failed! #" + String(WIFIbug));
      WIFIbug++;
      if (WIFIbug > 2) {
        ESP.restart();
      }
    } else {
      WIFIbug = 0;
    }
    Serial.print("Niveau Signal WIFI:");
    Serial.println(WiFi.RSSI());
    Serial.print("WIFIbug:");
    Serial.println(WIFIbug);
    //DDD.print("Niveau Signal WIFI:");
    //DDD.println(WiFi.RSSI());
    //DDD.print("WIFIbug:");
    //DDD.println(WIFIbug);
    Serial.println("Charge Lecture RMS (coeur 0) en ms - Min : " + String(int(previousTimeRMSMin)) + " Moy : " + String(int(previousTimeRMSMoy)) + "  Max : " + String(int(previousTimeRMSMax)));
    //DDD.println("Charge Lecture RMS (coeur 0) en ms - Min : " + String(int(previousTimeRMSMin)) + " Moy : " + String(int(previousTimeRMSMoy)) + "  Max : " + String(int(previousTimeRMSMax)));
    Serial.println("Charge Boucle générale (coeur 1) en ms - Min : " + String(int(previousLoopMin)) + " Moy : " + String(int(previousLoopMoy)) + "  Max : " + String(int(previousLoopMax)));
    //DDD.println("Charge Boucle générale (coeur 1) en ms - Min : " + String(int(previousLoopMin)) + " Moy : " + String(int(previousLoopMoy)) + "  Max : " + String(int(previousLoopMax)));
    int T = int(millis() / 1000);
    float DureeOn = float(T) / 3600;
    Serial.println("ESP32 ON depuis : " + String(DureeOn) + " heures");
    //DDD.println("ESP32 ON depuis : " + String(DureeOn) + " heures");
    //Temperature
    LectureTemperature();
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
  
  //Puissance est la puissance en entrée de maison. >0 si soutire. <0 si injecte
  //Cas du Triac. Action 0
  int Puissance = PuissanceS_M - PuissanceI_M;
  if (LesActions[0].Actif == 1 && LesActions[0].TypeEnCours(HeureCouranteDeci) >= 2) {
    float TriacSeuilPw = LesActions[0].Valmin(HeureCouranteDeci);
    float MaxTriac = LesActions[0].Valmax(HeureCouranteDeci);
    float GainBoucle = LesActions[0].Port;                               //Valeur stockée dans Port
    retardF = retardF + 0.1;                                             //On ferme très légèrement si pas de message reçu
    retardF = retardF + (Puissance - TriacSeuilPw) * GainBoucle / 2000;  // Gain de boucle de l'asservissement
    if (retardF < 0) { retardF = 0; }
    if (retardF < 100 - MaxTriac) { retardF = 100 - MaxTriac; }
    if (retardF > 100) { retardF = 100; }
    LesActions[0].On = true;
  } else {
    retardF = 100;
    LesActions[0].On = false;
  }
  retard = int(retardF);  //Valeure entiere pour piloter le Triac
  //Cas des GPIOS
  for (int i = 1; i < NbActions; i++) {
    LesActions[i].Activer(Puissance, HeureCouranteDeci, temperature);
  }
}

void InitGpioActions() {
  for (int i = 1; i < NbActions; i++) {
    if (LesActions[i].Actif == 1) {
      LesActions[i].InitGpio();
    }
  }
}
// ***********************************
// * Calage Zéro Energie quotidienne * -
// ***********************************
void EnerQuotidienM_Soutiree() {
  if (Energie_M_Soutiree < EAS_M_J0 || EAS_M_J0 == 0) {
    EAS_M_J0 = Energie_M_Soutiree;
  }
  EnergieJour_M_Soutiree = Energie_M_Soutiree - EAS_M_J0;
}
void EnerQuotidienM_Injectee() {
  if (Energie_M_Injectee < EAI_M_J0 || EAI_M_J0 == 0) {
    EAI_M_J0 = Energie_M_Injectee;
  }
  EnergieJour_M_Injectee = Energie_M_Injectee - EAI_M_J0;
}
void EnerQuotidienT_Soutiree() {
  if (Energie_T_Soutiree < EAS_T_J0 || EAS_T_J0 == 0) {
    EAS_T_J0 = Energie_T_Soutiree;
  }
  EnergieJour_T_Soutiree = Energie_T_Soutiree - EAS_T_J0;
}
void EnerQuotidienT_Injectee() {
  if (Energie_T_Injectee < EAI_T_J0 || EAI_T_J0 == 0) {
    EAI_T_J0 = Energie_T_Injectee;
  }
  EnergieJour_T_Injectee = Energie_T_Injectee - EAI_T_J0;
}


// **************
// * Heure DATE * -
// **************
void time_sync_notification(struct timeval *tv) {
  Serial.println("Notification of a time synchronization event");
  DATEvalid = true;
}

// ***************
// * Temperature *
// ***************
void LectureTemperature() {
  ds18b20.requestTemperatures();
  temperature = ds18b20.getTempCByIndex(0);
  if (temperature<-20 || temperature>130) { //Invalide. Pas de capteur?
    temperature=-125;
  }
  Serial.print("Température : ");
  Serial.print(temperature);
  Serial.println("°C");
  //DDD.print("Température : ");
  //DDD.print(temperature);
  //DDD.println("°C");
}

//****************
//* Gestion LEDs *
//****************
void Gestion_LEDs() {

  cptLEDyellow++;

  if (WiFi.status() != WL_CONNECTED) {  // Attente connexion au Wifi
    if (WiFi.getMode() == WIFI_STA) {   // en  Station mode
      cptLEDyellow = (cptLEDyellow + 6) % 10;
      cptLEDgreen = cptLEDyellow;
    } else {  //AP Mode
      cptLEDyellow = cptLEDyellow % 10;
      cptLEDgreen = (cptLEDyellow + 5) % 10;
    }
  } else {
    if (retard < 100) {
      cptLEDgreen = int((cptLEDgreen + 1 + 8 / (1 + retard / 10))) % 10;
    } else {
      cptLEDgreen = 10;
    }
  }
  if (cptLEDyellow > 5) {
    digitalWrite(LedYellow, LOW);
  } else {
    digitalWrite(LedYellow, HIGH);
  }
  if (cptLEDgreen > 5) {
    digitalWrite(LedGreen, LOW);
  } else {
    digitalWrite(LedGreen, HIGH);
  }
}
