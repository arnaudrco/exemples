
Télémètre avertisseur pour vélo

Jean-Pierre CARIOU -  Janvier 2024
Objectif :
Ce projet vise à renforcer la sécurité des cyclistes par un éclairage avertisseur de proximité d’un véhicule qui doublerait un peu trop près.
L’idée est de créer un signal lumineux qui avertit l’automobiliste qu’il est trop proche et donc dangereux pour le cycliste.
On retrouve les distances caractéristiques ici :

<img width="648" height="669" alt="image" src="https://github.com/user-attachments/assets/231127f8-02a4-4ee2-b71f-96fc94ffb9ad" />

Le projet se fonde sur l’utilisation d’un microcontrôleur Arduino, associé à un télémètre laser et un panneau de leds. L’ensemble sera intégré dans un boitier étanche et fixé au vélo.

Cahier des charges :

    • Dimensions du boitier  < 10 x 10 x 5 cm 
    • Panneau multi leds RVB 
    • S’allume en dessous de distances spécifiées
    • Eteint au-delà de 1.50m
    • Orange entre 1.50 et 2.50m
    • Rouge en dessous de 1.50m
    • Alimentée par batterie rechargeable USB 
    • Autonomie 10h min
    • Fixée à la tige de selle ou au guidon
    • Commandable du guidon (coupure en cas d’embouteillages)
    • Moins de 15€ de coût de composants (BOM)
    
<img width="1036" height="511" alt="image" src="https://github.com/user-attachments/assets/f2567082-5eca-4bb5-8d39-fef0fdd04a17" />


Choix de composants
Les télémètres laser existent en OEM en diverses versions, correspondant à plusieurs portées et différents coûts.
Le tableau ci-dessous résume les choix possibles :

Modèle
Photo
Caractéristiques
Prix (AliExpress)
VL53L0X

    • Plage de fonctionnement: 0.03m ~ 4m
    • Précision: ± 3%
    • FOV: 25°

1.50 €
VL53L1X

    • Plage de fonctionnement: 0.05m ~ 4m
    • Précision: ± 3%
    • FOV: 27°

3.50€
TF Luna

    • Plage de fonctionnement: 0.2m ~ 8m
    • Précision: ± 6cm @(0.2-3m), ± 2% @(3m-8m)
    • FOV: 2 ° 3

28€
TF mini plus

    • Plage de mesure : 10 cm à 12 m
    • Précision :
± 5 cm de 0,1 à 6 m
± 1 % de 6 à 12 m
    • Champ de vision : 3,6 °

43€

Pour étudier les distances adaptées, le fonctionnement en toutes conditions, nous utiliserons le plus puissant (TF mini plus ) pour le premier prototype.
Télémètre TF mini plus

Présentation et fonctionnalités :
Module miniature à laser LiDAR prévu pour mesurer des distances de 0,1 à 12 m. Ce module IP65 est résistant à l'eau, compact, léger, très robuste et économe en énergie.

Le TFmini-Plus est très performant avec une fréquence de mesure très élevée (jusqu'à 1000 Hz) et une faible zone morte (- de 10 cm) avec une immunité aux très fortes luminosités jusqu'à 70000 lux.

La mesure de distance est basée sur la méthode Time-Of-Flight ce qui permet de mesurer précisément les distances grâce à des impulsions infrarouges.
Programmation et communication : Ce module communique avec une carte compatible Arduino ou Raspberry Pi via une liaison I2C ou série.
Connectique : La sortie et l'alimentation du LiDAR sont disponibles sur un connecteur JST-SH 4 broches au pas de 1 mm.


Caractéristiques :
    • Alimentation : 5 Vcc
    • Consommation : ≤ 110 mA (140 mA en pic)
    • Interface : I2C ou UART
    • Niveau logique de communication : 3,3 Vcc (compatible 5 Vcc)
    • Plage de mesure : 10 cm à 12 m
    • Résolution : 5 mm
    • Zone morte : 10 cm
    • Précision :
± 5 cm de 0,1 à 6 m
± 1 % de 6 à 12 m
    • Fréquence de mesure : 1 à 1000 Hz
    • Longueur d'onde : 850 nm
    • Champ de vision : 3,6 °
    • Indice de protection : IP65
    • Longueur du cordon : 30 cm
    • Connecteur : JST 4 broches 1 mm
    • Dimensions : 35 x 21 x 18,5 mm
    • Poids : 11 g

Référence Benewake : TFmini-Plus

Panneau de leds :


Pilotage des leds :

    • Adafruit Neopixel 8x8 led matrix with Arduino individually addressable led matrix – YouTube

Prototype avec un Arduino micro:

Cablage du module :

Signaux  programmés:

Chaque led  est tricolore  R V B avec un niveau d’intensité lum de 0 à 255 sur chaque canal.
L’indexation des leds est en boustrophédon1  telle qu’indiquée dans les schémas ci-dessous.
Le programme Arduino allume chaque led par une instruction indiquent sa position, et l’intensité de chaque canal :
pixels.setPixelColor(ipixel, pixels.Color(lumRed, lumGreen, lumBlue));



Montage du prototype :

La conception 3D est faite avec le logiciel Blender.
Le matériau est le plastique PLA. Le 3 premières couches (3 x 0.2mm) sont en blanc pour diffuser la lumière, et le reste en noir.
Un joint entre le couvercle et le boîtier est imprimé en plastique souple TPU rouge.
De même un joint annulaire en TPU est mis sur l’interrupteur.




      

Une attache annulaire de feu arrière est adaptée au boîtier pour la fixation au vélo. Le boîtier est orientable.
Essais du prototype
   



Tests de consommation
La consommation est mesurée avec un ampèremètre au niveau de l’alimentation.
    • Rouge lum = 250: 175 mA
    • Jaune lum = 20 : 136 mA
    • Vert  lum = 10  : 130 mA
    • Aucune diode : 129 mA
L’alimentation est assurée par 4 Piles AA 1.5V en série. Chaque pile a une capacité de 2500 à 3000 mAh. En mode normal de fonctionnement, cela assure environ 20h d’autonomie

Prototype 2

Par rapport au proto 1, les améliorations sont :
    • Attache au vélo plus solide. La première a lâché très rapidement.
    • 
    • 
    • 
    • Ecran diffusant en fil translucide 0.4mm au lieu de Blanc 0.6mm. L’intensité des leds est bien améliorée
    • Fenêtre du télémètre plus rétrécie pour mieux le protéger de la pluie
    • Joint couvercle un plus épais
    • 

Conclusion :
Le prototype est fonctionnel et détecte les voitures à proximité. La luminosité est bonne même de jour. Le volume et la masse sont un peu forts et l’attache pas assez stable. L’autonomie est insuffisante. Le panneau de leds peut être simplifié et les piles remplacées par une batterie rechargeable.
