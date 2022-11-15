/*
   ______               _                  _///_ _ _                   _
  /   _  \             (_)                |  ___| |         | |                 (_)
  |  [_|  |__  ___  ___ _  ___  _ __      | |__ | | ___  ___| |_ _ __ ___  _ __  _  ___  _   _  ___
  |   ___/ _ \| __|| __| |/ _ \| '_ \_____|  __|| |/ _ \/  _|  _| '__/   \| '_ \| |/   \| | | |/ _ \
  |  |  | ( ) |__ ||__ | | ( ) | | | |____| |__ | |  __/| (_| |_| | | (_) | | | | | (_) | |_| |  __/
  \__|   \__,_|___||___|_|\___/|_| [_| \____/|_|\___|\____\__\_|  \___/|_| |_|_|\__  |\__,_|\___|
| |
\_|
  Fichier :       prgTestLedPiloteeEnPwmViaPotentiometre.ino

  Description :   Permet de faire varier la luminosité d'une LED avec un signal PWM 0-5V,
                  via un potentiomètre branché sur l'entrée analogique d'un Arduino Nano

  Auteur :        Jérôme TOMSKI (https://passionelectronique.fr/)
  Créé le :       10.11.2021

  A REICHART
  version esp

*/

#define pinOuEstBrancheLePotentiometre  A0       // Le potentiomètre servant à faire varier la luminosité de la LED sera branché sur l'entrée A0 de l'Arduino Nano
#define pinOuEstBrancheLaLED            D1 // La LED sera quant à elle branchée sur la sortie D3 de l'Arduino Nano (attention : toutes les sorties ne permettent pas de générer un signal PWM)

int valeurTensionEntreeAnalogique;              // Variable qui contiendra la valeur de la tension mesurée sur l'entrée analogique (valeur comprise entre 0 et 1023, car lecture sur 10 bits)


// ========================
// Initialisation programme
// ========================
void setup()
{
  Serial.begin(115200);
  // Définition de la broche où sera branché la LED en sortie
  pinMode(pinOuEstBrancheLePotentiometre, OUTPUT);

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

  valeurTensionEntreeAnalogique = analogRead(pinOuEstBrancheLePotentiometre);

  // *****************************************************************************************************************************
  // Génération du signal PWM
  // *****************************************************************************************************************************

  analogWrite(pinOuEstBrancheLaLED, valeurTensionEntreeAnalogique);

} 
