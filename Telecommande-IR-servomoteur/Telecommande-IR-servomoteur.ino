/*
  telecommande one line mode VCR

  flèches droite haut gauche

  */


#include <IRremote.h>
#include <Servo.h>
    
//---------SERVO-------------
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

int led = 13;    // LED connected to digital pin 9
int RECV_PIN = 9;
IRrecv irrecv(RECV_PIN);

decode_results results;

void setup() {
  // initialize the serial port:
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  pinMode(led, OUTPUT);
  myservo.attach(10);  // attaches the servo on pin 2 to the servo object
}
void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    switch (results.value) {
      case 0x10E7C23D : /* POWER */
        Serial.print("Decoded POWER");
        digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
        break; 
  //       ligne << > >>
      case 0x10E7B847  : /* << */
        Serial.print("Decoded <<");
        digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
        myservo.write(135);
        break;
      case 0x10E728D7: /* > */
        myservo.write(90);
        digitalWrite(led, LOW); 
        break;
      case 0x10E78877 : /* >> */
        myservo.write(45);
        digitalWrite(led, LOW); 
        break;
    }
    irrecv.resume(); // Receive the next value
  }



}
