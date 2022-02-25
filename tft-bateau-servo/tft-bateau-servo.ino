

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
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);



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
int oldcolor, currentcolor, color;

void setup(void) {
  Serial.begin(9600);
  Serial.println(F("Tft Commande Bateau DC switch"));
  tft.reset();
  
  uint16_t identifier = tft.readID();

  Serial.print(F("LCD driver chip: "));
  Serial.println(identifier, HEX);
    

  tft.begin(0x7785);

  tft.fillScreen(BLACK);
// dessine 9 boites de couleur
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, RED+GREEN);
  tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, GREEN);
  
  tft.fillRect(0, BOXSIZE, BOXSIZE, BOXSIZE, MAGENTA);
  tft.fillRect(BOXSIZE, BOXSIZE, BOXSIZE, BOXSIZE, GREEN+BLUE);
  tft.fillRect(BOXSIZE*2, BOXSIZE, BOXSIZE, BOXSIZE, BLUE);
  
  currentcolor = MAGENTA;
  pinMode(13, OUTPUT);
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

void loop()
{
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  // pressure of 0 means no pressing!
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
   mySwitch.enableTransmit(13);
    // scale from 0->1023 to tft.width
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
     color =  p.x  /BOXSIZE ; // 0 à 2
    Serial.print("("); Serial.print(p.x);
    Serial.print(", "); Serial.print(p.y);
    Serial.print(", "); Serial.print(color);
    Serial.println(")");
    
    if (p.y < BOXSIZE) {
 
     switch (color) {
    case 0:
      //do something when var equals 1
      currentcolor = RED; 
      break;
    case 1:
     currentcolor = YELLOW;
      break;
    default: 
     currentcolor = GREEN;
      // default is optional
   
  }

     mySwitch.send( p.x  ,24); 
       } else if(p.y < 2 * BOXSIZE) {  
 // 2° serie de paves

    mySwitch.send( 240 + p.x  , 24); 
     switch (color) {
    case 0:
      //do something when var equals 1
      currentcolor = MAGENTA; 
      break;
    case 1:
     currentcolor = CYAN;
      break;
    default: 
     currentcolor = BLUE;
      // default is optional
  }
  
       }
      tft.fillRect(BOXSIZE, BOXSIZE*3, BOXSIZE, BOXSIZE, currentcolor);
      delay(300);
  } else tft.fillRect(BOXSIZE, BOXSIZE*3, BOXSIZE, BOXSIZE, BLACK);
}
