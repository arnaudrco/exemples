brancher l'écran OLED sur les pins 26, 25 pour  SDA et SCL

#define PIN_GND 32
#define PIN_PLUS 33
#define SCL   25
#define SDA   26


déclarer un callback:
client.setCallback(callback); 

déclarer le topic:
client.subscribe("Topic");


implementer le callback:
void callback(char *topic, byte * payload, unsigned int length) {
….
}

Ajouter dans le loop:
client.loop();
