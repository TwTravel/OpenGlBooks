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
#if defined(HAVE_LIBCONTEXTGLPP)
    #include <ContextGL++/GLWindow.h>
    #include <ContextGL++/WMhelper.h>
#elif defined(HAVE_LIBSDL)
    #if defined(HAVE_CONFIG_H)
        #include SDL_HEADER
    #else
        #include <SDL/SDL.h>
    #endif
#endif
#if !defined(_WIN32)
    #if defined(HAVE_CONFIG_H)
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

extern const char* app_name();
extern void init(int w, int h);
extern void reshape(int w, int h);
extern void display();
extern void idle(bool& redisplay);
extern void destroy();

#if defined(HAVE_LIBCONTEXTGLPP)

using namespace contextglpp;

#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
#else
int main()
{
    void* hInstance = 0;
#endif

    GLWindow* context = new GLWindow(GLContext::DOUBLE | GLContext::DEPTH | GLContext::MULTISAMPLE | GLContext::STENCIL, 0,0,800,600, hInstance);
    context->create(app_name());
    context->show();
    context->makeCurrent();

    init(800,600);

    WMEvent* event = new WMEvent(*context);
    bool redisplay;
    bool done = false;
    while(!done)
    {
        /* Grab all the events off the queue. */
        while(event->nextEvent())
        {
            switch(event->type()) 
            {
                case WMEvent::CLOSE:
                    done = true;
                    break;
                case WMEvent::RESIZE:
                    reshape(event->windowResize().width,event->windowResize().height);
                    redisplay = true;
                    break;
            }
            event->passAlong();
        };

        if(redisplay)
        {
            display();
            context->swapBuffers();
            redisplay = false;
        }

        idle(redisplay);
    };

    destroy();
    delete event;
    delete context;

    return 0;
}

#elif defined(HAVE_LIBSDL)

int main(int argc, char *argv[])
{
    SDL_Surface *screen;

    /* Initialize the SDL library */
    if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0 )
    {
        fprintf(stderr,
                "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    /* Clean up on exit */
    atexit(SDL_Quit);

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

    SDL_Event event;
    bool redisplay = true;
    bool buttonDown = false;
    int mx, my;
    int frame_count = 0;
    Uint32 start_ticks = SDL_GetTicks();
    Uint32 end_ticks;

    init(800,600);

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

    destroy();    
    SDL_FreeSurface(screen);
}

#else
int main()
{
    cout<<"This example requires SDL or ContextGL++"<<endl;
    return 0;
}
#endif
