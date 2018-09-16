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

#include <cassert>
#define GLVECTOR_IOSTREAM
#include "GLMatrix.h"
#include "GLPlane.h"
#include <stdlib.h>
#ifndef _WIN32
    #ifdef HAVE_CONFIG_H
        #include GLU_HEADER
    #else
        #include <GL/glu.h>
    #endif
#else
    #include <GL/GLU.h>
#endif

using namespace mathglpp;

GLfloat angle = 0;
GLVector3f lightPos(0,1.7f,0);
GLMatrix4f view;

GLuint color_cube_list;
GLuint noncolor_cube_list;

void drawCube(const bool withColor)
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

    glBegin(GL_QUADS);
    if(withColor) glColor3f(0,0,1);
    glVertex3fv(&verts[3*indices[0]]);
    glVertex3fv(&verts[3*indices[1]]);
    glVertex3fv(&verts[3*indices[2]]);
    glVertex3fv(&verts[3*indices[3]]);
    if(withColor) glColor3f(0,1,0);
    glVertex3fv(&verts[3*indices[4]]);
    glVertex3fv(&verts[3*indices[5]]);
    glVertex3fv(&verts[3*indices[6]]);
    glVertex3fv(&verts[3*indices[7]]);
    if(withColor) glColor3f(0,1,1);
    glVertex3fv(&verts[3*indices[8]]);
    glVertex3fv(&verts[3*indices[9]]);
    glVertex3fv(&verts[3*indices[10]]);
    glVertex3fv(&verts[3*indices[11]]);
    if(withColor) glColor3f(1,0,0);
    glVertex3fv(&verts[3*indices[12]]);
    glVertex3fv(&verts[3*indices[13]]);
    glVertex3fv(&verts[3*indices[14]]);
    glVertex3fv(&verts[3*indices[15]]);
    if(withColor) glColor3f(1,0,1);
    glVertex3fv(&verts[3*indices[16]]);
    glVertex3fv(&verts[3*indices[17]]);
    glVertex3fv(&verts[3*indices[18]]);
    glVertex3fv(&verts[3*indices[19]]);
    if(withColor) glColor3f(1,1,0);
    glVertex3fv(&verts[3*indices[20]]);
    glVertex3fv(&verts[3*indices[21]]);
    glVertex3fv(&verts[3*indices[22]]);
    glVertex3fv(&verts[3*indices[23]]);
    glEnd();
}

const char* app_name()
{
    return "tutorial 2: projective shadows (faux soft)";
}

void reshape(int w, int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLMatrix4f projection;
    GLfloat aspect = h/(GLfloat)w;
    projection.loadFrustum(-1,1,-1*aspect,1*aspect,1,20);
    projection.glMultMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void init(int w, int h)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glEnable(GL_MULTISAMPLE);
    glPolygonOffset(1,1);
    glEnable(GL_POLYGON_OFFSET_FILL);
    reshape(w,h);
    view.loadTranslate(0,0,-4);

    //make the cude lists
    color_cube_list = glGenLists(1);
    noncolor_cube_list = glGenLists(1);

    glNewList(color_cube_list,GL_COMPILE);
        drawCube(true);
    glEndList();
    
    glNewList(noncolor_cube_list,GL_COMPILE);
        drawCube(false);
    glEndList();
    
}

void display()
{
    glClearColor(0.4f, 0.4f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();
    glDisable(GL_STENCIL_TEST);

    GLMatrix4f model;
    GLPlanef shadowPlane(GLVector3f(0.0f, 1.0f, 0.0f), GLVector3f(0.0f, -0.9999f, 0.0f));

    for(int i = 0; i < 22; ++i)
    {
        model = view;
        if(i == 0)
        {
            glDisable(GL_BLEND);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else
        if(i == 1)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glColor4f(0,0,0,1.0f);
        }
        else
        if(i == 2)
        {
            for(int j = 0; j < 2; ++j)
            {
                if(j == 0)
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glColor3f(0.8f, 0.8f, 0.8f);
                }
                else
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glColor3f(0.6f, 0.6f, 0.6f);
                }

                view.glLoadMatrix();
                glEnable(GL_STENCIL_TEST);
                glStencilFunc(GL_ALWAYS, 0x01, 0x01);
                glStencilOp(GL_REPLACE,GL_REPLACE,GL_REPLACE);
                glBegin(GL_QUADS);
                    glVertex3f(-2,-1,-2);
                    glVertex3f(-2,-1,2);
                    glVertex3f(2,-1,2);
                    glVertex3f(2,-1,-2);
                glEnd();
            }
            continue;
        }
        else
        {

            const GLfloat DEVIATION_0 = 0.050f;
            const GLfloat DEVIATION_1 = 0.025f;
            
            GLVector3f offsetVector = GLVector3f(0,0,0);
            if(i == 3)
            {
                //Set the blend state back to a small color subtraction
                glStencilFunc(GL_EQUAL, 0x01, 0x01);
                glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
                glEnable(GL_BLEND);
                glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
                glDisable(GL_DEPTH_TEST);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glColor3f(0.07f, 0.07f, 0.07f);
            }
            else if(i == 4) offsetVector = GLVector3f(0,DEVIATION_0,0);
            else if(i == 5) offsetVector = GLVector3f(0,-DEVIATION_0,0);
            else if(i == 6) offsetVector = GLVector3f(DEVIATION_0,0,0);
            else if(i == 7) offsetVector = GLVector3f(-DEVIATION_0,0,0);
            else if(i == 8) offsetVector = GLVector3f(0,0,DEVIATION_0);
            else if(i == 9) offsetVector = GLVector3f(0,0,-DEVIATION_0);
            
            else if(i == 10) offsetVector = GLVector3f(DEVIATION_1,DEVIATION_1,0);
            else if(i == 11) offsetVector = GLVector3f(DEVIATION_1,-DEVIATION_1,0);
            else if(i == 12) offsetVector = GLVector3f(-DEVIATION_1,DEVIATION_1,0);
            else if(i == 13) offsetVector = GLVector3f(-DEVIATION_1,-DEVIATION_1,0);
            else if(i == 14) offsetVector = GLVector3f(DEVIATION_1,0,DEVIATION_1);
            else if(i == 15) offsetVector = GLVector3f(DEVIATION_1,0,-DEVIATION_1);
            else if(i == 16) offsetVector = GLVector3f(-DEVIATION_1,0,DEVIATION_1);
            else if(i == 17) offsetVector = GLVector3f(-DEVIATION_1,0,-DEVIATION_1);
            else if(i == 18) offsetVector = GLVector3f(0,DEVIATION_1,DEVIATION_1);
            else if(i == 19) offsetVector = GLVector3f(0,DEVIATION_1,-DEVIATION_1);
            else if(i == 20) offsetVector = GLVector3f(0,-DEVIATION_1,DEVIATION_1);
            else if(i == 21) offsetVector = GLVector3f(0,-DEVIATION_1,-DEVIATION_1);

            model *= shadowPlane.pointProjectionMatrix(lightPos + offsetVector);

        }
        
        model.applyRotate(angle, 0.75f, 0.666f, 0.5f);
        model.applyScale(0.2f, 0.2f, 0.2f);
        model.glLoadMatrix();

        if(i == 0)
            glCallList(color_cube_list);
        else
            glCallList(noncolor_cube_list);

    }

    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
    view.glLoadMatrix();
    glPointSize(5.0f);
    glColor3f(0.8f, 0.5f, 0.5f);
    glBegin(GL_POINTS);
        lightPos.glVertex();
    glEnd();
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_LINES);
        lightPos.glVertex();
        glVertex3f(lightPos.x, -1, lightPos.z);
    glEnd();
}

void idle(bool& redisplay)
{
    angle += 0.25f;
    lightPos.x = 2 * sin(angle * 0.023f);
    lightPos.z = 2 * cos(angle * 0.037f);
    redisplay = true;
}

void destroy()
{
    glDeleteLists(color_cube_list,1);
    glDeleteLists(noncolor_cube_list,1);
}
