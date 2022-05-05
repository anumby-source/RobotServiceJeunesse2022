//
// Moteur FA-130
// pilote L298N
//


int pin_droite = 0; // broche enable du L298N pour le premier moteur
int pin_gauche = 2; //  broche enable du L298N pour le deuxième moteur
int speed_droite = 4; // Premier moteur
int speed_gauche = 5; // Deuxième moteur

int min_pwm = 0;
int max_pwm = 255;


int marche = 0;
int vitesse = 200;


void reset(){
    pinMode(pin_droite, OUTPUT);
    pinMode(pin_gauche, OUTPUT);
    pinMode(speed_droite, OUTPUT);
    pinMode(speed_gauche, OUTPUT);
    
    digitalWrite(pin_droite, HIGH);
    digitalWrite(pin_gauche, HIGH);
    analogWrite(speed_droite, 0);
    analogWrite(speed_gauche, 0);
}

int interrupt(){
    if (Serial.available()){
       String t = Serial.readStringUntil('\n');
       int commande = t.toInt();

       Serial.print("Commande=");
       Serial.println(commande);
       return (1);
    }
    return (0);
}

void sequence () {
    Serial.println("---------------------sequence -----------------");
    Serial.println("avance");
    analogWrite(speed_droite, vitesse);
    analogWrite(speed_gauche, vitesse);
    delay(5000);
    if (interrupt()) return;
    Serial.println("stop");
    analogWrite(speed_droite, 0);
    analogWrite(speed_gauche, 0);
    delay(5000);
    if (interrupt()) return;
    Serial.println("droite");
    analogWrite(speed_droite, vitesse);
    analogWrite(speed_gauche, 0);
    delay(1000);
    if (interrupt()) return;
    Serial.println("avance");
    analogWrite(speed_droite, vitesse);
    analogWrite(speed_gauche, vitesse);
    delay(5000);
    if (interrupt()) return;
    Serial.println("stop");
    analogWrite(speed_droite, 0);
    analogWrite(speed_gauche, 0);
    delay(5000);
    if (interrupt()) return;
    Serial.println("gauche");
    analogWrite(speed_droite, 0);
    analogWrite(speed_gauche, vitesse);
    delay(1000);
    if (interrupt()) return;
    Serial.println("avance");
    analogWrite(speed_droite, vitesse);
    analogWrite(speed_gauche, vitesse);
    delay(5000);
    if (interrupt()) return;
    Serial.println("stop");
    analogWrite(speed_droite, 0);
    analogWrite(speed_gauche, 0);
    delay(5000);
}

void setup() {
     Serial.begin(115200);
     while (!Serial);

     reset();
     // sequence();
}

int loops = 0;

void loop() {
    if (interrupt()){
      loops = 10;
      Serial.println("interruption");
    }

    if (loops < 5){
       Serial.println("séquence N° ");
       Serial.println(loops);
       sequence();
       loops += 1;
    }
    else{
      Serial.println("fin de séquences");
    }

    delay(100);
}
