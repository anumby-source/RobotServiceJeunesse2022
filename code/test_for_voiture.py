import random
# from graphics import *
# import uasyncio as asyncio
import asyncio

class ASync(object):
    def __init__(self, name):
        self.name = name
        
    async def bar(self, x):
        count = 0
        while True:
            pause = random.randint(0, 100)/100.0
            count += 1
            print(self.name, 'Instance: {} count: {} pause={}'.format(x, count, pause))
            await asyncio.sleep(pause)  # Pause 1s

    async def run(self):
        for x in range(random.randint(3, 6)):
            asyncio.create_task(self.bar(x))
        # await asyncio.sleep(10)

a = ASync("a")
b = ASync("b")
c = ASync("c")

asyncio.run(a.run())
asyncio.run(b.run())
asyncio.run(c.run())

async def main():
    await asyncio.sleep(10)

asyncio.run(main())
