# Introduction
Ce projet est une adapatation de la voiture 2 roues Anumby 2021 multi-capteur avec LED multicolore

pas besoin de pin pour la LED : la PIN D-IN de la LED est placée directement sur l'entrée TRIGGER du capteur ultrason

AUTO TEST

bip moteur

AUTO TEST ULTRA SON seul 

allumage de la LED neopixel sans moteur

Le capteur ultrason va allumer une led multicolore sans moteur

le moteur ne se met en route que lorsque la distance est < 10 cm

AUTO TEST OPTIQUE seul 

allume une led pour optique sans moteur

- Rouge pour la droite
- Verte tout droit si la balance est à l'équilibre
- Bleu  pour la gauche


- la couleur va changer du rouge, vert au bleu continûment  pour les positions intermédiaires

# Liste de materiel:
- __ESP8266 Nodemcu__ : https://fr.aliexpress.com/item/32659028315.html?spm=a2g0o.order_list.order_list_main.51.5f2c5e5bFkj0JY&gatewayAdapt=glo2fra
- __shield moteur L293D__ : https://fr.aliexpress.com/item/1005003186128473.html?spm=a2g0o.order_list.order_list_main.45.5f2c5e5bFkj0JY&gatewayAdapt=glo2fra
- __capteur ultrason HC-SR04__
- __diode neopixel ws2811__
- __2 cellules photovoltaïques__
- __2 résistances__

# Schema electrique

![image](https://github.com/anumby-source/RobotServiceJeunesse2022/assets/90700891/f265df17-b4af-40f0-a3b2-9371980d9690)








