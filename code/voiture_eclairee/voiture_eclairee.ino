
#define capteur A0
#define PinA 0 // broche enable du L298N pour le premier moteur
#define PinB 2 //  broche enable du L298N pour le deuxième moteur
#define SpeedA 5 // Premier moteur
#define SpeedB 4 // Deuxième moteur
#define LED1        D6 // On Trinket or Gemma, suggest changing this to 1
#define LED2        D7    // On Trinket or Gemma, suggest changing this to 1

#define NB_ITERATION        100  // itérations de la fonction delta   

#define ARRIERE 0 // recule
#define DROITE 1 // tourne à droite
#define GAUCHE 2 // tourne à gauche
#define AVANT 3 // avant

#define MAX 12 // sensibilite maximum avec réglage initial MAX / 2



int initial ; // valeur initiale du capteur balance lumière
int dir= AVANT ; // direction tout droit

int AA=700; // target Speed
int sensibilite ; // sensibilité
char buffer[30];
unsigned long tempoLampe ;        // will store last time LAMPE was updated
const long intervalLampe = 2000; 

class moteur {
private:
  int moteur_droit,moteur_gauche; // vitesse moteur
public:
  void start(void);
  void stop(void);
  void init(int v1, int v2, int dir);
};
void moteur::start(void) {
               analogWrite(SpeedA,moteur_droit);analogWrite(SpeedB,moteur_gauche);
}
void moteur::stop(void) {
      analogWrite(SpeedA,0);analogWrite(SpeedB,0);
  
}
void moteur::init(int v1, int v2, int dir) {
    analogWrite(SpeedA,v1);analogWrite(SpeedB,v2);
    moteur_droit=v1 ; moteur_gauche=v2;
    switch(dir) {
    case AVANT:
            digitalWrite(PinA, LOW);digitalWrite(PinB, LOW);
    break;
        case DROITE:
            digitalWrite(PinA, LOW);digitalWrite(PinB, HIGH);
    break;
        case GAUCHE:
            digitalWrite(PinA, HIGH);digitalWrite(PinB,LOW );
    break;
        case ARRIERE:
            digitalWrite(PinA, HIGH);digitalWrite(PinB, HIGH);
    break;
  }
}
moteur mmm;

void setup()
{
unsigned long currentMillis = millis();
tempoLampe=currentMillis;
Serial.begin(115200);
pinMode(PinA, OUTPUT);
pinMode(PinB, OUTPUT);
pinMode(SpeedA, OUTPUT);
pinMode(SpeedB, OUTPUT);
digitalWrite(PinA, LOW);
digitalWrite(PinB, LOW);
pinMode(LED1, OUTPUT);
pinMode(LED2, OUTPUT);
 
  bip();  // test moteur
   initial = analogRead(capteur);
         if (initial> 20){   // lecture des valeurs initiales (on suppose que les capteurs sont de part et d'autre de la ligne)
             bip();
         }
     sensibilite=MAX / 2; // sensibilite maximum avec réglage initial
       delay(1000);
        mmm.init(AA,AA,dir);
}
int delta() { // difference entre leds
  long RR=0 ;
  int i= NB_ITERATION;

  digitalWrite(LED1, HIGH);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED2, LOW);   // turn the LED on (HIGH is the voltage level)
//  delayMicroseconds(300); 
      while (i--) RR += analogRead(capteur); 
        Serial.print(RR/NB_ITERATION);
                Serial.print(" ");
                      // wait for a second
  digitalWrite(LED1, LOW);   // turn the LED off (HIGH is the voltage level)
    digitalWrite(LED2, HIGH);   // turn the LED on (HIGH is the voltage level)
 // delayMicroseconds(100);  
      i= NB_ITERATION;while (i--) RR -= analogRead(capteur); 
          Serial.println(RR/NB_ITERATION);
               
  digitalWrite(LED1, LOW);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED2, LOW); 
 return ( RR/NB_ITERATION );
             
}
void bip(){ // test moteur 
        digitalWrite(PinA, LOW);digitalWrite(PinB, LOW);
               analogWrite(SpeedA,AA);analogWrite(SpeedB,AA);
                           delay(200);
                analogWrite(SpeedA,0);analogWrite(SpeedB,0); 
                 delay(400);          
}
void loop()
{ // temporisation de 2s pour moteur
//  analogWrite(SpeedA, 0); analogWrite(SpeedB, 0);
int vvv;
  mmm.stop();
  int ddd = delta();
  mmm.start();
    unsigned long currentMillis = millis();
 if (abs(ddd) < sensibilite)  {
    if(dir!= AVANT){ 
      Serial.println(ddd);Serial.println("tout droit");
      dir=AVANT; tempoLampe=currentMillis;
    }
      if ( ddd > 0 ) {

                  vvv= map(ddd, 0, sensibilite, AA,0);
                  mmm.init(vvv,AA,dir);
    } else if (ddd < 0) {
           digitalWrite(PinA, LOW);digitalWrite(PinB, LOW); 
                             vvv= map(-ddd, 0, sensibilite, AA,0);
                               mmm.init(AA,vvv,dir);
    }
  } else { 
    ddd = ddd / 2; 
    if ( ddd > sensibilite ) ddd = sensibilite;
    if ( ddd < -sensibilite ) ddd = -sensibilite;
    if ( ddd > 0 ) {
      vvv= map(ddd, 0, sensibilite, 0,AA);
      if(dir!= GAUCHE){ 
      Serial.print(ddd);Serial.println("on tourne à gauche");
      dir= GAUCHE; tempoLampe=currentMillis;  
    }  
    // mmm.init(AA,vvv,dir);
       mmm.init(AA,AA,dir);
  }   else if (ddd < 0) {
    vvv= map(-ddd, 0, sensibilite, 0,AA);
    if(dir!= DROITE){ 
     Serial.print(ddd);Serial.println("on tourne à droite");
         dir=DROITE; tempoLampe=currentMillis;  
    } 
    // mmm.init(vvv,AA,dir);
       mmm.init(AA,AA,dir);
  } 
  }
     if(currentMillis - tempoLampe > intervalLampe) {
          Serial.println("on s'arrête");
            mmm.stop();
   } 
  delay(10);
  Serial.print(" ");
  Serial.print(vvv);
    Serial.print(" ");
}
