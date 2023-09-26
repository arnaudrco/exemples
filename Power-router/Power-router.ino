/*

Power_Router est un système qui permet d'utiliser l'excédent d'énergie autoproduit par 
l'allumage d'un appareil résistif (facteur de puissance proche de 1) ce qui évite l'injection 
au réseau public de distribution d'électricité.

Le principe de fonctionnement est le suivant :
- détection de phase entre courant et tension permet de savoir si on consomme ou bien on injecte
- en cas d'injection il se produit la mise en route progressive d'un dispositif d'absorption 
d'excédent de puissance 
- la mesure du courant permet d'ajuster au mieux le niveau d'absorption de cet excédent.
- Par ailleurs il est prévu une sortie temporisée de 30 secondes (paramétrable) lorsque le seuil 
d'injection est proche de 3W (paramétrable) permettant par exemple de couper l'injection d'une 
éolienne au profit de la charge de batteries. 

le programme prévoit :
- une sonde de tension : simple transfo 230V/5V Crête à Crête sur mi-tension (2.5V)
- une sonde de courant : 20A/25mA sur mi-tension (2.5V)
- un module de commande par triac
- un dispositif de détection de passage à zéro de la sinusoïde de tension secteur (par exemple 
l'optocoupleur H11AA1)
- la bibliothèque TimeOne.h à installer et disponible là : 
http://www.arduino.cc/playground/Code/Timer
- en option un afficheur LCD 1602 avec extension I2C

La gamme de puissances testée est va de 300 à 1000W

merci à Ryan McLaughlin <ryanjmclaughlin@gmail.com> pour avoir étudié et mis au point la partie 
commande du triac il y a quelques années et que j'ai repris dans ce programme :)
source : https://web.archive.org/web/20091212193047/http://www.arduino.cc:80/cgi-bin/yabb2/YaBB.pl?num=1230333861/15

_________________________________________________________________
|                                                               |
|       auteur : Philippe de Craene <dcphilippe@yahoo.fr        |
|           pour l' Association P'TITWATT                       |
_________________________________________________________________

Toute contribution en vue de l’amélioration de l’appareil est la bienvenue ! Il vous est juste
demandé de conserver mon nom et mon email dans l’entête du programme, et bien sûr de partager 
avec moi cette amélioration. Merci.

Récapitulatif des branchements :
 - broche A0 (analogique 0) => sonde de courant
 - broche A1 (analogique 1) => sonde de tension
 - broche A4 (analogique 4) => broche SDA sur LCD 
 - broche A5 (analogique 5) => broche SCL sur LCD
 - broche  2 (numérique 2)  => broche ZC (zero-cross) du module triac
 - broche  3 (numérique 3)  => LED indiquant le taux de pwm
 - broche 10 (numérique 10) => broche PWM (commande Triac) du module triac
 - brcohe 11 (numérique 11) => circuit de commande du délestage
 - broche 13 (numérique 13) => LED indiquant l'overflow

Chronologie des versions :
version 0.5 - 3 mai 2018     - boucle de décrémentation byDim --
version 0.8 - 5 juil. 2018   - 1ère version fonctionnelle, pb du pic de courant du triac 
version 1   - 6 juil. 2018   - ajout de la bibliothèque EmonLib.h pour mesure du secteur
version 1.4 - 7 juil. 2018   - simplification des tests sur sPower et byDim.
version 1.6 - 8 juil. 2018   - ajout LED d'overflow + optimisation des paramètres + iSeuilPoff
version 1.8 - 24 sept 2018   - ajout du pas variable sur byDim avec iDimStep
version 1.9 - 12 oct. 2018   - ajout d'une sortie temporisée de 5min à iSeuilPoff (25W) du seuil d'injection  
version 2.0 - 4 nov. 2018    - ajout d'un watchdog avec comptage de reset en EEPROM
version 2.2 - 7 nov. 2018    - iSeuilPtempo variable à part entière pour le délestage + correction coquille
version 2.3 - 16 dec 2018    - réaménagemet des messages console pour gagner du temps
version 2.4 - 12 jan 2019    - ajout d'un afficheur LCD 1602 avec extension I2C
version 3.2 - 17 jan 2019    - gain en performances en contournant EmonLib.h
version 3.2.4 - 1 mai 2020   - application filtre digital pass bas 

*/

#include <EEPROM.h>
#include <avr/wdt.h>    // documentation : https://tushev.org/articles/arduino/5/arduino-and-watchdog-timer
#include <TimerOne.h>   // librairie à installer depuis http://www.arduino.cc/playground/Code/Timer1
#include <LiquidCrystal_I2C.h> // librairie à installer depuis https://app.box.com/s/czde88f5b9vpulhf8z56

// Attention ! Renommer la librairie d'origine de l'IDE LiquidCrystal.h et LiquidCrystal.cpp

//  Affiche les mesures à la console cependant au détriment des performances
const bool bVerbose = false;       // pour le debugging
const bool bCalibration = false;   // pour la calibration

//
// Variables de calibration :
// Calibration des mesures des sondes pour avoir des valeurs correctes :
// en premier : fVCalibration doit être défini pour obtenir les 230V du secteur
// ensuite : fICalibration en comparant l'affichage de la console avec celui d’un wattmètre 
// pour s'approcher de la réalité. En l'absence de wattmètre effectuer le test avec une 
// charge résistive connue, par exemple une ampoule halogène neuve.
// REMARQUE : les variables de type float exigent un nombre à virgule
//
const float fVCalibration     = 0.826;  // pour la calibration de la tension
const float fICalibration     = 45; // pour la calibration de l'intensité
const float fPhaseCalibration = 0.7;   // valeur empirique compensant le déphasage lié aux capteurs...
const byte byTotalCount       = 20;    // nombre de demie-périodes étudiées pour la mesure

unsigned int iNombreIteration        = 0;       // nombre d'itérations durant les 20ms
unsigned int iTemps                  = 0;       // mesure du temps en secondes
const float fCyclesParSeconde        = 5.;
const float fMargeSecuriteWatt       = 0.;      // marge de sécurité en Watt pour être sûr de ne pas injecter
const int iCapaciteReservoirEnergie  = 3600;    // capacité du réservoir virtuel = 3600 joules
float fReservoirEnergie              = 0.;      // quantité d'énergie en joule stockée dans le réservoir virtuel

// Variables utiles au délestage
unsigned long iDelestStart    = 0;          // début du délestage
const float fDelestOn         = 30000.;     // seuil de puissance en mW pour démarrage du délestage
const float fBucketDelestOn   = 500.;       // seuil d'énergie en joules pour démarrage du delestage
const unsigned int iTempo     = 300;        // durée du délestage
bool bFlagDelest              = false;

// Détermination des entrées et sorties :
const byte byTriacPin             = 10;    // sortie numérique de commande de charge résistive
const byte byDelestPin            = 11;    // sortie temporisée à 5min à 20W du seuil 
const byte byTriacLedPin          = 3;     // sortie numérique pour la LED de test du triac
const byte byLimiteLedPin         = 13;    // sortie numérique pour la LED d'overflow
const byte byVoltCapteurPin       = 1;     // détecteur de tension entrée analogique 1
const byte byIntensiteCapteurPin  = 0;     // détecteur de courant entrée analogique 0
const byte byZeroCrossPin         = 2;     // détecteur de phase entrée digitale 2

// Variables de gestion des interruptions (zero-crossing) :
const byte byDimMax               = 128;      // valeur max de byDim pour inhiber le triac
byte byDim                        = byDimMax; // byDimming level (0-128)  0 = on, 128 = 0ff 
const char cPeriodStep            = 75;       // détermine la période du timer (65 pour 60Hz, 78 pour 50Hz, en µs)
                                              // suivant la formule (500000/AC_freq)/NumSteps = cPeriodStep 
                                              // 78*128=10ms=1/2 période 50Hz mais aux tests 76 marche mieux
volatile int i                = 0;            // Variable to use as a counter
volatile bool bZeroCross      = false;        // indicateur de zero-cross pour commander le triac
volatile bool bZeroCrossFlag  = false;        // indicateur de zero-cross pour calcul des puissances électriques

// Variables de calcul des grandeurs électriques :
int iLectureV, iLectureI;                     // tensions et courants en bits (0 à 1023 bits)
float fRealPower, fIrms, fVrms, fSqV, fSomV = 0, fSqI, fSomI = 0, fInstP, fSomP = 0;   
float fLastFilteredV, fOffsetV, fOffsetI, fFilteredV, fFilteredI, fPhaseShiftedV, fApparentPower, fPowerFactor;   
byte bZeroCrossCount          = 0;            // compteur de demie-périodes
       
// Variables pour le délestage
unsigned int iMemoTemps     = 0;              // memorisation du temps
bool bDelestage             = false;          // état du délestage

// Déclaration du LCD en mode I2C :
// toutes les infos ici : http://arduino-info.wikispaces.com/LCD-Blue-I2C
// Set the pins on the I2C chip used for LCD connections:  
//                   addr, en,rw,rs,d4,d5,d6,d7,bl,blpol  
// => connexion des 2 broches I2C sur l'Arduino Uno R3 : SDA en A4, SCL en A5
//
LiquidCrystal_I2C lcd(0x27, 16,2);   

// Setup
void setup() {
  pinMode(byTriacPin, OUTPUT);        // Set the Triac pin as output
  pinMode(byDelestPin, OUTPUT);
  pinMode(byTriacLedPin,  OUTPUT);    // Set the LED pin as output
  pinMode(byLimiteLedPin, OUTPUT);     

  //
  // A chaque changement d'état de byZeroCrossPin la fonction 'ZeroCrossDetect' est exécutée
  // quelque soit l'endroit où se trouvait l'exécution du programme : définition d'une interruption
  // documentation : https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
  //
  attachInterrupt(digitalPinToInterrupt(byZeroCrossPin), ZeroCrossDetect, RISING);  

  //
  // Autre mode d'interruption avec la librairie TimerOne.h : à chaque cPeriodStep écoulé dont la 
  // valeur est définie en microsecondes la fonction 'DimCheck' est lancée : cette interruption 
  // nous assure d'actionner le triac au bon moment
  //
  Timer1.initialize(cPeriodStep);                    // initialisation de la librairie TimerOne
  Timer1.attachInterrupt(DimCheck, cPeriodStep);  

  //
  // Utilisation de EEPROM pour récupérer et mettre à jour le nombre de redémarrage du programme
  // reconstruction du nombre redemarage, pour rappel EEPROM ne gère que le type unsigned char 
  //
  unsigned char cRedemarrageHigh = EEPROM.read(0);  // récupère la 1ère moitié du nombre
  unsigned char cRedemarrageLow  = EEPROM.read(1);  // récupère la 2ème moitié du nombre
  unsigned int iRedemarrage = (cRedemarrageHigh << 8) + cRedemarrageLow; 
  iRedemarrage++;
 // iRedemarrage = 0,
  EEPROM.update(0, highByte(iRedemarrage));
  EEPROM.update(1, lowByte(iRedemarrage));

  // Initialisation du LCD
  lcd.begin(16,2);                // initialize the lcd for 16 chars 2 lines
  lcd.clear();                    
  lcd.setCursor(0, 0);            
  lcd.print("P'TIWATT BONJOUR");  
  lcd.setCursor(0, 1);            
  lcd.print("   **********");  
    lcd.backlight();

  // Initialisation de la console
  Serial.begin(115200);
  Serial.println ();
  Serial.print("A NOTER : ");
  Serial.print(iRedemarrage);
  Serial.println(" ème redémarrage du programme");
  Serial.println(); 
  delay(500); 
  Serial.println();

  // Initilasation de variables
  fReservoirEnergie = 0;
  fOffsetV = 512.;
  fOffsetI = 512.;
  fSomV = 0.;
  fSomI = 0.;
  fLastFilteredV = 0.;
  iNombreIteration = 0;
  bDelestage = false;
  bFlagDelest = false;
  
  // Watchdog = reset après inaction supérieure à 500ms 
  //wdt_enable(WDTO_500MS);         
   
}

// ZERO CROSS DETECT : gestion du passage à zéro par interruption
void ZeroCrossDetect() {      // fonction appelée à chaque passage à zéro de la tension secteur
   bZeroCrossFlag   = true;   // témoin pour démarrer le calcul de la puissance
   bZeroCross       = true;   // témoin pour la commande du triac
}                                

// byDim CHECK : gestion du triac par interruption
void DimCheck() {                               // Function will fire the triac at the proper time
  if(bZeroCross == true && byDim < byDimMax ) { // First check to make sure the zero-cross has happened else do nothing
                                         
    if(i > byDim) {                         // i est un compteur qui détermine le retard au fire. plus byDim est élevé, plus de temps prendra le compteur i et plus tard
      digitalWrite(byTriacPin, HIGH);       // se fera le fire du triac 
      delayMicroseconds(50);                // Pause briefly to ensure the triac turned on
      digitalWrite(byTriacPin, LOW);        // Turn off the Triac gate, le triac reste conducteur
      i = 0;                                // Reset the accumilator
      bZeroCross = false;
    } 
    else { i++; }           // If the byDimming value has not been reached, incriment our counter
  }                         // End bZeroCross check
}                           // End DimCheck function


// Loop
void loop() {

  // calcul de la puissance réelle relevée par les capteurs avec fRealPower
  iNombreIteration = 0;
  fSomV = 0.;
  fSomI = 0.;
  fSomP = 0.;
  
  // A chaque passage à zéro de la tension du secteur réinitialisation périodique du compteur de 
  // passage à zéro bZeroCrossCount lorque le nombre de cycles de mesures byTotalCount est atteint 
  if (bZeroCrossCount >= byTotalCount) bZeroCrossCount = 0;

  // Processus de relevé de mesures durant le nombre défini de demi-périodes 
  while( bZeroCrossCount < byTotalCount ) {
    if( bZeroCrossFlag == true ) {            
      bZeroCrossFlag = false;
      bZeroCrossCount++;                            // incrémentation du compteur de demi-périodes secteur
    }  
    iNombreIteration++;                             // comptage du nombre d'itérations
    fLastFilteredV = fFilteredV;               //Used for delay/phase compensation

    // Lecture tension et courant
    iLectureV = analogRead(byVoltCapteurPin);       // mesure de V en bits - 0V = bit 512
    iLectureI = analogRead(byIntensiteCapteurPin);  // mesure de I en bits - 0A = bit 512

    // Appliquer un filtre digital pass bas
    fOffsetV = fOffsetV + ((float(iLectureV)-fOffsetV)/1024.);
    fFilteredV = iLectureV - fOffsetV;
    fOffsetI = fOffsetI + ((float(iLectureI)-fOffsetI)/1024.);
    fFilteredI = iLectureI - fOffsetI;

    // Calcul tension méthode RMS Root Mean Square
    fSqV  = fFilteredV * fFilteredV;  
    fSomV += fSqV;

    // Calcul courant méthode RMS
    fSqI  = fFilteredI * fFilteredI;
    fSomI += fSqI;

    // Phase calibration
    fPhaseShiftedV = fLastFilteredV + fPhaseCalibration * (fFilteredV - fLastFilteredV);

    // Calcul de la puissance instantanée 
    fInstP = fPhaseShiftedV * fFilteredI; 
    fSomP += fInstP;  
  }

  // Mémorisation des valeurs électriques
  if( iNombreIteration > 0 ) {

    // Calcul de la puissance réelle
    fRealPower = fVCalibration * fICalibration * fSomP / iNombreIteration;
    if( bCalibration == true ) { 
      fVrms = fVCalibration * sqrt(fSomV / iNombreIteration);
      fIrms = fICalibration * sqrt(fSomI / iNombreIteration);
      fApparentPower = fVrms * fIrms;
      fPowerFactor = fRealPower / fApparentPower ;
    }
  }

  // Calcul du taux de puissance à délester pour ne pas injecter avec byDim
  fReservoirEnergie += fRealPower / 1000. / fCyclesParSeconde;
  fReservoirEnergie += fMargeSecuriteWatt / fCyclesParSeconde;

  // Contrôle des limites min et max du réservoir virtuel
  if (fReservoirEnergie > iCapaciteReservoirEnergie) {fReservoirEnergie = iCapaciteReservoirEnergie; }
  if (fReservoirEnergie < 0) {fReservoirEnergie = 0;}
  
  // Determiner le fire du Triac
  if (fReservoirEnergie <= 1300.) {byDim = byDimMax;}
  else {
    if (fReservoirEnergie >= 2300.) {byDim = 0;}
    else {byDim = 128-((fReservoirEnergie - 1300) * .128);}
  }

  // Activation des LED surcharge et activité
  if (fReservoirEnergie >= 3600.) { digitalWrite(byLimiteLedPin, HIGH);}
  else { digitalWrite(byLimiteLedPin, LOW); }
  analogWrite(byTriacLedPin, byDimMax-byDim); 


  // Lecture du temps
  iTemps = millis()/1000;                   // lecture du temps en secondes   

  /*
  // Délestage lorsque "realPower" est supérieur au seuil de délestage 
  if( fRealPower > fDelestOn and fReservoirEnergie > fBucketDelestOn) {bDelestage = true;}
     
  if( bDelestage == true ) {
    if( bFlagDelest == false ) {
      digitalWrite(byDelestPin, HIGH);     // activation du délestage
      iDelestStart = iTemps;
      bFlagDelest = true;
    }
    else if (iTemps - iDelestStart > iTempo) {
      digitalWrite(byDelestPin, LOW);     // load shedding driver update
      bFlagDelest = false;
      bDelestage = false;
    }
  }  
  */
  
  // Affichage de ce qui se passe seulement toutes les 2 secondes
  if( iTemps >= iMemoTemps + 2 ) {
    iMemoTemps = iTemps;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("P=");
    lcd.print(String(-fRealPower/1000.,0));   
    lcd.print("W");
    lcd.setCursor(8, 0);
    lcd.print("R=");
    lcd.print(String(fReservoirEnergie,0));
    lcd.print("J");
    lcd.setCursor(0, 1);   
    lcd.print("D=");    
    lcd.print(byDim);

    /*
    lcd.setCursor(8, 1);   
    if( bDelestage == true ) {  
      lcd.print(iTemps - iDelestStart);
      lcd.print("s");
    }
    else { lcd.print("ARRETE"); }
    */
  }

  // Affichage pour la calibration
  if( bCalibration == true ) {
    Serial.print("Vrms=");
    Serial.print(fVrms);
    Serial.print("  | Irms=");
    Serial.print(fIrms/1000.);
    Serial.print("  |  rP=");
    Serial.print(fRealPower/1000.);
    Serial.print("  | aP=");
    Serial.print(fApparentPower/1000.);
    Serial.print("  |  pF=");
    Serial.print(fPowerFactor);
    Serial.println();
  }

  // Affichage pour le mode verbeux
  if( bVerbose == true ) {
    Serial.print("rP=");
    Serial.print(fRealPower/1000.);
    Serial.print("   |  sD=");
    Serial.print(fDelestOn/1000.);
    Serial.print("  |  E=");
    Serial.print(fReservoirEnergie);   
    Serial.print("  |  Dim=");
    Serial.print(byDim);
    if( bDelestage == true ) {  
      Serial.print(" | Delestage : ");
      Serial.print(iTemps - iDelestStart);
      Serial.println(" s");
    }
    Serial.println("");
  }
  else delay(1);           // pour la stabilité

  // Raz de la tempo du watchdog
  wdt_reset();                  
}
