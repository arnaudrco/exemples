// MPPT
// arnaudrco https://github.com/arnaudrco/exemples/blob/main/README.md
// placer le niveau POWER sur l'entrée A0

// Define stepper motor connections and steps per revolution:
#define pwmPin D1

#define delai 200
#define stepsPerScan 20 // nombre de pas par itération
#define seuil1 5 // nombre de pas pour le seuil bas
#define seuil2 15 // seuil haut
#define MIN 20 // nombre de pas pour le seuil bas
#define MAX 1000 // seuil haut


#define photoPin A0

int dir=1; // direction
int PWM=MIN;
int debug=1;
       
void setup() {
  Serial.begin(115200);
    Serial.println("Tracker solaire : PhotoVoltaique.");
  // Declare pins as output:
  pinMode(pwmPin, OUTPUT);
  // AUTOTEST 
     analogWrite(pwmPin, MAX);
     delay(100);
     if ( analogRead(photoPin) > 10) debug=0;
     
}
void loop() {

  int m=maximum(stepsPerScan);
  if (m>seuil2){
             Serial.println("Tracker solaire : continue !");
  } else if(m>seuil1){
              Serial.println("Tracker solaire : position trouvée !");
      dir= -dir; maximum(stepsPerScan-m);// recule

    } else {
      dir= -dir; maximum(stepsPerScan);// recule complétement
          Serial.println("Tracker solaire : change de sens");
    }
  
                delay(delai);
               
}   
int readPower(){ // lecture POWER
   analogWrite(pwmPin, PWM);
if (debug){ // simule une position puissance maximale à 512 
    if(PWM < 512) return (PWM); else return ( 512 - PWM );
} else return ( analogRead(photoPin) );

}
int maximum(int n) { // retourne la position du maximum sur n steps 

    if ( (PWM + dir * n ) > MAX) return (0); // 
    if ( (PWM + dir * n ) < MIN) return (0);
    int photoMax = readPower( ) ;
    int pos=0;

  // Spin the stepper motor 1 revolution slowly:
  for (int i = 0; i < n; i++) {
    // These four lines result in 1 step:
    PWM += dir;
    int ppp= readPower();
    // maximum
      if (ppp >photoMax ){
    photoMax = ppp;
    pos=i;
  }
  }
 Serial.print("pos :" );
 Serial.print(pos);
 Serial.print("photoMax :" );
   Serial.println(photoMax );
  return( pos);
}  
