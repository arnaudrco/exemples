
/* 
  TFmini-Plus + WS2812B on Arduino.
  JP CARIOU JAN 2024
  For Arduino boards with only one serial port like UNO board, the function of software visual serial port is to be used. 
*/
#include <SoftwareSerial.h>  //header file of software serial port
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#define maxDistance 1200  // max distance (cm)
#define DELAYVAL 100  // Time (in milliseconds) to pause between pixels
#define PIN 6         // pin on the Arduino is connected to the NeoPixels
#define NUMPIXELS 64  // Popular NeoPixel ring size
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
SoftwareSerial Serial1(2, 3);  //define software serial port name as Serial1 and define pin2 as RX and pin3 as TX
int dist;      //actual distance measurements of LiDAR
int strength;  //signal strength of LiDAR
float temperature;
int check;  //save check value
int i;
int uart[9];              //save data measured by LiDAR
const int HEADER = 0x59;  //frame header of data package
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
void setup() {
  pixels.begin();         // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear();         // Set all pixel colors to 'off'
  Serial.begin(9600);     //set bit rate of serial port connecting Arduino with computer
  Serial1.begin(115200);  //set bit rate of serial port connecting LiDAR with Arduino
  Wire.begin();
}
void loop() {
  int distance = 0;
  if (Serial1.available()) {         //check if serial port has data input
    if (Serial1.read() == HEADER) {  //assess data package frame header 0x59
      uart[0] = HEADER;
      if (Serial1.read() == HEADER) {  //assess data package frame header 0x59
        uart[1] = HEADER;
        for (i = 2; i < 9; i++) {  //save data in array
          uart[i] = Serial1.read();
        }
        check = uart[0] + uart[1] + uart[2] + uart[3] + uart[4] + uart[5] + uart[6] + uart[7];
        if (uart[8] == (check & 0xff)) {          //verify the received data as per protocol
          distance = uart[2] + uart[3] * 256;     //calculate distance value
          strength = uart[4] + uart[5] * 256;     //calculate signal strength value
          temperature = uart[6] + uart[7] * 256;  //calculate chip temperature
          temperature = temperature / 8 - 256;
          Serial.print("dist = ");
          Serial.print(distance);  //output measure distance value of LiDAR
          Serial.print('\t');
          Serial.print("strength = ");
          Serial.print(strength);  //output signal strength value
          Serial.print("\t Chip Temperature = ");
          Serial.print(temperature);
          Serial.println(" celsius degree");  //output chip temperature of Lidar
          Serial.println();
          pixels.clear();  // Set all pixel colors to 'off'
          pixels.show();   // Send the updated pixel colors to the hardware.
          if (distance < 150) {
            pixels.clear();   // Set all pixel colors to 'off'
            pixels.show();    // Send the updated pixel colors to the hardware.
            delay(DELAYVAL);  // Pause before next pass through loop
            // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
            int lumR = 250;
            int lumR2 = 250;
            pixels.setPixelColor(0, pixels.Color(lumR2, 0, 0));
            pixels.setPixelColor(1, pixels.Color(lumR2, 0, 0));
            pixels.setPixelColor(2, pixels.Color(lumR2, 0, 0));
            pixels.setPixelColor(3, pixels.Color(lumR2, 0, 0));
            pixels.setPixelColor(4, pixels.Color(lumR2, 0, 0));
            pixels.setPixelColor(5, pixels.Color(lumR2, 0, 0));
            pixels.setPixelColor(6, pixels.Color(lumR2, 0, 0));
            pixels.setPixelColor(7, pixels.Color(lumR2, 0, 0));
            pixels.setPixelColor(8, pixels.Color(lumR2, 0, 0));
            pixels.setPixelColor(9, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(10, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(11, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(12, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(13, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(14, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(15, pixels.Color(lumR2, 0, 0));
            pixels.setPixelColor(17, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(18, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(19, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(20, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(21, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(26, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(22, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(25, pixels.Color(lumR2, 0, 0));
            pixels.setPixelColor(26, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(26, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(27, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(28, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(29, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(30, pixels.Color(lumR2, 0, 0));
            pixels.setPixelColor(34, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(35, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(36, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(37, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(42, pixels.Color(lumR2, 0, 0));
            pixels.setPixelColor(43, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(44, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(45, pixels.Color(lumR2, 0, 0));
            pixels.setPixelColor(51, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(52, pixels.Color(lumR, 0, 0));
            pixels.setPixelColor(59, pixels.Color(lumR2, 0, 0));
            pixels.setPixelColor(60, pixels.Color(lumR2, 0, 0));
          } else {
            if (distance < 250) {
              pixels.clear();   // Set all pixel colors to 'off'
              pixels.show();    // Send the updated pixel colors to the hardware.
              delay(DELAYVAL);  // Pause before next pass through loop
              // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
              int lumR = 20;
              int lumR2 = 20;
              pixels.setPixelColor(0, pixels.Color(lumR2, lumR2, 0));
              pixels.setPixelColor(1, pixels.Color(lumR2, lumR2, 0));
              pixels.setPixelColor(2, pixels.Color(lumR2, lumR2, 0));
              pixels.setPixelColor(3, pixels.Color(lumR2, lumR2, 0));
              pixels.setPixelColor(4, pixels.Color(lumR2, lumR2, 0));
              pixels.setPixelColor(5, pixels.Color(lumR2, lumR2, 0));
              pixels.setPixelColor(6, pixels.Color(lumR2, lumR2, 0));
              pixels.setPixelColor(7, pixels.Color(lumR2, lumR2, 0));
              pixels.setPixelColor(8, pixels.Color(lumR2, lumR2, 0));
              pixels.setPixelColor(15, pixels.Color(lumR2, lumR2, 0));
              pixels.setPixelColor(17, pixels.Color(lumR, lumR2, 0));
              pixels.setPixelColor(22, pixels.Color(lumR, lumR2, 0));
              pixels.setPixelColor(25, pixels.Color(lumR2, lumR2, 0));
              pixels.setPixelColor(30, pixels.Color(lumR2, lumR2, 0));
              pixels.setPixelColor(34, pixels.Color(lumR, lumR2, 0));
              pixels.setPixelColor(37, pixels.Color(lumR, lumR2, 0));
              pixels.setPixelColor(42, pixels.Color(lumR2, lumR2, 0));
              pixels.setPixelColor(45, pixels.Color(lumR2, lumR2, 0));
              pixels.setPixelColor(51, pixels.Color(lumR, lumR2, 0));
              pixels.setPixelColor(52, pixels.Color(lumR, lumR2, 0));
              pixels.setPixelColor(59, pixels.Color(lumR2, lumR2, 0));
              pixels.setPixelColor(60, pixels.Color(lumR2, lumR2, 0));
            } else {
              int lumR = 10;
              int lumR2 = 10;
              pixels.setPixelColor(3, pixels.Color(0, lumR2, 0));
              pixels.setPixelColor(4, pixels.Color(0, lumR2, 0));
              pixels.setPixelColor(10, pixels.Color(0, lumR2, 0));
              pixels.setPixelColor(13, pixels.Color(0, lumR2, 0));
              pixels.setPixelColor(22, pixels.Color(0, lumR2, 0));
              pixels.setPixelColor(17, pixels.Color(0, lumR2, 0));
              pixels.setPixelColor(24, pixels.Color(0, lumR2, 0));
              pixels.setPixelColor(31, pixels.Color(0, lumR2, 0));
              pixels.setPixelColor(32, pixels.Color(0, lumR2, 0));
              pixels.setPixelColor(39, pixels.Color(0, lumR2, 0));
              pixels.setPixelColor(41, pixels.Color(0, lumR2, 0));
              pixels.setPixelColor(46, pixels.Color(0, lumR2, 0));
              pixels.setPixelColor(50, pixels.Color(0, lumR2, 0));
              pixels.setPixelColor(53, pixels.Color(0, lumR2, 0));
              pixels.setPixelColor(59, pixels.Color(0, lumR2, 0));
              pixels.setPixelColor(60, pixels.Color(0, lumR2, 0));
            }}
            pixels.show();    // Send the updated pixel colors to the hardware.
            delay(DELAYVAL);  // Pause before next pass through loop

            delay(10);
          }
        }
      }
    }
  }


