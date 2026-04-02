// version avec recherche automatique de l'adresse IP de la prise OpenBK
// https://github.com/arnaudrco/exemples/wiki/Connaitre-sa-consommation

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ------- CONFIG WIFI + TASMOTA -------
const char* ssid     = "NUMERICABLE-63";
const char* password = "xxx";
bool flag=0; // 
// const char* tasmota_ip = "192.168.1.82";  // <-- change ici
const char* tasmota = "192.168.1.";  // <-- change ici
int IP=80 ; // 82

// ------- OLED -------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET   -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ------- HTTP -------
WiFiClient client;
HTTPClient http;

// Temps entre 2 requêtes (ms)
unsigned long lastQuery = 0;
const unsigned long queryInterval = 5000;

void find_ip() {

  while(flag==false){
    IP++;
    if (IP > 254) IP = 1; // retour boucle
    fetchAndDisplay();
  }
  
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // I2C OLED sur D2 (SDA) / D1 (SCL)
  Wire.begin(D2, D1);  // GPIO4, GPIO5

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED KO !");
    while (true) { delay(1000); }
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("ESP8266 + Tasmota");
  display.println("Power / Total");
  display.display();

  // WiFi
  WiFi.begin(ssid, password);
  display.setCursor(0,20);
  display.println("WiFi...");
  display.display();

  Serial.print("WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("OK");

  display.setCursor(0,30);
  display.println("WiFi OK");
  display.display();
  delay(1000);
  find_ip() ;
}

void loop() {
  if (millis() - lastQuery > queryInterval && WiFi.status() == WL_CONNECTED) {
    lastQuery = millis();
    fetchAndDisplay();  }
}


void fetchAndDisplay() {
  String url = String("http://") + tasmota + String(IP) + "/cm?cmnd=STATUS%208";
  Serial.println("GET: " + url);

  http.begin(client, url);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    Serial.println("Réponse Tasmota:");
    Serial.println(payload);

    // Parsing JSON (ajuste la taille si besoin)
    StaticJsonDocument<2048> doc;
    DeserializationError err = deserializeJson(doc, payload);
    if (err) {
      Serial.print("JSON error: ");
      Serial.println(err.c_str());
      showError("JSON error");
    } else {
      // Selon ton Tasmota, Power/Yesterday peuvent être dans :
      // doc["StatusSNS"]["ENERGY"]["Power"]
      // doc["StatusSNS"]["ENERGY"]["Yesterday"]
      JsonVariant energy = doc["StatusSNS"]["ENERGY"];
      if (energy.isNull()) {
        showError("ENERGY manquant");
      } else {
        float power = energy["ReactivePower"] | 0.0;
        float Total = energy["ConsumptionTotal"] | 0.0;//  energy["Tota"] | 0.0;
        float yesterday = energy["Yesterday"] | 0.0;
        Serial.printf("Power: %.1f W, Total: %.3f kWh\n", power, Total);
        showData(power,Total, yesterday );
        flag = true ; // adresse IP OK
      }
    }
  } else {
    Serial.printf("HTTP error: %d\n", httpCode);
    showError("HTTP error");
   flag = false ;
  }
  http.end();
}

void showData(float power, float Total,float yesterday ) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Tasmota ENERGY");

  display.setTextSize(3);
  display.setCursor(0,16);
  display.print("P:");
  display.print(power, 0);
  display.println("W");

  display.setTextSize(1);
  display.setCursor(0,45);
  display.print("yesterday:");
  display.print(yesterday, 3);
  display.println(" kWh");

  display.setCursor(0,56);
  display.print("Total:");
  display.print(Total, 3);
  display.println(" kWh");

  display.display();
}

void showError(const char* msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Erreur Tasmota");
  display.setCursor(0,16);
  display.println(msg);
  display.display();
}
