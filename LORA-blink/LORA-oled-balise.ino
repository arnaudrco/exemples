/*
AFFICHAGE OLED en D4 D5 sans soudure
 * LoRa E220 ARNAUDRCO ajout d'un flash de 10 ms pour chaque reception d'un caractère 
 * suppression pullup 
 * Start device or reset to send a message
 * by Renzo Mischianti <https://mischianti.org>
 * https://mischianti.org/category/my-libraries/lora-e220-llcc68-devices/
 *
 * E220         ----- Wemos D1 mini
 * M0         ----- GND
 * M1         ----- GND
 * TX         ----- PIN D2 
 * RX         ----- PIN D3 
 * AUX        ----- Not connected
 * VCC        ----- 3.3v/5v
 * GND        ----- GND
 *
 */
#include "Arduino.h"
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ------- OLED -------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET   -1
#define PLUS   D4
#define characteres   120 // nombre de charactères max ; il faudrait tenir compte des CR
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Temps entre 2 requêtes (ms)
unsigned long lastQuery = 0;
const unsigned long queryInterval = 3000;
int comptage; // comptage du nombre de messages émis
int ligne ; // ligne pleine

// SoftwareSerial mySerial(D2, D3); // WeMos RX --> e220 TX - WeMos TX --> e220 RX
SoftwareSerial mySerial(D5, D6); // WeMos RX --> e220 TX - WeMos TX --> e220 RX
 
void setup() {

   pinMode(PLUS, OUTPUT);                   
  digitalWrite(PLUS,HIGH );   
 delay(100); 
  
  Serial.begin(115200);
    // I2C OLED sur D2 (SDA) / D1 (SCL)
    Wire.begin(D2, D3);  // GPIO4, GPIO5
//    Wire.begin(D6, D5);  // GPIO4, GPIO5
  delay(500);
  

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(LED_BUILTIN,HIGH );    // turn the LED off by making the voltage LOW

delay(500);




  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED KO !");
    while (true) { delay(1000); }
  }
//  display.setRotation(2); //rotates text on OLED 1=90 degrees, 2=180 degrees
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("ESP8266");
  display.println("Lora");
  display.display();
 
  Serial.println("Hi, I'm going to send message!");
 
  mySerial.begin(9600);
  mySerial.println("Hello, world?");
}
 
void loop() {
  char c;
  if (mySerial.available()) {
    Serial.write(c= mySerial.read());
/*     digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(10);                       // wait for a second
  digitalWrite(LED_BUILTIN,HIGH );    // turn the LED off by making the voltage LOW
*/

 if(ligne++ > characteres){
    display.clearDisplay();
      display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);

  ligne = 0;
 }
  display.print(c);
  display.display();
  
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
      if (millis() - lastQuery > queryInterval) {
    lastQuery = millis();
    mySerial.print(comptage++);mySerial.print(" ");
    if( comptage > 100 ) comptage = 0;// comptage 99 max
  }
}
