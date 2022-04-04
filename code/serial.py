
def test(s):
    c = r'1+2'
    b = c.encode()
    r1 = s.write(b)
    r2 = s.readline()
    r3 = s.readline()
    r4 = s.readline()
    r5 = s.readline()
    r6 = s.readline()
    print("r1=", r1, "r2=", r2, r3, r4, r5, r6)

def t():
    import serial
    PORT = "COM7"
    s = serial.serial_for_url('COM7',  timeout=1)
    test(s)
    s.close()

"""
r = s.write(serial.to_bytes([0x31, 0x0a, 0x32, 0x0a, 0x33, 0x0a]))
r = s.readline(), serial.to_bytes([0x31, 0x0a])
r = s.readline(), serial.to_bytes([0x32, 0x0a])
r = s.readline(), serial.to_bytes([0x33, 0x0a])
# this time we will get a timeout
r = s.readline(), serial.to_bytes([])
"""