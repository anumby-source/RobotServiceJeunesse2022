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

#define S1      16
#define S2      17
#define S3      18

// #define DEBUG   0

//                  0   1               2      3                  4         5        6    7   8
String dirs[9] = { "", "Arr&ecirc;t", "Avant", "Arri&egrave;re", "Droite", "Gauche", "", "", ""};
String text_cmd[19] = { "", "STOP", "AVANCE", "RECULE", "DROITE", "GAUCHE", "DROIT", "V1", "V2", "V3", \
                        "MANUEL", "COLLISION", "SUIVI", "BALANCE", "OK", "TEST", \
                        "S1", "S2", "S3"};
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
#ifdef DEBUG
         Serial.println("Motorisation> init");
#endif

         pinMode(this->pwm_gauche,  OUTPUT);
         pinMode(this->pwm_droite,  OUTPUT);
         pinMode(this->sens_gauche, OUTPUT);
         pinMode(this->sens_droite, OUTPUT);

         analogWriteFreq(100);
         this->stop();
      };

      void bip(void)  { // test moteur
#ifdef DEBUG
          Serial.println("Motorisation> bip");
#endif
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

      int boite_de_vitesse(int vitesse){
         if (vitesse == 0) return (0);
         return (map(vitesse, 1, 3, 128, 255));
      };

      void commande_gauche(int vitesse, int sens) {
         int v = this->boite_de_vitesse(vitesse);
#ifdef DEBUG
         Serial.print("VG=");
         Serial.println(v);
#endif
         analogWrite(this->pwm_gauche, v);
         digitalWrite(this->sens_gauche, sens);
      };

      void commande_droite(int vitesse, int sens) {
         int v = this->boite_de_vitesse(vitesse);
#ifdef DEBUG
         // Serial.print("VD=");
         // Serial.println(v);
#endif
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
#ifdef DEBUG
          Serial.println("Motorisation> stop");
#endif
          this->sens = STOP;
          this->direction = DROIT;
          this->commande_stop();
      };

      void avance()  {
#ifdef DEBUG
          Serial.println("Motorisation> avance");
#endif
          this->commande_stop();
          this->sens = AVANCE;
          this->direction = DROIT;
          commande_avance ();
      };

      void recule()  {
#ifdef DEBUG
          Serial.println("Motorisation> recule");
#endif
          this->commande_stop();
          this->sens = RECULE;
          commande_recule ();
      };

      void gauche()  {
#ifdef DEBUG
          Serial.println("Motorisation> gauche dir=" + String(this->sens));
#endif
          int dir = this->sens;
          this->direction = GAUCHE;
          this->sens = dir;
          this->commande_stop();
          this->commande_gauche(this->boite_vitesse - 1, HIGH);
          this->commande_droite(this->boite_vitesse, HIGH);
      };

      void droite()  {
#ifdef DEBUG
          Serial.println("Motorisation> droite dir=" + String(this->sens));
#endif
          int dir = this->sens;
          this->direction = DROITE;
          this->sens = dir;
          this->commande_stop();
          this->commande_gauche(this->boite_vitesse, HIGH);
          this->commande_droite(this->boite_vitesse - 1, HIGH);
      };
};

//-------------------- ULTRASON  -------------------------------
class Ultrason{
//define sound speed in cm/uS
#define SOUND_SPEED 0.034

  private:
    const int trigger = D7; // 13;
    const int echo = D6;    // 12;
    const int seuil1 = 70;  // si on est > seuil1 on avance, sinon on tourne à droite
    const int seuil2 = 20;  // si on est < seuil2 on stop car on n'a plus la place de tourner
    Motorisation* motor = NULL;
#define queue 10
    int mesures[queue];
    int pos = 0;

  public:
    Ultrason(Motorisation* motor){
      pinMode(this->trigger, OUTPUT); // Sets the trigger as an Output
      pinMode(this->echo, INPUT); // Sets the echo as an Input
      this->motor = motor;
      for (int i = 0; i < queue; i++) mesures[i] = 0;
    };
  
    int lecture_instantanee(){
       long duration;
       digitalWrite(this->trigger, LOW);
       delayMicroseconds(2);
       // Sets the trigger on HIGH state for 10 micro seconds
       digitalWrite(this->trigger, HIGH);
       delayMicroseconds(10);
       digitalWrite(this->trigger, LOW);
       // Reads the echo, returns the sound wave travel time in microseconds
       duration = pulseIn(this->echo, HIGH);
  
#ifdef DEBUG
       // Serial.println(duration * SOUND_SPEED/2);
#endif
       if ((duration < 60000) && (duration > 1)) {
          // Calcule la distance quand la lecture est vraisemblable
          return (duration * SOUND_SPEED/2);
      } else {
          return (0);
      };
    };
  
    int lecture(){
       int d = this->lecture_instantanee();
       this->mesures[this->pos] = d;

       int moyenne = 0;

       for (int i = this->pos; i < queue; i++) moyenne += mesures[i];
       if (this->pos > 0) {
         for (int i = 0; i < this->pos; i++) moyenne += mesures[i];
       }

       moyenne = int(moyenne/queue);

       this->pos += 1;
       if (this->pos >= queue) this->pos = 0;

       return (moyenne);       
    }
  
    int action(int commande){

      if (commande == V1 || commande == V2 || commande == V3) this->motor->action(commande);
      
      int sens = this->motor->sens;
      int direction = this->motor->direction;
      int obstacle = this->lecture();
      int choix = int(random(DROITE, GAUCHE + 1));
  
#ifdef DEBUG
      Serial.print("Ultrason action> obstacle=");
      Serial.print(obstacle);
      Serial.print(" cm");
      Serial.print(" mode=");
      Serial.print(text_mode[Mode]);
      Serial.print(" commande=");
      Serial.print(text_cmd[commande]);
      Serial.print(" sens=");
      Serial.print(text_cmd[sens]);
      Serial.print(" direction=");
      Serial.print(text_cmd[direction]);
      Serial.print(" choix=");
      Serial.print(text_cmd[choix]);
      Serial.println("");
#endif
  
      if (obstacle == 0) return (AVANCE);
  
      if (commande == 0) {
        commande = sens;
      }
  
      if (commande == STOP) this->motor->stop();
      else {
          if (obstacle < this->seuil2) {
            // on n'a plus la place de tourner
            this->motor->recule();
            return (RECULE);
          }
          else if (obstacle < this->seuil1) {
            // on contourne l'obstacle
            if (direction == DROIT) { 
              if (choix == DROITE) this->motor->droite();
              else this->motor->gauche();
            }
            else if (direction == DROITE) this->motor->droite();
            else if (direction == GAUCHE) this->motor->gauche();
          }
          else if (sens != AVANCE || direction != DROIT) {
            // pas d'obstacle on avance
            this->motor->avance();
          };
      }
  
      return (commande);
    };
};

//-------------------- OPTIQUE -------------------------------
class Optique{
private:
    Motorisation* motor = NULL;
    int capteur = A0;         // lecture

public:
    int sensibilite = 2;     // seuil de sensibilite droite/gauche
    int balance_faite = 0;
    long balance = 0;    // par défaut. En faisant la balance des blancs on peut ajuster cette valeur
    long min_cps = 1000;
    long max_cps = -1000;
    long mid_cps = 1000;

   Optique(Motorisation* motor){
       this->motor = motor;
       this->set_sensibilite(S2);
   };

   long lecture(){
       return (analogRead(this->capteur));
   };

   void balance_des_blancs(int commande){
     if (commande == STOP) {
        this->min_cps = 1000;
        this->max_cps = -1000;
        this->mid_cps = 1000;
     }
     this->motor->stop();
       // positionner une feuille blanche très près du capteur optique
     long valeur_brute = this->lecture();
     long valeur = 512 - valeur_brute;
  
     if (valeur > this->max_cps) this->max_cps = valeur;
     if (valeur < this->min_cps) this->min_cps = valeur;
  
     this->mid_cps = (this->max_cps + this->min_cps)/2.0;

#ifdef DEBUG
     Serial.print("Balance lecture brute=");
     Serial.print(valeur_brute);
     Serial.print(" lecture=");
     Serial.print(valeur);
     Serial.print(" [min, mid, max}=[");
     Serial.print(this->min_cps);
     Serial.print(",");
     Serial.print(this->mid_cps);
     Serial.print(",");
     Serial.print(this->max_cps);
     Serial.print("] ");
     Serial.println("");
#endif

     if (commande == OK) {
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

   int set_sensibilite(int commande) {
      this->sensibilite = map(commande, S1, S3, 20, 100);
      return (this->sensibilite);
   }

   void action(int commande){

       if (commande == V1 || commande == V2 || commande == V3) this->motor->action(commande);

       if (commande == S1 || commande == S2 || commande == S3) {
          this->set_sensibilite(commande);
       }

       int sens = this->motor->sens;
       int direction = this->motor->direction;       
       int vitesse;

       long valeur_brute = this->lecture();
       long valeur = 512 - valeur_brute;
       int delta = this->delta();

//#ifdef DEBUG
       Serial.print("Optique action> delta=");
       Serial.print(delta);
       Serial.print(" lecture brute=");
       Serial.print(valeur_brute);
       Serial.print(" lecture=");
       Serial.print(valeur);
       Serial.print(" balance=");
       Serial.print(this->balance);
       Serial.print(" mode=");
       Serial.print(text_mode[Mode]);
       Serial.print(" commande=");
       Serial.print(text_cmd[commande]);
       Serial.print(" sens=");
       Serial.print(text_cmd[sens]);
       Serial.print(" direction=");
       Serial.print(text_cmd[direction]);
       Serial.print(" sensibilite=");
       Serial.print(this->sensibilite);
       Serial.println("");
//#endif

      if (commande == STOP) this->motor->stop();
      else {
         if (abs(delta) < this->sensibilite)  {
            // pas de différence notable
            if (this->motor->sens != AVANCE) {
#ifdef DEBUG
               Serial.println(delta);
               Serial.println("tout droit");
#endif
               this->motor->avance();
            }
         } else {
            if (delta > 0 ) {
               // lumière à droite => il faut redresser vers la droite
               this->motor->droite();
            } else if (delta < 0) {
               // lumière à gauche => il faut redresser vers la gauche
               this->motor->gauche();
            }
            delay(10);
            this->motor->avance();
         }

      }

      /*
       // on calcule la vitesse à appliquer proportionnelle au delta
       vitesse = map(abs(delta), 0, 512, 0, 4);

       */
   }
};

//-------------------- WEB -------------------------------

class Web {
  public:
    int id = 0;
    
  public:
    Web (){
      }
      
    void init()
    {
        IPAddress apIP(44, 44, 44, 44);         // Définition de l'adresse IP statique.
        const char *password = "12345678";      // mot de passe (*** A modifier ***)
        this->id = ESP.getChipId();
        char ssid[40];
        sprintf(ssid, "ANUMBY_%d", this->id); // ceci différencie la connexion pour chaque voiture
    
#ifdef DEBUG
        Serial.print("Initialise le réseau ");
        Serial.print(ssid);
        Serial.print(" adresse ");
        Serial.println(apIP);
#endif
    
        // declaration du wifi:
        WiFi.mode(WIFI_AP);
        WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
        WiFi.softAP(ssid, password);
    }

    String header()
    {
      String html = String(" \
<!DOCTYPE html> \
<html> \
<head> \
<style> \
  .echo {width: 100px; font-size: 150%;} \
  .cmd { padding:10px 10px 10px 10px; \
            margin:10px; \
            width:100%; \
            background-color: red; \
            font-size: 250%; \
            color:white;} \
  .param { margin:10px; \
            width:100%; \
            background-color: yellow; \
            font-size: 150%; \
            color:blue;} \
  .button { padding:10px 10px 10px 10px; \
            margin:10px; \
            width:100%; \
            background-color: green; \
            font-size: 250%; \
            color:white;} \
</style> \
<center>");
      return (html);
    }

    String trailer(){
      String html = String(" \
  </center> \
</head> \
</html>");
      return (html);
    }

    virtual String pager(int commande) {
      String html = this->header() + this->trailer();
      return (html);
    }

    virtual int decoder(String request){
      return 0;
    }
};

class Web_Balance: public Web {
  public:
     Optique* optique;

     Web_Balance(Optique* optique) {
        this->optique = optique;
      }
      
    String pager(int commande){
      String html = this->header() + String(" \
    <h1>Robot Service Jeunesse - Balance des blancs</h1> \
    <form> \
      <label for='identification'>identification:</label> \n \
      <input class='echo' id='identification' name='identification'  value=' ") + String(ESP.getChipId()) + String("'> \
      <br> \
      <label for='min'>min:</label> \
      <input class='echo' id='min' name='min'  value='") + String(this->optique->min_cps) + String("'> \
      <label for='mid'>mid:</label> \
      <input class='echo' id='mid' name='mid'  value='") + String(this->optique->mid_cps) + String("'> \
      <label for='min'>max:</label> \
      <input class='echo' id='max' name='max'  value='") + String(this->optique->max_cps) + String("'> \
      <br> \
      <table> \
        <tr> \
          <td> <button name='LED0' class='button' value='") + String(STOP) + String("' type='submit'> Reset </button> </td> \
          <td> <button name='LED0' class='button' value='") + String(BALANCE) + String("' type='submit'> Balance </button> </td> \
          <td> <button name='LED0' class='button' value='") + String(OK) + String("' type='submit'> Ok </button> </td> \
        </tr> \
      </table> \
    </form> ") + this->trailer();
      return (html);
    }
};

class Web_Motorisation: public Web {
  public:
    Motorisation* motor = NULL;

  public:
    Web_Motorisation(Motorisation* motor){
        this->motor = motor;
    }
    
    String pager(int commande){
      String html = this->header() + String(" \
    <h1>Robot Service Jeunesse - Test motorisation</h1> \
    <form> \
      <table> \
          <tr> \
              <td> <button name='LED0' class='cmd' value='") + String(COLLISION) + String("' type='submit'> Collision </button></td> \
              <td> <button name='LED0' class='cmd' value='") + String(SUIVI) + String("' type='submit'> Suivi </button></td> \
          </tr> \
      </table> \
      <TABLE> \
          <TR> \
              <TD> <button name='LED0' class='button' value='") + String(AVANCE) + String("' type='submit'> Avant </button></TD> \
              <TD> <button name='LED0' class='button' value='") + String(RECULE) + String("' type='submit'> Arri&egrave;re </button></TD> \
          </TR> \
      </TABLE> \
      <TABLE> \
          <TR> \
              <TD> <button name='LED0' class='button' value='") + String(GAUCHE) + String("' type='submit'> Gauche </button></TD> \
              <TD> <button name='LED0' class='button' value='") + String(STOP) + String("' type='submit'> Stop </button></TD> \
              <TD> <button name='LED0' class='button' value='") + String(DROITE) + String("' type='submit'> Droite </button></TD> \
          </TR> \
          <TR> \
               <TD> <p class='param'> Vitesse </p></TD> \
               <TD> <button name='LED0' class='param' value='") + String(V1) + String("' type='submit'> V1 </button></TD> \
               <TD> <button name='LED0' class='param' value='") + String(V2) + String("' type='submit'> V2 </button></TD> \
               <TD> <button name='LED0' class='param' value='") + String(V3) + String("' type='submit'> V3 </button></TD> \
          </TR> \
      </TABLE> \
      <label for='vitesse'>vitesse:</label> \n \
      <input class='echo' id='vitesse' name='vitesse'  value=' ") + String(this->motor->boite_vitesse) + String("'> \
    </form> ") + this->trailer();
      return (html);
    }
};


class Web_Collision: public Web {
  public:
    String pager(int commande){
      String html = this->header() + String(" \
    <h1>Robot Service Jeunesse - Mode Automatique</h1> \
    <form> \
      <table> \
          <tr> \
              <td> <button name='LED0' class='cmd' value='") + String(MANUEL) + String("' type='submit'> Manuel </button></td> \
              <td> <button name='LED0' class='cmd' value='") + String(SUIVI) + String("' type='submit'> Suivi </button></td> \
          </tr> \
      </table> \
      <TABLE> \
          <TR> \
              <TD> <button name='LED0' class='button' value='") + String(AVANCE) + String("' type='submit'> Avant </button></TD> \
              <TD> <button name='LED0' class='button' value='") + String(STOP) + String("' type='submit'> Stop </button></TD> \
              <TD> <button name='LED0' class='button' value='") + String(RECULE) + String("' type='submit'> Arri&egrave;re </button></TD> \
          </TR> \
          <TR> \
               <TD> <p class='param'> Vitesse </p></TD> \
               <TD> <button name='LED0' class='param' value='") + String(V1) + String("' type='submit'> V1 </button></TD> \
               <TD> <button name='LED0' class='param' value='") + String(V2) + String("' type='submit'> V2 </button></TD> \
               <TD> <button name='LED0' class='param' value='") + String(V3) + String("' type='submit'> V3 </button></TD> \
          </TR> \
      </TABLE> \
    </form> ") + this->trailer();
      return (html);
    }
};

class Web_Suivi: public Web {
  public:
    String pager(int commande){
      String html = this->header() + String(" \
    <h1>Robot Service Jeunesse - Mode Automatique</h1> \
    <form> \
      <table> \
          <tr> \
              <td> <button name='LED0' class='cmd' value='") + String(MANUEL) + String("' type='submit'> Manuel </button></td> \
              <td> <button name='LED0' class='cmd' value='") + String(COLLISION) + String("' type='submit'> Collision </button></td> \
          </tr> \
          <tr> \
              <td> <button name='LED0' class='cmd' value='") + String(BALANCE) + String("' type='submit'> Balance Gauche/Droite </button></td> \
          </tr> \
      </table> \
      <TABLE> \
          <TR> \
              <TD> <button name='LED0' class='button' value='") + String(AVANCE) + String("' type='submit'> Avant </button></TD> \
              <TD> <button name='LED0' class='button' value='") + String(STOP) + String("' type='submit'> Stop </button></TD> \
              <TD> <button name='LED0' class='button' value='") + String(RECULE) + String("' type='submit'> Arri&egrave;re </button></TD> \
          </TR> \
          <TR> \
               <TD> <p class='param'> Vitesse </p></TD> \
               <TD> <button name='LED0' class='param' value='") + String(V1) + String("' type='submit'> V1 </button></TD> \
               <TD> <button name='LED0' class='param' value='") + String(V2) + String("' type='submit'> V2 </button></TD> \
               <TD> <button name='LED0' class='param' value='") + String(V3) + String("' type='submit'> V3 </button></TD> \
          </TR> \
          <TR> \
               <TD> <p class='param'> Sensibilit&eacute; </p></TD> \
               <TD> <button name='LED0' class='param' value='") + String(S1) + String("' type='submit'> S1 </button></TD> \
               <TD> <button name='LED0' class='param' value='") + String(S2) + String("' type='submit'> S2 </button></TD> \
               <TD> <button name='LED0' class='param' value='") + String(S3) + String("' type='submit'> S3 </button></TD> \
          </TR> \
      </TABLE> \
    </form> ") + this->trailer();
      return (html);
    }
};

Motorisation M;
Optique O(&M);
Ultrason U(&M);

class Coyote{
public:
   boolean flag;
   void init(){
      //      pinMode(D7, OUTPUT);
   }

   int blink(){
      flag = !flag;
      //    digitalWrite(D7, flag);
   };
};

Web W;
Web_Balance WB(&O);
Web_Motorisation WM(&M);
Web_Collision WC;
Web_Suivi WS;
int first = 1;

void handle_args(){
  String message = "Number of args received:";
  message += server.args();

  int commande = 0;

  for (int i = 0; i < server.args(); i++) {
    message += "Arg nº" + (String) i + " –> ";
    message += server.argName(i) + ": ";
    message += server.arg(i) + "\n";

    if (server.argName(i) == "LED0") {
      commande = server.arg(i).toInt();
#ifdef DEBUG
      Serial.print("Mode=");
      Serial.print(Mode);
      Serial.print(" commande=");
      Serial.print(commande);
      Serial.println("");
#endif
      if (first == 1) {
        first = 0;
        Mode = MANUEL;

#ifdef DEBUG
        Serial.print("handle_args-first> Mode = ");
        Serial.print(Mode);
        Serial.print(" ");
        Serial.print(text_mode[Mode]);
        Serial.println("");
#endif

        commande = 0;
      }
      else {
          if (Mode == MANUEL) {
            if (commande == COLLISION) Mode = COLLISION;
            else if (commande == SUIVI) Mode = SUIVI;
          }
          else if (Mode == COLLISION) {
            if (commande == MANUEL) Mode = MANUEL;
            else if (commande == SUIVI) {
              Mode = SUIVI;
            }
          }
          else if (Mode == SUIVI) {
            if (commande == MANUEL) Mode = MANUEL;
            else if (commande == BALANCE) {
              Mode = BALANCE;
            }
            else if (commande == COLLISION) {
              Mode = COLLISION;
            }
          }
          else if (Mode == BALANCE) {
            O.balance_des_blancs(commande);
            if (commande == OK) Mode = SUIVI;
          }

#ifdef DEBUG
          Serial.print("handle_args> Mode = ");
          Serial.print(Mode);
          Serial.print(" ");
          Serial.print(text_mode[Mode]);
          Serial.println("");
#endif

          if (Mode == MANUEL) M.action(commande);
          else if (Mode == COLLISION) U.action(commande);
          else if (Mode == SUIVI) O.action(commande);
       }
    }
  }
#ifdef DEBUG
  Serial.print("handleRoute> ");
  Serial.println(message);
#endif

  String html = "";
  if (Mode == MANUEL) html = WM.pager(0);
  else if (Mode == BALANCE) html = WB.pager(0);
  else if (Mode == COLLISION) html = WC.pager(0);
  else if (Mode == SUIVI) html = WS.pager(0);

  server.send(200, "text/html", html);       //Response to the HTTP request
}



void setup()
{
    Serial.begin(115200);
    W.init();

    server.on("/", handle_args); //Associate the handler function to the path

    // if you get here you have connected to the WiFi
    Serial.println("Connected.");
    server.begin();

    delay(1000);
}

void loop() {
  /*
    {
        Serial.flush();
        Serial.end();
        Serial.begin(115200);
    }
    */
    server.handleClient();

    if (Mode == COLLISION){
      int commande = U.action(0);
    }

    //O.balance_des_blancs(0);

    if (Serial.available()){
       String t = Serial.readStringUntil('\n');
       int commande = t.toInt();
#ifdef DEBUG
       Serial.print("Commande=");
       Serial.println(commande);
#endif
       M.action(commande);
    }

    delay(100);
}
