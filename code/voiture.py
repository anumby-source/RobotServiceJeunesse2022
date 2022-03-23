"""
 Première version du code de contrôle du robot
    Arnaud Reichart
 version MicroPython
    Chris Arnault
"""

# include <ESP8266WiFi.h>
# include <DNSServer.h>
# include <ESP8266WebServer.h>
# //  # include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager

import time
from machine import ADC
from machine import Pin, PWM

try:
  import usocket as socket
except:
  import socket

import network

import esp
esp.osdebug(None)

import gc
gc.collect()

DROITE = 1        # tourne à droite
GAUCHE = 2        # tourne à gauche
MILIEU = 510      # pont de résistance 1024 / 2 environ
MAX = 100         # sensibilite maximum avec réglage initial MAX / 2
MIN = 0           # sensibilite min

class Robot(object):
    def __init__(self):
        self.capteur = ADC(0)

        self.PinA = Pin(0, Pin.OUT)  # broche enable du L298N pour le premier moteur
        self.PinB = Pin(2, Pin.OUT)  # broche enable du L298N pour le deuxième moteur
        self.SpeedA = PWM(Pin(5, Pin.OUT), freq=1000)  # Premier moteur
        self.SpeedB = PWM(Pin(4, Pin.OUT), freq=1000)  # Deuxième moteur

        # self.sensibilite = MAX   # écart admissible par rapport à la valenitiale
        self.initial = 0  # valeur initiale du capteur balance lumière
        self.dir = 0  # direction 0: tout droit

        self.AA = 600 # target Speed
        self.sensibilite = MIN # sensibilité
        self.tempoLampe = 0 # will store last time LAMPE was updated
        self.intervalLampe = 2000

        self.PinA.off()
        self.PinB.off()

        self.bip() # test moteur
        self.initial = self.capteur.read()
        if self.initial > 100: # lecture des valeurs initiales (on suppose que les capteurs sont de part et d'autre de la ligne)
            self.bip()

        if abs(self.initial - MILIEU) < (MAX / 4):
            self.bip()
        else:
            self.initial = MILIEU

        self.sensibilite = MAX / 2 # sensibilite maximum avec réglage initial

        time.sleep_us(1000)

    def bip(self):  # test moteur
        self.PinA.off()
        self.PinB.off()
        self.SpeedA.duty(self.AA)
        self.SpeedB.duty(self.AA)

        time.sleep_us(200)

        self.SpeedA.duty(0)
        self.SpeedB.duty(0)

        time.sleep_us(400)

    def patinage(self):  # 2 moteurs en marche avant pour éviter le patinage
        self.PinA.off()
        self.PinB.off()
        self.SpeedA.duty(self.AA)
        self.SpeedB.duty(self.AA)

        time.sleep_us(1)

    def capteur_operate(self):
        # temporisation de 2s pour moteur
        valeur = self.capteur.read()

        # temporisation de 2s pour moteur
        self.currentMillis = millis()

        if (self.currentMillis - self.tempoLampe) > self.intervalLampe:
            # print("on s'arrête");
            self.SpeedA.duty(0)
            self.SpeedB.duty(0)

        if abs(valeur - self.initial) < self.sensibilite:
            if self.dir != 0:
                print(valeur)
                print("tout droit")
                self.dir = 0
                self.tempoLampe = self.currentMillis
                self.PinA.off()
                self.PinB.off()
                self.SpeedA.duty(self.AA)
                self.SpeedB.duty(self.AA)
        else:
            if valeur > self.initial:
                if self.dir != GAUCHE:
                    print(valeur)
                    print("on tourne à gauche")
                    self.dir = GAUCHE
                    self.tempoLampe = self.currentMillis
                    self.patinage()
                    self.PinA.off()
                    self.PinB.off()
                    self.SpeedA.duty(0)
                    self.SpeedB.duty(self.AA)
                    # self.PinA.on()
                    # self.PinB.off()
                    # self.SpeedA.duty(AA)
                    # self.SpeedB.duty(AA)
            elif valeur < self.initial:
                if self.dir != DROITE:
                    print(valeur)
                    print("on tourne à droite")
                    self.dir = DROITE
                    self.tempoLampe = self.currentMillis
                    self.patinage()
                    self.PinA.off()
                    self.PinB.off()
                    self.SpeedA.duty(self.AA)
                    self.SpeedB.duty(0)
                    # self.PinA.off()
                    # self.PinB.on()
                    # self.SpeedA.duty(AA)
                    # self.SpeedB.duty(AA)

    def server_operate(self):
        client = server.available()
        if not client: return

        request = client.readStringUntil('\r')

        # -----------------PAVE HAUT------------
        if request.indexOf("LED0=1") != -1:
            self.AA += 50
            if self.AA > 1023: self.AA = 1023
        if request.indexOf("LED0=2") != -1:
            self.tempoLampe = self.currentMillis
            self.PinB.off()
            self.PinA.off()
            self.SpeedA.duty(self.AA)
            self.SpeedB.duty(self.AA)
        if request.indexOf("LED0=3") != -1:
            self.sensibilite += 10
            if self.sensibilite > MAX: self.sensibilite = MAX

        # -----------------PAVE CENTRE------------
        if request.indexOf("LED0=4") != -1:
            self.tempoLampe = self.currentMillis
            self.PinA.off()
            self.PinB.on()
            self.SpeedA.duty(self.AA)
            self.SpeedB.duty(self.AA)
        if request.indexOf("LED0=5") != -1:
            self.SpeedA.duty(0)
            self.SpeedB.duty(0)
        if request.indexOf("LED0=6") != -1:
            self.tempoLampe = self.currentMillis
            self.PinA.on()
            self.PinB.off()
            self.SpeedA.duty(self.AA)
            self.SpeedB.duty(self.AA)

        # -----------------PAVE BAS------------
        if request.indexOf("LED0=7") != -1:
            self.AA -= 50
            if self.AA < 0: self.AA = 0
        if request.indexOf("LED0=8") != -1:
            self.tempoLampe = self.currentMillis
            self.PinA.on()
            self.PinB.on()
            self.SpeedA.duty(self.AA)
            self.SpeedB.duty(self.AA)
        if request.indexOf("LED0=9") != -1:
            self.sensibilite -= 10
            if self.sensibilite < MIN: self.sensibilite = MIN

        # Affichage de la sensibilite
        valeur = self.capteur.read()
        sprintf(buffer, " M=%d (%d) B=%d", self.AA, valeur - self.initial, self.sensibilite)
        # print(buffer)
        client.print(self.web_page())
        request = ""

    def web_page(self):
        html = """
<!DOCTYPE html>
<html>
<head>
<style> .button { padding:10px 10px 10px 10px; width:100%;  background-color: green; font-size: 500%;color:white;} </style>
<center>
<h1>ARNAUD</h1>
  <form>
      <TABLE BORDER>
          <TR>
              <TD> <button name='LED0' class='button' value='1' type='submit'> << </button></TD>
              <TD> <button name='LED0' class='button' value='2' type='submit'> ^ </button></TD>
              <TD> <button name='LED0' class='button' value='3' type='submit'> >> </button></TD>
          </TR>
          <TR>
              <TD> <button name='LED0' class='button' value='4' type='submit'> < </button></TD>
              <TD> <button name='LED0' class='button' value='5' type='submit'> ooo </button></TD>
              <TD> <button name='LED0' class='button' value='6' type='submit'> > </button></TD>
          </TR>
          <TR>
              <TD> <button name='LED0' class='button' value='7' type='submit'> << </button></TD>
              <TD> <button name='LED0' class='button' value='8' type='submit'> v </button></TD>
              <TD> <button name='LED0' class='button' value='9' type='submit'> >> </button></TD>
          </TR>
      </TABLE>
  </form>
</center>
</head> 
</html>"""


        h = """
<html>
<head>
  <title>ESP Web Server</title> 
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,"> 
  <style>html{font-family: Helvetica; display:inline-block; margin: 0px auto; text-align: center;}
  h1{color: #0F3376; padding: 2vh;}
  p{font-size: 1.5rem;}.button{display: inline-block; 
                               background-color: #e7bd3b; 
                               border: none;
                               border-radius: 4px; 
                               color: white; 
                               padding: 16px 40px; 
                               text-decoration: none; 
                               font-size: 30px; 
                               margin: 2px; 
                               cursor: pointer;}
                        .button2{background-color: #4286f4;}</style>
</head>
<body> 
  <h1>ESP Web Server</h1> 
  <p>GPIO state: <strong>""" + gpio_state + """</strong></p>
  <p>
      <a href="/?led=on"><button class="button">ON</button></a>
  </p>
  <p>
      <a href="/?led=off"><button class="button button2">OFF</button></a>
  </p>
</body>
</html>
    """
        return html






IPAddress apIP(44, 44, 44, 44)  # Définition de l'adresse IP statique.
# const char * ssid = "RCO"; # Nom du reseau wifi(***A modifier ***)
password = "12345678" # mot de passe (***A modifier ***)
# ESP8266WebServer server(80)
buffer = bytearray[30]


def setup():
    currentMillis = millis()
    ssid = bytearray[30]
    sprintf(ssid, "RCO_%d", ESP.getChipId())


    # declaration du wifi:
    # WiFi.mode(WIFI_AP)
    # WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0))
    # WiFi.softAP(ssid, password)

    ssid = 'REPLACE_WITH_YOUR_SSID'

    station = network.WLAN(network.STA_IF)

    station.active(True)
    station.connect(ssid, password)

    while station.isconnected():
        pass

    print('Connection successful')
    print(station.ifconfig())

    # if you get here you have connected to the WiFi
    print("Connected.")

    server.begin()




robot = Robot()
while True:
    robot.capteur_operate()
    robot.server_operate()

    time.sleep_us(100)


