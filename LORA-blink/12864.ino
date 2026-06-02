/*Pour le ST7567A en mode SPI avec ESP8266 :

Connexions :
- ESP8266 | ST7567A
- D5 (GPIO14) | SCK
- D7 (GPIO13) | SDA (MOSI)
- D1 (GPIO5) | RST
- D2 (GPIO4) | CS
- D0 (GPIO16) | A0 (RS)
- 3V3 | VCC
- GND | GND

*/


#include <U8g2lib.h>

U8G2_ST7567_ENH_DG128064_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 14, /* data=*/ 13, /* cs=*/ 4, /* dc=*/ 16, /* reset=*/ 5);

void setup() {
  u8g2.begin();
 // u8g2.setContrast(50); // Contraste (0-255)
 // u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFont(u8g2_font_fub20_tf);
// u8g2.setFont(u8g2_font_nlogis12_tf);
  u8g2.setFontMode(0);
}

void loop() {
  u8g2.clearBuffer();
  u8g2.drawStr(10, 30, "ANUMBY");
  u8g2.sendBuffer();
  delay(1000);
}
