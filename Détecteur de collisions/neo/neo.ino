// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library
// A REICHART : allume une led 
// Rouge si distance < 10 cm
// Verte  si distance > 40 cm
// PIN D-IN placée sur entrée TRIGGER du capteur ultrason

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            D5

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      1

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);



//-------------------- ULTRASON  -------------------------------

class Ultrason{
  private:
#define SOUND_SPEED 0.034
    const int trigger = D6;
    const int echo = D7;
    //define sound speed in cm/uS
 
  public:
     const int max = 40;  // si on est > seuil1 on avance, si non on tourne à droite
    const int min = 10;  // si on est < seuil2 on stop car on n'a plus la place de tourner

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

     //   Serial.println(duration * SOUND_SPEED/2);
     if ((duration < 60000) && (duration > 1)) {
        // Calcule la distance quand la lecture est vraisemblable
        return (duration * SOUND_SPEED/2);
    } else {
        return (0);
    };
  };
};
Ultrason U;
//------------------NEO --------------
#define NB_COLOR 4 // nombre de couleurs

void neo(int color){ 
  if (color <0 ) color=0;
  if (color > NB_COLOR) color= NB_COLOR;
  switch(color){
        case 0 :// rouge
          pixels.setPixelColor(0, pixels.Color(0, 150, 0)); break;
        case 1 ://jaune
          pixels.setPixelColor(0, pixels.Color(150, 250, 0)); break;
        case 2 :// blanc
        pixels.setPixelColor(0, pixels.Color(150, 150, 150)); break;
        case 3 :// bleu
          pixels.setPixelColor(0, pixels.Color(0, 0, 150)); break;
       case 4 ://vert
          pixels.setPixelColor(0, pixels.Color(150, 0, 0)); break;
  }
    pixels.show();   // Send the updated pixel colors to the hardware.
}

void setup() {
   Serial.begin(115200);
        Serial.println("NEO");
  pixels.begin(); // This initializes the NeoPixel library.
    neo(1); delay(100);
    neo(2); delay(100);
      neo(3); delay(100);
      neo(-1); delay(100);
}

void loop() {
      int u;
      Serial.println(u = U.read());    
      neo ( (NB_COLOR * ( u - U.min)) / (U.max - U.min));

      
      delay(600);
  
}
