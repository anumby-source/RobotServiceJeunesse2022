
unsigned long tempoLampe ;        // will store last time LAMPE was updated
const long intervalLampe = 2000;

class Motorisation{


  private:
    int PinA = 0; // broche enable du L298N pour le premier moteur
    int PinB = 2; //  broche enable du L298N pour le deuxième moteur
    int SpeedA = 5; // Premier moteur
    int SpeedB = 4; // Deuxième moteur
     int avant = HIGH;
     int arriere = LOW;

  public:
  Motorisation()  {
    pinMode(this->PinA, OUTPUT);
    pinMode(this->PinB, OUTPUT);
    pinMode(this->SpeedA, OUTPUT);
    pinMode(this->SpeedB, OUTPUT);
    digitalWrite(this->PinA, this->avant);
    digitalWrite(this->PinB, this->avant);
  };
  
  void bip(void)  { // test moteur
      int vitesse = 700;
      digitalWrite(this->PinA, this->avant);
      digitalWrite(this->PinB, this->avant);
      analogWrite(this->SpeedA,vitesse);
      analogWrite(this->SpeedB,vitesse);
      delay(100);
      analogWrite(this->SpeedA,0);
      analogWrite(this->SpeedB,0);
      delay(400);
  };
  
  void stop(void)  {
      analogWrite(this->SpeedA,0);
      analogWrite(this->SpeedB,0);
  };
  
  void avance(int vitesse)  {
      analogWrite(this->SpeedA,vitesse);
      analogWrite(this->SpeedB,vitesse);
      digitalWrite(this->PinA, this->avant);
      digitalWrite(this->PinB, this->avant);
  };
  
  void droite(int vitesse)  {
      analogWrite(this->SpeedA, vitesse);
      analogWrite(this->SpeedB, vitesse);
      digitalWrite(this->PinA, this->avant);
      digitalWrite(this->PinB, this->arriere);
  };
  
  void gauche(int vitesse)  {
      analogWrite(this->SpeedA, vitesse);
      analogWrite(this->SpeedB, vitesse);
      digitalWrite(this->PinA, this->arriere);
      digitalWrite(this->PinB, this->avant);
  };
  
  void recule(int vitesse)  {
      analogWrite(this->SpeedA, vitesse);
      analogWrite(this->SpeedB, vitesse);
      digitalWrite(this->PinA, this->arriere);
      digitalWrite(this->PinB, this->arriere);
  };
  
};


Motorisation M;

void sequence(int vitesse) {
    Serial.println();
    Serial.print("-----------Sequence-----------");
    Serial.println(vitesse);
    Serial.println();

    delay(2000);

    Serial.println("avance");
    M.avance(vitesse);
    delay(2000);
    M.stop();

    Serial.println("droite");
    M.droite(vitesse);
    delay(2000);
    M.stop();

    Serial.println("gauche");
    M.gauche(vitesse);
    delay(2000);
    M.stop();

    Serial.println("arriere");
    M.recule(vitesse);
    delay(2000);
    M.stop();

    delay(2000);
}

void setup()
{
    unsigned long currentMillis = millis();
    tempoLampe=currentMillis;
    Serial.begin(9600);

    delay(1000);

    Serial.println("bip");
    M.bip();

    sequence(700);
    delay(5000);

}

void loop() {
}
