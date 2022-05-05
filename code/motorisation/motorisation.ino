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
#define V1      6
#define V2      7
#define V3      8

#define MANUEL    10
#define COLLISION 11
#define SUIVI     12
#define BALANCE   13
#define OK        14
#define TEST      15

//                  0   1               2      3                  4         5        6    7   8
String dirs[9] = { "", "Arr&ecirc;t", "Avant", "Arri&egrave;re", "Droite", "Gauche", "", "", ""};
String text_cmd[16] = { "", "STOP", "AVANCE", "RECULE", "DROITE", "GAUCHE", "V1", "V2", "V3", " ", "MANUEL", "COLLISION", "SUIVI", "BALANCE", "OK", "TEST"};
String text_mode[3] = { "MANUEL", "COLLISION", "SUIVI"};

ESP8266WebServer server(80);            // Port du serveur
int Mode = BALANCE;


//-------------------- ROBOT -------------------------------
class Robot {
  public:
    int initial ; // valeur initiale du capteur balance lumière
    int valeur;
    int sensibilite ; // sensibilité
    int commande = 0;
    int dir = STOP;
    int vitesse = 0;

    void set_commande(int commande){
      this->commande = commande;
      if (commande == STOP || commande == AVANCE || commande == RECULE) this->dir = commande;
      };

    void set_vitesse(int vitesse){
      Serial.println("Robot> init. vitesse=" + vitesse);
      this->vitesse = vitesse;
      };

    int direction(){
      return (this->dir);
    };
};


//-------------------- MOTEUR -------------------------------
class Motorisation{
  public:
     Robot* robot = NULL;
     int sens_droite = D4;  //0; // broche enable du L298N pour le premier moteur
     int sens_gauche = D3;  //2; //  broche enable du L298N pour le deuxième moteur
     int pwm_droite = D2;   //4; // Premier moteur
     int pwm_gauche = D1;   // 5; // Deuxième moteur
     int boite_vitesse = 1;
     int accel[4] = {0, 500 , 700 , 1024 };  // arret, 1ere, 2eme, 3eme

  Motorisation(Robot* robot){
     this->robot = robot;
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
     this->robot->set_commande(commande);

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
     analogWrite(this->pwm_gauche, accel[vitesse]);
     digitalWrite(this->sens_gauche, sens);
  }

  void commande_droite(int vitesse, int sens) {
     analogWrite(this->pwm_droite, accel[vitesse]);
     digitalWrite(this->sens_droite, sens);
  }

  void stop(void)  {
      Serial.println("Motorisation> stop");
      this->commande_gauche(0, LOW);
      this->commande_droite(0, LOW);
  };

  void avance()  {
      Serial.println("Motorisation> avance");
      this->robot->set_commande(AVANCE);
      this->stop();
      delay(100);
      this->commande_gauche(this->boite_vitesse, HIGH);
      this->commande_droite(this->boite_vitesse, HIGH);
  };

  void recule()  {
      Serial.println("Motorisation> recule");
      this->robot->set_commande(RECULE);
      this->stop();
      delay(100);
      this->commande_gauche(this->boite_vitesse, LOW);
      this->commande_droite(this->boite_vitesse, LOW);
  };

  void gauche()  {
      Serial.println("Motorisation> gauche dir=" + String(this->robot->dir));
      this->stop();
      delay(100);
      this->commande_gauche(this->boite_vitesse - 1, HIGH);
      this->commande_droite(this->boite_vitesse, HIGH);
  };

  void droite()  {
      Serial.println("Motorisation> droite dir=" + String(this->robot->dir));
      this->stop();
      delay(100);
      this->commande_gauche(this->boite_vitesse, HIGH);
      this->commande_droite(this->boite_vitesse - 1, HIGH);
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

  int lecture(){
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
    int obstacle = this->lecture();
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
class Optique{
private:
    Robot* robot = NULL;
    Motorisation* motor = NULL;
    int capteur = A0;         // lecture

public:
    int sensibilite = 10;     // seuil de sensibilite droite/gauche
    int balance_faite = 0;
    long balance = 0;    // par défaut. En faisant la balance des blancs on peut ajuster cette valeur
    long min_cps = 1000;
    long max_cps = -1000;
    long mid_cps = 1000;

   Optique(Robot* robot, Motorisation* motor){
       this->robot = robot;
       this->motor = motor;
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

     //Serial.print("Balance ");
     //Serial.print(this->min_cps);
     //Serial.print(",");
     //Serial.print(this->mid_cps);
     //Serial.print(",");
     //Serial.print(this->max_cps);
     //Serial.println("");
  
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

   void action(){
       int vitesse;
       int dir = 0;

       // this->motor->stop();

       int delta = this->delta();
       this->motor->avance();

       // on calcule la vitesse à appliquer proportionnelle au delta
       vitesse = map(abs(delta), 0, 512, 0, 4);

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
             this->motor->droite();
          } else if (delta < 0) {
             // lumière à gauche => il faut redresser vers la gauche
             this->motor->gauche();
          }
          delay(10);
          this->motor->avance();
       }
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
    
        Serial.print("Initialise le réseau ");
        Serial.print(ssid);
        Serial.print(" adresse ");
        Serial.println(apIP);
    
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
              <td> <button name='LED0' class='cmd' value='") + String(TEST) + String("' type='submit'> Test </button></td> \
          </tr> \
      </table> \
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
              <td> <button name='LED0' class='cmd' value='") + String(TEST) + String("' type='submit'> Test </button></td> \
          </tr> \
      </table> \
    </form> ") + this->trailer();
      return (html);
    }
};

Robot R;
Motorisation M(&R);
Optique O(&R, &M);
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

Coyote C;

void auto_test(){
    unsigned long currentMillis = millis();
    int flag = 1;
    M.bip();
    C.init();
    if (O.lecture() > 10) M.bip();
    if (abs(O.lecture() - 512) < 100 ) {
       M.bip();
       //balance_des_blancs();
       Serial.print("Balance des blancs");
       Serial.println(O.balance);
    }
    M.boite_vitesse = 1;
    while(flag) {
          int retour = U.action();
          //  Serial.println(U.lecture());
          Mode = retour;
          if (retour == STOP) {
              Mode = MANUEL;
              flag = 0;
          } else if (retour == AVANCE) {
              int delta;
              Serial.print("delta :");
              delta = O.delta();
              Serial.print(delta);
              Serial.print(" distance :");
              Serial.println(U.lecture());
              if (abs(delta) < 50)  {
                    M.avance();
              } else {
                   C.blink();
                   if (C.flag) delta = - delta;
                   if (delta > 0 ) {
                        // lumière à droite => il faut redresser vers la droite
                        M.droite();
                   } else if (delta < 0) {
                        // lumière à gauche => il faut redresser vers la gauche
                        M.gauche();
                   }
               }
          } else {
              U.action();
          }
     }
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
      Serial.print("Mode=");
      Serial.print(Mode);
      Serial.print(" commande=");
      Serial.print(commande);
      Serial.println("");
      if (first == 1) {
        first = 0;
        Mode = BALANCE;
        commande = 0;
      }
      else {
          if (Mode == MANUEL) {
            if (commande == COLLISION) Mode = COLLISION;
            else if (commande == SUIVI) Mode = SUIVI;
            else M.action(commande);
          }
          else if (Mode == BALANCE) {
            if (commande == BALANCE) O.balance_des_blancs();
            else if (commande == OK) Mode = MANUEL;
          }
          else if (Mode == COLLISION) {
            if (commande == MANUEL) Mode = MANUEL;
            else if (commande == TEST) {
              auto_test();
              Mode = COLLISION;
            }
            else if (commande == SUIVI) {
              Mode = SUIVI;
            }
            U.action();
          }
          else if (Mode == SUIVI) {
            if (commande == MANUEL) Mode = MANUEL;
            else if (commande == TEST) {
              auto_test();
              Mode = SUIVI;
            }
            else if (commande == COLLISION) {
              Mode = COLLISION;
            }
            O.action();
          }
       }
    }
  }
  Serial.print("handleRoute> ");
  Serial.println(message);

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
    server.handleClient();

    if (Serial.available()){
       String t = Serial.readStringUntil('\n');
       int commande = t.toInt();
       Serial.print("Commande=");
       Serial.println(commande);
       M.action(commande);
    }

    delay(100);
}
