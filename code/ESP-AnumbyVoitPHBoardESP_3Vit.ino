// Programme de commande Voiture 3 à travers d'un pont H
// 05/10/2018 - Version 0.0.0 creation
// 20/12/2018 - Adaptation pour shield ESP8266 avec pont h
// 15/01/2019 - Correction bugs
// 16/01/2019 - Performances ....

// Gestion des vitesses:
// 0 = arret
// 1 à 3 vitesse marche avant ou arriere


#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>

IPAddress    apIP(44, 44, 44, 44);      // Définition de l'adresse IP statique.
const char *ssid = "Anumby-Voit2";      // Nom du reseau wifi (*** A modifier ***)
const char *password = "12345678";      // mot de passe       (*** A modifier ***)
ESP8266WebServer server(80);            // Port du serveur

int choix = 0;                          // à l'arret pour commencer

int BoiteVitesse = 1;
int valeurAcceleration[4] = {0, 400 , 700 , 1024 };  // arret, 1ere, 2eme, 3eme


const int PwmDroite   = D2;
const int PwmGauche   = D1;

const int SensDroite  = D4;
const int SensGauche  = D3;

bool debug = false;


String getPage() {
  String page = "<html lang=fr-FR><head>";
  page += "<title>Commande voiture 2</title>";
  page += "<style> body { background-color: #fffff; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  page += "</head><body><h1>Anumby - commande voiture</h1>";
  if (choix == 0) {
    page += "<h3>Arret (vitesse "; page += BoiteVitesse; page += ")</h3>";
  } else if (choix == 1) {
    page += "<h3>Avance (vitesse "; page += BoiteVitesse; page += ")</h3>";
  } else if (choix == 2) {
    page += "<h3>Recule (vitesse "; page += BoiteVitesse; page += ")</h3>";
  } else if (choix == 3) {
    page += "<h3>Gauche (vitesse "; page += BoiteVitesse; page += ")</h3>";
  } else if (choix == 4) {
    page += "<h3>Droite (vitesse "; page += BoiteVitesse; page += ")</h3>";
  } else {
    page += "<h3>Commande invalide</h3>";
  }
  //page += "<br>";
  //page += "<a href=\"/?Avance=1\"style=\" background-color: green; color: white; padding: 20px; text-decoration: none;margin-left: 210px; border-radius: 40px\">  Avance</a>";
  //page += "<a href=\"/?Vitesse3=1\"style=\" background-color: blue; color: white; padding: 20px; text-decoration: none;margin-right:350px; border-radius: 40px\">(  3  )</a>";
  //page += "<br> <br> <br> <br> <br>";
  //page += "<a href=\"/?Gauche=1\"style=\" background-color: green; color: white; padding: 20px; text-decoration: none;margin-left: 30px; border-radius: 40px\">Gauche</a>";
  //page += "<a href=\"/?Arret=1\" style=\" background-color: red; color: white; padding: 20px; text-decoration: none;margin-left: 60px; border-radius: 40px\">Arret</a>";
  //page += "<a href=\"/?Droite=1\"style=\" background-color: green; color: white; padding: 20px; text-decoration: none;margin-left: 60px; border-radius: 40px\">Droite</a>";
  //page += "<a href=\"/?Vitesse2=1\"style=\" background-color: blue; color: white; padding: 20px; text-decoration: none;margin-left:195px; border-radius: 40px\">(  2  )</a>";
  //page += "<br> <br> <br> <br> <br>";
  //page += "<a href=\"/?Recule=1\"style=\" background-color: green; color: white; padding: 20px; text-decoration: none;margin-left: 210px; border-radius: 40px\">Recule</a>";
  //page += "<a href=\"/?Vitesse1=1\"style=\" background-color: blue; color: white; padding: 20px; text-decoration: none;margin-left:350px; border-radius: 40px\">(  1  )</a>";

  page += "<a href=\"/?Avance=1\"style=\" position : absolute; font-size : 26px; background-color: green; color: white; padding: 20px; text-decoration: none;margin-left: 210px; border-radius: 40px\">  Avance</a>";
  page += "<a href=\"/?Vitesse3=1\"style=\" position : absolute; font-size : 26px; background-color: blue; color: white; padding: 20px; text-decoration: none; margin-left: 650px; border-radius: 40px\">V:3</a>";
  page += "<br> <br> <br> <br> <br>";



  page += "<a href=\"/?Gauche=1\"style=\" position : absolute; font-size : 26px; background-color: green; color: white; padding: 20px; text-decoration: none;margin-left: 30px; border-radius: 40px\">Gauche</a>";
  page += "<a href=\"/?Arret=1\" style=\" position : absolute; font-size : 26px; background-color: red; color: white; padding: 20px; text-decoration: none;margin-left: 225px; border-radius: 40px\">Arret</a>";
  page += "<a href=\"/?Droite=1\"style=\" position : absolute; font-size : 26px;background-color: green; color: white; padding: 20px; text-decoration: none;margin-left: 395px; border-radius: 40px\">Droite</a>";
  page += "<a href=\"/?Vitesse2=1\"style=\" position : absolute; font-size : 26px;background-color: blue; color: white; padding: 20px; text-decoration: none;margin-left:650px; border-radius: 40px\">V:2</a>";

  page += "<br> <br> <br> <br> <br>";


  page += "<a href=\"/?Recule=1\"style=\" position : absolute; font-size : 26px; background-color: green; color: white; padding: 20px; text-decoration: none;margin-left: 215px; border-radius: 40px\">Recule</a>";
  page += "<a href=\"/?Vitesse1=1\"style=\" position : absolute; font-size : 26px; background-color: blue; color: white; padding: 20px; text-decoration: none;margin-left:650px; border-radius: 40px\">V:1</a>";

  page += "</body></html>";

  return page;
}

void handleRoot() {
  if (debug)
    Serial.println ("Entree handleRoot");

  int V1 = server.arg("Vitesse1").toInt();
  if (1 == V1) {
    BoiteVitesse = 1;
  }

  int V2 = server.arg("Vitesse2").toInt();
  if (1 == V2) {
    BoiteVitesse = 2;
  }

  int V3 = server.arg("Vitesse3").toInt();
  if (1 == V3) {
    BoiteVitesse = 3;
  }

  int S1 =  server.arg("Avance").toInt();
  if (1 == S1) {
    choix = 1;
  }
  int S2 =  server.arg("Recule").toInt();
  if (1 == S2) {
    choix = 2;
  }
  int S3 =  server.arg("Gauche").toInt();
  if (1 == S3) {
    choix = 3;
  }
  int S4 =  server.arg("Droite").toInt();
  if (1 == S4) {
    choix = 4;
  }
  int S0 =  server.arg("Arret").toInt();
  if (1 == S0) {
    choix = 0;
  }

  commandeVoiture (choix);


  server.send ( 200, "text/html", getPage() );
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}

void CommandeMoteurGauche (int vitesse, int sens) {

  //int valeur = map(vitesse, 0, 3, 100, 1024);

  analogWrite (PwmGauche, valeurAcceleration[vitesse]);
  digitalWrite (SensGauche, sens);

  if (debug) {
    Serial.print ("CMD Gauche - valeur = ");
    Serial.print (valeurAcceleration[vitesse]);
    Serial.print (",  ");
    Serial.println (sens);
  }
}

void CommandeMoteurDroite (int vitesse, int sens) {

  //int valeur = map(vitesse, 0, 3, 100, 1024);

  analogWrite (PwmDroite, valeurAcceleration[vitesse]);
  digitalWrite (SensDroite, sens);

  if (debug) {
    Serial.print ("CMD Droite - valeur = ");
    Serial.print (valeurAcceleration[vitesse]);
    Serial.print (",  ");
    Serial.println (sens);
  }
}

void arret () {
  if (debug)
    Serial.println (" ||||   On s'arrete  |||");

  CommandeMoteurGauche (0, LOW);
  CommandeMoteurDroite (0, LOW);
}

void avance () {
  if (debug)
    Serial.println (" ==>   On avance  >>>");
  CommandeMoteurGauche (BoiteVitesse, HIGH);
  CommandeMoteurDroite (BoiteVitesse, HIGH);
}

void recule () {
  if (debug)
    Serial.println (" ==<   On recule  <<<");
  CommandeMoteurGauche (BoiteVitesse, LOW);
  CommandeMoteurDroite (BoiteVitesse, LOW);
}

void gauche () {
  if (debug)
    Serial.println (" ==|  On tourne a gauche  ==|");
  CommandeMoteurGauche (BoiteVitesse - 1, HIGH);
  CommandeMoteurDroite (BoiteVitesse, HIGH);

}

void droite () {
  if (debug)
    Serial.println (" |==  On tourne a droite  |==");
  CommandeMoteurGauche (BoiteVitesse, HIGH);
  CommandeMoteurDroite (BoiteVitesse - 1, HIGH);
}


void commandeVoiture (int action) {

  switch (action) {
    case 1:
      avance();
      break;

    case 2:
      recule();
      break;

    case 3:
      gauche();
      break;

    case 4:
      droite();
      break;

    default:
      arret();
  }


}

void setup()
{
  Serial.begin(115200);                      // Pour les traces

  pinMode(PwmGauche,   OUTPUT);
  pinMode(PwmDroite,   OUTPUT);
  pinMode(SensGauche, OUTPUT);
  pinMode(SensDroite, OUTPUT);

  analogWriteFreq(100);

  arret();

  // declaration du wifi:
  WiFiManager wifiManager;
  //set-up the custom IP address
  //WiFi.mode(WIFI_AP_STA);
  WiFi.mode(WIFI_AP);

  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);
  // IPAddress myIP = WiFi.softAPIP();

  server.on ( "/", handleRoot );                   // Page web
  server.on ( "/Avance", handleRoot);              // Avance
  server.on ( "/Recule", handleRoot);              // Recule
  server.on ( "/Gauche", handleRoot);              // Gauche
  server.on ( "/Droite", handleRoot);              // Droite
  server.on ( "/Arret",  handleRoot);              // Arret
  server.on ( "/inline", []() {                    // Etat du serveur
    server.send ( 200, "text/plain", "Commande Voiture OK" );
  } );
  server.onNotFound (handleNotFound);

  server.begin();

  if (debug)
    Serial.println ("      >>>>>>>>>>>>  fin du 7up  <<<<<<<<<<<<<<<<<<");

  delay (100);

}

void loop()
{
  if ( 0 == WiFi.softAPgetStationNum()) {
    arret ();

    if (debug)
      Serial.println ("Pas de console de commande trouvée");

    delay(1000);
  } else {
    server.handleClient();
  }
}
