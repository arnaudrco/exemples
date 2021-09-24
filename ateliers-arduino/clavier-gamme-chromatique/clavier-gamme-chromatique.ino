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
<center><h1>ARNAUD";
String html2 ="</h1> \
<form> \
<TABLE BORDER> \
  <style> .button { padding:10px 10px 10px 10px; width:100%;  background-color: blue ; font-size: 500%;color:white;} </style>\
    <style> .button2 { padding:10px 10px 10px 10px; width:100%;  background-color: green ; font-size: 500%;color:white;} </style>\
      <style> .button3 { padding:10px 10px 10px 10px; width:100%;  background-color: yellow ; font-size: 500%;color:white;} </style>\
        <style> .button4 { padding:10px 10px 10px 10px; width:100%;  background-color: orange ; font-size: 500%;color:white;} </style>\
 <TR> <TD> <button name='LED0' class='button' value='1' type='submit'>C</button> </TD> <TD> <button name='LED0' class='button'  value='2' type='submit'>#</button></TD> <TD> <button name='LED0' class='button' value='3' type='submit'>D</button> </TD> <TD> <button name='LED0' class='button' value='4' type='submit'>#</button> </TD> <TD> <button name='LED0' class='button'  value='5' type='submit'>E</button></TD> <TD> <button name='LED0' class='button' value='6' type='submit'>F</button> </TD> <TD> <button name='LED0' class='button' value='7' type='submit'>#</button> </TD> <TD> <button name='LED0' class='button'  value='8' type='submit'>G</button>  </TD><TD> <button name='LED0' class='button' value='9' type='submit'>#</button> </TD> <TD> <button name='LED0' class='button' value='X' type='submit'>A</button> </TD> <TD> <button name='LED0' class='button'  value='Y' type='submit'>#</button></TD> <TD> <button name='LED0' class='button' value='Z' type='submit'>B</button> </TD> </TR>\ 
 <TR> <TD> <button name='LED0' class='button2' value='A' type='submit'>C</button> </TD> <TD> <button name='LED0' class='button2'  value='B' type='submit'>#</button></TD> <TD> <button name='LED0' class='button2' value='C' type='submit'>D</button> </TD> <TD> <button name='LED0' class='button2' value='D' type='submit'>#</button> </TD> <TD> <button name='LED0' class='button2'  value='E' type='submit'>E</button></TD> <TD> <button name='LED0' class='button2' value='F' type='submit'>F</button> </TD> <TD> <button name='LED0' class='button2' value='G' type='submit'>#</button> </TD> <TD> <button name='LED0' class='button2'  value='H' type='submit'>G</button>  </TD><TD> <button name='LED0' class='button2' value='I' type='submit'>#</button> </TD> <TD> <button name='LED0' class='button2' value='J' type='submit'>A</button> </TD> <TD> <button name='LED0' class='button2'  value='K' type='submit'>#</button></TD> <TD> <button name='LED0' class='button2' value='L' type='submit'>B</button> </TD> </TR>\ 
 <TR> <TD> <button name='LED0' class='button3' value='a' type='submit'>C</button> </TD> <TD> <button name='LED0' class='button3'  value='b' type='submit'>#</button></TD> <TD> <button name='LED0' class='button3' value='c' type='submit'>D</button> </TD> <TD> <button name='LED0' class='button3' value='d' type='submit'>#</button> </TD> <TD> <button name='LED0' class='button3'  value='e' type='submit'>E</button></TD> <TD> <button name='LED0' class='button3' value='f' type='submit'>F</button> </TD> <TD> <button name='LED0' class='button3' value='g' type='submit'>#</button> </TD> <TD> <button name='LED0' class='button3'  value='h' type='submit'>G</button>  </TD><TD> <button name='LED0' class='button3' value='i' type='submit'>#</button> </TD> <TD> <button name='LED0' class='button3' value='j' type='submit'>A</button> </TD> <TD> <button name='LED0' class='button3'  value='k' type='submit'>#</button></TD> <TD> <button name='LED0' class='button3' value='l' type='submit'>B</button> </TD> </TR>\ 
 <TR> <TD> <button name='LED0' class='button4' value='o' type='submit'>C</button> </TD> <TD> <button name='LED0' class='button4'  value='p' type='submit'>#</button></TD> <TD> <button name='LED0' class='button4' value='q' type='submit'>D</button> </TD> <TD> <button name='LED0' class='button4' value='r' type='submit'>#</button> </TD> <TD> <button name='LED0' class='button4'  value='s' type='submit'>E</button></TD> <TD> <button name='LED0' class='button4' value='t' type='submit'>F</button> </TD> <TD> <button name='LED0' class='button4' value='u' type='submit'>#</button> </TD> <TD> <button name='LED0' class='button4'  value='v' type='submit'>G</button>  </TD><TD> <button name='LED0' class='button4' value='w' type='submit'>#</button> </TD> <TD> <button name='LED0' class='button4' value='x' type='submit'>A</button> </TD> <TD> <button name='LED0' class='button4'  value='y' type='submit'>#</button></TD> <TD> <button name='LED0' class='button4' value='z' type='submit'>B</button> </TD> </TR>\ 
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
 <TR> <TD> <button name='LED0' class='button4' value='1' type='submit'>C3</button> </TD> <TD> <button name='LED0' class='button4'  value='2' type='submit'>D3</button></TD> <TD> <button name='LED0' class='button' value='3' type='submit'>E3</button> </TD> </TR> \
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
//-----------------OCTAVE 3------------
if(request.indexOf("LED0=1") != -1) { 
   BB=3;tone(Buzzer, AA=NOTE_C3, 128);
}
if(request.indexOf("LED0=2") != -1){ 
//  digitalWrite(PinB, HIGH);digitalWrite(PinA, HIGH);
//  analogWrite(SpeedA,AA);analogWrite(SpeedB,BB);
   BB=3;tone(Buzzer, AA=NOTE_CS3, 128);
}
if(request.indexOf("LED0=3") != -1){
   BB=3;tone(Buzzer, AA=NOTE_D3, 128);
  
}
if(request.indexOf("LED0=4") != -1){
     BB=3;tone(Buzzer, AA=NOTE_DS3, 128);
}
if(request.indexOf("LED0=5") != -1){
     BB=3;tone(Buzzer, AA=NOTE_E3, 128);
}
if(request.indexOf("LED0=6") != -1){
   BB=3;tone(Buzzer, AA=NOTE_F3, 128);
}
if(request.indexOf("LED0=7") != -1){
    BB=3;tone(Buzzer, AA=NOTE_FS3, 128);
}
if(request.indexOf("LED0=8") != -1){
   BB=3;tone(Buzzer, AA=NOTE_G3, 128);
}
if(request.indexOf("LED0=9") != -1){
    BB=3;tone(Buzzer, AA=NOTE_GS3, 128);
}
if(request.indexOf("LED0=X") != -1){
    BB=3;tone(Buzzer, AA=NOTE_A3, 128);
}
if(request.indexOf("LED0=Y") != -1){
   BB=3;tone(Buzzer, AA=NOTE_AS3, 128);
}
if(request.indexOf("LED0=Z") != -1){
    BB=3;tone(Buzzer, AA=NOTE_B3, 128);
}
//-----------------OCTAVE 4------------
if(request.indexOf("LED0=A") != -1) { 
  BB=4;tone(Buzzer, AA=NOTE_C4, 128);
}
if(request.indexOf("LED0=B") != -1){ 
//  digitalWrite(PinB, HIGH);digitalWrite(PinA, HIGH);
//  analogWrite(SpeedA,AA);analogWrite(SpeedB,BB);
   BB=4;tone(Buzzer, AA=NOTE_CS4, 128);
}
if(request.indexOf("LED0=C") != -1){
  BB=4;tone(Buzzer, AA=NOTE_D4, 128);
  
}
if(request.indexOf("LED0=D") != -1){
     BB=4;tone(Buzzer, AA=NOTE_DS4, 128);
}
if(request.indexOf("LED0=E") != -1){
     BB=4;tone(Buzzer, AA=NOTE_E4, 128);
}
if(request.indexOf("LED0=F") != -1){
   BB=4;tone(Buzzer, AA=NOTE_F4, 128);
}
if(request.indexOf("LED0=G") != -1){
    BB=4;tone(Buzzer, AA=NOTE_FS4, 128);
}
if(request.indexOf("LED0=H") != -1){
   BB=4;tone(Buzzer, AA=NOTE_G4, 128);
}
if(request.indexOf("LED0=I") != -1){
    BB=4;tone(Buzzer, AA=NOTE_GS4, 128);
}
if(request.indexOf("LED0=J") != -1){
    BB=4;tone(Buzzer, AA=NOTE_A4, 128);
}
if(request.indexOf("LED0=K") != -1){
   BB=4;tone(Buzzer, AA=NOTE_AS4, 128);
}
if(request.indexOf("LED0=L") != -1){
    BB=4;tone(Buzzer, AA=NOTE_B4, 128);
}
//---------------------------------- 5 ----------------------------------
if(request.indexOf("LED0=a") != -1) { 
   BB=5;tone(Buzzer, AA=NOTE_C5, 128);
}
if(request.indexOf("LED0=b") != -1){ 
//  digitalWrite(PinB, HIGH);digitalWrite(PinA, HIGH);
//  analogWrite(SpeedA,AA);analogWrite(SpeedB,BB);
   BB=5;tone(Buzzer, AA=NOTE_CS5, 128);
}
if(request.indexOf("LED0=c") != -1){
   BB=5;tone(Buzzer, AA=NOTE_D5, 128);
  
}
if(request.indexOf("LED0=d") != -1){
     BB=5;tone(Buzzer, AA=NOTE_DS5, 128);
}
if(request.indexOf("LED0=e") != -1){
     BB=5;tone(Buzzer, AA=NOTE_E5, 128);
}
if(request.indexOf("LED0=f") != -1){
   BB=5;tone(Buzzer, AA=NOTE_F5, 128);
}
if(request.indexOf("LED0=g") != -1){
    BB=5;tone(Buzzer, AA=NOTE_FS5, 128);
}
if(request.indexOf("LED0=h") != -1){
   BB=5;tone(Buzzer, AA=NOTE_G5, 128);
}
if(request.indexOf("LED0=i") != -1){
    BB=5;tone(Buzzer, AA=NOTE_GS5, 128);
}
if(request.indexOf("LED0=j") != -1){
    BB=5;tone(Buzzer, AA=NOTE_A3, 128);
}
if(request.indexOf("LED0=k") != -1){
   BB=5;tone(Buzzer, AA=NOTE_AS5, 128);
}
if(request.indexOf("LED0=l") != -1){
    BB=5;tone(Buzzer, AA=NOTE_B5, 128);
}
//--------------------6 ----------------------------
if(request.indexOf("LED0=o") != -1) { 
   BB=1;tone(Buzzer, AA=NOTE_C6, 128);
}
if(request.indexOf("LED0=p") != -1){ 
//  digitalWrite(PinB, HIGH);digitalWrite(PinA, HIGH);
//  analogWrite(SpeedA,AA);analogWrite(SpeedB,BB);
   BB=1;tone(Buzzer, AA=NOTE_CS6, 128);
}
if(request.indexOf("LED0=q") != -1){
   BB=1;tone(Buzzer, AA=NOTE_D6, 128);
  
}
if(request.indexOf("LED0=r") != -1){
     BB=1;tone(Buzzer, AA=NOTE_DS6, 128);
}
if(request.indexOf("LED0=s") != -1){
     BB=1;tone(Buzzer, AA=NOTE_E6, 128);
}
if(request.indexOf("LED0=t") != -1){
   BB=1;tone(Buzzer, AA=NOTE_F6, 128);
}
if(request.indexOf("LED0=u") != -1){
    BB=1;tone(Buzzer, AA=NOTE_FS6, 128);
}
if(request.indexOf("LED0=v") != -1){
   BB=1;tone(Buzzer, AA=NOTE_G6, 128);
}
if(request.indexOf("LED0=w") != -1){
    BB=1;tone(Buzzer, AA=NOTE_GS6, 128);
}
if(request.indexOf("LED0=x") != -1){
    BB=1;tone(Buzzer, AA=NOTE_A6, 128);
}
if(request.indexOf("LED0=y") != -1){
   BB=1;tone(Buzzer, AA=NOTE_AS6, 128);
}
if(request.indexOf("LED0=z") != -1){
    BB=1;tone(Buzzer, AA=NOTE_B6, 128);
}

// Affichage fréquence et octave
sprintf(buffer, " %d Hz Octave=%d ", AA,BB);
//Serial.println(buffer);
client.print(html1);
client.print(buffer);
client.print(html2);
// client.print(html);
request="";
}
}
