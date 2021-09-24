#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  
#include "pitches.h"

// notes in the melody:
int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
int noteDurations[] = { 4, 8, 8, 4, 4, 4, 4, 4  };

#define Buzzer D1 // Buzzer
int AA=1000; // target Speed
int BB=1000;
char buffer[30];

WiFiServer server(80);

String html1 ="<!DOCTYPE html> \
<html> \
<head> \
<style> .button { padding:10px 10px 10px 10px; width:100%;  background-color: grey ; font-size: 500%;color:white;} </style>\
<center><h1>ARNAUD";
String html2 ="</h1> \
<form> \
<TABLE BORDER> \
 <TR> <TD> <button name='LED0' class='button' value='1' type='submit'>C</button> </TD> <TD> <button name='LED0' class='button'  value='2' type='submit'>#</button></TD> <TD> <button name='LED0' class='button' value='3' type='submit'>D</button> </TD> <TD> <button name='LED0' class='button' value='4' type='submit'>#</button> </TD> <TD> <button name='LED0' class='button'  value='5' type='submit'>E</button></TD> <TD> <button name='LED0' class='button' value='6' type='submit'>F</button> </TD> <TD> <button name='LED0' class='button' value='7' type='submit'>#</button> </TD> <TD> <button name='LED0' class='button'  value='8' type='submit'>G</button>  </TD><TD> <button name='LED0' class='button' value='9' type='submit'>#</button> </TD> <TD> <button name='LED0' class='button' value='A' type='submit'>A</button> </TD> <TD> <button name='LED0' class='button'  value='B' type='submit'>#</button></TD> <TD> <button name='LED0' class='button' value='C' type='submit'>B</button> </TD> </TR>\ 
</TABLE> \
</form> \
</center>\
</head> \
</html>";

/*
String html1 ="<!DOCTYPE html> \
<html> \
<head> \
<style> .button { padding:10px 10px 10px 10px; width:100%;  background-color: green; font-size: 500%;color:white;} </style>\
<center><h1>ARNAUD";
String html2 ="</h1> \
<form> \
<TABLE BORDER> \
 <TR> <TD> <button name='LED0' class='button' value='1' type='submit'>C3</button> </TD> <TD> <button name='LED0' class='button'  value='2' type='submit'>D3</button></TD> <TD> <button name='LED0' class='button' value='3' type='submit'>E3</button> </TD> </TR> \
 <TR> <TD> <button name='LED0' class='button'  value='4' type='submit'>F3</button>  </TD> <TD> <button name='LED0' class='button'  value='5' type='submit'>F# </button></TD> <TD> <button name='LED0' class='button' value='6' type='submit'>G3</button> </TD> </TR>\
 <TR> <TD> <button name='LED0' class='button'  value='7' type='submit'>A3</button>  </TD> <TD> <button name='LED0' class='button'  value='8' type='submit'>B3</button></TD> <TD> <button name='LED0' class='button'  value='9' type='submit'>C4</button> </TD> </TR>\
</TABLE> \
</form> \
</center>\
</head> \
</html>";
*/
void melodie() {
   // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(Buzzer, melody[thisNote], noteDuration);
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(Buzzer);
  }
}
void setup()
{
  Serial.begin(115200);
    tone(Buzzer, 200, 128);
 WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("Connected.");
  server.begin();
  // test buzzer
      tone(Buzzer, 300, 128);
 //  melodie();
}

void loop()
{ // temporisation de 4s pour moteur
  WiFiClient client = server.available();   // Listen for incoming clients

if(client)
{
String request = client.readStringUntil('\r');
//-----------------PAVE HAUT------------
if(request.indexOf("LED0=1") != -1) { 
  AA +=50; if(AA>1023) AA=1023;
     tone(Buzzer, AA=NOTE_C3, 128);
}
if(request.indexOf("LED0=2") != -1){ 
//  digitalWrite(PinB, HIGH);digitalWrite(PinA, HIGH);
//  analogWrite(SpeedA,AA);analogWrite(SpeedB,BB);
   tone(Buzzer, AA=NOTE_CS3, 128);
}
if(request.indexOf("LED0=3") != -1){
  BB +=50; if(BB>1023) BB=1023;
     tone(Buzzer, AA=NOTE_D3, 128);
  
}
//-----------------PAVE CENTRE------------
if(request.indexOf("LED0=4") != -1){
     tone(Buzzer, AA=NOTE_DS3, 128);
}
if(request.indexOf("LED0=5") != -1){
     tone(Buzzer, AA=NOTE_E3, 128);
}
if(request.indexOf("LED0=6") != -1){
   tone(Buzzer, AA=NOTE_F3, 128);
}
//-----------------PAVE BAS------------
if(request.indexOf("LED0=7") != -1){
    tone(Buzzer, AA=NOTE_FS3, 128);
}
if(request.indexOf("LED0=8") != -1){
   tone(Buzzer, AA=NOTE_G3, 128);
}
if(request.indexOf("LED0=9") != -1){
    tone(Buzzer, AA=NOTE_GS3, 128);
}
//-----------------PAVE BAS------------
if(request.indexOf("LED0=A") != -1){
    tone(Buzzer, AA=NOTE_A3, 128);
}
if(request.indexOf("LED0=B") != -1){
   tone(Buzzer, AA=NOTE_AS3, 128);
}
if(request.indexOf("LED0=C") != -1){
    tone(Buzzer, AA=NOTE_B3, 128);
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
