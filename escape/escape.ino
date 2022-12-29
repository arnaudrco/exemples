#include <ESP8266WiFi.h>
#include <PubSubClient.h> 
#include <MFRC522.h>  
#include <SPI.h>      
#include <EEPROM.h>   
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h> 

#define pinRST 9    
#define pinSS 10
const byte lcdAddr = 0x27;
const byte lcdCols = 16;
const byte lcdRows = 2;
LiquidCrystal_I2C lcd(lcdAddr, lcdCols, lcdRows);
String initial="Scan Your card";
String Line1_fixed;
String Line2_scrolling_base = "Registration Done!";
byte readCard[4];    
int successRead;
MFRC522 mfrc522(pinSS, pinRST);   
MFRC522::MIFARE_Key key;   
const int buzzer=16;
const char* subtopic = "inTopic"; //you can give any intopic and outtopic names as long as you make sure you provide the same names in the backend(or mqtt app)
const char* pubtopic = "outTopic";
const char* subtopic2 = "inTopic2";
const char* pubtopic2 = "outTopic2";
const char* mqtt_user = "velofqbk"; //enter you mqtt username here
const char* mqtt_pass = "uy4FH4uiHP8i"; //enter your mqtt password
int mqtt_port =  10601; //enter the mqtt port number
const char* mqtt_server = "postman.cloudmqtt.com"; //enter your mqtt server name
const int interruptPin = 0; 
const int LED=2;
bool flag = LOW;
WiFiClient espClient;
PubSubClient client(espClient);

void setup() { 
  Serial.begin(9600); 
  pinMode(LED,OUTPUT); 
  pinMode(interruptPin, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, RISING); 
  pinMode(buzzer,OUTPUT);
  Serial.begin(9600);
  lcd.begin(4,5);
  lcd.init();
  lcd.backlight();
  delay(5000);
  Serial.println("MAC:");
  Serial.println(WiFi.macAddress());
  Serial.print("calling onDemandConfigPortal");
  onDemandConfigPortal();
  client.setServer(mqtt_server, mqtt_port);
  SPI.begin();        
  mfrc522.PCD_Init(); 
  Serial.println("RFID reading process initiated.");   
  Serial.println("Please scan your RFID card to the reader.");
} 

void onDemandConfigPortal()
{
    WiFiManager wifiManager;
      if (!wifiManager.startConfigPortal("OnDemandAP")) 
      {
        Serial.println("failed to connect and hit timeout");
        delay(3000);
        //reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(5000);
      }
      Serial.println("connected...yeey :)"); 
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.subscribe(subtopic);
      client.subscribe(subtopic2);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() 
{ 
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if(flag==LOW)
     eventMode();
  else
     registerMode();       
} 

void handleInterrupt() { 
    if(flag==LOW)
      flag=HIGH;
     else
      flag=LOW; 
}

void eventMode()
{
  digitalWrite(LED, LOW);
  lcd.clear();
  lcd.print(initial);
  lcd.setCursor(4,1);
   lcd.print("CHECK IN");
  getID2();
}

void registerMode()
{
   digitalWrite(LED, HIGH);
   lcd.clear();
   lcd.print(initial);
   lcd.setCursor(4,1);
   lcd.print("REGISTER");
   getID();
}
void getID() 
{
  if ( ! mfrc522.PICC_IsNewCardPresent())  
  {
    return ;
  }

 if ( ! mfrc522.PICC_ReadCardSerial())   
  {
    return ;
  }
  
  digitalWrite(buzzer,HIGH);
  delay(500);
  digitalWrite(buzzer,LOW);
  for (int i = 0; i < mfrc522.uid.size; i++) {  
    readCard[i] = mfrc522.uid.uidByte[i];   
    Serial.print(readCard[i], HEX); 
  }
   char msg[50];
   msg[0]='\0';
   array_to_string(readCard,4,msg);
   client.publish(pubtopic,msg);
   mfrc522.PICC_HaltA();    
   client.setCallback(callback);
}

void getID2() 
{
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return ;
  }

 if ( ! mfrc522.PICC_ReadCardSerial())    
  {
    return ;
  }
  
    digitalWrite(buzzer,HIGH);
    delay(500);
    digitalWrite(buzzer,LOW);
  for (int i = 0; i < mfrc522.uid.size; i++) {  
    readCard[i] = mfrc522.uid.uidByte[i];   
    Serial.print(readCard[i], HEX);  
  }
   char msg[50];
   msg[0]='\0';
   array_to_string(readCard,4,msg);
   byte mac[6];
   WiFi.macAddress(mac);
   char msg1[50];
   msg1[0]='\0';
   array_to_string(mac,6,msg1);
   char mid[]=",";
   char finalMsg[50];
   finalMsg[0]='\0';
   strcat(finalMsg,msg);
   strcat(finalMsg,mid);
   strcat(finalMsg,msg1);
   Serial.print("final msg:");
   Serial.println(finalMsg);
   Serial.print("MAC: ");
   Serial.println(WiFi.macAddress());
   client.publish(pubtopic2,finalMsg);
   Serial.print("\n");
   mfrc522.PICC_HaltA();     
  client.setCallback(callback2);
  
}

void printOnLCD()
{
  String Line2_scrolling =  "                "; 
  Line2_scrolling += Line2_scrolling_base; 
  int Line2_length = Line2_scrolling.length(); 
  int Line2_ChunkNumber = floor(Line2_length / 16 + 2); 
  int Line2_LastChunkLength = Line2_length - ((Line2_ChunkNumber - 2) * 16); 
  int Line2_ChunkLength; 
  if (Line2_LastChunkLength == 0) { 
    Line2_ChunkNumber--;
  }
  String chunk[Line2_ChunkNumber]; 
  for (int j = 0; j < Line2_ChunkNumber - 1; j++) { 
    chunk[j] = Line2_scrolling.substring(j * 16 , j * 16 + 16);
  }
  for (int k = 0; k < Line2_ChunkNumber - 1; k++) {
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print(Line1_fixed); 
    lcd.setCursor(0, 0);
    lcd.print(chunk[k]); 
    lcd.print(chunk[k + 1]); 
    if (k != Line2_ChunkNumber - 2) {
      Line2_ChunkLength = 16;
    }
    else if (Line2_LastChunkLength != 0) {
      Line2_ChunkLength = Line2_LastChunkLength;
    }
    for (int i = 1; i < Line2_ChunkLength + 1; i++) {
      lcd.scrollDisplayLeft(); 
      lcd.setCursor(i, 1); 
      lcd.print(Line1_fixed); 
      delay(500); 
    }
  }
}

void array_to_string(byte a[],unsigned int len,char buffer[])
{
  for(unsigned int i=0;i<len;i++)
  {
    byte nib1=(a[i]>>4)&0x0F;
    byte nib2=(a[i]>>0)&0x0F;
    buffer[i*2+0]=nib1 < 0x0A ? '0' + nib1 : 'A'+ nib1 - 0x0A;
    buffer[i*2+1]=nib2 < 0x0A ? '0' + nib2 : 'A'+ nib2 - 0x0A;
  }
  buffer[len*2]='\0';
}

void callback(char* topic, byte* payload, unsigned int length) {
  Line1_fixed = "CODE: ";
  char password[20];
  password[0]='\0';
  Serial.print("Message arrived [");
  Serial.print(subtopic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    Line1_fixed=Line1_fixed+((char)payload[i]);
  }
  Serial.println();
  Serial.println("fixedline= "+Line1_fixed);
  printOnLCD();
  if(Line1_fixed.length()==6)
  {
    client.setCallback(callback);
  }
  else
  {
    printOnLCD();
  }
}

void callback2(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived [");
  Serial.print(subtopic2);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
   
  }
  Serial.println();
  if((char)payload[0]=='1')
  {
    
    Serial.println("access granted");
    lcd.clear();
    String result="ACCESS GRANTED!";
    lcd.setCursor(0,0);
    lcd.print(result);
    delay(2000);
  }
  else
  {
    Serial.println("access denied");
    lcd.clear();
    String result="ACCESS DENIED!";
    lcd.setCursor(0,0);
    lcd.print(result);
    delay(2000);
  }
}  
