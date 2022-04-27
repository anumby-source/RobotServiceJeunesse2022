"""
Cette simulation essaie de reproduire l'algorithme d'évitement des collisions

1) on construit un contour aléatoire
2) pon positionne le véhicule au centre du contour
3) on applique la motorisation et la détection de collision
4) on modélise l'algorithme d'évitement
"""

from tkinter import *
import numpy as np
import cmath

MODE = "COLLISION"
MODE = "YEUX"

# -----------------------------------------------------------------------------------------
def rot(points, origin, angle):
    return (points - origin) * np.exp(complex(0, angle)) + origin


# -----------------------------------------------------------------------------------------
class Vector(object):
    pass


def draw_point(p, canvas, color="green"):
    x = p.real
    y = p.imag
    o = canvas.create_oval(x - 4, y - 4, x + 4, y + 4, fill=color, width=2)


# -----------------------------------------------------------------------------------------
class Vector(object):
    def __init__(self, p1: complex, p2: complex, label=""):
        self.label = label
        self.p1 = p1
        self.p2 = p2
        self.v = p2 - p1
        self.angle = cmath.phase(self.v)

    def init(self, origin: complex, vector: complex):
        self.p1 = origin
        self.v = vector
        self.p2 = self.p1 + self.v
        self.angle = cmath.phase(self.v)

    def rotate(self, da):
        points = rot(self.p2, self.p1, da)
        self.p2 = points[0]

    def unit(self):
        result = Vector(0, 0)
        r = cmath.polar(self.v)[0]
        v = self.v/r
        result.init(self.p1, v)
        return result

    def extend(self):
        result = Vector(0, 0)
        r = cmath.polar(self.v)[0]
        v = 500*self.v/r
        result.init(self.p1, v)
        return result

    def draw(self, canvas, color="black"):
        canvas.create_line(self.p1.real, self.p1.imag, self.p2.real, self.p2.imag, fill=color, width=1)
        self.p1.draw(canvas, color)
        self.p2.draw(canvas, color)

    def params_droite(self):
        # calcul de la pente du segment [P1, P2]
        x1 = self.p1.real
        y1 = self.p1.imag
        x2 = self.p2.real
        y2 = self.p2.imag
        try:
            m = (y2 - y1) / (x2 - x1)
        except:
            m = None

        b = None
        if m is not None:
            b = self.p1.imag - m * self.p1.real

        return m, b

    def intersect(self, other):
        # on calcule les deux droites
        m1, b1 = self.params_droite()
        m2, b2 = other.params_droite()

        # intersection des deux droites d1 et d2
        if m1 is None and m2 is not None:
            # d1 est verticale mais pas d2
            x = self.p1.real
            y = m2 * x + b2
        elif m2 is None and m1 is not None:
            # d2 est verticale mais pas d1
            x = other.p1.real
            y = m1 * x + b1
        elif m1 is None and m2 is None:
            # d1 et d2 sont verticales
            return self.p1.real == other.p1.real
        else:
            # droites quelconques
            try:
                x = -(b1 - b2) / (m1 - m2)
                y = m1 * x + b1
            except:
                x = 0
                y = 0

        # est-ce que le point d'intersection est à l'interieur des segments ?
        xv1p1 = self.p1.real
        xv1p2 = self.p2.real
        xv2p1 = other.p1.real
        xv2p2 = other.p2.real

        # teste point d'intersection dans segment v2
        test2 = False
        if xv2p1 < xv2p2:
            test2 = (x >= xv2p1) and (x <= xv2p2)
        elif xv2p1 > xv2p2:
            test2 = (x >= xv2p2) and (x <= xv2p1)

        # teste point d'intersection dans segment v1
        if xv1p1 < xv1p2:
            test1 = (x >= xv1p1) and (x <= xv1p2)
            return test2 and test1, x, y
        elif xv1p1 > xv1p2:
            test1 = (x >= xv1p2) and (x <= xv1p1)
            return test2 and test1, x, y
        else:
            return False, None, None


# -----------------------------------------------------------------------------------------
class Robot(object):
    def __init__(self, canvas=None):
        self.canvas = canvas
        self.angle = 0
        self.w = 10
        self.h = 10
        self.centre = complex(0, 0)
        self.p1 = complex(-self.w, -self.h)
        self.p2 = complex(self.w, -self.h)
        self.p3 = complex(self.w, self.h)
        self.p4 = complex(-self.w, self.h)

        self.droit = complex(0, self.h)
        self.avant_droit = complex(self.w, self.h)
        self.optique_droit = complex(2*self.w, self.h)

        self.gauche = complex(0, -self.h)
        self.avant_gauche = complex(self.w, -self.h)
        self.optique_gauche = complex(2*self.w, -self.h)

        self.avant = complex(self.w, 0)

    def draw_points(self, color="black"):
        draw_point(self.centre, self.canvas, color)
        draw_point(self.droit, self.canvas, color)
        self.canvas.create_line(self.centre.real, self.centre.imag, self.droit.real, self.droit.imag, fill=color, width=1)
        draw_point(self.avant_droit, self.canvas, color)
        self.canvas.create_line(self.droit.real, self.droit.imag, self.avant_droit.real, self.avant_droit.imag, fill=color, width=1)
        draw_point(self.gauche, self.canvas, color)
        self.canvas.create_line(self.centre.real, self.centre.imag, self.gauche.real, self.gauche.imag, fill=color, width=1)
        draw_point(self.avant_gauche, self.canvas, color)
        self.canvas.create_line(self.gauche.real, self.gauche.imag, self.avant_gauche.real, self.avant_gauche.imag, fill=color, width=1)


    def moveby(self, dp):
        self.centre += dp
        self.p1 += dp
        self.p2 += dp
        self.p3 += dp
        self.p4 += dp
        self.avant += dp

        self.droit += dp
        self.avant_droit += dp
        self.optique_droit += dp
        self.gauche += dp
        self.avant_gauche += dp
        self.optique_gauche += dp

    def rotate(self, da):
        origin = self.centre

        points = rot(np.array([
            self.p1,
            self.p2,
            self.p3,
            self.p4,
            self.droit,
            self.avant_droit,
            self.optique_droit,
            self.gauche,
            self.avant_gauche,
            self.optique_gauche,
            self.avant]), origin, da)

        self.p1 = points[0]
        self.p2 = points[1]
        self.p3 = points[2]
        self.p4 = points[3]
        self.droit = points[4]
        self.avant_droit = points[5]
        self.optique_droit = points[6]
        self.gauche = points[7]
        self.avant_gauche = points[8]
        self.optique_gauche = points[9]
        self.avant = points[10]

    def direction(self):
        v = Vector(self.centre, self.avant)
        return v

    def direction_droit(self):
        v = Vector(self.droit, self.avant_droit)
        return v

    def direction_gauche(self):
        v = Vector(self.gauche, self.avant_gauche)
        return v

    def tourne_droite(self):
        angle = 0.05 * 2 * np.pi
        self.rotate(angle)

    def tourne_gauche(self):
        angle = - 0.05 * 2 * np.pi
        self.rotate(angle)

    def avance(self, canvas):
        p0 = self.centre
        u = self.direction().unit()
        robot.moveby(u.p2 - self.centre)
        p1 = self.centre
        canvas.create_line(p0.real, p0.imag, p1.real, p1.imag, fill="black", width=1)


    def draw(self, canvas):
        images = []
        p = canvas.create_polygon(self.p1.real, self.p1.imag,
                                  self.p2.real, self.p2.imag,
                                  self.p3.real, self.p3.imag,
                                  self.p4.real, self.p4.imag,
                                  fill="green")
        l = canvas.create_line(self.centre.real, self.centre.imag,
                               self.avant.real, self.avant.imag,
                               fill="red", width=2)

        # dir = self.direction().extend()
        # d = canvas.create_line(dir.p1.real, dir.p1.imag, dir.p2.real, dir.p2.imag, fill="black", width=1)

        images.append(p)
        images.append(l)

        if MODE == "COLLISION":
            dd = self.direction_droit().extend()
            dd = canvas.create_line(dd.p1.real, dd.p1.imag, dd.p2.real, dd.p2.imag, fill="black", width=1)

            dg = self.direction_gauche().extend()
            dg = canvas.create_line(dg.p1.real, dg.p1.imag, dg.p2.real, dg.p2.imag, fill="black", width=1)

            images.append(dd)
            images.append(dg)

        # images.append(d)

        return images


# -----------------------------------------------------------------------------------------
class Contour(object):
    def __init__(self, w, h, canvas):
        self.width = w
        self.height = h
        self.centre = complex(self.width / 2, self.height / 2)
        self.canvas = canvas

    def aleatoire(self, ncoins, mode="Contour"):
        rayon_min = 50
        rayon_max = 200
        angles = np.sort(np.random.random_sample(ncoins) * np.pi * 2)
        rayons = (np.random.random_sample(ncoins) * (rayon_max - rayon_min) + rayon_min)
        coss = np.cos(angles)
        sins = np.sin(angles)

        sommets = [complex(self.centre.real + rayons[i] * coss[i],
                         self.centre.imag + rayons[i] * sins[i]) for i in range(ncoins)]

        segs = []

        x1 = 0
        y1 = 0
        x0 = 0
        y0 = 0
        for i, sommet in enumerate(sommets):
            if i == 0:
                x1 = int(sommet.real)
                y1 = int(sommet.imag)
                x0 = x1
                y0 = y1
            else:
                x2 = int(sommet.real)
                y2 = int(sommet.imag)
                # print(x1, y1, x2, y2)
                segs.append(Vector(complex(x1, y1), complex(x2, y2)))
                x1 = x2
                y1 = y2

        segs.append(Vector(complex(x1, y1), complex(x0, y0)))

        if mode == "Contour":
            color = "blue"
            width = 3
            capstyle = "butt"
        else:
            color = "red"
            width = 30
            capstyle = "round"

        for seg in segs:
            self.canvas.create_line(seg.p1.real, seg.p1.imag, seg.p2.real, seg.p2.imag,
                                    fill=color, width=width, capstyle=capstyle)

        return segs


t = 0
previous = None


# -----------------------------------------------------------------------------------------
def get_color(canvas, route, x, y):
    ids = canvas.find_overlapping(x, y, x, y)
    #print(",".join([canvas.type(i) for i in ids]))
    for id in ids:
        if id in route and canvas.itemcget(id, "fill") == "red":
            # print("get_color", ids, route, id)
            return "red"
    return None, "white"


# -----------------------------------------------------------------------------------------
def animate_collision(images=None):
    global t, previous
    if images is not None:
        for i in images:
            zone_dessin.delete(i)

    images = robot.draw(zone_dessin)
    t += 1
    # if t > 100: return
    ext = robot.direction().extend()
    ext_droit = robot.direction_droit().extend()
    ext_gauche = robot.direction_gauche().extend()

    d1 = 0
    d2 = 0
    d3 = 0
    d = 0
    for s in segments:
        test1, x1, y1 = s.intersect(ext)
        test2, x2, y2 = s.intersect(ext_droit)
        test3, x3, y3 = s.intersect(ext_gauche)
        if test1 or test2 or test3:
            # print(test1, test2, test3)
            inter1 = complex(x1, y1)
            inter2 = complex(x2, y2)
            inter3 = complex(x3, y3)
            d1 = cmath.polar(inter1 - ext.p1)[0]
            d2 = cmath.polar(inter2 - ext_droit.p1)[0]
            d3 = cmath.polar(inter3 - ext_gauche.p1)[0]

            """
            print("intersect test=", test, x, y, "p1=", s.p1.real, s.p1.imag, 
                  "p2=", s.p2.real, s.p2.imag, "extp1=", ext.p1.real,
                  ext.p1.imag, "extp2=", ext.p2.real, ext.p2.imag)
                  """
            break

    l = zone_dessin.create_line(s.p1.real, s.p1.imag, s.p2.real, s.p2.imag, fill="red", width=3)
    # o1 = zone_dessin.create_oval(x1 - 4, y1 - 4, x1 + 4, y1 + 4, fill="green", width=3)
    o2 = zone_dessin.create_oval(x2 - 4, y2 - 4, x2 + 4, y2 + 4, fill="green", width=3)
    o3 = zone_dessin.create_oval(x3 - 4, y3 - 4, x3 + 4, y3 + 4, fill="green", width=3)
    images.append(l)
    # images.append(o1)
    images.append(o2)
    images.append(o3)

    if previous is not None:
        here = robot.centre
        zone_dessin.create_line(previous.real, previous.imag, here.real, here.imag, fill="black", width=1)

    previous = robot.centre

    # print("distance=", d)
    d = min(d1, d2, d3)
    # print("intersect:", test1, test2, test3, d1, d2, d3, d)
    if d > 50:
        dir = robot.direction()
        u = dir.unit()
        robot.moveby(u.p2 - robot.centre)
    else:
        robot.rotate(np.pi/10)

    Fenetre.after(200, animate_collision, images)


starting = 0
off = 1
left_eye_on = 2
right_eye_on = 3
both_eyes_on = 4

previous_state = starting
# -----------------------------------------------------------------------------------------
def animate_yeux(images=None):
    global t, previous, previous_state

    def get_colors():
        cg = get_color(zone_dessin, route, robot.optique_gauche.real, robot.optique_gauche.imag)
        cd = get_color(zone_dessin, route, robot.optique_droit.real, robot.optique_droit.imag)
        return cg, cd

    def cherche():
        gauche = np.random.random() < 0.5
        while True:
            if gauche:
                robot.tourne_gauche()
            else:
                robot.tourne_droite()
            cg, cd = get_colors()
            if cg == "red" and cd == "red":
                return both_eyes_on

    if images is not None:
        for i in images:
            zone_dessin.delete(i)

    images = robot.draw(zone_dessin)
    t += 1

    cg, cd = get_colors()

    def eye_status(state):
        s = ["starting", "off", "left eye on", "right eye on", "both eyes on"]
        return s[state]

    previous_eye_status = eye_status(previous_state)

    if cg == "red" and cd == "red":
        state = both_eyes_on
    elif cg == "red":
        state = left_eye_on
    elif cd == "red":
        state = right_eye_on
    else:
        state = off

    # print(cg, cd, state)

    new_eye_status = eye_status(state)

    if previous is not None:
        here = robot.centre
        zone_dessin.create_line(previous.real, previous.imag, here.real, here.imag, fill="black", width=1)

    previous = robot.centre

    action = "???"

    if previous_state == starting:
        # on n'est pas sur la route => on continue d'avancer jusqu'à retrouver la rou
        action = "avance"
        robot.avance(zone_dessin)
        if state != both_eyes_on:
            state = starting
    elif previous_state == left_eye_on:
        if state == left_eye_on:
            # on entre ou on est est entré sur la route => on continue
            action = "tourne left"
            robot.tourne_gauche()
        elif state == right_eye_on:
            # on entre u on est est entré sur la route => on continue
            # action = "tourne right"
            # robot.tourne_droite()
            action = "cherche"
            state = cherche()
        elif state == both_eyes_on:
            # on entre u on est est entré sur la route => on continue
            action = "avance"
            robot.avance(zone_dessin)
        else:
            # on quitte la route on tourne pour rentrer
            action = "cherche"
            state = cherche()
    elif previous_state == right_eye_on:
        if state == left_eye_on:
            # on entre u on est est entré sur la route => on continue
            # action = "tourne left"
            # robot.tourne_gauche()
            action = "cherche"
            state = cherche()
        elif state == right_eye_on:
            # on entre u on est est entré sur la route => on continue
            action = "tourne right"
            robot.tourne_droite()
        elif state == both_eyes_on:
            # on entre u on est est entré sur la route => on continue
            action = "avance"
            robot.avance(zone_dessin)
        else:
            # on quitte la route on tourne pour rentrer
            action = "cherche"
            state = cherche()
    elif previous_state == both_eyes_on:
        if state == left_eye_on:
            # on doit tourner pour retrouver la route
            # jusqu'à tant que state = 3
            action = "tourne left"
            robot.tourne_gauche()
        if state == right_eye_on:
            # on doit tourner pour retrouver la route
            # jusqu'à tant que state = 3
            action = "tourne right"
            robot.tourne_droite()
        elif state == both_eyes_on:
            # on est sur la route => on continue d'avancer
            action = "avance"
            robot.avance(zone_dessin)
        elif state == off:
            # on a quitté la route => on doit tourner pour retrouver la route
            action = "tourne right"
            robot.tourne_droite()
    elif previous_state == off:
        if state == off:
            # on n'est pas sur la route => on continue d'avancer jusqu'à retrouver la route
            action = "tourne right"
            robot.tourne_droite()
        elif state == left_eye_on:
            # on n'est pas sur la route => on continue d'avancer jusqu'à retrouver la route
            action = "tourne left"
            robot.tourne_gauche()
        elif state == right_eye_on:
            # on n'est pas sur la route => on continue d'avancer jusqu'à retrouver la route
            action = "tourne right"
            robot.tourne_droite()
        elif state == both_eyes_on:
            # on n'est pas sur la route => on continue d'avancer jusqu'à retrouver la route
            action = "avance"
            robot.avance(zone_dessin)

    print(previous_eye_status, "->", new_eye_status, "=>", action)

    previous_state = state

    Fenetre.after(200, animate_yeux, images)


"""
for id in zone_dessin.find_all():
    print(id, zone_dessin.type(id), zone_dessin.coords(id))
    if zone_dessin.type(id) == "line":
        rect = zone_dessin.coords(id)
        zone_dessin.create_line(rect[0], rect[1], rect[2], rect[3])
        #zone_dessin.create_rectangle(rect[0], rect[1], rect[2], rect[3])
"""

def test_find_color():
    for i in range(100):
        p = np.random.random_sample(2) * 500
        x = p[0]
        y = p[1]
        id, c = get_color(zone_dessin, x, y)

        if id is not None and zone_dessin.type(id) == "line":
            rect = zone_dessin.coords(id)
            zone_dessin.create_rectangle(rect[0], rect[1], rect[2], rect[3])
            print("color at ", x, y, c, "id=", id, zone_dessin.type(id))
            draw_point(complex(x, y), zone_dessin, color="green")
        else:
            # print("color at ", x, y, c)
            # draw_point(complex(x, y), zone_dessin, color="white")
            pass


# -----------------------------------------------------------------------------------------
width = 500
height = 500

Fenetre = Tk()
zone_dessin = Canvas(Fenetre, width=width, height=height, bg="yellow")

contour = Contour(w=width, h=height, canvas=zone_dessin)

# cree le robot et position au centre de l'image avec une orientation aléatoire
robot = Robot(zone_dessin)
robot.moveby(contour.centre)
images = robot.draw(zone_dessin)

def test_moving():
    for i in range(20): robot.avance(zone_dessin)
    for i in range(5): robot.tourne_droite()
    for i in range(20): robot.avance(zone_dessin)
    for i in range(3): robot.tourne_gauche()
    for i in range(20): robot.avance(zone_dessin)

    robot.draw(zone_dessin)
    draw_point(robot.optique_droit, zone_dessin, "green")
    draw_point(robot.optique_gauche, zone_dessin, "red")

# test_moving()

if MODE == "YEUX":
    mode_contour = "Route"
elif MODE == "COLLISION":
    mode_contour = "Contour"

segments = contour.aleatoire(ncoins=15, mode=mode_contour)

# test_find_color()

route = []

for id in zone_dessin.find_all():
    if zone_dessin.type(id) == "line":
        # print(id, zone_dessin.type(id), zone_dessin.coords(id), zone_dessin.itemcget(id, "fill"))
        c = zone_dessin.itemcget(id, "fill")
        # rect = zone_dessin.coords(id)
        # zone_dessin.create_line(rect[0], rect[1], rect[2], rect[3])
        #zone_dessin.create_rectangle(rect[0], rect[1], rect[2], rect[3])
        route.append(id)

if MODE == "YEUX":
    animate_yeux(images)
elif MODE == "COLLISION":
    animate_collision(images)

zone_dessin.pack()

Fenetre.mainloop()
