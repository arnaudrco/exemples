# FPV-car
First Person View car

montage sans soudure

<img width="846" height="380" alt="image" src="https://github.com/user-attachments/assets/7eeaa492-e6cf-4705-b4a5-02f26d603e88" />



# Installation

- uploader le firmware micropython dans l'ESP32-cam (_micropython_camera_feeeb5ea3_esp32_idf4_4.bin_),
- copier les fichiers python et le fichier _index.html_ sur l'ESP32-cam,
- dans le fichier _WifiConnect.py_, mettre à jour les identifiants de connexion ('myssid' et 'mypwd'),



# Utilisation

Connecter l'ESP32-S3 au PC et lancer WifiConnect dans le shell Thonny pour afficher l'adresse IP  de l'ESP32-cam :

```
>>> from WifiConnect import WifiConnect
>>> WifiConnect()
```

à la fin du fichier _main.py_ , ajouter la ligne : ```from fpv_car import *```

Déconnecter l'ESP32-cam du PC, et mettre le robot sous tension

Dans le navigateur du smartphone, ouvrir une page à l'adresse IP de l'ESP32-cam pour afficher la fenêtre vidéo et le joystick de commande.
