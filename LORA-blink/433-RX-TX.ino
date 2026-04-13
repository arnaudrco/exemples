// ArnaudRCO https://github.com/arnaudrco/exemples/wiki/LORA-pour-les-pros
// avec un petit affichage ; montage sur LORA-02 (LX1262) très économique en 433 MHz

#include <SPI.h>
#include <LoRa.h>

// #define NSS 15
#define NSS D0
// #define RST 4
#define RST D8
#define DIO0 5


// ------- OLED -------
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET   -1
#define PLUS   D4
#define LIGNES   5 // nombre de lignes  sur écran
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int ligne ; // lignes pleines

// Temps entre 2 requêtes (ms)
unsigned long lastQuery = 0;
const unsigned long queryInterval = 3000;
int comptage; // comptage du nombre de messages émis

void setup() {

  
   pinMode(PLUS, OUTPUT);                   
  digitalWrite(PLUS,HIGH );   
 delay(400); 
  
  Serial.begin(115200);
    // I2C OLED sur D2 (SDA) / D1 (SCL)
    Wire.begin(D2, D3);  // GPIO4, GPIO5

  
  while (!Serial);
  
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
  display.println("LORA");
  display.println("433MHz");
  display.display();
  
  LoRa.setPins(NSS, RST, DIO0);

  LoRa.setTxPower(20);     // Puissance 20dBm
LoRa.setSpreadingFactor(7);  // SF7 (équilibre vitesse/portée)
LoRa.setSignalBandwidth(125E3); // 125kHz
  
  while (!LoRa.begin(433E6)) {
    Serial.println("LoRa init échoué!");

  }
    Serial.println("");
  Serial.println("LoRa RÉCEPTEUR prêt");
       display.println("Balise LoRa");
  display.display();
  
  String message = "Arnaud";
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();
}


void loop() {
  char c;
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Reçu: ");
    while (LoRa.available()) {
      Serial.print(c=LoRa.read());
       display.print(c);
  display.display();
    }
    Serial.println();
    if(ligne++ > LIGNES ){ // LIGNES MAX
    display.clearDisplay();
      display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);

  ligne = 0;
 }
      display.println("");
      display.display();
  }

      if (millis() - lastQuery > queryInterval) {
        
        String message = String(comptage++);
          LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();
   Serial.println(comptage);

    if( comptage > 99 ) comptage = 0;// comptage 99 max
    lastQuery = millis();
  }
    
 
}
