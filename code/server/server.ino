//
// Essais de préparation du l'interface Web augmenté
//

#include <ESP8266WiFi.h>
//#include <DNSServer.h>
#include <ESP8266WebServer.h>

char buffer[30];

//WiFiServer server(80);
ESP8266WebServer server(80);            // Port du serveur

class Robot {
  public:
    int initial ; // valeur initiale du capteur balance lumière
    int dir = 0; // direction 0 : tout droit
    int valeur;
    int AA = 600; // target Speed
    int sensibilite ; // sensibilité
    int commande = 0;
  
};

void handleSpecificArg() { 
    String message = "";
    
    if (server.arg("Temperature")== ""){     //Parameter not found
        message = "Temperature Argument not found";
    }else{     //Parameter found
        message = "Temperature Argument = ";
        message += server.arg("Temperature");     //Gets the value of the query parameter    
    }

    server.send(200, "text/plain", message);          //Returns the HTTP response
}



String web_page(int commande, int dir)
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

    String html = html1 + html2_1 + String(commande) + html2_2 + dirs[dir] + html3 + html4;

      //client.print(html1);
      //client.print(html2_1);
      //client.print(commande);
      //client.print(html2_2);
      //client.print(dirs[dir]);
      //client.print(html3);
      //client.print(html4);
      // client.print(html);
    return (html);
}



class Web {
  public:
    void init(Robot* robot);
    WiFiClient client();
    int action();
  private:
    Robot* robot;
};

WiFiClient Web::client()
{
   // return (server.available());  
}

void handleGenericArgs(){
  //Serial.println("handleRoute");
  //server.send ( 200, "text/html", web_page(0, 0) );


  String message = "Number of args received:";
  message += server.args();

  for (int i = 0; i < server.args(); i++) {
    message += "Arg nº" + (String) i + " –> ";
    message += server.argName(i) + ": ";
    message += server.arg(i) + "\n";
  } 
  Serial.print("handleRoute> ");
  Serial.println(message);


  String html = web_page(0, 0);
  
  server.send(200, "text/html", html);       //Response to the HTTP request
}

void Web::init(Robot* robot)
{
    this->robot = robot;

    IPAddress apIP(44, 44, 44, 44);          // Définition de l'adresse IP statique.  
    // const char *ssid = "RCO";            // Nom du reseau wifi (*** A modifier ***)
    const char *password = "12345678";      // mot de pasensibilitee (*** A modifier ***)

    char ssid[30];
    sprintf(ssid, "RCO_%d", ESP.getChipId());

    // declaration du wifi:
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(ssid, password);

    server.on("/", handleGenericArgs); //Associate the handler function to the path
    //server.on("/specificArgs", handleSpecificArg);   //Associate the handler function to the path

    // if you get here you have connected to the WiFi
    Serial.println("Connected.");

    server.begin();
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
  
  web_page(commande, dir);
  
  request = "";
  return (commande);
}


Robot robot;
Web web;

void setup()
{
    Serial.begin(115200);
    web.init(&robot);

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  //Print the local IP to access the server
}

void loop()
{ // temporisation de 2s pour moteur
   server.handleClient();

   delay(100);
}
