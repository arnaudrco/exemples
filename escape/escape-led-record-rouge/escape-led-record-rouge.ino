/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read new NUID from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to the read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the type, and the NUID if a new card has been detected. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino       Esp8266
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST         D2
 * SPI SS      SDA(SS)      10            53        D10        10               10          D1
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16          D7
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14          D6
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15          D5
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 * 
 * A REICHART esp8266 ajouté 
 * 
 * version avec nuid de BOUTEILLE enregistré
 */

#include <SPI.h>
#include <MFRC522.h>
#include <TM1637Display.h>

// Module connection pins (Digital Pins)
#define CLK D3
#define DIO D4

#define LED D0
#define ROUGE D8 // led Rouge pour Arthur

//#define SS_PIN 10
//#define RST_PIN 9

// A MODIFIER SUIVANT NUID 
#define BOUTEILLE1 0xC353
#define BOUTEILLE2 0x518A
#define BOUTEILLE3 0x51FF

#define SS_PIN D1
#define RST_PIN D2

TM1637Display display(CLK, DIO);
uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];

void setup() { 
    pinMode(LED, OUTPUT);
        pinMode(ROUGE, OUTPUT);
      allume();
  Serial.begin(115200);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 


  // All segments on TM 1637
    display.clear();
    display.setBrightness(0x0f);
  display.setSegments(data);
   delay(1000);
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}
 
void loop() {

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

 // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
    
     if (int i=isRecord(rfid.uid.uidByte)){

    Serial.print(F("Bouteille n "));
    Serial.println(i);
    allume();

     } else {
          allume_rouge();
     }
  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("A new card has been detected."));

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
   
    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
//    Serial.print(F("In dec: "));
//    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
  }
  else Serial.println(F("Card read previously."));

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}


/**
 *  A COMPLETER 
    si le nuid de la bouteille n est enregistré isRecord retourne le nombre n 
 */
 int isRecord( byte *buffer ){ 
    unsigned long k;
    k= buffer[0] * 256 + buffer[1] ;
 //       k= 65536 * k + buffer[2] * 256 + buffer[3]; // VERSION 8 chiffres si besoin 
    switch (k){
          case BOUTEILLE1: // nuid de la bouteille 1
          return(1);
      // statements
      break;

    case BOUTEILLE2:
      // statements
       return(2);
      break;

       case BOUTEILLE3:
          return(3);
      // statements
      break;

      /* A COMPLETER POUR n BOUTEILLES */
      
    }
    return(0);

    
  
 }

void printHex(byte *buffer, byte bufferSize) {
    display.clear();
     if (int i=isRecord(buffer)){

    Serial.print(F("Bouteille n "));
    Serial.println(i);
    allume();
         
  display.showNumberDec(i, false);
    delay(1000);
 }
  display.showNumberHexEx(buffer[0] * 256 + buffer[1] );        // Expect: f1Af

  
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void allume(){ // -------------Allume une led----------------------

    digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);   

}
void allume_rouge(){ // -------------Allume une led rouge----------------------


      digitalWrite(ROUGE, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(ROUGE, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
