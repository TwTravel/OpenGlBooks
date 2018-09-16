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

#define GLVECTOR_IOSTREAM
#include "GLMatrix.h"
#include <stdlib.h>
#ifdef HAVE_LIBCONTEXTGLPP
    #include <ContextGL++/GLWindow.h>
    #include <ContextGL++/WMhelper.h>
#elif defined(HAVE_LIBSDL)
    #if defined(HAVE_CONFIG_H)
        #include SDL_HEADER
    #else
        #include <SDL/SDL.h>
    #endif
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

using namespace std;
using namespace mathglpp;

GLMatrix4f viewMatrix;
GLMatrix4f projMatrix;

double uniform(){double retval = (double)rand()/RAND_MAX; assert(retval <= 1); assert(retval >= 0); return retval; }
float* vec;
int nVec;

void reshape(int w, int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    projMatrix.loadFrustum(-1,1,-1*((float)h/(float)w),1*((float)h/(float)w),2,20);
    glMatrixMode(GL_MODELVIEW);
}

void display()
{
    static GLMatrix4f modelView;
    static float time = 0;

    modelView = projMatrix*viewMatrix;
    modelView.applyScale(2*cos(time*.75)+1,2*cos(time*2.333)+1,2*sin(time*1.66)+1);
    time += 0.001;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*software Transform*/
    glColor3f(1,0,1);
    glBegin(GL_POINTS);
    modelView.glVertex3v(nVec,vec);
    glEnd();

    /*hardware transform*/
//    modelView.glLoadMatrix();
//    glColor3f(1,0,1);
//    glBegin(GL_POINTS);
//    for(register int i = 0; i < nVec; ++i)
//        glVertex3fv(vec + i*3);
//    glEnd();
}

void idle(bool& redisplay)
{
    static float pitch = 0,yaw = 0,roll = 0;

    viewMatrix.loadIdentity();
    viewMatrix.applyTranslate(0,0,-5);
    viewMatrix.applyRotate(pitch,1,0,0);
    viewMatrix.applyRotate(yaw,  0,1,0);
    viewMatrix.applyRotate(roll, 0,0,1);

    pitch += 0.1;
    yaw += 0.01;
    roll += 0.001;

    for(int i = 0; i < nVec/10; ++i)
    {
        //choose 2 particles and make one chase, the other flee
        //like insects trying to mate.

        int p1 = (int)floor(uniform()*nVec);
        int p2 = p1;
        while(p2 == p1)
        {
            p2 = (int)floor(uniform()*nVec);
        };

        //apply some attraction/repulsion
        GLVector3f part1(vec + p1*3);
        GLVector3f part2(vec + p2*3);
        GLVector3f diff = part1-part2;
        diff.normalize();

        part1 -= diff*0.01;
        part2 -= diff*0.01;

        part1.copyTo(vec + p1*3);
        part2.copyTo(vec + p2*3);
    };

    redisplay = true;
}

#ifdef HAVE_LIBCONTEXTGLPP

using namespace contextglpp;

int main(int argc, char *argv[])
{
    projMatrix.loadFrustum(-1,1,-1*(640/480),1*(640/480),2,20);

    srand(time(NULL));
    //make the vector
    nVec = 2000;
    vec = new float[nVec*3];
    for(register int i = 0; i < (nVec*3); ++i)
        vec[i] = 2*(uniform()-.5);

    /*
     * Initialize the display in a 640x480 24-bit mode,
     * requesting a software surface
     */

    GLWindow* context = new GLWindow(GLContext::DOUBLE | GLContext::DEPTH, 0,0,800,600,0);
    context->create("MathGL++ example");
    context->show();
    context->makeCurrent();
    
    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();

    reshape(800,600);

    WMEvent* event = new WMEvent(*context);
    bool redisplay;
    
    while(1)
    {
        /* Grab all the events off the queue. */
        while( event->nextEvent() )
        {
            switch( event->type() )
            {
                case WMEvent::CLOSE:
                    delete event;
                    delete context;
                    exit( 0 );
                    break;
                case WMEvent::RESIZE:
                    reshape(event->windowResize().width,event->windowResize().height);
                    redisplay = true;
                    break;
            }
        };

        if(redisplay)
        {
            display();
            context->swapBuffers();
            redisplay = false;
        }

        idle(redisplay);
    };
}
#elif HAVE_LIBSDL
int main(int argc, char *argv[])
{
    SDL_Surface *screen;

    projMatrix.loadFrustum(-1,1,-1*(640/480),1*(640/480),2,20);

    /* Initialize the SDL library */
    if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0 )
    {
        fprintf(stderr,
                "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    /* Clean up on exit */
    atexit(SDL_Quit);

    srand(time(NULL));
    //make the vector
    nVec = 2000;
    vec = new float[nVec*3];
    for(register int i = 0; i < (nVec*3); ++i)
        vec[i] = 2*(uniform()-.5);

    /*
     * Initialize the display in a 640x480 24-bit mode,
     * requesting a software surface
     */
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    if ( (screen=SDL_SetVideoMode( 800, 600, 24, SDL_OPENGL )) == NULL )
    {
        SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
        SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 6 );
        SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
        SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
        if ( (screen=SDL_SetVideoMode( 800, 600, 16, SDL_OPENGL )) == NULL )
        {
            fprintf(stderr, "Couldn't set GL mode: %s\n", SDL_GetError());
            SDL_Quit();
            return 1;
        }
    }

    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();
    SDL_Event event;
    bool redisplay = true;
    bool buttonDown = false;
    int mx, my;
    int frame_count = 0;
    Uint32 start_ticks = SDL_GetTicks();
    Uint32 end_ticks;

    reshape(800,600);

    while(1)
    {
        /* Grab all the events off the queue. */
        while( SDL_PollEvent( &event ) )
        {
            switch( event.type )
            {
                case SDL_KEYDOWN:
                    break;
                case SDL_MOUSEBUTTONUP:
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    break;
                case SDL_MOUSEMOTION:
                    break;
                case SDL_QUIT:
                    /* Handle quit requests (like Ctrl-c). */
            end_ticks = SDL_GetTicks();
            cout<<"fps: "<< 1000*frame_count/(end_ticks-start_ticks)<<endl;
                    exit( 0 );
                    break;
                case SDL_VIDEORESIZE:
            reshape(event.resize.w,event.resize.h);
            redisplay = true;
                break;
                case SDL_VIDEOEXPOSE:
            redisplay = true;
                break;
            }
        };

    if(redisplay)
    {
        display();
        SDL_GL_SwapBuffers();
        redisplay = false;
        ++frame_count;
    }

    idle(redisplay);
    };

    SDL_FreeSurface(screen);
}
#else
int main()
{
    cout<<"This example requires SDL or ContextGL++"<<endl;
    return 0;
}
#endif
