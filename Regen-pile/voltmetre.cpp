#include <Arduino.h>

// ===== CONFIGURATION AFFICHEUR =====
// Segments : A, B, C, D, E, F, G, DP
const int segmentPins[] = {PB4, PD3, PC3, PC5, PC6, PD2, PD4, PC4};

// Anodes communes des 3 chiffres
const int digitPins[] = {PA1, PA2, PA3};

// Encodage 7-segments pour 0-9
const byte digitPatterns[] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};

// ===== CONFIGURATION ADC =====

const int ADC_PIN = PD5; // Broche d'entrée analogique
const float MAX_VOLTAGE = 15.0; // Tension maximale à mesurer
const float ADC_REFERENCE = 3.3; // Tension de référence STM8S003
const int ADC_RESOLUTION = 1024; // Résolution 10-bit

// R1 = 10kΩ, R2 = 2.2kΩ
// Divisor ratio = 2165 / (9880 + 2200) = 0.179742
const float DIVISOR_RATIO = 0.179742;
const float ADC_COEFFICIENT = 0.017929; //(ADC_REFERENCE / ADC_RESOLUTION) / DIVISOR_RATIO;

int dispDigits[3] = {0, 0, 0};
bool dispDots[3] = {false, false, false};
int muxIndex = 0;
unsigned long muxLastTime = 0;
const unsigned long MUX_INTERVAL_MS = 2;
unsigned long voltageLastTime = 0;
const unsigned long VOLTAGE_INTERVAL_MS = 1000;

void setup() {
  // Configurer les broches des segments en sortie
  for (int i = 0; i < 8; i++) {
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], HIGH);
  }
  
  // Configurer les broches des anodes communes
  for (int i = 0; i < 3; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], LOW);
  }
  
  // Configurer l'entrée analogique
  pinMode(ADC_PIN, INPUT);
}

void displayDigit(int digitPosition, int number, bool decimalPoint) {
  // Préparer le pattern puis activer uniquement le digit demandé
  byte pattern = digitPatterns[number % 10];
  if (decimalPoint) {
    pattern |= 0x80; // activer le point décimal
  }
  for (int seg = 0; seg < 8; seg++) {
    digitalWrite(segmentPins[seg], (pattern & (1 << seg)) ? LOW : HIGH);
  }
  digitalWrite(digitPins[digitPosition], HIGH);
}

void refreshDisplay() {
  unsigned long now = millis();
  if (now - muxLastTime < MUX_INTERVAL_MS) {
    return;
  }
  muxLastTime = now;
  for (int i = 0; i < 3; i++) {
    digitalWrite(digitPins[i], LOW);
  }
  displayDigit(muxIndex, dispDigits[muxIndex], dispDots[muxIndex]);
  muxIndex = (muxIndex + 1) % 3;
}

void updateDisplayDigits(float voltage) {
  // Limiter à 2 décimales
  voltage = constrain(voltage, 0.0, 15.9);
  if (voltage < 10.0) {
    int displayValue = (int)(voltage * 100 + 0.5);
    dispDigits[0] = (displayValue / 100) % 10; // Unités
    dispDigits[1] = (displayValue / 10) % 10;  // Décimales (0.1)
    dispDigits[2] = displayValue % 10;         // Centièmes (0.01)
    dispDots[0] = true;
    dispDots[1] = false;
    dispDots[2] = false;
  } else {
    int displayValue = (int)(voltage * 10 + 0.5);
    dispDigits[0] = (displayValue / 100) % 10; // dizaine
    dispDigits[1] = (displayValue / 10) % 10;  // unité
    dispDigits[2] = displayValue % 10;         // décimale
    dispDots[0] = false;
    dispDots[1] = true;
    dispDots[2] = false;
  }
}

float readVoltage();

float readVoltageSmoothed() {
  float sum = 0;
  for (int i = 0; i < 4; i++) {
    sum += readVoltage();
  }
  return sum / 4.0;
}

float readVoltage() {
  // Lire la valeur brute de l'ADC (0-1023)
  int rawValue = analogRead(ADC_PIN);
  
  // Convertir en tension réelle
  float voltage = rawValue * ADC_COEFFICIENT;
  
  return voltage;
}

void loop() {
  // Lire et afficher la tension
  unsigned long now = millis();
  if (now - voltageLastTime >= VOLTAGE_INTERVAL_MS) {
    voltageLastTime = now;
    float voltage = readVoltageSmoothed();
    updateDisplayDigits(voltage);
  }
  refreshDisplay();
}
