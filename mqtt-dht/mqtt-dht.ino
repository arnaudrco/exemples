
#include <WiFi.h>
#include <PubSubClient.h>


// Update these with values suitable for your network.
const char* ssid = "NUMERICABLE-63";
const char* password = "camille01";
const char* mqtt_server = "192.168.1.90";
#define mqtt_port 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define MQTT_SERIAL_PUBLISH_CH "/ESP32/test"
#define MQTT_SERIAL_RECEIVER_CH "/SonOffExp/DHT/temperature"

WiFiClient wifiClient;

PubSubClient client(wifiClient);

//-------------------------------- OLED------------------------------
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"
#define PIN_GND 32
#define PIN_PLUS 33
#define SCL   25
#define SDA   26

SSD1306Wire display(0x3c, SDA, SCL);//initialisation écran

//initialise le buffer
void initDisplay(void) {
 // Initialise the buffer
 display.setLogBuffer(5, 30);
 display.drawLogBuffer(64, 20);
 display.setFont(ArialMT_Plain_16);
 display.display();
 delay(500);
}
// initialise le buffer avec les parametres choisis
//parametre: String text, int x, int y, int size
//renvoie: rien
void printBuffer(String text, int x, int y, int size){
 switch (size) {
 case 10:
 display.setFont(ArialMT_Plain_10);
 break;
 case 16:
 display.setFont(ArialMT_Plain_16);
 break;
 case 24:
 display.setFont(ArialMT_Plain_24);
 break;
 }
 display.drawString(x, y, text);
}
void init_oled(){
  pinMode(PIN_PLUS, OUTPUT);      // sets the digital pin as output
  pinMode(PIN_GND, OUTPUT); 
  digitalWrite(PIN_PLUS, HIGH);
  digitalWrite(PIN_GND, LOW);
      Serial.print("init_oled ");
  delay(100);
   display.init();
 display.flipScreenVertically();
 display.setContrast(255);
 initDisplay();
 initDisplay();
 display.clear();
 printBuffer("STARTING",0, 10, 16);
 display.display();
}
//------------------------
void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      client.publish("/icircuit/presence/ESP32/", "hello world");
      // ... and resubscribe
      client.subscribe(MQTT_SERIAL_RECEIVER_CH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {
    Serial.println("-------new message from broker-----");
    Serial.print("channel:");
    Serial.println(topic);
    Serial.print("data:");
    Serial.write(payload, length);
    char tempString[50];
    for(int i=0;i< length; i++){
      tempString[i]=payload[i];
    }
     display.clear();
 printBuffer("MQTT",0, 10, 16);
    tempString[length]=0;
    printBuffer(topic,0, 30, 10);
   printBuffer(tempString,0, 46, 16);
  display.display();
    Serial.println();
}

void setup() {
  Serial.begin(115200);
    init_oled();
  Serial.setTimeout(500);// Set time out for
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
}

void publishSerialData(char *serialData){
  if (!client.connected()) {
    reconnect();
  }
  client.publish(MQTT_SERIAL_PUBLISH_CH, serialData);
}
void loop() {
   client.loop();
     reconnect();
 } 
