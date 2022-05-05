//
// Code auto test des capteurs pour le Robot pour l'événement Service Jeunesse 2022
//
// le coeur du code est hérité du code développé par Arnaud
// Reformatté et structuré pour l'algorithmique en C++ par Chris


#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

class Ultrason{
//define sound speed in cm/uS
#define SOUND_SPEED 0.034

  private:
    const int trigger = 13; // D7;
    const int echo = 12;    // D6;
    const int seuil1 = 40;  // si on est > seuil1 on avance, sinon on tourne à droite
    const int seuil2 = 10;  // si on est < seuil2 on stop car on n'a plus la place de tourner

  public:
  Ultrason(){
    pinMode(this->trigger, OUTPUT); // Sets the trigger as an Output
    pinMode(this->echo, INPUT); // Sets the echo as an Input
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
};

Ultrason U;

void setup()
{
   Serial.begin(115200);
   delay(2000);
}

void loop()
{
   int valeur = U.read();
   Serial.println(valeur);
   delay(100);
}
