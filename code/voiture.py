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

capteur = A0
PinA = 0          # broche enable du L298N pour le premier moteur
PinB = 2          # broche enable du L298N pour le deuxième moteur
SpeedA = 5        # Premier moteur
SpeedB = 4        # Deuxième moteur
DROITE = 1        # tourne à droite
GAUCHE = 2        # tourne à gauche
MILIEU = 510      # pont de résistance 1024 / 2 environ
MAX = 100         # sensibilite maximum avec réglage initial MAX / 2
MIN = 0           # sensibilite min

# sensibilite = MAX   # écart admissible par rapport à la valenitiale
initial          # valeur initiale du capteur balance lumière
dir = 0          # direction 0: tout droit

IPAddress apIP(44, 44, 44, 44)  # Définition de l'adresse IP statique.
# const char * ssid = "RCO"; # Nom du reseau wifi(***A modifier ***)
password = "12345678" # mot de passe (***A modifier ***)
# ESP8266WebServer server(80)

AA = 600 # target Speed
sensibilite # sensibilité
buffer = bytearray[30]
tempoLampe = 0 # will store last time LAMPE was updated
intervalLampe = 2000

WiFiServer server(80)

html1 = """<!DOCTYPE html>
<html>
<head>
<style> .button { padding:10px 10px 10px 10px; width:100%;  background-color: green; font-size: 500%;color:white;} </style>
<center><h1>ARNAUD"""

html2 = """</h1>
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
  </form> """

html3 = """</center>
</head> 
</html>"""

def setup():
    currentMillis = millis()
    ssid = bytearray[30]
    sprintf(ssid, "RCO_%d", ESP.getChipId())

    Serial.begin(115200)

    pinMode(PinA, OUTPUT)
    pinMode(PinB, OUTPUT)
    pinMode(SpeedA, OUTPUT)
    pinMode(SpeedB, OUTPUT)
    digitalWrite(PinA, LOW)
    digitalWrite(PinB, LOW)

    # declaration du wifi:
    WiFi.mode(WIFI_AP)
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0))
    WiFi.softAP(ssid, password)

    # if you get here you have connected to the WiFi
    Serial.println("Connected.")

    server.begin()
    bip() # test moteur
    initial = analogRead(capteur)
    if (initial > 100): # lecture des valeurs initiales (on suppose que les capteurs sont de part et d'autre de la ligne)
        bip()

    if (abs(initial - MILIEU) < (MAX / 4)):
        bip()
    else:
        initial = MILIEU

    sensibilite = MAX / 2 # sensibilite maximum avec réglage initial
    delay(1000)


def bip():  # test moteur
    digitalWrite(PinA, LOW)
    digitalWrite(PinB, LOW)
    analogWrite(SpeedA, AA)
    analogWrite(SpeedB, AA)
    delay(200)
    analogWrite(SpeedA, 0)
    analogWrite(SpeedB, 0)
    delay(400)


def patinage():    # 2 moteurs en marche avant pour éviter le patinage
    digitalWrite(PinA, LOW)
    digitalWrite(PinB, LOW)
    analogWrite(SpeedA, AA)
    analogWrite(SpeedB, AA)
    delay(1)

def loop():
    # temporisation de 2s pour moteur
    valeur = analogRead(capteur)

    # temporisation de 2s pour moteur
    currentMillis = millis()

    if (currentMillis - tempoLampe > intervalLampe):
        # Serial.println("on s'arrête");
        digitalWrite(SpeedA, 0)
        digitalWrite(SpeedB, 0)

    if (abs(valeur - initial) < sensibilite):
        if (dir != 0):
            Serial.println(valeur)
            Serial.println("tout droit")
            dir = 0
            tempoLampe = currentMillis
            digitalWrite(PinA, LOW)
            digitalWrite(PinB, LOW)
            analogWrite(SpeedA, AA)
            analogWrite(SpeedB, AA)
        else:
            if (valeur > initial):
                if (dir != GAUCHE):
                    Serial.print(valeur)
                    Serial.println("on tourne à gauche")
                    dir = GAUCHE
                    tempoLampe = currentMillis
                    patinage()
                    digitalWrite(PinA, LOW)
                    digitalWrite(PinB, LOW)
                    digitalWrite(SpeedA, 0)
                    analogWrite(SpeedB, AA)
                    # digitalWrite(PinA, HIGH)
                    # digitalWrite(PinB, LOW)
                    # digitalWrite(SpeedA, AA)
                    # analogWrite(SpeedB, AA)
                else if (valeur < initial):
                    if (dir != DROITE):
                        Serial.print(valeur)
                        Serial.println("on tourne à droite")
                        dir = DROITE
                        tempoLampe = currentMillis
                        patinage()
                        digitalWrite(PinA, LOW)
                        digitalWrite(PinB, LOW)
                        analogWrite(SpeedA, AA)
                        digitalWrite(SpeedB, 0)
                        # digitalWrite(PinA, LOW)
                        # digitalWrite(PinB, HIGH)
                        # analogWrite(SpeedA, AA)
                        # digitalWrite(SpeedB, AA)

        client = server.available()
        if (client):
            request = client.readStringUntil('\r')

            # -----------------PAVE HAUT------------
            if (request.indexOf("LED0=1") != -1):
                AA += 50
                if (AA > 1023) AA=1023
            if (request.indexOf("LED0=2") != -1):
                tempoLampe = currentMillis
                digitalWrite(PinB, LOW)
                digitalWrite(PinA, LOW)
                analogWrite(SpeedA, AA)
                analogWrite(SpeedB, AA)
            if (request.indexOf("LED0=3") != -1):
                sensibilite += 10
                if (sensibilite > MAX) sensibilite = MAX

            # -----------------PAVE CENTRE------------
            if (request.indexOf("LED0=4") != -1):
                tempoLampe = currentMillis
                digitalWrite(PinA, LOW)
                digitalWrite(PinB, HIGH)
                analogWrite(SpeedA, AA)
                analogWrite(SpeedB, AA)
            if (request.indexOf("LED0=5") != -1):
                analogWrite(SpeedA, 0)
                analogWrite(SpeedB, 0)
            if (request.indexOf("LED0=6") != -1):
                tempoLampe = currentMillis
                digitalWrite(PinA, HIGH)
                digitalWrite(PinB, LOW)
                analogWrite(SpeedA, AA)
                analogWrite(SpeedB, AA)

            # -----------------PAVE BAS------------
            if (request.indexOf("LED0=7") != -1):
                AA -= 50
                if (AA < 0) AA=0
            if (request.indexOf("LED0=8") != -1):
                tempoLampe = currentMillis
                digitalWrite(PinA, HIGH)
                digitalWrite(PinB, HIGH)
                analogWrite(SpeedA, AA)
                analogWrite(SpeedB, AA)
            if (request.indexOf("LED0=9") != -1):
                sensibilite -= 10
                if (sensibilite < MIN) sensibilite = MIN

        # Affichage de la sensibilite
        sprintf(buffer, " M=%d (%d) B=%d", AA, valeur - initial, sensibilite)
        # Serial.println(buffer)
        client.print(html1)
        client.print(buffer)
        client.print(html2)
        # client.print(html)
        request = ""


    delay(100)

