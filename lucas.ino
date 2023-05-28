int capteur = 2;
int detection = 0;
int lamp = 3;
int buzzer = 4;

void setup() {
  // put your setup code here, to run once:
  
  pinMode(capteur, INPUT);
  pinMode(lamp, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  detection = digitalRead(capteur);
  if (detection == HIGH){
    Serial.print("Mouvement\n");
    digitalWrite(lamp, HIGH);
    tone(buzzer, 600); // allume le buzzer actif arduino
    delay(500);
    tone(buzzer, 900); // allume le buzzer actif arduino
    delay(500);
    noTone(buzzer);  // désactiver le buzzer actif arduino
    delay(500);
  }else{
    Serial.print("Pas de mouvement \n");
    digitalWrite(lamp, LOW);
  }
}
