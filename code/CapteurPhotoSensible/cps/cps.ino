//
// Code auto test des capteurs pour le Robot pour l'événement Service Jeunesse 2022
//
// le coeur du code est hérité du code développé par Arnaud
// Reformatté et structuré pour l'algorithmique en C++ par Chris


#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>



//-------------------- OPTIQUE -------------------------------
/*
 * Les deux capteurs photo-sensibles (CPS) sont connectés entre eux à travers un pont de 2 résistances identiques
 * La lecture se fait sur le point milieu du pont de résistances
 * La valeur lue est dans la gamme [0..1024]
 * Lorsque les leux CPS reçoivent la même lumière, la valeur lue sera donc 512
 */
class Optique{
private:
    int capteur = A0;         // lecture
    long min_cps = 1000;
    long max_cps = -1000;
    long mid_cps = 1000;

public:
    int sensibilite = 10;     // seuil de sensibilite droite/gauche
    int balance_faite = 0;
    long balance = 0;    // par défaut. En faisant la balance des blancs on peut ajuster cette valeur

   Optique(){
   };

   long lecture(){
       return (analogRead(this->capteur));
   };

   void balance_des_blancs(){
       // positionner une feuille blanche très près du capteur optique
     long valeur = 512 - this->lecture();
  
     if (valeur > this->max_cps) this->max_cps = valeur;
     if (valeur < this->min_cps) this->min_cps = valeur;
  
     this->mid_cps = (this->max_cps + this->min_cps)/2.0;

     Serial.print("Balance ");
     Serial.print(this->min_cps);
     Serial.print(",");
     Serial.print(this->mid_cps);
     Serial.print(",");
     Serial.print(this->max_cps);
     Serial.println("");
  
     if (Serial.available()) {
        String t = Serial.readStringUntil('\n');
        this->balance_faite = 1;
        this->balance = this->mid_cps;
     }
   };

   long delta(){
      // mesure la différence entre le capteur de droite et le capteur de gauche
      // delta > 0 => lumière à droite
      // delta < 0 => lumière à gauche 

       long valeur = 512 - this->lecture();
       return (valeur - this->balance);
   };

};

Optique O;

long previous = -1000;
int balance_done = 0;

void setup()
{
   Serial.begin(115200);
   
   delay(2000);
}

void loop()
{
   if (O.balance_faite == 0) {
     O.balance_des_blancs();
   }
   else {
     long valeur = O.delta();
     Serial.print("after balance ");
     Serial.println(valeur);
   };

   delay(100);
}
