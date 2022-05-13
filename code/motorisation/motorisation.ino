//
// Code auto test des capteurs pour le Robot pour l'événement Service Jeunesse 2022
//
// le coeur du code est hérité du code développé par Arnaud
// Reformatté et structuré pour l'algorithmique en C++ par Chris
//
// Utilisation de la carte LOLIN(WEMOS) D1 R2 & mini 


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define STOP    1
#define AVANCE  2
#define RECULE  3
#define DROITE  4
#define GAUCHE  5
#define DROIT   6
#define V1      7
#define V2      8
#define V3      9

#define MANUEL    10
#define COLLISION 11
#define SUIVI     12
#define BALANCE   13
#define OK        14
#define TEST      15

//                  0   1               2      3                  4         5        6    7   8
String dirs[9] = { "", "Arr&ecirc;t", "Avant", "Arri&egrave;re", "Droite", "Gauche", "", "", ""};
String text_cmd[16] = { "", "STOP", "AVANCE", "RECULE", "DROITE", "GAUCHE", "DROIT", "V1", "V2", "V3", "MANUEL", "COLLISION", "SUIVI", "BALANCE", "OK", "TEST"};
String text_mode[13] = { "", "", "", "", "", "", "", "", "", "", "MANUEL", "COLLISION", "SUIVI"};

ESP8266WebServer server(80);            // Port du serveur
int Mode = MANUEL;


//-------------------- MOTEUR -------------------------------
class Motorisation{
  public:
     int sens_droite = D4;    //0 broche enable du L298N pour le premier moteur
     int sens_gauche = D3;    //2 broche enable du L298N pour le deuxième moteur
     int pwm_droite = D2;     //4 Premier moteur
     int pwm_gauche = D1;     //5 Deuxième moteur
     int boite_vitesse = 1;
     int sens = STOP;         // STOP, AVANCE, RECULE
     int direction = DROIT;   // DROIT, DROITE, GAUCHE

      Motorisation(){
         Serial.println("Motorisation> init");

         pinMode(this->pwm_gauche,  OUTPUT);
         pinMode(this->pwm_droite,  OUTPUT);
         pinMode(this->sens_gauche, OUTPUT);
         pinMode(this->sens_droite, OUTPUT);

         analogWriteFreq(100);
         this->stop();
      };

      void bip(void)  { // test moteur
          Serial.println("Motorisation> bip");
          this->avance();
          delay(100);
          this->stop();
          delay(400);
      };

      void action(int commande){
         if (commande == AVANCE) this->avance();
         else if (commande == RECULE) this->recule();
         else if (commande == DROITE) this->droite();
         else if (commande == GAUCHE) this->gauche();
         else if (commande == STOP) this->stop();
         else if (commande == V1) this->boite_vitesse = 1;
         else if (commande == V2) this->boite_vitesse = 2;
         else if (commande == V3) this->boite_vitesse = 3;
      };

      void commande_gauche(int vitesse, int sens) {
         int v = map(vitesse, 0, 3, 0, 255);
         // Serial.print("VG=");
         // Serial.println(v);
         analogWrite(this->pwm_gauche, v);
         digitalWrite(this->sens_gauche, sens);
      };

      void commande_droite(int vitesse, int sens) {
         int v = map(vitesse, 0, 3, 0, 255);
         // Serial.print("VD=");
         // Serial.println(v);
         analogWrite(this->pwm_droite, v);
         digitalWrite(this->sens_droite, sens);
      };

      void commande_stop() {
          this->commande_gauche(0, LOW);
          this->commande_droite(0, LOW);
      };

      void commande_avance() {
          this->commande_gauche(this->boite_vitesse, HIGH);
          this->commande_droite(this->boite_vitesse, HIGH);
      }

      void commande_recule() {
          this->commande_gauche(this->boite_vitesse, LOW);
          this->commande_droite(this->boite_vitesse, LOW);
      }

      void stop(void)  {
          Serial.println("Motorisation> stop");
          this->sens = STOP;
          this->direction = DROIT;
          this->commande_stop();
      };

      void avance()  {
          Serial.println("Motorisation> avance");
          this->commande_stop();
          delay(100);
          this->sens = AVANCE;
          this->direction = DROIT;
          commande_avance ();
      };

      void recule()  {
          Serial.println("Motorisation> recule");
          this->commande_stop();
          delay(100);
          this->sens = RECULE;
          commande_recule ();
      };

      void gauche()  {
          Serial.println("Motorisation> gauche dir=" + String(this->sens));
          int dir = this->sens;
          this->direction = GAUCHE;
          this->commande_stop();
          delay(100);
          this->sens = dir;
          this->commande_gauche(this->boite_vitesse - 1, HIGH);
          this->commande_droite(this->boite_vitesse, HIGH);
      };

      void droite()  {
          Serial.println("Motorisation> droite dir=" + String(this->sens));
          int dir = this->sens;
          this->direction = DROITE;
          this->commande_stop();
          delay(100);
          this->sens = dir;
          this->commande_gauche(this->boite_vitesse, HIGH);
          this->commande_droite(this->boite_vitesse - 1, HIGH);
      };
};


Motorisation M;

void setup()
{
    Serial.begin(115200);
    delay(1000);
}

void loop() {

    if (Serial.available()){
       String t = Serial.readStringUntil('\n');
       int commande = t.toInt();
       Serial.print("Commande=");
       Serial.println(commande);
       M.action(commande);
    }

    delay(100);
}
