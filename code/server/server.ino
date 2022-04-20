//
// Essais de préparation du l'interface Web augmenté
//

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

char buffer[30];

WiFiServer server(80);

class Robot {
  public:
    int initial ; // valeur initiale du capteur balance lumière
    int dir = 0; // direction 0 : tout droit
    int valeur;
    int AA = 600; // target Speed
    int sensibilite ; // sensibilité
    int commande = 0;
  
};

class Web {
  public:
    void init(Robot* robot);
    void web_page(WiFiClient client, int commande, int dir);
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

    IPAddress apIP(44, 44, 44, 1);          // Définition de l'adresse IP statique.  
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

void Web::web_page(WiFiClient client, int commande, int dir)
{
    String html1 ="<!DOCTYPE html> \
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
<p>";

    String html2_1 = "</p></h1> \
  <form> \
      <label for='commande'>commande:</label> \
      <input class='echo' id='commande' name='commande'  value='";
    String html2_2 = "'> \
      <label for='direction'>direction:</label> \
      <input class='echo' id='direction' name='direction'  value=' ";
    String html3 = "'> \
      <table> \
          <tr> \
              <td> <button name='LED0' class='cmd' value='A' type='submit'> Manuel </button></td> \
              <td> <button name='LED0' class='cmd' value='B' type='submit'> Collision </button></td> \
              <td> <button name='LED0' class='cmd' value='C' type='submit'> Suiveur </button></td> \
          </tr> \
      </table> \
  </form> \
  <form> \
      <TABLE> \
          <TR> \
              <TD> <p class='param'> Sensibilit&eacute; </p></TD> \
              <TD> <button name='LED0' class='param' value='1' type='submit'> << </button></TD> \
              <TD> <button name='LED0' class='param' value='3' type='submit'> >> </button></TD> \
          </TR> \
          <TR> \
              <TD> <p class='param'> Vitesse </p></TD> \
              <TD> <button name='LED0' class='param' value='7' type='submit'> << </button></TD> \
              <TD> <button name='LED0' class='param' value='9' type='submit'> >> </button></TD> \
          </TR> \
          <TR> \
              <TD> </TD> \
              <TD> <button name='LED0' class='button' value='2' type='submit'> Avant </button></TD> \
              <TD> </TD> \
          </TR> \
          <TR> \
              <TD> <button name='LED0' class='button' value='4' type='submit'> Gauche </button></TD> \
              <TD> <button name='LED0' class='button' value='5' type='submit'> Stop </button></TD> \
              <TD> <button name='LED0' class='button' value='6' type='submit'> Droite </button></TD> \
          </TR>\
          <TR> \
              <TD> </TD> \
              <TD> <button name='LED0' class='button' value='8' type='submit'> Arri&egrave;re </button></TD> \
              <TD> </TD> \
          </TR>\
      </TABLE> \
  </form> ";

    String html4 ="</center>\
</head> \
</html>";

    String dirs[5] = { "Arr&ecirc;t", "Avant", "Arri&egrave;re", "Gauche", "Droite"};

      client.print(html1);
      client.print(html2_1);
      client.print(commande);
      client.print(html2_2);
      client.print(dirs[dir]);
      client.print(html3);
      client.print(html4);
      // client.print(html);
}

int Web::action()
{
  WiFiClient client = this->client();

  if (!client) return (0);  
  String request = client.readStringUntil('\r');
  
  int commande = 0;
  int dir = this->robot->dir;
  
  //-----------------COMMANDES------------
  if (request.indexOf("LED0=A") != -1) commande = 10;
  if (request.indexOf("LED0=B") != -1) commande = 11;
  if (request.indexOf("LED0=C") != -1) commande = 12;
  
  //-----------------PAVE HAUT------------
  if (request.indexOf("LED0=1") != -1) commande = 1;
  if (request.indexOf("LED0=2") != -1) commande = 2;
  if (request.indexOf("LED0=3") != -1) commande = 3;
  
  //-----------------PAVE CENTRE------------
  if (request.indexOf("LED0=4") != -1) commande = 4;
  if (request.indexOf("LED0=5") != -1) commande = 5;
  if (request.indexOf("LED0=6") != -1) commande = 6;
  
  //-----------------PAVE BAS------------
  if (request.indexOf("LED0=7") != -1) commande = 7;
  if (request.indexOf("LED0=8") != -1) commande = 8;
  if (request.indexOf("LED0=9") != -1)  commande = 9;
  
  // Serial.print("commande = ");
  // Serial.println(commande);

  if (commande == 2) dir = 1;
  if (commande == 4) dir = 3;
  if (commande == 5) dir = 0;
  if (commande == 6) dir = 4;
  if (commande == 8) dir = 2;

  this->robot->commande = commande;
  this->robot->dir = dir;
  
  this->web_page(client, commande, dir);
  
  request = "";
  return (commande);
}


Robot robot;
Web web;


void setup()
{
   Serial.begin(9600);
   web.init(&robot);

}

void loop()
{ // temporisation de 2s pour moteur
  web.action();

   delay(100);
}
