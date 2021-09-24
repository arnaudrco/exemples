#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager

#define PinA 0
#define PinB 2
#define SpeedA 5
#define SpeedB 4

int AA=1000; // target Speed
int BB=1000;
char buffer[30];
unsigned long tempoLampe = 0;        // will store last time LAMPE was updated
const long intervalLampe = 4000; 


WiFiServer server(80);

  
String html1 ="<!DOCTYPE html> \
<html> \
<head> \
<style> .button { padding:10px 10px 10px 10px; width:100%;  background-color: green; font-size: 500%;color:white;} </style>\
<center><h1>ARNAUD";
String html2 ="</h1> \
<form> \
<TABLE BORDER> \
 <TR> <TD> <button name='LED0' class='button' value='1' type='submit'><<</button> </TD> <TD> <button name='LED0' class='button'  value='2' type='submit'>^</button></TD> <TD> <button name='LED0' class='button' value='3' type='submit'>>></button> </TD> </TR> \
 <TR> <TD> <button name='LED0' class='button'  value='4' type='submit'><</button>  </TD> <TD> <button name='LED0' class='button'  value='5' type='submit'>ooo </button></TD> <TD> <button name='LED0' class='button' value='6' type='submit'>></button> </TD> </TR>\
 <TR> <TD> <button name='LED0' class='button'  value='7' type='submit'><<</button>  </TD> <TD> <button name='LED0' class='button'  value='8' type='submit'>v</button></TD> <TD> <button name='LED0' class='button'  value='9' type='submit'>>></button> </TD> </TR>\
</TABLE> \
</form> \
</center>\
</head> \
</html>";
void setup()
{
unsigned long currentMillis = millis();
Serial.begin(115200);
pinMode(PinA, OUTPUT);
pinMode(PinB, OUTPUT);
pinMode(SpeedA, OUTPUT);
pinMode(SpeedB, OUTPUT);
digitalWrite(PinA, LOW);
digitalWrite(PinB, LOW);
// test moteur 
  digitalWrite(PinA, HIGH);digitalWrite(PinB, HIGH);
  analogWrite(SpeedA,AA);analogWrite(SpeedB,BB);
delay(200);
  analogWrite(SpeedA,0);
analogWrite(SpeedB,0);
  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  
  // Uncomment and run it once, if you want to erase all the stored information
  //wifiManager.resetSettings();
  
  // set custom ip for portal
  //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("AutoConnectAP");
  // or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();
  
  // if you get here you have connected to the WiFi
  Serial.println("Connected.");
  
  server.begin();
  // test moteur 
  digitalWrite(PinA, LOW);digitalWrite(PinB, LOW);
  analogWrite(SpeedA,AA);analogWrite(SpeedB,BB);
delay(200);
  analogWrite(SpeedA,0);
analogWrite(SpeedB,0);

}

void loop()
{ // temporisation de 4s pour moteur
    unsigned long currentMillis = millis();
   if(currentMillis - tempoLampe > intervalLampe) {
        analogWrite(SpeedA,0);
        analogWrite(SpeedB,0);
        tempoLampe=currentMillis;
   }
WiFiClient client=server.available();
if(client)
{
String request = client.readStringUntil('\r');
//-----------------PAVE HAUT------------
if(request.indexOf("LED0=1") != -1) { 
  AA +=50; if(AA>1023) AA=1023;
}
if(request.indexOf("LED0=2") != -1){ 
  tempoLampe=currentMillis;
  digitalWrite(PinB, HIGH);digitalWrite(PinA, HIGH);
  analogWrite(SpeedA,AA);analogWrite(SpeedB,BB);
}
if(request.indexOf("LED0=3") != -1){
  BB +=50; if(BB>1023) BB=1023;
  
}
//-----------------PAVE CENTRE------------
if(request.indexOf("LED0=4") != -1){
  tempoLampe=currentMillis;
  digitalWrite(PinA, LOW); digitalWrite(PinB, HIGH);
  analogWrite(SpeedA,AA);analogWrite(SpeedB,BB);
}
if(request.indexOf("LED0=5") != -1){
  analogWrite(SpeedA,0); analogWrite(SpeedB,0);
}
if(request.indexOf("LED0=6") != -1){
  tempoLampe=currentMillis;
  digitalWrite(PinA, HIGH); digitalWrite(PinB, LOW);
  analogWrite(SpeedA,AA);analogWrite(SpeedB,BB);
}
//-----------------PAVE BAS------------
if(request.indexOf("LED0=7") != -1){
 AA -=50; if(AA<0) AA=0;
}
if(request.indexOf("LED0=8") != -1){
  tempoLampe=currentMillis;
  digitalWrite(PinA, LOW);digitalWrite(PinB, LOW);
  analogWrite(SpeedA,AA);analogWrite(SpeedB,BB);
}
if(request.indexOf("LED0=9") != -1){
 BB -=50; if(BB<0) BB=0;
}
// Affichage de la vitesse
sprintf(buffer, " A=%d B=%d ", AA,BB);
//Serial.println(buffer);
client.print(html1);
client.print(buffer);
client.print(html2);
// client.print(html);
request="";
}
}
