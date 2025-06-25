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
#include <EEPROM.h>






#define SS_PIN D4
#define RST_PIN D8

#define BOOT D3
#define GND D1
#define PLUS D0
#define PIN D2 // On Trinket or Gemma, suggest changing this to 1




#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 1 // Popular NeoPixel ring size
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
MFRC522 rfid(SS_PIN,RST_PIN ); // Instance of the class

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];
byte flag_programmation = 0 ;
unsigned long nuid = 0; 

void setup() { 
  EEPROM.begin(4);// 4 octets
 pinMode(BOOT, INPUT_PULLUP);
                pinMode(GND, OUTPUT);
                    digitalWrite(GND, LOW); 
                    pinMode(PLUS, OUTPUT);
                    digitalWrite(PLUS, HIGH); 

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear(); // Set all pixel colors to 'off'


  Serial.begin(115200);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  allume();
    if(digitalRead(BOOT)==LOW) {
      allume_blanc();
      flag_programmation = 1 ; // programmation ID
    }   
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);

  EEPROM.get(0,nuid); // lecture de nuid
              Serial.print("lecture de nuid =");
              Serial.println(nuid);
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
  
  }
  else {
    
    Serial.println(F("Card read previously."));
        printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
//    Serial.print(F("In dec: "));
//    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
     if (int i=isRecord(rfid.uid.uidByte)){
      Serial.print(F("Bouteille n "));
      Serial.println(i);
      allume_vert();

     } else {
          allume_rouge();
     }
  }

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
    k=  k * 65536 + buffer[2] * 256 + buffer[3]; // VERSION 8 chiffres si besoin 
       if (k== 0xFFFFFFFF ) return(0);
    if(flag_programmation){
        EEPROM.put(0, k); // ecriture de boardId
        EEPROM.commit();
        flag_programmation = 0;
        nuid = k;
        Serial.println("Identifiant lu");
        return(1);
    }

    if ( k == nuid) return(1);
    return(0);

    
  
 }

void printHex(byte *buffer, byte bufferSize) {
 //   display.clear();
     if (int i=isRecord(buffer)){

    Serial.print(F("Bouteille n "));
    Serial.println(i);

         
//  display.showNumberDec(i, false);
    delay(1000);
 }
 // display.showNumberHexEx(buffer[0] * 256 + buffer[1] );        // Expect: f1Af

  
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void allume(){ // -------------Allume une led en bleu ----------------------

   pixels.setPixelColor(0, pixels.Color(0, 0, 150)); // vert rouge bleu

    pixels.show();   // Send the updated pixel colors to the hardware.

      delay(500) ;              // wait for a second
 

}

void allume_blanc(){ // -------------Allume led en blanc ----------------------

   pixels.setPixelColor(0, pixels.Color(150, 150, 150)); // vert rouge bleu

    pixels.show();   // Send the updated pixel colors to the hardware.
 
}
void allume_vert(){ // -------------Allume une led----------------------

     pixels.setPixelColor(0, pixels.Color(150, 0, 0));

    pixels.show();   // Send the updated pixel colors to the hardware.


}
void allume_rouge(){ // -------------Allume une led rouge----------------------

    pixels.setPixelColor(0, pixels.Color(0, 0,0));

    pixels.show();   // Send the updated pixel colors to the hardwar

    delay(100);
    pixels.setPixelColor(0, pixels.Color(0, 150,0));

    pixels.show();   // Send the updated pixel colors to the hardware.

}
