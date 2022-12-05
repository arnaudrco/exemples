/*

  A REICHART
  version arduino
  
  (attention : toutes les sorties ne permettent pas de générer un signal PWM)

*/

#define pinOuEstBrancheLePotentiometre  A0       // Le potentiomètre servant à faire varier la luminosité de la LED sera branché sur l'entrée A0 de l'Arduino Nano
#define pinOuEstBrancheLaLED            3 // La LED sera quant à elle branchée sur la sortie D3 de l'Arduino Nano 


int valeurTensionEntreeAnalogique;              // Variable qui contiendra la valeur de la tension mesurée sur l'entrée analogique (valeur comprise entre 0 et 1023, car lecture sur 10 bits)
int valeurRaccordCycliqueSignalPwm;             // Variable qui contiendra la valeur du rapport cyclique du signal PWM à générer

// ========================
// Initialisation programme
// ========================
void setup()
{
    Serial.begin(115200);
    Serial.println("Tracker solaire : PhotoVoltaique.");

  // Définition de la broche où sera branché la LED en sortie
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  pinMode(valeurTensionEntreeAnalogique, INPUT);
  // Nota : pas besoin de déclarer l'entrée analogique en entrée, car c'est sous entendu, par défaut

}

// =================
// Boucle principale
// =================
void loop()
{

  // *****************************************************************************************************************************
  // Lecture de la tension présente sur l'entrée analogique, où est branché le potentiomètre (son "point milieu", plus exactement)
  // *****************************************************************************************************************************
  // Pour rappel : la valeur retournée sera comprise entre 0 et 1023, car il s'agit là d'une lecture sur 10 bits (0 correspondant à 0V, et 1023 à +5V)

  valeurTensionEntreeAnalogique = analogRead(pinOuEstBrancheLePotentiometre)/ 3; // le potentiomètre est relié sur 3.3 V de référence

  // *****************************************************************************************************************************
  // Conversion tension -> rapport cyclique
  // *****************************************************************************************************************************
  // Comme la valeur mesurée sur l'entrée analogique sera exprimée sous la forme d'un nombre compris entre 0 et 1023,
  // et que la valeur du rapport cyclique à renseigner dans le code arduino devra être comprise entre 0 et 255,
  // alors il faut convertir les mesures 0-1023 en valeur 0-255


  // *****************************************************************************************************************************
  // Génération du signal PWM
  // *****************************************************************************************************************************
          Serial.println(valeurTensionEntreeAnalogique );
  analogWrite(pinOuEstBrancheLaLED, valeurTensionEntreeAnalogique);
    delay(100); 

} 
