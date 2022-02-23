// Generátor funkcí AD9833

// připojení potřebné knihovny
#include <AD9833.h>
// definice propojovacího pinu FSYNC
#define pinFsync A4
#define pinSwitch 3
#define SAMPLE 1000 

// inicializace modulu z knihovny s nastavením pinu FSYNC
AD9833 gen(pinFsync);
int ff;// choix des fréquences
int    tt=0;// triangle ou rectangle
void setup() {
  // zahájení komunikace po sériové lince
  // rychlostí 9600 baud
   ff= analogRead(A0) /100;

  pinMode(pinSwitch, INPUT_PULLUP);  
     Serial.begin(9600);
     Serial.print("AD9833 0 : 10KHz 1 : 100 KHz 2 : 1MHz 3 : 10MHz ");
      Serial.println(ff);

  // zahájení komunikace s modulem AD9833
  gen.Begin();
  // zapnutí výstupu s výchozím signálem
  // typu SINUS s frekvencí 1 kHz
  Serial.println("Start programu, sinus 1000 Hz");
  gen.EnableOutput(true);
  delay(1000);
  // nastavení trojúhelníkového signálu
  // o frekvenci 2000 Hz do REG1
  // a jeho zapnutí na výstup
  Serial.println("Trojuhelnik 2000 Hz");
  gen.ApplySignal(TRIANGLE_WAVE, REG1, 2000);
  delay(1000);
  // změna frekvence v REG1 na 1000 Hz
  Serial.println("Zmena trojuhelniku na 1000 Hz");
  gen.SetFrequency(REG1, 1000);
  delay(100);
  // přepnutí zpět na REG0
  Serial.println("Prepnuti zpet na 1000 Hz sinus");
  gen.SetOutputSource(REG0);
  delay(100);
  // nastavení REG0 na obdélníkový signál
  // o frekvenci 1500 Hz
  Serial.println("Zmena na obdelnik 1500 Hz");
  gen.SetWaveform(REG0, SQUARE_WAVE);
  gen.SetFrequency(REG0, 1500);
  delay(100);
  // nastavení REG0 na půlobdélníkový signál
  // o frekvenci 1000 Hz
  Serial.println("Zmena na pul-obdelnik 1000 Hz");
  gen.SetWaveform(REG0, HALF_SQUARE_WAVE);
  gen.SetFrequency(REG0, 1000);
  delay(100);
  // nastavení REG0 na sinusový signál
  // o frekvenci 1000 Hz
  Serial.println("Zmena na sinus 1000 Hz");
  gen.SetWaveform(REG0, SINE_WAVE);
  delay(100);
  // postupné zvyšování fázového posuvu od 0 do 180 stupňů
  Serial.println("Zvysovani frekvence z 1000 na 4000 Hz");
  for (int i = 0; i < 30; i++) {
    gen.IncrementFrequency(REG0, 100);
    delay(10);
  }
  delay(100);
  // nastavení fázového posuvu na 180 stupňů
  Serial.println("Nastaveni fazoveho posuvu na 180 stupnu");
  gen.SetPhase(REG1, 180);
  delay(1000);
  // reset modulu pro získání výchozího stavu
  Serial.println("Reset modulu");
  gen.Reset();

}

void loop() {
  unsigned long  a= analogRead(A0);
  for (int i=0; i < SAMPLE; i++){ a = a+ analogRead(A0); }
  a = a * 100 / SAMPLE/ 1022 ; // moyenne de 1023 vers 100
  if(a>100) a=100; // seuil à 100
   unsigned long f;

if (digitalRead(pinSwitch)==0) tt=1-tt;// toggle triangle
    switch(ff) {
    case 0 : f= 100 * a; break;
    case 1 : f=1000 *  a; break;
    case 2 : f= 10000 * a ; break;
      case 3 : f= 100000 * a ; break;
    default: f= 1000 * map( a,0,100, 5000,30000); break; // 5 MHz à 30 MHz max
    }
    
   if(f<30) f=20; // minimum
         Serial.println(f);
   //   gen.ApplySignal(TRIANGLE_WAVE,REG0,f);
 if(!tt) gen.ApplySignal(SQUARE_WAVE,REG0,f); else gen.ApplySignal(TRIANGLE_WAVE,REG0,f);
      

    delay(200);
}
