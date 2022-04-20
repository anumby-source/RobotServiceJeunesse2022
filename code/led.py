import uasyncio as asyncio

async def blink(led, period_ms):
    while True:
        led.on()
        await asyncio.sleep_ms(5)
        led.off()
        await asyncio.sleep_ms(period_ms)

async def main(led):
    asyncio.create_task(blink(led, 700))
    await asyncio.sleep_ms(10_000)

# Running on a pyboard
from pyb import LED
uasyncio.run(main(LED(1)))

# Running on a generic board
from machine import Pin
uasyncio.run(main(Pin(1)))
