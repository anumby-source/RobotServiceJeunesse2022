#
# Librairie permettant d’utiliser le capteur de distance ultrason HC-SR04
# Tester sur NodeMCU Lolin et Wemos D1 mini
#
# Auteur iTechnoFrance
#
# pin_trig : pin pour envoyer les impulsions
# pin_echo : pin pour mesurer la distance
#
# Mesure la distance entre 2 cm et 4 m
#
from machine import Pin, PWM
import machine, time

class HCSR04():
    def __init__(self, pin_echo, pin_trig):
        self.pin_echo = pin_echo
        self.pin_trig = Pin(pin_trig, Pin.OUT)  # definit la broche TRIGGER en sortie
        self.pin_echo = Pin(pin_echo, Pin.IN)  # definit la broche ECHO en entree
        self.pin_trig.off()  # broche TRIGGER niveau bas au repos
        # on definit un Timeout si le retour echo depasse 23.8 ms
        # 410 cm  * 2 (aller/retour) * 29.1 (vitesse du son 1cm = 29.1us)
        tmo = int(410 * 2 * 29.1)
        self.tmo = tmo

    def lecture_distance(self):
        self.pin_trig.off()  # broche TRIGGER niveau bas au repos
        time.sleep_us(2)  # on attend 20us
        self.pin_trig.on()  # broche TRIGGER niveau haut
        time.sleep_us(10)  # pendant 10us
        self.pin_trig.off()
        self.temps_distance = machine.time_pulse_us(self.pin_echo, 1, self.tmo)
        # print("temps_distance", self.temps_distance)
        if (self.temps_distance == -1):
            # Timeout, mesure distance > 4m
            self.calcul_distance_cm = -1
        else:
            # le temps que met l’echo est a diviser par 2 (aller-retour)
            # la vitesse du son est de 0.034320 cm/us (343.2 m/s)
            # pour 1 cm donc on obtient 29.1 us
            self.calcul_distance_cm = (self.temps_distance / 2) / 29.1
        return(self.calcul_distance_cm)

#
#                   Mesure de distance
#
# Programme permettant d’utiliser la librairie qui gere le capteur de
# distance ultrason HC-SR04
# Tester sur NodeMCU Lolin et Wemos D1 mini
#
# Auteur iTechnoFrance
#
# pin_echo : pin pour mesurer la distance
# pin_trig : pin pour envoyer les impulsions





pin_echo = 4  # sortie D2 –> GPIO04
pin_trigger = 5  # sortie D1 –> GPIO05

# initialisation librairie HC-SR04

hc_sr04 = HCSR04(pin_echo, pin_trigger)

while True:
    distance = hc_sr04.lecture_distance()
    if (distance == -1):
        print ("Mesure supérieure à 4 mètres")
    else:
        print ((distance), "cm")
    time.sleep(0.1)



