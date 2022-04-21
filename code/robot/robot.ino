//
// Essais de préparation du l'interface Web augmenté
//

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#define STOP 6
#define AVANCE 2
#define RECULE 8
#define DROITE 5
#define GAUCHE 4
#define LENT 7
#define RAPIDE 9

#define MANUEL 10
#define COLLISION 11
#define SUIVI 12

#define SENSMIN 1
#define SENSMAX 3

WiFiServer server(80);

class Robot {
  public:
    int initial ; // valeur initiale du capteur balance lumière
    int valeur;
    int AA = 600; // target Speed
    int sensibilite ; // sensibilité
    int commande = 0;
    int dir = STOP;

    void set_commande(int commande){
      this->commande = commande;
      if (commande == STOP || commande == AVANCE || commande == RECULE || commande == DROITE || commande == GAUCHE) this->dir = commande;
      };

    int get_dir(){
      return (this->dir);
    };

};

class Web {
  public:
    void init(Robot* robot);
    void web_page(WiFiClient client, int commande);
    WiFiClient client();
    int action();
  private:
    Robot* robot;
};

WiFiClient Web::client()
{
   return (server.available());
}

void Web::init(Robot* robot)
{
    this->robot = robot;

    IPAddress apIP(44, 44, 44, 44);         // Définition de l'adresse IP statique.
    // const char *ssid = "RCO";            // Nom du reseau wifi (*** A modifier ***)
    const char *password = "12345678";      // mot de pasensibilitee (*** A modifier ***)
    //ESP8266WebServer server(80);

    char ssid[30];
    sprintf(ssid, "RCO_%d", ESP.getChipId());

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
    //                  0   1     2      3     4         5          6           7       8
    String dirs[9] = { "", "", "Avant", "", "Gauche", "Droite", "Arr&ecirc;t", "", "Arri&egrave;re"};

    String html = String("<!DOCTYPE html> \
<html> \
<head> \
<style> \
  .echo {width: 50px;} \
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
      <input class='echo' id='commande' name='commande'  value='") + commande +
String("'> \
      <label for='direction'>direction:</label> \
      <input class='echo' id='direction' name='direction'  value=' ") + dirs[this->robot->get_dir()] +
String("'> \
      <table> \
          <tr> \
              <td> <button name='LED0' class='cmd' value='") + String(MANUEL) + String("' type='submit'> Manuel </button></td> \
              <td> <button name='LED0' class='cmd' value='") + String(COLLISION) + String("' type='submit'> Collision </button></td> \
              <td> <button name='LED0' class='cmd' value='") + String(SUIVI) + String("' type='submit'> Suiveur </button></td> \
          </tr> \
      </table> \
  </form> \
  <form> \
      <TABLE> \
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
          <TR> \
              <TD> </TD> \
              <TD> <button name='LED0' class='button' value='") + String(AVANCE) + String("' type='submit'> Avant </button></TD> \
              <TD> </TD> \
          </TR> \
          <TR> \
              <TD> <button name='LED0' class='button' value='") + String(GAUCHE) + String("' type='submit'> Gauche </button></TD> \
              <TD> <button name='LED0' class='button' value='") + String(STOP) + String("' type='submit'> Stop </button></TD> \
              <TD> <button name='LED0' class='button' value='") + String(DROITE) + String("' type='submit'> Droite </button></TD> \
          </TR>\
          <TR> \
              <TD> </TD> \
              <TD> <button name='LED0' class='button' value='") + String(RECULE) + String("' type='submit'> Arri&egrave;re </button></TD> \
              <TD> </TD> \
          </TR>\
      </TABLE> \
  </form> \
  </center>\
</head> \
</html>");

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

  // Serial.print("commande = "
  // Serial.println(commande);

  this->robot->set_commande(commande);
  
  this->web_page(client, commande);

  request = "";
  return (commande);
}

class Motorisation{
  private:
     int PinA = 0; // broche enable du L298N pour le premier moteur
     int PinB = 2; //  broche enable du L298N pour le deuxième moteur
     int SpeedA = 5; // Premier moteur
     int SpeedB = 4; // Deuxième moteur
     int avant = HIGH;
     int arriere = LOW;
     int vitesse = 700;

  public:

  Motorisation()  {
    pinMode(this->PinA, OUTPUT);
    pinMode(this->PinB, OUTPUT);
    pinMode(this->SpeedA, OUTPUT);
    pinMode(this->SpeedB, OUTPUT);
    digitalWrite(this->PinA, this->avant);
    digitalWrite(this->PinB, this->avant);
  };

  void action(int commande){
     if (commande == AVANCE) this->avance();
     else if (commande == RECULE) this->recule();
     else if (commande == DROITE) this->droite();
     else if (commande == GAUCHE) this->gauche();
     else if (commande == STOP) this->stop();
     else if (commande == LENT) this->lent();
     else if (commande == RAPIDE) this->rapide();
  };

  void bip(void)  { // test moteur
      digitalWrite(this->PinA, this->avant);
      digitalWrite(this->PinB, this->avant);
      analogWrite(this->SpeedA, this->vitesse);
      analogWrite(this->SpeedB, this->vitesse);
      delay(100);
      analogWrite(this->SpeedA, 0);
      analogWrite(this->SpeedB, 0);
      delay(400);
  };

  void lent(){
      this->vitesse = 300;
  }

  void rapide(){
      this->vitesse = 700;
  }

  void stop(void)  {
      analogWrite(this->SpeedA, 0);
      analogWrite(this->SpeedB, 0);
  };

  void avance()  {
      analogWrite(this->SpeedA, this->vitesse);
      analogWrite(this->SpeedB, this->vitesse);
      digitalWrite(this->PinA, this->avant);
      digitalWrite(this->PinB, this->avant);
  };

  void droite()  {
      analogWrite(this->SpeedA, this->vitesse);
      analogWrite(this->SpeedB, this->vitesse);
      digitalWrite(this->PinA, this->avant);
      digitalWrite(this->PinB, this->arriere);
  };

  void gauche()  {
      analogWrite(this->SpeedA, this->vitesse);
      analogWrite(this->SpeedB, this->vitesse);
      digitalWrite(this->PinA, this->arriere);
      digitalWrite(this->PinB, this->avant);
  };

  void recule()  {
      analogWrite(this->SpeedA, this->vitesse);
      analogWrite(this->SpeedB, this->vitesse);
      digitalWrite(this->PinA, this->arriere);
      digitalWrite(this->PinB, this->arriere);
  };

};


Robot robot;
Web web;
Motorisation M;


void setup()
{
   Serial.begin(9600);
   web.init(&robot);
   M.bip();
}

void loop()
{ // temporisation de 2s pour moteur
   int commande = web.action();
   M.action(commande);
   delay(100);
}
