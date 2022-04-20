
#define capteur A0
#define PinA 0 // broche enable du L298N pour le premier moteur
#define PinB 2 //  broche enable du L298N pour le deuxième moteur
#define SpeedA 5 // Premier moteur
#define SpeedB 4 // Deuxième moteur


#define NB_ITERATION        100  // itérations de la fonction delta   

#define ARRIERE 0 // recule
#define DROITE 1 // tourne à droite
#define GAUCHE 2 // tourne à gauche
#define AVANT 3 // avant
const int trigPin = D6;
const int echoPin = D7;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034

int initial ; // valeur initiale du capteur balance lumière
int dir= AVANT ; // direction tout droit

int AA=700; // target Speed
 // int AA=0; // TEST UNIQUEMENT
unsigned long tempoLampe ;        // will store last time LAMPE was updated
const long intervalLampe = 2000; 
/*
//-------------------------------- MOTEUR -----------------
class moteur {
private:
  int moteur_droit,moteur_gauche; // vitesse moteur
public:
  void start(int dir);
  void stop(void);
  void init(int v1, int v2 );
  void bip(void);
};
void moteur::start(int dir) {
               analogWrite(SpeedA,moteur_droit);analogWrite(SpeedB,moteur_gauche);
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
void moteur::stop(void) {
      analogWrite(SpeedA,0);analogWrite(SpeedB,0);
  
}
void moteur::init(int v1, int v2) {
    analogWrite(SpeedA,v1);analogWrite(SpeedB,v2);
    moteur_droit=v1 ; moteur_gauche=v2;
}
void moteur::bip(void){ // test moteur 
        digitalWrite(PinA, HIGH);digitalWrite(PinB, HIGH);
               analogWrite(SpeedA,moteur_droit);analogWrite(SpeedB,moteur_gauche);
               delay(100);
                analogWrite(SpeedA,0);analogWrite(SpeedB,0); 
                 delay(400);          
}
moteur mmm;
*/
//-------------- SET UP--------------------------
void bip(void){ // test moteur 
        digitalWrite(PinA, HIGH);digitalWrite(PinB, HIGH);
               analogWrite(SpeedA,AA);analogWrite(SpeedB,AA);
               delay(100);
                analogWrite(SpeedA,0);analogWrite(SpeedB,0); 
                 delay(400);          
}
void moteur_stop(void) {
      analogWrite(SpeedA,0);analogWrite(SpeedB,0);
  
}
void moteur_droite(void) {
      analogWrite(SpeedA,AA);analogWrite(SpeedB,AA);
      digitalWrite(PinA, HIGH);digitalWrite(PinB, LOW);
  
}
void moteur_avance(void) {
      analogWrite(SpeedA,AA);analogWrite(SpeedB,AA);
      digitalWrite(PinA, HIGH);digitalWrite(PinB, HIGH);
  
}
void setup()
{
unsigned long currentMillis = millis();
tempoLampe=currentMillis;
Serial.begin(115200);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
pinMode(PinA, OUTPUT);
pinMode(PinB, OUTPUT);
pinMode(SpeedA, OUTPUT);
pinMode(SpeedB, OUTPUT);
digitalWrite(PinA, LOW);
digitalWrite(PinB, LOW);
 //        mmm.init(AA,AA);
   //              mmm.start(ARRIERE);
  bip();  // test moteur
       delay(1000);

}
int ultrason() {
    long duration;
    digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  //   Serial.println(duration * SOUND_SPEED/2);
   if ((duration < 60000) && (duration > 1)) {   // Calculate the distance
    return( duration * SOUND_SPEED/2 ); 
   } else {
    return(0); 
   } 
}
void loop() { 
  // unsigned long currentMillis = millis();
 int Len_cm;
  Len_cm = ultrason();
     Serial.println(Len_cm);
      if (Len_cm < 18) {
      // very close object (<10 cm), stop the robot (standby)
      moteur_stop();
    } else if (Len_cm < 40) {
      // turn around
      moteur_droite();
      delay(random(220, 800));
    }
    else {
      moteur_avance();
    }
  }
