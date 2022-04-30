valeurs positives lumière à gauche
valeurs négatives lumière à droite

Robot.ino: Améliorations de l'interface web
Robot.ino: Changement de la logique pour les rotations:
- quand on commande une rotation on avance uniquement une roue (l'autre est à l'arrêt)
- dès qu'on a effectué une rotation d'un petit angle, on repart dans la direction normale (droit)
- on effectue les rotation soit en avant soit en arrière selon le sens de déplacement courant

