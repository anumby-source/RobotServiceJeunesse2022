try:
 import usocket as socket        #importing socket
except:
 import socket

try:
 import uselect as select        #importing socket
except:
 import select

import network            #importing network
import esp                 #importing ESP

esp.osdebug(None)

import gc
gc.collect()

STOP    = 1
AVANCE  = 2
RECULE  = 3
DROITE  = 4
GAUCHE  = 5
LENT    = 6
RAPIDE  = 7
SENSMIN = 8
SENSMAX = 9

MANUEL    = 10
COLLISION = 11
SUIVI     = 12
BALANCE   = 13

dirs = [ "", "Arr&ecirc;t", "Avant", "Arri&egrave;re", "Droite", "Gauche", "", "", ""]
text_cmd = [ "", "STOP", "AVANCE", "RECULE", "DROITE", "GAUCHE", "LENT", "RAPIDE", "SENSMIN", "SENSMAX", "MANUEL", "COLLISION", "SUIVI", "BALANCE"]
text_mode = [ "MANUEL", "COLLISION", "SUIVI"]

def web_page(client, commande):

    t1 = text_cmd[commande]
    t2 = text_mode[0]
    t3 = dirs[0]
    t4 = MANUEL
    t5 = COLLISION
    t6 = SUIVI
    t7 = BALANCE
    t8 = AVANCE
    t9 = RECULE
    t10 = GAUCHE
    t11 = STOP
    t12 = DROITE
    
    html_a = """
<!DOCTYPE html> \n
<html> \n
<head> \n
<style> \n
  .echo {width: 100px; font-size: 150%;} \n
  .cmd { padding:10px 10px 10px 10px; \n
            margin:10px; \n
            width:100%; \n
            background-color: red; \n
            font-size: 250%; \n
            color:white;} \n
  .param { margin:10px; \n
            width:100%; \n
            background-color: yellow; \n
            font-size: 150%; \n
            color:blue;} \n
  .button { padding:10px 10px 10px 10px; \n
            margin:10px; \n
            width:100%; \n
            background-color: green; \n
            font-size: 250%; \n
            color:white;} \n
</style> \n
  <center> <h1>Robot Service Jeunesse</h1> \n
"""

    html_b = """
  <form> \n
      <label for='commande'>commande:</label> \n
      <input class='echo' id='commande' name='commande' value='{t1}'> \n
      <label for='mode'>mode:</label> \n
      <input class='echo' id='mode' name='mode' value='{t2}'> \n
      <label for='direction'>direction:</label> \n
      <input class='echo' id='direction' name='direction' value='{t3}'> \n
      <table> \n
          <tr> \n
              <td> <button name='LED0' class='cmd' value='{t4}' type='submit'> Manuel </button></td> \n
              <td> <button name='LED0' class='cmd' value='{t5}' type='submit'> Collision </button></td> \n
              <td> <button name='LED0' class='cmd' value='{t6}' type='submit'> Suivi </button></td> \n
              <td> <button name='LED0' class='cmd' value='{t7}' type='submit'> Blancs </button></td> \n
          </tr> \n
      </table> \n
  </form> \n
  <form> \n
      <TABLE> \n
          <TR> \n
              <TD> <button name='LED0' class='button' value='{t8}' type='submit'> Avant </button></TD> \n
              <TD> <button name='LED0' class='button' value='{t9}' type='submit'> Arri&egrave;re </button></TD> \n
          </TR> \n
      </TABLE> \n
      <TABLE> \n
          <TR> \n
              <TD> <button name='LED0' class='button' value='{t10}' type='submit'> Gauche </button></TD> \n
              <TD> <button name='LED0' class='button' value='{t11}' type='submit'> Stop </button></TD> \n
              <TD> <button name='LED0' class='button' value='{t12}' type='submit'> Droite </button></TD> \n
          </TR> \n
          <TR> \n
              <TD> <p class='param'> Vitesse </p></TD> \n
              <TD> <button name='LED0' class='param' value='{t13}' type='submit'> Lent </button></TD> \n
              <TD> <button name='LED0' class='param' value='{t14}' type='submit'> Rapide </button></TD> \n
          </TR> \n
      </TABLE> \n
      <label for='vitesse'>vitesse:</label> \n
      <input class='echo' id='vitesse' name='vitesse'  value='{t15}'> \n \
  </form> \n
""".format(t1=t1, t2=t2, t3=t3, t4=t4,
           t5=t5, t6=t6, t7=t7, t8=t8,
           t9=t9, t10=t10, t11=t11, t12=t12,
           t13=LENT, t14=RAPIDE, t15=0)

    html_c = """
  </center> \n
</head> \n
</html>
"""
    
    while True:
        try:
            n = len(html_a) + len(html_b) + len(html_c)
            ns = 0
            ns += client.write(html_a)
            ns += client.write(html_b)
            ns += client.write(html_c)
            client.close()
            print("sending answers", n, ns)
            break
        except:
            pass


def do_something_else():
    print("doing somethig else")
    
def client_handler(commande):
    print("client_handler", commande)
    

ssid = 'RCO_123'                  #Set your own 
password = '12345678'      #Set your own password


ap = network.WLAN(network.AP_IF)
ap.active(False)            #activating
ap.active(True)            #activating
# ap.ifconfig(('192.168.4.1', '255.255.255.0', '192.168.4.1', '208.67.222.222'))
ap.config(essid=ssid, password=password)

while ap.active() == False:
  pass

print('Connection is successful')
print(ap.ifconfig())

i = 0
while True:
    print('Creating socket')
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)   #creating socket object
    try:
        print('Binding socket on port ', 80)
        s.bind(('', 80))
        break
    except:
        s.close()

    if i > 10:
        print("timeout")
        break
    i += 1

# s.setsockoption(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
#s.close()
#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)   #creating socket object

s.listen(5)
print('Listening socket')


def read(s):
    r, w, err = select.select((s,), (), (), 1)
    if not r: return
    for readable in r:
        client, addr = s.accept()
        print('Got data from %s' % str(addr))
        request = client.recv(2048).decode()
        request = str(request)
        l = len(request)
        pos = request.find("/?LED")
        if pos > 0:
            request = request[pos+7:]
            pos = request.find("&")
            commande = request[0:pos]
            print('len={} Content = [{}] '.format(l, request))

            web_page(client, int(commande))
            
            client_handler(commande)
        else:
            web_page(client, 0)
            
            print("no commande")
        

while True:
    read(s)
    do_something_else()
