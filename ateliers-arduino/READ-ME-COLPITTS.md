capteur de champ magnétique

Amusez vous à réaliser avec du matériel de récupération un détecteur de canettes métalliques  pour faire une poêle à dépolluer. Il faut une boite en carton, du fil de téléphone, une self ajustable et quelques composants récupérés sur un vieux poste de radio à transistors


https://user-images.githubusercontent.com/90700891/178440713-50278df6-b641-4403-97e2-cd89286fb585.mp4

Le montage utilise 2 oscillateurs Colpitts branchés sur une porte logique NAND d'un circuit intégré très économique CD4011

capacités suivant la self : 2 x 2 nF pour une fréquence de 200 KHz

capacités suivant le nombre de tours de fil téléphone : 2 x 22 nF pour 3 m de fil
 
Pour le filtre passe bas :résistance de 15 Kohms avec une capacité de 22 nF

La porte NAND prend en entrée les 2 oscillateurs et fournit un signal dont la valeur moyenne est de 3/4 du voltage du 4011

résistance ajoutée de 47 Kohms vers la masse 

Calcul du colpitts :

![colpitts](https://github.com/arnaudrco/exemples/blob/main/ateliers-arduino/colpitss-calcul.png)

Passe bas d'ordre 1 :

![passe bas](https://github.com/arnaudrco/exemples/blob/main/ateliers-arduino/passe-bas.png)

Simulation sous LTSpice réalisée par Etienne du Repair Café d'Orsay



https://user-images.githubusercontent.com/90700891/183950473-4037eb96-cf70-466e-aba6-e0d78b4ac187.mp4



![image](https://user-images.githubusercontent.com/90700891/179235423-2dede77b-be4b-40c9-91a4-f446927ebc59.png)

Réalisation d'un inductancemetre

![image](https://user-images.githubusercontent.com/90700891/183016660-ece7ec2c-8a40-4c21-bfc2-32ef0df9db37.png)

![image](https://user-images.githubusercontent.com/90700891/183016831-47a7b8bf-74bd-407f-b83e-0a8bd39b46a0.png)


![image](https://user-images.githubusercontent.com/90700891/183016317-8eef02a3-aba4-4e5a-8135-2098673903ac.png)




