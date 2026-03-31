
# Prise du commerce

Les prises du commerce exigent une application sur smartphone pas vraiment pratique. L'acces par l'interface TUYA des prises connectée est difficile, Franco a réalisé une installation avec OPENBEKEN ; la prise apparait sur le réseau WIFI sur une simple adresse 192.168.1.198

<img width="833" height="434" alt="image" src="https://github.com/user-attachments/assets/740d3e3a-a786-49e2-9277-2c2b2b6ba6dc" />

avec un simple esp32 j'affiche ma production électrique ( 6 W ) et j'ai même un historique de ma production pour l'ensemble de la journée !

<img width="380" height="451" alt="image" src="https://github.com/user-attachments/assets/0ad7a080-8705-4930-a7cb-9bb90b9db6e4" />


## accès au logiciel interne

<img width="488" height="498" alt="image" src="https://github.com/user-attachments/assets/d42be438-ffd2-4282-88bf-f00b01b94ed3" />

l'opération est délicate sur les prise récentes 

<img width="784" height="619" alt="image" src="https://github.com/user-attachments/assets/c6225d40-9d33-47e7-84c9-100450219118" />


# [Connaître sa consommation](https://github.com/arnaudrco/exemples/wiki/Connaitre-sa-consommation)

Bien connaître sa consommation est une première étape pour mieux consommer. 

## dialogue avec notre prise

la prise est connectée à une adresse : ici 192.168.1.174 ; en tapant sur un navigateur en donnant l'adresse de ma prise connectée

    192.168.1.174/cm?cmnd=STATUS%208

j'obtiens le résultat

<img width="516" height="367" alt="image" src="https://github.com/user-attachments/assets/7a28bc37-7b78-43d2-ae7e-0fda1b0a6751" />

Le code interprète les données "JSON" pour les afficher sur l'écran OLED

{
  "StatusSNS": {
    "Time": "2026-03-22T17:05:13",
    "ENERGY": {
      "TotalStartTime": "2026-03-21T23:08:15",
      "Total": 0.355,
      "Yesterday": 0,
      "Today": 0.355,
      "Power": 27,
      "ApparentPower": 36,
      "ReactivePower": 24,
      "Factor": 0.75,
      "Voltage": 239,
      "Current": 0.152
    }
  }
}



# Affichage de la production solaire

<img width="723" height="480" alt="image" src="https://github.com/user-attachments/assets/a83a638f-4d87-4e7b-b679-60b156a332fb" />


Connaître sa production avec un panneau solaire ? visualisation des variations par pas de 5 minutes ; suivez le guide avec Franco

<img width="874" height="580" alt="image" src="https://github.com/user-attachments/assets/f438a85d-53dc-43fc-b3bf-28aa9a11c181" />

## Affichage de l'historique

<img width="606" height="247" alt="image" src="https://github.com/user-attachments/assets/a97c819a-99b7-47e8-b252-f15b5109770b" />

Pourquoi ne pas écrire sa propre interface avec une prise connectée BK7231N 

<img width="479" height="456" alt="image" src="https://github.com/user-attachments/assets/3fe7934d-ff40-4afe-aa7e-457866f0e551" />

l'interface TUYA d'origine n'est pas pratique ; Franco a reprogrammé la prise 

https://github.com/openshwprojects/OpenBK7231T_App

 OpenBK7231T_App puis dans la partie Introduction choisir BK7231N.
Prise reprogrammée avec BK231GUIflashtool + serveur avec une carte esp32 

## lecture des informations

<img width="321" height="603" alt="image" src="https://github.com/user-attachments/assets/8120efb9-e769-46cc-ab96-83bc2f9775d7" />

une premiere interface

<img width="800" height="534" alt="image" src="https://github.com/user-attachments/assets/ce24f2ad-69ff-4122-8a56-451a3952322e" />

# guide

Guide Express : Dashboard solaire sur ESP32



[CODE](https://github.com/arnaudrco/exemples/blob/main/connaitre-sa-consommation/main.py)

Attention, il faut introduire dans "Connexion WiFi" les identifiants

        wlan.connect("NUMERICABLE-63","xxxx")  # <-- ton SSID / mot de passe
 
# Guide Complet : Flasher la LSPA8 (BK7231N) et la piloter avec un ESP32

Ce guide récapitule toutes les étapes pour transformer votre prise Tuya LSPA8/LSPA9 (puce Beken CB2S / lecteur BL0942) en une prise 100% locale sous **OpenBeken**, puis la piloter avec votre ESP32 en MicroPython.

## 1. Le matériel à acheter (Adaptateur USB vers Série TTL)

Pour flasher la carte mère de la prise sans la griller, il vous faut un adaptateur USB qui communique obligatoirement en **3.3V**.

**Mots-clés pour la recherche (Amazon, AliExpress, etc.) :**

* `Module FTDI FT232RL USB vers TTL`
* `Module CH340G USB vers Serie RS232 TTL`
* `Adaptateur CP2102 USB TTL`

⚠️ **Le point critique lors de l'achat :**
Vérifiez sur les photos du produit qu'il y a un "Jumper" (un petit cavalier jaune ou noir qui relie deux broches) ou un interrupteur permettant de basculer entre **5V** et **3.3V** (souvent annoté `VCC`). Ou alors, un module qui ne sort QUE du 3.3V. Si vous injectez du 5V dans la puce CB2S, elle sera définitivement détruite.

*N'oubliez pas de prendre quelques fils de connexion (type "Dupont Femelle-Mâle" ou "Femelle-Femelle") que vous pourrez dénuder d'un côté pour les souder temporairement sur la prise.*

---

## 2. Le branchement et la soudure (Hors Tension 220V !)

1. Ouvrez délicatement la prise LSPA8.
2. Repérez le module WiFi vertical nommé **CB2S**.
3. Effectuez ces 4 soudures entre la puce et votre nouvel adaptateur USB fraîchement réglé sur 3.3V :

| Adaptateur USB (3.3V) | Puce CB2S (LSPA8) |
| :--- | :--- |
| Broche **RX** | Broche **TX1** |
| Broche **TX** | Broche **RX1** |
| Broche **GND** | Broche **GND** |
| Broche **3.3V** | Broche **3V3** (Ne connectez ce fil qu'au dernier moment pour lancer le flashage) |

---

## 3. Le Flashage sous Windows (Méthode UART)

1. **Téléchargez le firmware** : Allez sur le [GitHub OpenBeken](https://github.com/openshwprojects/OpenBK7231T_App/releases) et prenez le fichier `.bin` **QIO** pour **BK7231N** (ex: `OpenBK7231N_QIO_1.xx.xxx.bin`). Ce fichier QIO est spécialement prévu pour le flashage filaire (UART).
2. **Téléchargez le logiciel** : Récupérez le [BK7231GUIFlashTool](https://github.com/openshwprojects/BK7231GUIFlashTool/releases) et lancez `BK7231GUIFlashTool.exe`.
3. **Configurez l'outil** :
   * Port : Votre port COM (ex: COM3)
   * Baud Rate : `115200` (Souvent beaucoup plus stable que les valeurs par défaut)
   * Chip Type : `BK7231N`
   * Firmware : Le fichier `.bin` téléchargé juste avant.
4. **Le moment fatidique** :
   * Assurez-vous que le fil 3.3V n'est **PAS** encore branché à la puce.
   * Cliquez sur le bouton "Do backup and flash new".
   * Le texte "Getting bus... (now, please do reboot by CEN or by power off/on)" apparait dans la console. **Immédiatement**, connectez le fil 3.3V à la puce CB2S pour qu'elle s'allume !
   * Le téléchargement démarre et indique *Success!* à la fin.
   * Si ça échoue (Failed ou *Getting bus failed... X/100*), débranchez le fil 3.3V de la puce (attendez 2/3 secondes), puis rebranchez-le au moment précis où le texte défile. C'est une question de timing !

---

## 4. Configuration finale et capteur de consommation

1. Dessoudez les fils, remontez la prise dans son plastique protecteur et **branchez-la au mur (220V)**.
2. Avec un téléphone, connectez-vous au nouveau réseau WiFi `OpenBeken_xxxx`.
3. Allez sur le navigateur : `http://192.168.4.1`.
4. Dans **Config > Config WiFi**, connectez la prise à votre Box internet (SSID / Mot de passe). Retrouvez sa nouvelle IP locale, et reconnectez-vous à la prise (ex: `192.168.1.50`).
5. Dans **Config > Configure Startup** (ou Custom Command), écrivez `startDriver BL0942` et enregistrez.
6. Dans **Config > Configure Module**, assignez les pins physiques (extraites de votre configuration d'usine) :
   * Pin 6 : `Btn` (Bouton), Index 1
   * Pin 8 : `LED_n` (Led statut WiFi), Index 1
   * Pin 26 : `Rel` (Relais), Index 1
7. Cliquez sur **Restart**. Au redémarrage, vous devriez entendre le "clac" de la LSPA8 via l'interface web, et lire la Tension / Intensité / Puissance !

---

## 5. Le code MicroPython test pour votre ESP32

Dans l'application Thonny, utilisez ce code (en remplaçant l'IP de la LSPA8 et vos identifiants WiFi) :

```python
import network
import time
import urequests
import ujson

# Vos paramètres
WIFI_SSID = "XXX"
WIFI_PASS = "XXX"
PRISE_IP = "192.168.100" 

def connect_wifi():
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    if not wlan.isconnected():
        print("Connexion au point d'accès WiFi...")
        wlan.connect(WIFI_SSID, WIFI_PASS)
        while not wlan.isconnected():
            time.sleep(1)
            print(".", end="")
    print("\nConnecté ! IP de l'ESP32:", wlan.ifconfig()[0])

def lire_consommation():
    try:
        url = f"http://{PRISE_IP}/cm?cmnd=Status%208" # Lecture de la puce BL0942 via l'API Tasmota/OBK
        r = urequests.get(url, timeout=5)
        donnees = ujson.loads(r.text)
        r.close()
        
        if "StatusSNS" in donnees and "ENERGY" in donnees["StatusSNS"]:
            energie = donnees["StatusSNS"]["ENERGY"]
            print(f"| Tension : {energie.get('Voltage')} V")
            print(f"| Courant : {energie.get('Current')} A")
            print(f"| Puissance : {energie.get('Power')} W")
        else:
            print("Capteur de puissance non lu.")
    except Exception as e:
        print("Erreur:", e)

def set_prise(etat):
    # etat doit être True (Allumer) ou False (Eteindre)
    val = "On" if etat else "Off"
    try:
        url = f"http://{PRISE_IP}/cm?cmnd=Power1%20{val}"
        r = urequests.get(url, timeout=5)
        print(f"La prise a reçu l'ordre : {val}")
        r.close()
    except Exception as e:
        print("Erreur de communication:", e)

# Test :
connect_wifi()
print("\n--- Allumage de la prise LSPA8 ---")
set_prise(True)
time.sleep(3) # On laisse le temps au capteur de mesurer l'appareil branché dessus

print("\n--- Infos de consommation ---")
lire_consommation()

time.sleep(3)

print("\n--- Extinction de la prise LSPA8 ---")
set_prise(False)
```

---

## 6. Bonus : Calibrer le capteur de consommation

Puisque le firmware d'origine Tuya a été effacé et que vous avez installé OpenBeken dessus, les réglages d'étalonnage usine de votre prise ont été perdus (c'est le cas sur toutes les prises génériques sans puce dédiée !). Voici comment recalibrer votre LSPA8 pour avoir des valeurs parfaites :

1. **La Tension (V)** : Mesurez la tension de la prise avec votre multimètre (ex: `231.6`).
   * Ouvrez votre navigateur internet PC/Téléphone et entrez cette URL (en changeant l'IP ESP32 si besoin) :
   * `http://192.168.1.5/cm?cmnd=VoltageSet%20231.6`

2. **La Puissance (W)** : Branchez tout de suite un appareil dont la consommation est fixe et connue (ex: une vraie ampoule à filament de 60W, ou un convecteur 2000W) et **allumez** la prise (très important).
   * Entrez l'URL (pour notre exemple d'ampoule 60W) :
   * `http://192.168.1.5/cm?cmnd=PowerSet%2060.0`

3. **Le Courant (A)** : L'intensité est très simple à calculer mathématiquement (Puissance divisée par Tension). Pour notre exemple avec l'ampoule 60W : 60W / 231.6V = `0.259A`.
   * Entrez l'URL :
   * `http://192.168.1.5/cm?cmnd=CurrentSet%200.259`

*Et c'est tout ! OpenBeken va automatiquement recalculer ses constantes de mesure internes à la volée, sans que vous n'ayez besoin de redémarrer. Vos relevés sur l'interface ESP32 seront désormais identiques à votre multimètre !*

