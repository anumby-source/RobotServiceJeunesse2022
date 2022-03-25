"""
 Première version du code de contrôle du robot avec éclairage
   Arnaud Reichart
 version MicroPython
    Chris Arnault
"""

import time
from machine import ADC
from machine import Pin, PWM
import esp
import gc


ARRIERE = 0            # recule
DROITE = 1             # tourne à droite
GAUCHE = 2             # tourne à gauche
AVANT = 3              # avant
NB_ITERATION = 100     # itérations de la fonction delta
MIN = 0
MAX = 12               # sensibilite maximum avec réglage initial MAX / 2

esp.osdebug(None)
gc.collect()


def map_vitesse(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min


class Moteur(object):
    def __init__(self):

        self.PinA = Pin(0, Pin.OUT)  # broche enable du L298N pour le premier moteur
        self.PinB = Pin(2, Pin.OUT)  # broche enable du L298N pour le deuxième moteur
        self.SpeedA = PWM(Pin(5, Pin.OUT), freq=1000)  # Premier moteur
        self.SpeedB = PWM(Pin(4, Pin.OUT), freq=1000)  # Deuxième moteur

        self.PinA.off()
        self.PinB.off()

        self.vitesse = 600  # target Speed
        self.dir = AVANT    # direction 0: tout droit

        self.moteur_droit = self.vitesse
        self.moteur_gauche = self.vitesse

        self.bip()      # test moself.moteur_droself.moteur_droit

    def init(self, v1, v2, direction):
        self.moteur_droit = v1
        self.moteur_gauche = v2
        self.SpeedA.duty(self.moteur_droit)
        self.SpeedB.duty(self.moteur_gauche)

        if direction == AVANT:
            self.PinA.off()
            self.PinB.off()
        elif direction == DROITE:
            self.PinA.off()
            self.PinB.on()
        elif direction == GAUCHE:
            self.PinA.on()
            self.PinB.off()
        elif direction == ARRIERE:
            self.PinA.on()
            self.PinB.on()

        self.dir = direction

    def bip(self):  # test moteur
        self.PinA.off()
        self.PinB.off()
        self.SpeedA.duty(self.moteur_droit)
        self.SpeedB.duty(self.moteur_gauche)

        time.sleep_us(200)

        self.SpeedA.duty(0)
        self.SpeedB.duty(0)

        time.sleep_us(400)

    def start(self):  # 2 moteurs en marche avant pour éviter le patinage
        self.PinA.off()
        self.PinB.off()
        self.SpeedA.duty(self.moteur_droit)
        self.SpeedB.duty(self.moteur_gauche)

        time.sleep_us(1)

    def stop(self):
        self.SpeedA.duty(0)
        self.SpeedB.duty(0)

    def tout_droit(self):
        self.dir = 0
        self.PinA.off()
        self.PinB.off()
        self.SpeedA.duty(self.moteur_droit)
        self.SpeedB.duty(self.moteur_gauche)

    def a_gauche(self):
        # self.PinA.on()
        # self.PinB.off()
        self.dir = GAUCHE
        self.start()
        self.PinA.off()
        self.PinB.off()
        self.SpeedA.duty(0)
        self.SpeedB.duty(self.moteur_gauche)
        # self.PinA.on()
        # self.PinB.off()
        # self.SpeedA.duty(self.moteur_droit)
        # self.SpeedB.duty(self.moteur_gauche)

    def a_droite(self):

        # self.PinA.off()
        # self.PinB.on()

        self.dir = DROITE
        self.start()
        self.PinA.off()
        self.PinB.off()
        self.SpeedA.duty(self.moteur_droit)
        self.SpeedB.duty(0)
        # self.PinA.off()
        # self.PinB.on()
        # self.SpeedA.duty(self.moteur_droit)
        # self.SpeedB.duty(self.moteur_gauche)

    def en_arriere(self):
        self.PinA.on()
        self.PinB.on()
        self.SpeedA.duty(self.moteur_droit)
        self.SpeedB.duty(self.moteur_gauche)

    def accelere(self, acceleration):
        self.vitesse += acceleration
        if self.vitesse > 1023:
            self.vitesse = 1023
        if self.vitesse < 0:
            self.vitesse = 0


class Robot(object):
    def __init__(self):
        self.now = time.ticks_ms()

        self.capteur = ADC(0)

        self.LED1 = Pin(6, Pin.OUT)
        self.LED2 = Pin(7, Pin.OUT)

        # self.sensibilite = MAX   # écart admissible par rapport à la valeur initiale
        self.initial = 0  # valeur initiale du capteur balance lumière

        self.sensibilite = MIN     # sensibilité
        self.tempoLampe = 0        # will store last time LAMPE was updated
        self.intervalLampe = 2000

        self.initial = self.capteur.read()

        self.moteur = Moteur()
        # lecture des valeurs initiales (on suppose que les capteurs sont de part et d'autre de la ligne)
        if self.initial > 20:
            self.moteur.bip()

        self.sensibilite = MAX / 2    # sensibilite maximum avec réglage initial

        time.sleep_us(1000)

    def delta(self):
        # difference entre leds long
        value = 0

        self.LED1.on()   # turn the LED on(HIGH is the voltage level)
        self.LED2.off()  # turn the LED on(HIGH is the voltage level)

        # delayMicroseconds(300)

        for i in range(NB_ITERATION):
            value += self.capteur.read()

        print(value / NB_ITERATION)

        # wait for a second

        self.LED1.off()    # turn the LED off(HIGH is the voltage level)
        self.LED2.on()     # turn the LED on(HIGH is the level)

        # delayMicroseconds(100)

        for i in range(NB_ITERATION):
            value -= self.capteur.read()

        print(value / NB_ITERATION)

        self.LED1.off()
        self.LED2.off()

        return value / NB_ITERATION

    def run(self):
        # temporisation de 2 s pour moteur

        vitesse = 0
        self.moteur.stop()
        delta = self.delta()
        self.moteur.start()
        self.now = time.ticks_ms()
        if abs(delta) < self.sensibilite:
            if self.moteur.dir != AVANT:
                print(delta)
                print("tout droit")
                self.moteur.dir = AVANT
                self.tempoLampe = self.now
            if delta > 0:
                vitesse = map_vitesse(delta, 0, self.sensibilite, self.moteur.vitesse, 0)
                self.moteur.init(vitesse, self.moteur.vitesse, self.moteur.dir)
            elif delta < 0:
                self.moteur.PinA.off()
                self.moteur.PinB.off()
                vitesse = map_vitesse(-delta, 0, self.sensibilite, self.moteur.vitesse, 0)
                self.moteur.init(self.moteur.vitesse, vitesse, self.moteur.dir)
        else:
            delta = delta / 2
            if delta > self.sensibilite:
                delta = self.sensibilite
            if delta < -self.sensibilite:
                delta = -self.sensibilite
            if delta > 0:
                vitesse = map_vitesse(delta, 0, self.sensibilite, 0, self.moteur.vitesse)
                if self.moteur.dir != GAUCHE:
                    print(delta)
                    print("on tourne à gauche")
                    self.moteur.dir = GAUCHE
                    self.tempoLampe = self.now

                # self.moteur.init(self.moteur.vitesse, vitesse, self.moteur.dir)
                self.moteur.init(self.moteur.vitesse, self.moteur.vitesse, self.moteur.dir)
            elif delta < 0:
                vitesse = map_vitesse(-delta, 0, self.sensibilite, 0, self.moteur.vitesse)
                if self.moteur.dir != DROITE:
                    print(delta, "on tourne à droite")
                    self.moteur.dir = DROITE
                    self.tempoLampe = self.now
                # self.moteur.init(vitesse, self.moteur.vitesse, self.moteur.dir)
                self.moteur.init(self.moteur.vitesse, self.moteur.vitesse, self.moteur.dir)
        if (self.now - self.tempoLampe) > self.intervalLampe:
            print("on s'arrête")
            self.moteur.stop()

        time.sleep_us(100)
        print(vitesse)
