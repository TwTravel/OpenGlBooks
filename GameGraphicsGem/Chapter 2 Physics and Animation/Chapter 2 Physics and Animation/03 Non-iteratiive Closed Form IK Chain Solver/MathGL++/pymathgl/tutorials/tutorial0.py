#!/usr/bin/env python
# Written by Jacques Gasselin de Richebourg
# Email: jacquesgasselin@hotmail.com
# Copyright 2006
# Distributed under LGPL
#
# A Simple tutorial of how to use PyMathGL and PyContextGL.
# The example should display a rotating cube in primary colors
# on a white backdrop, on a 800 by 600 pixel window.
#
from OpenGL.GL import *
import ContextGL
import MathGL
import Numeric

angle = 0
#prepare the  cube verts
verts = Numeric.array([[-1, -1,  1],
        [ 1, -1,  1],
        [ 1,  1,  1],
        [-1,  1,  1],
        [-1, -1, -1],
        [ 1, -1, -1],
        [ 1,  1, -1],
        [-1,  1, -1]],'f')

#prepare the 6 faces
indices0 = Numeric.array([3,0,1,2],'i')
indices1 = Numeric.array([2,1,5,6],'i')
indices2 = Numeric.array([6,5,4,7],'i')
indices3 = Numeric.array([7,4,0,3],'i')
indices4 = Numeric.array([1,0,4,5],'i')
indices5 = Numeric.array([6,7,3,2],'i')

def reshape(w, h):
    glViewport(0,0,w,h)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    aspect = float(h)/float(w)
    #Just to show its use here.
    #If this is all there is going to be for the projection
    #you might as well just do the glOrtho call yourself.
    projection = MathGL.GLMatrix()
    projection.loadOrtho(-1,1,-1*aspect,1*aspect,1,20)
    projection.glMultMatrix()
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

def init(w, h):
    glEnable(GL_DEPTH_TEST)
    glEnable(GL_CULL_FACE)
    reshape(w,h)

def display():
    glClearColor(1,1,1,1)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    #It is good practise to separate the view matrix from the model matrix
    #this way scene graph manipulation is a lot easier. Here it is just for
    #show though.
    #A proper scene graph would obtain the view matrix as the inverse matrix
    #of the model to world matrix. Here the inverse is done implicitly.
    #Move the eye 2 step in z. ie move the world -2 steps.
    view = MathGL.GLMatrix()
    view.loadTranslate(0,0,-2)
    view.glMultMatrix()
    model = MathGL.GLMatrix()
    global angle
    #Rotate the cube
    model.loadRotate(angle,0.75,0.666,0.5)
    model.glMultMatrix()
    angle += 0.01
    global verts
    global indices
    glScalef(0.2,0.2,0.2)
    #Draw the cube
    glVertexPointerf(verts)
    glEnableClientState(GL_VERTEX_ARRAY)
    glColor3f(0,0,1)
    glDrawElementsui(GL_QUADS,indices0)
    glColor3f(0,1,0)
    glDrawElementsui(GL_QUADS,indices1)
    glColor3f(0,1,1)
    glDrawElementsui(GL_QUADS,indices2)
    glColor3f(1,0,0)
    glDrawElementsui(GL_QUADS,indices3)
    glColor3f(1,0,1)
    glDrawElementsui(GL_QUADS,indices4)
    glColor3f(1,1,0)
    glDrawElementsui(GL_QUADS,indices5)

def mainLoop():
    window = ContextGL.GLWindow(ContextGL.DOUBLE | ContextGL.DEPTH,0,0,800,600)
    window.create("PyContextGL & PyMathGL tutorial 0")
    window.show()
    window.makeCurrent()
    event = ContextGL.WMEvent(window)
    done = False
    init(800,600)
    while not done:
        display()
        window.swapBuffers()
        #poll the event queue for new events
        while event.nextEvent():
            event_type = event.type()
            #close on mouse click or exit button
            if event_type == ContextGL.CLOSE_EVENT:
                done = True
                window.close()
            elif event_type == ContextGL.BUTTON_PRESS_EVENT:
                done = True


#run the main loop 
mainLoop()
