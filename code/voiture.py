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


class Sever(object):
    def __init__(self):
        pass

    def do_connect(self):
        ssid = "RCO_{ssid}".format(ssid=machine.unique_id())
        password = "12345678"  # mot de passe (***A modifier ***)
        wlan = network.WLAN(network.STA_IF)
        wlan.active(True)
        if not wlan.isconnected():
            print('connecting to network...')
            wlan.connect(ssid, password)
            while not wlan.isconnected():
                pass
        print('network config:', wlan.ifconfig())

        """
        IPAddress apIP(44, 44, 44, 44)  # Définition de l'adresse IP statique.
        """

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

        if led.value() == 1:
            gpio_state = "ON"
        else:
            gpio_state = "OFF"

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

    def listen(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.bind(('', 80))
        s.listen(5)

        while True:
            conn, addr = s.accept()
            print('Got a connection from %s' % str(addr))
            request = conn.recv(1024)
            request = str(request)
            print('Content = %s' % request)
            position = request.find('LED0=')
            if position > 0:
                answer = request.split("LED0=")[1][0]
                return answer

            """
            response = web_page()
            conn.send('HTTP/1.1 200 OK\n')
            conn.send('Content-Type: text/html\n')
            conn.send('Connection: close\n\n')
            conn.sendall(response)
            conn.close()
            """


class Moteur(object):
    def __init__(self):
        self.PinA = Pin(0, Pin.OUT)  # broche enable du L298N pour le premier moteur
        self.PinB = Pin(2, Pin.OUT)  # broche enable du L298N pour le deuxième moteur
        self.SpeedA = PWM(Pin(5, Pin.OUT), freq=1000)  # Premier moteur
        self.SpeedB = PWM(Pin(4, Pin.OUT), freq=1000)  # Deuxième moteur

        self.PinA.off()
        self.PinB.off()

        self.vitesse = 600 # target Speed
        self.dir = 0    # direction 0: tout droit

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
        self.moteur = Moteur()

        # self.sensibilite = MAX   # écart admissible par rapport à la valeur initiale
        self.initial = 0  # valeur initiale du capteur balance lumière

        self.sensibilite = MIN     # sensibilité
        self.tempoLampe = 0        # will store last time LAMPE was updated
        self.intervalLampe = 2000

        self.initial = self.capteur.read()
        # lecture des valeurs initiales (on suppose que les capteurs sont de part et d'autre de la ligne)
        if self.initial > 100:
            self.moteur.bip()

        if abs(self.initial - MILIEU) < (MAX / 4):
            self.moteur.bip()
        else:
            self.initial = MILIEU

        self.sensibilite = MAX / 2    # sensibilite maximum avec réglage initial

        self.server = Sever()
        self.server.do_connect()

        time.sleep_us(1000)

    def capteur_operate(self):
        # temporisation de 2s pour moteur
        valeur = self.capteur.read()

        # temporisation de 2s pour moteur
        self.now = time.ticks_ms()

        if (self.now - self.tempoLampe) > self.intervalLampe:
            # print("on s'arrête");
            self.moteur.stop()

        if abs(valeur - self.initial) < self.sensibilite:
            if self.moteur.dir != 0:
                print(valeur)
                print("tout droit")
                self.tempoLampe = self.now
                self.moteur.tout_droit()
        else:
            if valeur > self.initial:
                if self.moteur.dir != GAUCHE:
                    print(valeur)
                    print("on tourne à gauche")
                    self.tempoLampe = self.now
                    self.moteur.a_gauche()
            elif valeur < self.initial:
                if self.moteur.dir != DROITE:
                    print(valeur)
                    print("on tourne à droite")
                    self.tempoLampe = self.now
                    self.moteur.a_droite()

    def server_operate(self):
        answer = self.server.listen()

        # -----------------PAVE HAUT------------
        if answer == "1":
            self.moteur.accelere(50)
        if answer == "2":
            self.tempoLampe = self.now
            self.moteur.tout_droit()
        if answer == "3":
            self.sensibilite += 10
            if self.sensibilite > MAX:
                self.sensibilite = MAX

        # -----------------PAVE CENTRE------------
        if answer == "4":
            self.tempoLampe = self.now
            self.moteur.a_droite()
        if answer == "5":
            self.moteur.stop()
        if answer == "6":
            self.tempoLampe = self.now
            self.moteur.a_gauche()

        # -----------------PAVE BAS------------
        if answer == "7":
            self.moteur.accelere(-50)
        if answer == "8":
            self.tempoLampe = self.now
            self.moteur.en_arriere()
        if answer == "9":
            self.sensibilite -= 10
            if self.sensibilite < MIN:
                self.sensibilite = MIN

        # Affichage de la sensibilite
        # valeur = self.capteur.read()
        # buffer = " M={v} D={d} B={s}".format(v=self.vitesse, d=valeur - self.initial, s=self.sensibilite)
        # print(buffer)


robot = Robot()
while True:
    robot.capteur_operate()
    robot.server_operate()

    time.sleep_us(100)


