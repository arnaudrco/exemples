# esp8266


## une réalisation facile

Bip bip ! en utisant 2 modules LORA et 2 vieux esp8266, voici de quoi s'amuser !

<img width="723" height="467" alt="image" src="https://github.com/user-attachments/assets/7981e280-12c1-4ab1-8bf8-82691d04da21" />

https://mischianti.org/ebyte-lora-e220-llcc68-device-for-arduino-esp32-or-esp8266-specs-and-basic-use-1/#esp32-sketch_1

https://github.com/arnaudrco/exemples/wiki/LORA

## version avec balise

envoie un message

## version avec écran

 I2C OLED sur D6 (SDA) / D5 (SCL)

<img width="385" height="509" alt="image" src="https://github.com/user-attachments/assets/7c8755ad-63e5-445e-a0d3-07d9a0d07bb8" />

## version avec écran en réduisant le nombre de fils

<img width="564" height="680" alt="image" src="https://github.com/user-attachments/assets/27eb234a-cfa8-44e8-b6f2-c03dced6c77d" />

# version LORA "433"

433-RX-TX.ino avec un petit affichage ; montage sur LORA-02 (LX1262) très économique en 433 MHz ; recoie et envoie un message avec un compteur

<img width="382" height="401" alt="image" src="https://github.com/user-attachments/assets/57ee3a91-1042-48a0-9ac6-6d9c5f48e12d" />

# version LORA "433" et qualité de transmission 

433-RX-TX-RSSI-1367.ino avec un affichage en dB de la transmission

<img width="1093" height="602" alt="image" src="https://github.com/user-attachments/assets/24184d48-af61-4a7d-a519-2c7024a38f55" />

# version LORA 868 MHz

Pas facile d'utiliser les anciens modules sx1262 ; j'ai pris un montage qui affiche le RSSI

<img width="909" height="655" alt="image" src="https://github.com/user-attachments/assets/2ecc8fe7-0e8e-41c1-b1d3-06a71bc5ffcd" />

LORA-blink/1262-spi.ino

# version RSSI

afficher la qualité de transmission 

https://github.com/arnaudrco/CROUS-micro-python/blob/main/LORA/README.md

# écran faible consommation

Pas facile de rtouver le bon pilote

<img width="391" height="445" alt="image" src="https://github.com/user-attachments/assets/67cf106a-e30b-4365-a0bc-b5d234f64ae2" />

Petite police

    u8g2.setFont(u8g2_font_6x10_tf);

Pour avoir des grandes polices

- u8g2_font_fub30_tf (30 pixels de haut)
- u8g2_font_fub20_tf (20 pixels)
- u8g2_font_fub25_tf (25 pixels)
- u8g2_font_logu30_tf (30 pixels, chiffres)


