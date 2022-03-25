import time
from machine import ADC
from machine import Pin, PWM

"""
try:
  import usocket as socket
except:
  import socket

import network

import esp
esp.osdebug(None)
"""

import gc
gc.collect()

PinA = Pin(0, Pin.OUT)  # broche enable du L298N pour le premier moteur
PinB = Pin(2, Pin.OUT)  # broche enable du L298N pour le deuxième moteur
SpeedA = PWM(Pin(5, Pin.OUT))  # Premier moteur
SpeedA.freq(1000)
SpeedB = PWM(Pin(4, Pin.OUT))  # Deuxième moteur
SpeedB.freq(1000)
PinA.off()
PinB.off()
PinA.read()


