#!/usr/bin/env python
from OpenGL.GL import *
import ContextGL
import MathGL

window = ContextGL.GLWindow(ContextGL.DOUBLE | ContextGL.MULTISAMPLE,0,0,800,600)
window.create("A simple Python example")
window.show()
window.makeCurrent()

event = ContextGL.WMEvent(window)

done = False

mat = MathGL.GLMatrix()
mat.loadIdentity()
angle = 0

while not done:
    glClear(GL_COLOR_BUFFER_BIT)
    mat.loadRotate(angle,0,1,0)
    mat.glLoadMatrix()
    glScalef(.9,.9,.9)
    glColor3f(1,0,0)
    glBegin(GL_LINE_STRIP)
    glVertex2f(0,0)
    glVertex2f(0,1)
    glVertex2f(1,1)
    glVertex2f(1,0)
    glVertex2f(0.5,0)
    glVertex2f(0.5,0.5)
    glEnd()
    window.swapBuffers()
    while event.nextEvent():
        event_type = event.type()
        if event_type == ContextGL.CLOSE_EVENT:
            done = True
            window.close()
        elif event_type == ContextGL.BUTTON_PRESS_EVENT:
            done = True
    angle += 0.1
