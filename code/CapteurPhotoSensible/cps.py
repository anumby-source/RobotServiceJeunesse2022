from machine import Pin, ADC
from time import sleep

"""
lecture d'une résitance photo-sensible
pont de résistance: [220ohm - A0 - cps] alim 3.3v

résistance variable = [50000 ... 500] ohms
3.3v R=220ohm => [0.014 .. 1.452]
"""

def cps(pot):
    a = 40
    b = 430
    v = pot.read()
    return (v - a)/(b - a)


pot = ADC(0)
while True:
    v = cps(pot)
    print(v)
    sleep(0.1)

  