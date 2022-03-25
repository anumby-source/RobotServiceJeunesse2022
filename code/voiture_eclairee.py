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
esp.osdebug(None)

import gc
gc.collect()

ARRIERE = 0            # recule
DROITE = 1             # tourne à droite
GAUCHE = 2             # tourne à gauche
AVANT = 3              # avant
NB_ITERATION = 100     # itérations de la fonction delta
MAX = 12               # sensibilite maximum avec réglage initial MAX / 2

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

        self.bip()      # test moteur

    def bip(self):  # test moteur
        self.PinA.off()
        self.PinB.off()
        self.SpeedA.duty(self.vitesse)
        self.SpeedB.duty(self.vitesse)

        time.sleep_us(200)

        self.SpeedA.duty(0)
        self.SpeedB.duty(0)

        time.sleep_us(400)

    def patinage(self):  # 2 moteurs en marche avant pour éviter le patinage
        self.PinA.off()
        self.PinB.off()
        self.SpeedA.duty(self.vitesse)
        self.SpeedB.duty(self.vitesse)

        time.sleep_us(1)

    def stop(self):
        self.SpeedA.duty(0)
        self.SpeedB.duty(0)

    def tout_droit(self):
        self.dir = 0
        self.PinA.off()
        self.PinB.off()
        self.SpeedA.duty(self.vitesse)
        self.SpeedB.duty(self.vitesse)

    def a_gauche(self):
        # digitalWrite(PinA, HIGH)
        # digitalWrite(PinB, LOW)
        self.dir = GAUCHE
        self.patinage()
        self.PinA.off()
        self.PinB.off()
        self.SpeedA.duty(0)
        self.SpeedB.duty(self.vitesse)
        # self.PinA.on()
        # self.PinB.off()
        # self.SpeedA.duty(self.vitesse)
        # self.SpeedB.duty(self.vitesse)

    def a_droite(self):

        # digitalWrite(PinA, LOW)
        # digitalWrite(PinB, HIGH)

        self.dir = DROITE
        self.patinage()
        self.PinA.off()
        self.PinB.off()
        self.SpeedA.duty(self.vitesse)
        self.SpeedB.duty(0)
        # self.PinA.off()
        # self.PinB.on()
        # self.SpeedA.duty(self.vitesse)
        # self.SpeedB.duty(self.vitesse)

    def en_arriere(self):
        self.PinA.on()
        self.PinB.on()
        self.SpeedA.duty(self.vitesse)
        self.SpeedB.duty(self.vitesse)

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
        RR = 0

        self.LED1.on()   # turn the LED on(HIGH is the voltage level)
        self.LED2.off()  # turn the LED on(HIGH is the voltage level)

        # delayMicroseconds(300)

        for i in range(NB_ITERATION):
            RR += self.capteur.read()

        print(RR / NB_ITERATION)

        # wait for a second

        self.LED1.off() # turn the LED off(HIGH is the voltage level)
        self.LED2.on() # turn the LED on(HIGH is the level)

        # delayMicroseconds(100)

        for i in range(NB_ITERATION):
            RR -= self.capteur.read()

        print(RR / NB_ITERATION)

        self.LED1.off()
        self.LED2.off()

        return (RR / NB_ITERATION)

    def run(self):
        # temporisation de 2 s pour moteur
        # analogWrite(SpeedA, 0)
        analogWrite(SpeedB, 0)
        vvv = 0
        self.moteur.stop()
        ddd = self.delta()
        self.moteur.start()
        self.now = time.ticks_ms()
        if abs(ddd) < sensibilite:
            if dir != AVANT:
                print(ddd)
                print("tout droit")
                dir = AVANT
                self.tempoLampe = self.now
        if ddd > 0:
            vvv = map(ddd, 0, sensibilite, AA, 0)
            self.moteur.init(vvv, AA, dir)
        elif ddd < 0:
            digitalWrite(PinA, LOW)
            digitalWrite(PinB, LOW)
            vvv= map(-ddd, 0, sensibilite, AA, 0)
            self.moteur.init(AA, vvv, dir)
        else:
        ddd = ddd / 2
        if ddd > sensibilite:
            ddd = sensibilite
        if ddd < -sensibilite:
            ddd = -sensibilite
        if ddd > 0:
            vvv = map(ddd, 0, sensibilite, 0, AA)
        if dir != GAUCHE:
            print(ddd)
            print("on tourne à gauche")
            dir = GAUCHE
            self.tempoLampe = self.now

        # self.moteur.init(AA, vvv, dir)
        self.moteur.init(AA, AA, dir)
        } else if (ddd < 0) {
        vvv = map(-ddd, 0, sensibilite, 0, AA)
        if (dir != DROITE) {
        print(ddd)
        print("on tourne à droite")
        dir = DROITE
        self.tempoLampe = self.now
        }
        # self.moteur.init(vvv, AA, dir)
        self.moteur.init(AA, AA, dir)
        }
        }
        if (self.now - self.tempoLampe > self.intervalLampe) {
        print("on s'arrête")
        self.moteur.stop()
        }
        delay(10)
        print(" ")
        print(vvv)
        print(" ")
