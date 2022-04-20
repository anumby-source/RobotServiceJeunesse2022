import uasyncio as asyncio
import time

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

async def handle_client(reader, writer):
    print("start listening")
    while True:
        data = await reader.read(1024)
        print("data ??")
        if not data:
            writer.close()
            break
        d = data.strip().encode()
        print(d)
        r = web_page("aaa")
        writer.write(r.encode())
        await writer.drain()
    

loop = asyncio.get_event_loop()

async def starting():
    print("start_server")
    asyncio.start_server(handle_client, "44.44.44.44", 80)

asyncio.run(starting())
    
loop.run_forever()


"""
async def run_server():
    server = await asyncio.start_server(handle_client, "44.44.44.44", 80)
    async with server:
        await server.serve_forever()

asyncio.run(run_server())
"""


"""
async def handle_client(reader, writer)

s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

"""

"""
import asyncio, socket

async def handle_client(reader, writer):
    request = None
    while request != 'quit':
        request = (await reader.read(255)).decode('utf8')
        response = str(eval(request)) + '\n'
        writer.write(response.encode('utf8'))
        await writer.drain()
    writer.close()

async def run_server():
    server = await asyncio.start_server(handle_client, 'localhost', 15555)
    async with server:
        await server.serve_forever()

asyncio.run(run_server())

"""
# (ap_if.ifconfig()[0])
