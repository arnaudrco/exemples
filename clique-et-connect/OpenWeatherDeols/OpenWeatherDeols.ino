#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <OpenWeatherMapCurrent.h>
#include <OpenWeatherMapForecast.h>

// -------------- 4 LED 3 fils 

#ifndef LED_BUILTIN
#define LED_BUILTIN 13 // ESP32 DOES NOT DEFINE LED_BUILTIN
#endif

int LED = LED_BUILTIN;
 
LiquidCrystal_I2C lcd(0x27,20,4); 
OpenWeatherMapCurrentData conditions;// OWM toutes les 10 minutes 

 
const char* ssid     = "NUMERICABLE-63";      // SSID of local network
const char* password = "";   // Password on network
// String APIKEY = "ec0047d2b3ba1a8a5603d31e7ee77e88";
String APIKEY = "3ea8b8cf51c53c621d3cadbbd2a251b2"; // la poste
//String CityID = "6454880"; // Orsay
//String CityID = "3026204"; // Chateauroux
//String CityID = "3019733"; // Esnandes
//String CityID = "2969679"; // Versailles
// String CityID = "6618620"; //Paris
// String CityID = "2989317"; //Orléans 
String CityID = "3021570"; //Deols



// constants won't change. Used here to set a pin number:
const int ROUGE =  D3;// the number of the LED pin
const int VERT =  D3;// the number of the LED pin
const int BLEU =  D3;// the number of the LED pin pin
const int NOIR =  D3;// the number of the LED pin pin

const int CC =  D0;// the number of the LED pin
const int L1 =  D5;// the number of the LED pin pin
const int L2 =  D6;// the number of the LED pin
const int L3 =  D7;// the number of the LED pin
const int L4 =  D8;// the number of the LED pin

int interval = 1000;     // interval at which to blink (milliseconds)
int intensite=20; // intensité des LED
unsigned long previousMillis ; 

WiFiClient client;
char servername[]="api.openweathermap.org";
String result;
 
int  counter = 60;
 
String weatherDescription ="";
//String weatherLocation = "";
char weatherLocation[10] = "Deols";
String Country;
float Temperature;
float Humidity;
float Pressure;

int Windspeed =10;
int Windeg ;
int Id=0;

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster

}
void setupAuto() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  //set led pin as output
  pinMode(LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect


  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  //reset settings - for testing
  // wm.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wm.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wm.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

}
void setup() {
  // set the digital pin as output:
   // set the digital pin as output:
      
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(L3, OUTPUT);
   pinMode(L4, OUTPUT);
  pinMode(CC, OUTPUT);
      
  // test LED rampe
 for( int n = 0; n<8 ;n++) {
    rampe(n);
    delay(500);
  }
    eteint();
  previousMillis=millis();
      
//  Serial.begin(115200);
  int cursorPosition=0;
  lcd.init(); 
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   Connecting");  
  Serial.println("Connecting");

   Windspeed = 10 ;// vent
 
    
/*  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.setCursor(cursorPosition,2); 
    lcd.print(".");
    cursorPosition++;
  } */
  lcd.clear();

  setupAuto();
  
  lcd.print("   Connected!");
  Serial.println("Connected");
  delay(1000);
       getWeatherData();
}
void frequence(){ // ajuste intervalle suivant le code ID météo
  int i = Id % 100 ; // deux derniers chiffres
  if (i>= 30) {
    interval= 32;
  } else if (i>= 20) {
    interval= 64;
  } else if (i>= 10) {
    interval= 128;
  } else if (i>= 3) {
    interval= 256;
  } else if (i>= 2) {
    interval= 512;
  } else {
    interval= 1024;
  }
}
void delayms( int ms){
  while (ms--) waitms();
}
void waitms(){ // avec interval pour RGB et periode pour vent
    unsigned long t = ( millis()- previousMillis);
    unsigned int ttt = t / interval;
  
    vent(t); 
   if(Id >= 800) { allume( VERT, VERT, ttt);  
   } else if(Id >= 700){ allume( VERT, BLEU, ttt);
   } else if(Id >= 500){ allume( VERT, ROUGE, ttt);
   } else if(Id >= 300){ allume( ROUGE, BLEU, ttt);
   } else if(Id >= 200){ allume( ROUGE, ROUGE, ttt);
   } 
    delay(1);
}
void eteint(){
    digitalWrite(CC, LOW);
    digitalWrite(L1, LOW);
    digitalWrite(L2, LOW);
    digitalWrite(L3, LOW);
      digitalWrite(L4, LOW);
}
void rampe( unsigned int n){ // allume la n led de la rampe 0 à 7 
   if(!(n%2)) { // rampe impaire
    digitalWrite(CC, LOW);
    digitalWrite(L1, LOW);
    digitalWrite(L2, LOW);
    digitalWrite(L3, LOW);
      digitalWrite(L4, LOW);
    switch(n/2) {
      case 0 : analogWrite(L1, intensite);break ; 
      case 1 : analogWrite(L2, intensite);  break;  
     case 2 : analogWrite(L3, intensite); break ;
     case 3 : analogWrite(L4, intensite);break ;
     }  
   } else {
     digitalWrite(CC, HIGH);
    digitalWrite(L1, HIGH);
    digitalWrite(L2, HIGH);
    digitalWrite(L3, HIGH);
      digitalWrite(L4, HIGH);
   switch(n/2) {
      case 0 : analogWrite(L1, 255-intensite);break ; 
      case 1 : analogWrite(L2, 255-intensite);  break;  
     case 2 : analogWrite(L3, 255-intensite); break ;
     case 3 : analogWrite(L4, 255-intensite);break ;
     }  
   }

}
void vent( int t){ // allume les LED vent suivant l'horloge t
    int periode = 1000/ (1+(Windspeed / 5)) ;    // periode du vent
  unsigned int ppp = t / periode;
unsigned int i = (( Windeg +22 )/45) % 8; // 8 quadrants
//unsigned int i = (( 2 * Windeg + 22 )/45) % 16; // 16 quadrants
   
   if (!(i%2)) { // pair
      if (ppp%2) {
        rampe(i/2);
      } else eteint();
   } else {
    if (ppp%2) {
        rampe(i/2);
      } else rampe(i/2+1);
   }
   
}
void allume(int pin1, int pin2, int ttt){ // allume pin1 eteint pin2 suivant parité
  digitalWrite(ROUGE, LOW);
  digitalWrite(VERT, LOW);
  digitalWrite(BLEU, LOW);
  if ( ttt % 2) {  
        analogWrite(pin1, intensite);
 } else if (pin1 != pin2 ) analogWrite(pin2, intensite);
}
void loop() {
 //   if(counter == 12) //Get new data every 10 minutes
    if(counter == 60) //Get new data every 10 minutes
    {
      counter = 0;
      displayGettingData();
      delayms(1000);
      getWeatherData();
    }else
    {
      counter++;
      displayWeather(weatherLocation,weatherDescription);
      delayms(5000);
      displayConditions(Temperature,Humidity,Pressure);
      delayms(5000);
    }
}
 
void getWeatherData() //client function to send/receive GET request data.
{
  if (client.connect(servername, 80)) {  //starts client connection, checks for connection
    client.println("GET /data/2.5/weather?id="+CityID+"&units=metric&APPID="+APIKEY);
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  } 
  else {
    Serial.println("connection failed"); //error message if no client connect
    Serial.println();
  }
 
  while(client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
      result = result+c;
    }
 
  client.stop(); //stop client
  result.replace('[', ' ');
  result.replace(']', ' ');
  Serial.println(result);
 
char jsonArray [result.length()+1];
result.toCharArray(jsonArray,sizeof(jsonArray));
jsonArray[result.length() + 1] = '\0';
 
StaticJsonBuffer<1024> json_buf;
JsonObject &root = json_buf.parseObject(jsonArray);
if (!root.success())
{
  Serial.println("parseObject() failed");
}
 else{

float id = root["weather"]["id"];

 if(id){ // il existe des données météo

String location = root["name"];
String country = root["sys"]["country"];
float temperature = root["main"]["temp"];
float humidity = root["main"]["humidity"];
String weather = root["weather"]["main"];
String description = root["weather"]["description"];
float pressure = root["main"]["pressure"];

float windspeed = root["wind"]["speed"];
float windeg = root["wind"]["deg"];

weatherDescription = description;

location.replace("é","e");// remplace
location.replace("è","e");

strncpy(weatherLocation, &location[0], 6);// tronque 5 caractères

Country = country;
Temperature = temperature;
Humidity = humidity;
Pressure = pressure;
Windspeed=windspeed;
Windeg=windeg ;
Id=id;
      frequence();// ajuste la fréquence des LED
        vent(((Windeg+45)/90) % 4); // rose des vents
 }
}
}
void displayWeather(String location,String description)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(weatherLocation);
//  lcd.print(", ");
//  lcd.print(Country);
    lcd.print(" ");
  lcd.print(Windspeed);
    lcd.print("Km/h"); 
      lcd.print(" ");
  lcd.print(Windeg);
   lcd.print((char)223);
        
  lcd.setCursor(0,1);
    lcd.print(Id);
  lcd.print(" ");
   lcd.print(description);
}
 
void displayConditions(float Temperature,float Humidity, float Pressure)
{
  lcd.clear();
  lcd.print("T:"); 
 lcd.print(Temperature,1);
 lcd.print((char)223);
 lcd.print("C ");
 
 //Printing Humidity
 lcd.print(" H:");
 lcd.print(Humidity,0);
 lcd.print(" %");
 
 //Printing Pressure
 lcd.setCursor(0,1);
 lcd.print("P: ");
 lcd.print(Pressure,1);
 lcd.print(" hPa");
 
}
 
void displayGettingData()
{
  lcd.clear();
  lcd.print("Getting data");
}
