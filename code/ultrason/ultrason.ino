#include <Arduino_BuiltIn.h>


const int trigPin = 13; //D7;
const int echoPin = 12; //d6;
//const int trigPin = D6;
//const int echoPin = D7;
//define sound speed in cm/uS
#define SOUND_SPEED 0.034

void setup()
{
    Serial.begin(9600);

    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
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
}
