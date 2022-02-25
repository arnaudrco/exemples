
#include <Adafruit_GFX.h>    // Core graphics library
#include "Andersmmg_TFTLCD.h" // Hardware-specific library
#include <TouchScreen.h>
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();



#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
//TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);



// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin


Andersmmg_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOXSIZE 80
#define PENRADIUS 3
int value,protocol;

void setup(void) {
  Serial.begin(9600);
  Serial.println(F("Tft Commande Bateau DC switch"));
 tft.reset();
  uint16_t identifier = tft.readID();
  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
 //   return;
 identifier = 0x9341;
}
 tft.begin(identifier);
  tft.fillScreen(BLACK);
   Serial.println(testFillScreen());

  Serial.print(F("Circles (outline)        "));
 // Serial.println(testCircles(10, WHITE));
 testText();
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000
 int i=9;
 
void loop()
{

  pinMode(2,INPUT);
  mySwitch.enableReceive(0);  // Receiver on inerrupt 0 => that is pin #2


  if (mySwitch.available()) {
    if(i==0){
    
     // retour en haut 
        i=10;
           pinMode(2,OUTPUT);
     tft.fillScreen(BLACK);
       tft.setCursor(0, 0);
        pinMode(2,INPUT);
       }
    i--;
      unsigned  currentMillis = millis() << 2;
 //   output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(),mySwitch.getReceivedProtocol());
//  tft.begin(identifier);
  tft.setTextColor( (int) currentMillis);
  value=mySwitch.getReceivedValue();
     Serial.print("Decimal: ");
  Serial.println(value);
  protocol=mySwitch.getReceivedProtocol();
     Serial.print("Protocol: ");
  Serial.println(protocol);
    mySwitch.resetAvailable();
     pinMode(2,OUTPUT);
     tft.setTextSize(2);
          tft.print("Decimal: ");
 tft.println(value);
  tft.print("Protocol: ");
tft.println(protocol);
 
  


}
}

unsigned long testText() {
  tft.fillScreen(BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  tft.setTextSize(1);
  tft.println("Bonjour Arnaud!");
  tft.setTextColor(YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.setTextColor(GREEN);
}
unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(BLACK);
  tft.fillScreen(RED);
  tft.fillScreen(GREEN);
  tft.fillScreen(BLUE);
  tft.fillScreen(BLACK);
  return micros() - start;
}
