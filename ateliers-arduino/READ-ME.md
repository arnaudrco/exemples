capteur de champ magnétique

Amusez vous à réaliser avec du matériel de récupération un détecteur de canettes métalliques  pour faire une poêle à dépolluer. Il faut une boite en carton, du fil de téléphone, une self ajustable et quelques composants récupérés sur un vieux poste de radio à transistors


https://user-images.githubusercontent.com/90700891/178440713-50278df6-b641-4403-97e2-cd89286fb585.mp4

Le montage utilise 2 oscillateurs Colpitts branchés sur une porte logique NAND d'un circuit intégré très économique CD4011

capacités suivant la self : 2 x 2 nF pour une féquence de 200 KHz

capacités suivant le nombre de tours de fil téléphone : 2 x 22 nF pour 3 m de fil
 
Pour le filtre passe bas :résistance de 15 Kohms avec une capacité de 22 nF

La porte NAND prend en entrée les 2 oscillateurs et fournit un signal dont la valeur moyenne est de 3/4 du voltage du 4011

résistance ajoutée de 47 Kohms vers la masse 

Calcul du colpitts

![colpitts](https://github.com/arnaudrco/exemples/blob/main/ateliers-arduino/colpitss-calcul.png)

Calcul du passe bas

![passe bas](https://github.com/arnaudrco/exemples/blob/main/ateliers-arduino/passe-bas.png)


