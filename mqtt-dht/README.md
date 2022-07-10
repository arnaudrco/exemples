automate pour communiquer entre 2 objets connectés 

1) Sur l'objet connecté distant : déclarer dans son protocole MQTT un "topic", par exemple une temperature d'un capteur DHT sur un dispositif SonOff sur le topic "/SonOffExp/DHT/temperature"

2) Sur votre esp, recopier le logiciel pour lire l'information distante publiée sur le topic déclaré dans MQTT_SERIAL_RECEIVER_CH

3) brancher l'écran OLED sur les pins 26, 25 pour  SDA et SCL : l'écran OLED va lire "24" si la température lue est de 24°

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
