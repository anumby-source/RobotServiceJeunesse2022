
1 bip : moteur
2 bip : capteur optique branché
3 bip : capteur optique à l'équilibre
si la distance ultrason est inférieure à seuil2 : passage en manuel

  const int seuil1 = 40;  // si on est > seuil1 on avance, si non on tourne à droite
  const int seuil2 = 10;  // si on est < seuil2 on stop car on n'a plus la place de tourner


