/***************************************************************************
 *   Copyright (C) 2004 by Jacques Gasselin                                *
 *   jacquesgasselin@hotmail.com                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef _WIN32
    #ifdef HAVE_CONFIG_H
        #include GLU_HEADER
    #else
        #include <GL/glu.h>
    #endif
#else
    #include <GL/GLU.h>
#endif
#include <cassert>
#define GLVECTOR_IOSTREAM
#include "GLMatrix.h"
#include <stdlib.h>

using namespace mathglpp;

GLfloat angle = 0;

const char* app_name()
{
    return "tutorial 1: rotate, scale & translate";
}

void reshape(int w, int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLMatrix4f projection;
    GLfloat aspect = h/(GLfloat)w;
    projection.loadOrtho(-1,1,-1*aspect,1*aspect,1,20);
    projection.glMultMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void init(int w, int h)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glPolygonOffset(1,1);
    glEnable(GL_POLYGON_OFFSET_FILL);
    reshape(w,h);
}

void display()
{
    static const GLfloat verts[] =
    {
        -1, -1,  1,
         1, -1,  1,
         1,  1,  1,
        -1,  1,  1,
        -1, -1, -1,
         1, -1, -1,
         1,  1, -1,
        -1,  1, -1
    };

    static const GLint indices[] =
    {
        3,0,1,2,
        2,1,5,6,
        6,5,4,7,
        7,4,0,3,
        1,0,4,5,
        6,7,3,2
    };

    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    GLMatrix4f view;
    view.loadTranslate(0,0,-2);
    view.glMultMatrix();
    
    GLMatrix4f model;
    model.loadRotate(angle,0.75,0.666,0.5);
    model.applyScale(.2,.2,.2);
    model.glMultMatrix();

    for(int i = 0; i < 2; ++i)
    {
        if(i == 0)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColor3f(0,0,0);
        glBegin(GL_QUADS);
            if(i == 0) glColor3f(0,0,1);
            glVertex3fv(&verts[3*indices[0]]);
            glVertex3fv(&verts[3*indices[1]]);
            glVertex3fv(&verts[3*indices[2]]);
            glVertex3fv(&verts[3*indices[3]]);
            if(i == 0) glColor3f(0,1,0);
            glVertex3fv(&verts[3*indices[4]]);
            glVertex3fv(&verts[3*indices[5]]);
            glVertex3fv(&verts[3*indices[6]]);
            glVertex3fv(&verts[3*indices[7]]);
            if(i == 0) glColor3f(0,1,1);
            glVertex3fv(&verts[3*indices[8]]);
            glVertex3fv(&verts[3*indices[9]]);
            glVertex3fv(&verts[3*indices[10]]);
            glVertex3fv(&verts[3*indices[11]]);
            if(i == 0) glColor3f(1,0,0);
            glVertex3fv(&verts[3*indices[12]]);
            glVertex3fv(&verts[3*indices[13]]);
            glVertex3fv(&verts[3*indices[14]]);
            glVertex3fv(&verts[3*indices[15]]);
            if(i == 0) glColor3f(1,0,1);
            glVertex3fv(&verts[3*indices[16]]);
            glVertex3fv(&verts[3*indices[17]]);
            glVertex3fv(&verts[3*indices[18]]);
            glVertex3fv(&verts[3*indices[19]]);
            if(i == 0) glColor3f(1,1,0);
            glVertex3fv(&verts[3*indices[20]]);
            glVertex3fv(&verts[3*indices[21]]);
            glVertex3fv(&verts[3*indices[22]]);
            glVertex3fv(&verts[3*indices[23]]);
        glEnd();
    }
}

void idle(bool& redisplay)
{
    angle+=0.05f;
    redisplay = true;
}
