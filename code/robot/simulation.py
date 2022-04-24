"""
Cette simulation essaie de reproduire l'algorithme d'évitement des collisions

1) on construit un contour aléatoire
2) pon positionne le véhicule au centre du contour
3) on applique la motorisation et la détection de collision
4) on modélise l'algorithme d'évitement
"""

from tkinter import *
import numpy as np


# -----------------------------------------------------------------------------------------
class Point(object):
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __copy__(self):
        return Point(self.x, self.y)

    def moveby(self, dp):
        self.x += dp.x
        self.y += dp.y

    def moveto(self, p):
        self.x = p.x
        self.y = p.y

    def distance(self, other):
        dx = self.x - other.x
        dy = self.y - other.y
        return np.sqrt(dx*dx + dy*dy)


# -----------------------------------------------------------------------------------------
class Vector(object):
    def __init__(self, p1, p2):
        self.p1 = p1
        self.p2 = p2
        dx = p2.x - p1.x
        dy = p2.y - p1.y
        self.r = np.sqrt(dx*dx + dy*dy)
        try:
            self.angle = np.arctan(dy/dx)
        except:
            self.angle = np.pi/2

        if dx < 0:
            self.angle += np.pi
        elif dy < 0:
            self.angle += 2*np.pi

    def rotate(self, da):
        self.angle += da
        p = Point(self.p1.x, self.p1.y)
        dp = Point(self.r*np.cos(self.angle),
                   self.r*np.sin(self.angle))
        p.moveby(dp)
        self.p2 = p

    def unit(self):
        p1 = self.p1
        dp = Point((self.p2.x - self.p1.x) / self.r, (self.p2.y - self.p1.y) / self.r)
        return Vector(p1, dp)

    def extend(self):
        origin = Point(self.p1.x, self.p1.y)
        x1 = self.p1.x
        y1 = self.p1.y
        x2 = self.p2.x
        y2 = self.p2.y
        try:
            m = (y2 - y1)/(x2 - x1)
            x3 = 100*(x2 - x1) + x1
            y3 = m*(x3 - x1) + y1
        except:
            x3 = x1
            y3 = 100*(y2 - y1) + y1

        return Vector(origin, Point(x3, y3))


# -----------------------------------------------------------------------------------------
class Robot(object):
    def __init__(self):
        self.angle = 0
        self.w = 10
        self.h = 10
        self.centre = Point(0, 0)
        self.p1 = Point(-self.w, -self.h)
        self.p2 = Point(self.w, -self.h)
        self.p3 = Point(self.w, self.h)
        self.p4 = Point(-self.w, self.h)
        self.avant = Point(self.w, 0)

    def moveby(self, dp):
        self.centre.moveby(dp)
        self.p1.moveby(dp)
        self.p2.moveby(dp)
        self.p3.moveby(dp)
        self.p4.moveby(dp)
        self.avant.moveby(dp)

    def rotate(self, da):
        def turn_point(p):
            v = Vector(self.centre, p)
            v.rotate(da)
            return v.p2
        self.p1 = turn_point(self.p1)
        self.p2 = turn_point(self.p2)
        self.p3 = turn_point(self.p3)
        self.p4 = turn_point(self.p4)
        self.avant = turn_point(self.avant)

    def direction(self):
        v = Vector(Point(self.centre.x, self.centre.y),
                   Point(self.avant.x, self.avant.y))
        # return v.unit()
        return v

    def draw(self, canvas):
        images = []
        p = canvas.create_polygon(self.p1.x, self.p1.y,
                                  self.p2.x, self.p2.y,
                                  self.p3.x, self.p3.y,
                                  self.p4.x, self.p4.y,
                                  fill="green")
        l = canvas.create_line(self.centre.x, self.centre.y,
                               self.avant.x, self.avant.y,
                               fill="red", width=2)

        dir = self.direction().extend()
        d = canvas.create_line(dir.p1.x, dir.p1.y, dir.p2.x, dir.p2.y, fill="black", width=1)

        images.append(p)
        images.append(l)
        images.append(d)

        return images


# -----------------------------------------------------------------------------------------
class Contour(object):
    def __init__(self, w, h, canvas):
        self.width = w
        self.height = h
        self.centre = Point(self.width / 2, self.height / 2)
        self.canvas = canvas

    def aleatoire(self, ncoins):
        rayon_min = 50
        rayon_max = 200
        angles = np.sort(np.random.random_sample(ncoins) * np.pi * 2)
        rayons = (np.random.random_sample(ncoins) * (rayon_max - rayon_min) + rayon_min)
        coss = np.cos(angles)
        sins = np.sin(angles)

        sommets = [Point(self.centre.x + rayons[i] * coss[i],
                         self.centre.y + rayons[i] * sins[i]) for i in range(ncoins)]

        segs = []

        x1 = 0
        y1 = 0
        x0 = 0
        y0 = 0
        for i, sommet in enumerate(sommets):
            if i == 0:
                x1 = int(sommet.x)
                y1 = int(sommet.y)
                x0 = x1
                y0 = y1
            else:
                x2 = int(sommet.x)
                y2 = int(sommet.y)
                print(x1, y1, x2, y2)
                segs.append(Vector(Point(x1, y1), Point(x2, y2)))
                self.canvas.create_line(x1, y1, x2, y2, fill="blue", width=3)
                x1 = x2
                y1 = y2

        segs.append(Vector(Point(x1, y1), Point(x0, y0)))
        self.canvas.create_line(x1, y1, x0, y0, fill="blue", width=3)

        return segs


# -----------------------------------------------------------------------------------------
def intersect(v1, v2):
    def pente(p1, p2):
        # calcul de la pente du segment [P1, P2]
        x1 = p1.x
        y1 = p1.y
        x2 = p2.x
        y2 = p2.y
        try:
            m = (y2 - y1) / (x2 - x1)
        except:
            m = None

        return m

    def b(p, m):
        # calcul de la coord d'origine du segment passant par P avec la pente m
        if m is None:
            return None
        return p.y - m*p.x

    # on calcule les deux droites
    m1 = pente(v1.p1, v1.p2)
    b1 = b(v1.p1, m1)

    m2 = pente(v2.p1, v2.p2)
    b2 = b(v2.p1, m2)

    # intersection des deux droites d1 et d2
    if m1 is None and m2 is not None:
        # d1 est verticale mais pas d2
        x = v1.p1.x
        y = m2*x + b2
    elif m2 is None and m1 is not None:
        # d2 est verticale mais pas d1
        x = v2.p1.x
        y = m1*x + b1
    elif m1 is None and m2 is None:
        # d1 et d2 sont verticales
        return v1.p1.x == v2.p1.x
    else:
        # droites quelconques
        x = -(b1 - b2)/(m1 - m2)
        y = m1*x + b1

    # est-ce que le point d'intersection est à l'interieur des segments ?
    xv1p1 = v1.p1.x
    xv1p2 = v1.p2.x
    xv2p1 = v2.p1.x
    xv2p2 = v2.p2.x

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


# -----------------------------------------------------------------------------------------
def animate(images=None):
    global t
    if images is not None:
        for i in images:
            zone_dessin.delete(i)

    images = robot.draw(zone_dessin)
    t += 1
    # if t > 100: return
    ext = robot.direction().extend()
    d = 0
    for s in segments:
        test, x, y = intersect(s, ext)
        if test:
            inter = Point(x, y)
            d = ext.p1.distance(inter)

            """
            print("intersect test=", test, x, y, "p1=", s.p1.x, s.p1.y, 
                  "p2=", s.p2.x, s.p2.y, "extp1=", ext.p1.x,
                  ext.p1.y, "extp2=", ext.p2.x, ext.p2.y)
                  """
            l = zone_dessin.create_line(s.p1.x, s.p1.y, s.p2.x, s.p2.y, fill="red", width=3)
            o = zone_dessin.create_oval(x - 4, y - 4, x + 4, y + 4, fill="green", width=3)
            images.append(l)
            images.append(o)
            break

    print("distance=", d)
    if d > 50:
        u = robot.direction().unit()
        robot.moveby(u.p2)
    else:
        robot.rotate(t*np.pi/2000)

    Fenetre.after(200, animate, images)


# -----------------------------------------------------------------------------------------
width = 500
height = 500

Fenetre = Tk()
zone_dessin = Canvas(Fenetre, width=width, height=height, bg="yellow")

contour = Contour(w=width, h=height, canvas=zone_dessin)
segments = contour.aleatoire(ncoins=15)

# cree le robot et position au centre de l'image avec une orientation aléatoire
robot = Robot()
robot.moveby(contour.centre)
robot.rotate(np.random.random()*2*np.pi)

images = robot.draw(zone_dessin)
dir = robot.direction()

t = 0

animate(images)

zone_dessin.pack()

Fenetre.mainloop()
