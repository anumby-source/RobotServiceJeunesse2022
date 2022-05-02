mode "coyote"  : il est attiré par la lumière comme un papillon ; s'il y a un obstacle, après contournement, il est en mode répulsif

diagramme papillon :

              if (abs(delta) < 100)  {
                    M.avance();
                } else {
              if (delta > 0 ) {
             // lumière à droite => il faut redresser vers la droite
                            M.droite();

              } else if (delta < 0) {
             // lumière à gauche => il faut redresser vers la gauche
                            M.gauche();
            }
           } 


répulsif : delta = -delta; 
