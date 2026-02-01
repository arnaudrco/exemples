# FPV-car
First Person View car

https://github.com/user-attachments/assets/c1b2bee0-59e9-4a63-bc8f-71db247ff93a

# Schema électrique

<p>
  <img src="./schema electrique.png" width=600>
</p>

# Installation

- uploader le firmware micropython dans l'ESP32-cam (_micropython_camera_feeeb5ea3_esp32_idf4_4.bin_),
- copier les fichiers python et le fichier _index.html_ sur l'ESP32-cam,
- dans le fichier _WifiConnect.py_, mettre à jour les identifiants de connexion ('myssid' et 'mypwd'),



# Utilisation

Connecter l'ESP32-cam au PC et lancer WifiConnect dans le shell Thonny pour afficher l'adresse IP  de l'ESP32-cam :

```
>>> from WifiConnect import WifiConnect
>>> WifiConnect()
```

à la fin du fichier _main.py_ , ajouter la ligne : ```from fpv_car import *```

Déconnecter l'ESP32-cam du PC, et mettre le robot sous tension

Dans le navigateur du smartphone, ouvrir une page à l'adresse IP de l'ESP32-cam pour afficher la fenêtre vidéo et le joystick de commande.
