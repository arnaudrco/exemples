
# Une web radio simple avec un esp32S2

La broche SCK doit être mise à 0V (GND ).


<img width="693" height="718" alt="image" src="https://github.com/user-attachments/assets/a1e75963-dccd-4137-8385-eee513524571" />

## materiel 

esp32S2 avec memoire PSRAM et sortie jack sur PCM5100

Lire [webradio](https://github.com/arnaudrco/exemples/wiki/Webradio) 

Code à compiler avec les options  outils > partition > HUGE APP et "enable" SPRAM

bibliothèque https://github.com/schreibfaul1/ESP32-audioI2S
