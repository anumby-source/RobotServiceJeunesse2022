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
            x = -(b1 - b2) / (m1 - m2)
            y = m1 * x + b1

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

        self.gauche = complex(0, -self.h)
        self.avant_gauche = complex(self.w, -self.h)

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
        self.gauche += dp
        self.avant_gauche += dp

    def rotate(self, da):
        origin = self.centre

        points = rot(np.array([
            self.p1,
            self.p2,
            self.p3,
            self.p4,
            self.droit,
            self.avant_droit,
            self.gauche,
            self.avant_gauche,
            self.avant]), origin, da)

        self.p1 = points[0]
        self.p2 = points[1]
        self.p3 = points[2]
        self.p4 = points[3]
        self.droit = points[4]
        self.avant_droit = points[5]
        self.gauche = points[6]
        self.avant_gauche = points[7]
        self.avant = points[8]

    def direction(self):
        v = Vector(self.centre, self.avant)
        return v

    def direction_droit(self):
        v = Vector(self.droit, self.avant_droit)
        return v

    def direction_gauche(self):
        v = Vector(self.gauche, self.avant_gauche)
        return v

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

        dd = self.direction_droit().extend()
        dd = canvas.create_line(dd.p1.real, dd.p1.imag, dd.p2.real, dd.p2.imag, fill="black", width=1)

        dg = self.direction_gauche().extend()
        dg = canvas.create_line(dg.p1.real, dg.p1.imag, dg.p2.real, dg.p2.imag, fill="black", width=1)

        images.append(p)
        images.append(l)
        # images.append(d)
        images.append(dd)
        images.append(dg)

        return images


# -----------------------------------------------------------------------------------------
class Contour(object):
    def __init__(self, w, h, canvas):
        self.width = w
        self.height = h
        self.centre = complex(self.width / 2, self.height / 2)
        self.canvas = canvas

    def aleatoire(self, ncoins):
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
                print(x1, y1, x2, y2)
                segs.append(Vector(complex(x1, y1), complex(x2, y2)))
                self.canvas.create_line(x1, y1, x2, y2, fill="blue", width=3)
                x1 = x2
                y1 = y2

        segs.append(Vector(complex(x1, y1), complex(x0, y0)))
        self.canvas.create_line(x1, y1, x0, y0, fill="blue", width=3)

        return segs


t = 0
previous = None


# -----------------------------------------------------------------------------------------
def animate(images=None):
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

    Fenetre.after(200, animate, images)


# -----------------------------------------------------------------------------------------
width = 500
height = 500

Fenetre = Tk()
zone_dessin = Canvas(Fenetre, width=width, height=height, bg="yellow")

contour = Contour(w=width, h=height, canvas=zone_dessin)

# cree le robot et position au centre de l'image avec une orientation aléatoire
robot = Robot(zone_dessin)
robot.moveby(contour.centre)
# robot.draw_points()
angle = np.random.random() * 2 * np.pi
# angle = 0.3 * 2 * np.pi
robot.rotate(angle)
# robot.draw_points("red")
images = robot.draw(zone_dessin)

segments = contour.aleatoire(ncoins=15)


animate(images)

zone_dessin.pack()

Fenetre.mainloop()
