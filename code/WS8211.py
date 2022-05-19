import machine, neopixel
import time
import random


np = neopixel.NeoPixel(machine.Pin(4), 1)

# print("w")

dt = 20

"""
print("w")
for i in range(255):
    np[0] = (255, 255, 255)
    np.write()
    time.sleep_ms(dt)
"""

np[0] = (0, 0, 0)
np.write()
time.sleep_ms(1000)

print("green")
for i in range(255):
    np[0] = (i, 0, 0)
    np.write()
    time.sleep_ms(dt)

print("red")
for i in range(255):
    np[0] = (0, i, 0)
    np.write()
    time.sleep_ms(dt)

print("blue")
for i in range(255):
    np[0] = (0, 0, i)
    np.write()
    time.sleep_ms(dt)

print("yellow")
for i in range(255):
    np[0] = (i, i, 0)
    np.write()
    time.sleep_ms(dt)

print("cyan")
for i in range(255):
    np[0] = (i, 0, i)
    np.write()
    time.sleep_ms(dt)

print("violet")
for i in range(255):
    np[0] = (0, i, i)
    np.write()
    time.sleep_ms(dt)

np[0] = (0, 0, 0)
np.write()
