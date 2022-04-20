import uos
import network
import socket
import time
import uasyncio as asyncio

def web_page(commande):
    htmls = ["""<!DOCTYPE html>
    <html>
    <head>""",
             """
    <style>
      .cmd { padding:10px 10px 10px 10px;
                width:100%;
                background-color: red;
                font-size: 200%;
                color:white;}
      .button { padding:20px 20px 20px 20px;
                width:100%;
                background-color: green;
                font-size: 150%;
                color:white;}
      .td { padding:20px 20px 20px 20px;
            spacing:50px 50px 50px 50px;
            }
    </style>""",
             """
    <center><h1>Robot Service Jeunesse {commande} 
    """,
            """
    </h1>
      <form>
          <table>
              <tr>
                  <td class='td'> <button name='LED0' class='cmd' value='A' type='submit'> Manuel </button></td>
                  <td class='td'> <button name='LED0' class='cmd' value='B' type='submit'> Collision </button></td>
                  <td class='td'> <button name='LED0' class='cmd' value='C' type='submit'> Suiveur </button></td>
              </tr>
          </table>
      </form>
      """,
            """
      <form>
    <TABLE>
    """,
             """<TR>
    <TD class='td'> <button name='LED0' class='button' value='1' type='submit'> << </button></TD>
    <TD class='td'> <button name='LED0' class='button' value='2' type='submit'> Avance </button></TD>
    <TD class='td'> <button name='LED0' class='button' value='3' type='submit'> >> </button></TD>
    </TR>""",
             """<TR>
    <TD class='td'> <button name='LED0' class='button' value='4' type='submit'> Gauche </button></TD>
    <TD class='td'> <button name='LED0' class='button' value='5' type='submit'> Arret </button></TD>
    <TD class='td'> <button name='LED0' class='button' value='6' type='submit'> Droite </button></TD>
    </TR>""",
             """<TR>
    <TD class='td'> <button name='LED0' class='button' value='7' type='submit'> << </button></TD>
    <TD class='td'> <button name='LED0' class='button' value='8' type='submit'> Recule </button></TD>
    <TD class='td'> <button name='LED0' class='button' value='9' type='submit'> >> </button></TD>
    </TR>""",
             """
    </TABLE>
    </form>
    """,
            """
    </center>
    </head>
    </html>
    """]
    page = ""
    for html in htmls:
        has = html.find("{commande}") > 0
        html = html.replace("{commande}", "<div>" + commande + "</div>")
        if has: print(">>> ", html)
        page += html
    return page



class Server(object):
    def __init__(self):
        pass

    async def run(self):
        # for x in range(random.randint(3, 6)):
        self.start_server()
        asyncio.create_task(self.waiting())
        # await asyncio.sleep(10)

    def start_server(self):
        ssid = "ESP8266-ssid"
        password = "12345678"

        ap = network.WLAN(network.AP_IF)
        ap.config(essid=ssid, password=password)
        ap.active(True)
        print("ifconfig=", ap.ifconfig())
        ap.ifconfig(("44.44.44.44", '255.255.255.0', '192.168.4.1', '208.67.222.222'))

        while not ap.active():
            print("waiting ...")
            time.sleep(1)

        print("ifconfig=", ap.ifconfig())
        # print("config=", ap.config())
        print("ap=", dir(ap))

        while not ap.isconnected():
            print("waiting connexion...")
            time.sleep(1)

        print("now connected...")

        self.mysocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.mysocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

        self.mysocket.bind(('', 80))
        self.mysocket.listen(5)

    async def waiting(self):
        while True:
            print("waiting connect from client")
            
            # loop = asyncio.get_running_loop()
            reader, writer = await asyncio.open_connection(host="44.44.44.44", port=80)

            try:
                data = await reader.read(1024)
                data = data.decode()
            except:
                print("error")
                writer.close()
                continue

            print("data> ", data)
            
            """
            conn, addr = self.mysocket.accept()
                
            print('Connected from: %s' % str(addr))
            print()
            request = conn.recv(1024)
            answer = request.decode()
            print('answer1: %s' % answer)
            print()
            """
            
            commande = ""
            pos = answer.find("/?LED0=")
            if pos > 0:
                answer = answer[pos+7:pos+15]
                pos = answer.find("\r")
                commande = answer[0:pos]
                commande = "{}".format(commande)
                print("answer2: [", commande, "]", pos)

            page = web_page(commande)

            #conn.send('HTTP/1.1 200 OK\n')
            #conn.send('Content-Type: text/html\n')
            #conn.send('Connection: close\n\n')
            conn.sendall(page)
            conn.close()
            
            await asyncio.sleep(0.001)
    

class Capteur(object):
    def __init__(self, name):
        self.name = name

    async def run(self):
        # for x in range(random.randint(3, 6)):
        
        asyncio.create_task(self.bar(1))
        # await asyncio.sleep(10)

    async def bar(self, x):
        count = 0
        while True:
            # pause = random.randint(0, 100)/100.0
            count += 1
            print(self.name, 'Instance: {} count: {}'.format(x, count))
            await asyncio.sleep(0.1)  # Pause 1s


a = Capteur("a")
b = Capteur("b")
server = Server()

# asyncio.run(a.run())
# asyncio.run(b.run())
asyncio.run(server.run())

async def main():
    print("main> start")
    while True:
        await asyncio.sleep(10)
    print("main> end")

print("A")

asyncio.run(main())

print("B")
