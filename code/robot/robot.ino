//
// Code auto test des capteurs pour le Robot pour l'événement Service Jeunesse 2022
//
// le coeur du code est hérité du code développé par Arnaud
// Reformatté et structuré pour l'algorithmique en C++ par Chris


#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#define STOP    1
#define AVANCE  2
#define RECULE  3
#define DROITE  4
#define GAUCHE  5
#define LENT    6
#define RAPIDE  7
#define SENSMIN 8
#define SENSMAX 9

#define MANUEL    10
#define COLLISION 11
#define SUIVI     12
#define BALANCE   13

//                  0   1               2      3                  4         5        6    7   8
String dirs[9] = { "", "Arr&ecirc;t", "Avant", "Arri&egrave;re", "Droite", "Gauche", "", "", ""};
String text_cmd[14] = { "", "STOP", "AVANCE", "RECULE", "DROITE", "GAUCHE", "LENT", "RAPIDE", "SENSMIN", "SENSMAX", "MANUEL", "COLLISION", "SUIVI", "BALANCE"};
String text_mode[3] = { "MANUEL", "COLLISION", "SUIVI"};

WiFiServer server(80);
int Mode = MANUEL;


//-------------------- ROBOT -------------------------------
class Robot {
  public:
    int initial ; // valeur initiale du capteur balance lumière
    int valeur;
    int sensibilite ; // sensibilité
    int commande = 0;
    int dir = STOP;

    void set_commande(int commande){
      this->commande = commande;
      if (commande == STOP || commande == AVANCE || commande == RECULE) this->dir = commande;
      };

    int direction(){
      return (this->dir);
    };
};


//-------------------- WEB -------------------------------
class Web {
  public:
    void init(int id, Robot* robot);
    void web_page(WiFiClient client, int commande);
    WiFiClient client();
    int action();
  private:
    Robot* robot = NULL;
};

WiFiClient Web::client()
{
   return (server.available());
}

void Web::init(int id, Robot* robot)
{
    this->robot = robot;

    IPAddress apIP(44, 44, 44, 44);         // Définition de l'adresse IP statique.
    const char *password = "12345678";      // mot de passe (*** A modifier ***)
    char ssid[30];
    //sprintf(ssid, "RCO_%d", ESP.getChipId()); // ceci différencie la connexion pour chaque voiture
    sprintf(ssid, "RCO_%d", id); // ceci différencie la connexion pour chaque voiture

    Serial.print("Initialise le réseau ");
    Serial.print(ssid);
    Serial.print(" adresse ");
    Serial.println(apIP);

    // declaration du wifi:
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(ssid, password);

    // if you get here you have connected to the WiFi
    Serial.println("Connected.");

    server.begin();
}

void Web::web_page(WiFiClient client, int commande)
{
    String html = String("<!DOCTYPE html> \
<html> \
<head> \
<style> \
  .echo {width: 100px; font-size: 150%;} \
  .cmd { padding:10px 10px 10px 10px; \
            margin:10px; \
            width:100%;  \
            background-color: red; \
            font-size: 250%;\
            color:white;} \
  .param { margin:10px; \
            width:100%;  \
            background-color: yellow; \
            font-size: 150%;\
            color:blue;} \
  .button { padding:10px 10px 10px 10px; \
            margin:10px; \
            width:100%;  \
            background-color: green; \
            font-size: 250%; \
            color:white;} \
</style> \
<center><h1>Robot Service Jeunesse \
</h1> \
  <form> \
      <label for='commande'>commande:</label> \
      <input class='echo' id='commande' name='commande'  value='") + text_cmd[commande] +
String("'> \
      <label for='mode'>mode:</label> \
      <input class='echo' id='mode' name='mode'  value='") + text_mode[Mode - MANUEL] +
String("'> \
      <label for='direction'>direction:</label> \
      <input class='echo' id='direction' name='direction'  value=' ") + dirs[this->robot->direction()] +
String("'> \
      <table> \
          <tr> \
              <td> <button name='LED0' class='cmd' value='") + String(MANUEL) + String("' type='submit'> Manuel </button></td> \
              <td> <button name='LED0' class='cmd' value='") + String(COLLISION) + String("' type='submit'> Collision </button></td> \
              <td> <button name='LED0' class='cmd' value='") + String(SUIVI) + String("' type='submit'> Suivi </button></td> \
              <td> <button name='LED0' class='cmd' value='") + String(BALANCE) + String("' type='submit'> Blancs </button></td> \
          </tr> \
      </table> \
  </form> \
  <form> \
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
          </TR>\
      </TABLE> \
  </form> \
  </center>\
</head> \
</html>");

/*

          <TR> \
              <TD> <p class='param'> Sensibilit&eacute; </p></TD> \
              <TD> <button name='LED0' class='param' value='") + String(SENSMIN) + String("' type='submit'> << </button></TD> \
              <TD> <button name='LED0' class='param' value='") + String(SENSMAX) + String("' type='submit'> >> </button></TD> \
          </TR> \
          <TR> \
              <TD> <p class='param'> Vitesse </p></TD> \
              <TD> <button name='LED0' class='param' value='") + String(LENT) + String("' type='submit'> Lent </button></TD> \
              <TD> <button name='LED0' class='param' value='") + String(RAPIDE) + String("' type='submit'> Rapide </button></TD> \
          </TR> \

*/

      client.print(html);
}

int Web::action()
{
  WiFiClient client = this->client();

  if (!client) return (0);
  String request = client.readStringUntil('\r');

  int commande = 0;

  //-----------------COMMANDES------------
  if (request.indexOf("LED0=" + String(MANUEL)) != -1) commande = MANUEL;
  else if (request.indexOf("LED0=" + String(COLLISION)) != -1) commande = COLLISION;
  else if (request.indexOf("LED0=" + String(SUIVI)) != -1) commande = SUIVI;
  else if (request.indexOf("LED0=" + String(BALANCE)) != -1) commande = BALANCE;

  //-----------------PAVE HAUT------------
  else if (request.indexOf("LED0=" + String(SENSMIN)) != -1) commande = SENSMIN;
  else if (request.indexOf("LED0=" + String(AVANCE)) != -1) commande = AVANCE;
  else if (request.indexOf("LED0=" + String(SENSMAX)) != -1) commande = SENSMAX;

  //-----------------PAVE CENTRE------------
  else if (request.indexOf("LED0=" + String(GAUCHE)) != -1) commande = GAUCHE;
  else if (request.indexOf("LED0=" + String(STOP)) != -1) commande = STOP;
  else if (request.indexOf("LED0=" + String(DROITE)) != -1) commande = DROITE;

  //-----------------PAVE BAS------------
  else if (request.indexOf("LED0=" + String(LENT)) != -1) commande = LENT;
  else if (request.indexOf("LED0=" + String(RECULE)) != -1) commande = RECULE;
  else if (request.indexOf("LED0=" + String(RAPIDE)) != -1)  commande = RAPIDE;

  this->robot->set_commande(commande);

  if (commande == COLLISION) Mode = COLLISION;
  else if (commande == SUIVI) Mode = SUIVI;
  else if (commande == MANUEL) Mode = MANUEL;
  else if (commande == STOP) Mode = MANUEL;
  else if (commande == BALANCE) Mode = MANUEL;

  // Serial.print("commande = "
  // Serial.println(commande);

  this->web_page(client, commande);

  request = "";
  return (commande);
};


//-------------------- MOTEUR -------------------------------
class Motorisation{
  private:
     Robot* robot = NULL;
     int pin_droite = 0; // broche enable du L298N pour le premier moteur
     int pin_gauche = 2; //  broche enable du L298N pour le deuxième moteur
     int speed_droite = 4; // Premier moteur
     int speed_gauche = 5; // Deuxième moteur
     int en_avant = HIGH;  // sens de rotation du moteur
     int en_arriere = LOW;

  public:
     int vitesse_max = 400;
     int vitesse_min = 200;

  Motorisation(Robot* robot){
    this->robot = robot;
    pinMode(this->pin_droite, OUTPUT);
    pinMode(this->pin_gauche, OUTPUT);
    pinMode(this->speed_droite, OUTPUT);
    pinMode(this->speed_gauche, OUTPUT);
    digitalWrite(this->pin_droite, this->en_avant);
    digitalWrite(this->pin_gauche, this->en_avant);
  };

  void action(int commande){
     this->robot->set_commande(commande);

     if (commande == AVANCE) this->avance();
     else if (commande == RECULE) this->recule();
     else if (commande == DROITE) this->droite();
     else if (commande == GAUCHE) this->gauche();
     else if (commande == STOP) this->stop();
     else if (commande == LENT) this->lent();
     else if (commande == RAPIDE) this->rapide();
  };

  void init(int vitesse_droite, int vitesse_gauche){
      Serial.println("Motorisation> init");
      // this->stop();
      // this->robot->set_commande(AVANCE);
      // digitalWrite(this->pin_droite, this->en_avant);
      // digitalWrite(this->pin_gauche, this->en_avant);
      analogWrite(this->speed_droite, vitesse_droite);
      analogWrite(this->speed_gauche, vitesse_gauche);
  };

  void bip(void)  { // test moteur
      Serial.println("Motorisation> bip");
      digitalWrite(this->pin_droite, this->en_avant);
      digitalWrite(this->pin_gauche, this->en_avant);
      analogWrite(this->speed_droite, this->vitesse_min);
      analogWrite(this->speed_gauche, this->vitesse_min);
      delay(100);
      analogWrite(this->speed_droite, 0);
      analogWrite(this->speed_gauche, 0);
      delay(400);
  };

  void lent(){
  }

  void rapide(){
  }

  void stop(void)  {
      Serial.println("Motorisation> stop");
      this->robot->set_commande(STOP);
      analogWrite(this->speed_droite, 0);
      analogWrite(this->speed_gauche, 0);
  };

  void avance()  {
      Serial.println("Motorisation> avance");
      this->robot->set_commande(AVANCE);
      analogWrite(this->speed_droite, this->vitesse_max);
      analogWrite(this->speed_gauche, this->vitesse_max);
      digitalWrite(this->pin_droite, this->en_avant);
      digitalWrite(this->pin_gauche, this->en_avant);
  };

  void droite()  {
      Serial.println("Motorisation> droite");
      analogWrite(this->speed_droite, 0);
      analogWrite(this->speed_gauche, this->vitesse_max);
      if (this->robot->dir == AVANCE)
      {
        digitalWrite(this->pin_droite, this->en_avant);
        digitalWrite(this->pin_gauche, this->en_avant);
        delay(1000);
        this->avance();
      }
      else if (this->robot->dir == RECULE) {
        digitalWrite(this->pin_droite, this->en_arriere);
        digitalWrite(this->pin_gauche, this->en_arriere);
        delay(1000);
        this->recule();
      }
      else this->stop();
  };

  void gauche()  {
      Serial.println("Motorisation> gauche");
      analogWrite(this->speed_droite, this->vitesse_max);
      analogWrite(this->speed_gauche, 0);
      if (this->robot->dir == AVANCE)
      {
        digitalWrite(this->pin_droite, this->en_avant);
        digitalWrite(this->pin_gauche, this->en_avant);
        delay(1000);
        this->avance();
      }
      else if (this->robot->dir == RECULE) {
        digitalWrite(this->pin_droite, this->en_arriere);
        digitalWrite(this->pin_gauche, this->en_arriere);
        delay(1000);
        this->recule();
      }
      else this->stop();
  };

  void recule()  {
      Serial.println("Motorisation> recule");
      this->robot->set_commande(RECULE);
      analogWrite(this->speed_droite, this->vitesse_max);
      analogWrite(this->speed_gauche, this->vitesse_max);
      digitalWrite(this->pin_droite, this->en_arriere);
      digitalWrite(this->pin_gauche, this->en_arriere);
  };

};


//-------------------- ULTRASON  -------------------------------
class Ultrason{
//define sound speed in cm/uS
#define SOUND_SPEED 0.034

  private:
    const int trigger = 13; // D7;
    const int echo = 12;    // D6;
    const int seuil1 = 40;  // si on est > seuil1 on avance, sinon on tourne à droite
    const int seuil2 = 10;  // si on est < seuil2 on stop car on n'a plus la place de tourner
    Motorisation* motor = NULL;

  public:
  Ultrason(Motorisation* motor){
    pinMode(this->trigger, OUTPUT); // Sets the trigger as an Output
    pinMode(this->echo, INPUT); // Sets the echo as an Input
    this->motor = motor;
  };

  int read(){
     long duration;
     digitalWrite(this->trigger, LOW);
     delayMicroseconds(2);
     // Sets the trigger on HIGH state for 10 micro seconds
     digitalWrite(this->trigger, HIGH);
     delayMicroseconds(10);
     digitalWrite(this->trigger, LOW);
     // Reads the echo, returns the sound wave travel time in microseconds
     duration = pulseIn(this->echo, HIGH);

     // Serial.println(duration * SOUND_SPEED/2);
     if ((duration < 60000) && (duration > 1)) {
        // Calcule la distance quand la lecture est vraisemblable
        return (duration * SOUND_SPEED/2);
    } else {
        return (0);
    };
  };

  int action(){
    int obstacle = this->read();
    if (obstacle == 0) {}
    else if (obstacle < this->seuil2) {
      // on n'a plus la place de tourner
      this->motor->stop();
      return (STOP);
    }
    else if (obstacle < this->seuil1) {
      // on contourne l'obstacle
      this->motor->droite();
    }
    else {
      // pas d'obstacle on avance
      this->motor->avance();
    };
    return (AVANCE);
  };
};

//-------------------- OPTIQUE -------------------------------
/*
 * Les deux capteurs photo-sensibles (CPS) sont connectés entre eux à travers un pont de 2 résistances identiques
 * La lecture se fait sur le point milieu du pont de résistances
 * La valeur lue est dans la gamme [0..1024]
 * Lorsque les leux CPS reçoivent la même lumière, la valeur lue sera donc 512
 */
class Optique{
private:
    Robot* robot = NULL;
    Motorisation* motor = NULL;
    int capteur = A0;         // lecture
    int balance_faite = 0;

public:
    int sensibilite = 10;     // seuil de sensibilite droite/gauche
    long balance = 1024/2;    // par défaut. En faisant la balance des blancs on peut ajuster cette valeur

   Optique(Robot* robot, Motorisation* motor){
       this->robot = robot;
       this->motor = motor;
   };

   long lecture(){
       return (analogRead(this->capteur));
   };

   void balance_des_blancs(){
       // positionner une feuille blanche très près du capteur optique
       this->balance = this->lecture();
       this->balance_faite = 1;
   };

   long delta(){
      // mesure la différence entre le capteur de droite et le capteur de gauche
      // delta > 0 => lumière à droite
      // delta < 0 => lumière à gauche

       return (this->balance - this->lecture());
   };

   void action(){
       int vitesse;
       int dir = 0;

       // this->motor->stop();

       int delta = this->delta();
       this->motor->avance();

       // on calcule la vitesse à appliquer proportinnelle au delta
       vitesse = map(abs(delta), 0, 512, 0, this->motor->vitesse_max);

       if (abs(delta) < this->sensibilite)  {
          // pas de différence notable
          if (this->robot->dir != AVANCE) {
             // on remet le robot en marche avant
             // Serial.println(delta);
             // Serial.println("tout droit");
             this->motor->avance();
          }
       } else {
          if (delta > 0 ) {
             // lumière à droite => il faut redresser vers la droite
             this->motor->init(vitesse, this->motor->vitesse_max);
          } else if (delta < 0) {
             // lumière à gauche => il faut redresser vers la gauche
             this->motor->init(this->motor->vitesse_max, vitesse);
          }
          delay(10);
          this->motor->avance();
       }
    }
};

Web web;
Robot robot;
Motorisation M(&robot);
Ultrason U(&M);
Optique O(&robot, &M);


/* ------------------AUTO-TEST ----------------

1 bip : moteur
2 bip : capteur optique branché
3 bip : capteur optique à l'équilibre

si la distance ultrason est inférieure à seuil2 : passage en manuel
const int seuil1 = 40;  // si on est > seuil1 on avance, si non on tourne à droite
const int seuil2 = 10;  // si on est < seuil2 on stop car on n'a plus la place de tourner

*/
void auto_test(){
    M.bip();
    Serial.println("lire droite");
    Serial.println(O.lecture());
    if (O.lecture() > 10) M.bip();
    if (abs(O.lecture() - 512) < 100 ) M.bip();
    // while(1) Serial.println(O.lecture() - 512);

    // while(1) Serial.println(U.read());
};


int id = 0;
int done = 0;


void setup()
{
   Serial.begin(115200);
   Serial.println("ROBOT");
   auto_test();

    delay(2000);
 
    Serial.println("");
    Serial.println("donnez votre id ");
}

int read_id(){
    if(Serial.available()){
        while (done == 0){
          String t = Serial.readStringUntil('\n');
          id = t.toInt();
          if (id > 0){
              Serial.print("id=");
              Serial.println(id);
              done = 1;
              return (id);
            }
        }
    }
 }

void loop()
{
   if (done == 0) {
     id = read_id();
     web.init(id, &robot);
   }
   else {
     int commande = web.action();
  
     if (commande > 0) {
        Serial.print("commande :");
        Serial.println(commande);
     }
  
     if (commande == BALANCE)
     {
         O.balance_des_blancs();
         Serial.print("Balance des blancs");
         Serial.println(O.balance);
     }
  
     //Serial.print("Mode ");
     //Serial.println(Mode);
     
     if (commande == STOP) M.stop();
  
     if (Mode == MANUEL){
        M.action(commande);
     }
     else if (Mode == COLLISION){
        int retour = U.action();
        if (retour == STOP) Mode = MANUEL;
     }
     else if (Mode == SUIVI){
        //Serial.print("delta entre les capteurs");
        Serial.println(O.delta());
        O.action();
     };
  
     delay(100);

   
   }

}
