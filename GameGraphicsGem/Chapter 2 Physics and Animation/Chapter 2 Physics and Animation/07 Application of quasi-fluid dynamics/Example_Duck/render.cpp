
#include <windows.h>


#include <gl\gl.h>
#include <gl\glu.h>


#include <stdio.h>  // need to glPrint
#include <stdarg.h> // need to glPrint
#include <string.h> // need to glPrint


#include "render.h"
#include "mesh.h"



extern int iMx,iMy; // mouse rotation cordinates
extern float X,Y,Z; // shift camera


void Asix(void); //Asix drawing fuction

unsigned int	base;

GLvoid glPrint(const char *fmt, ...)	// Custom GL "Print" Routine
{
	char		text[256];	// Holds Our String
	va_list		ap;		// Pointer To List Of Arguments

	if (fmt == NULL)		// If There's No Text
		return;			// Do Nothing

	va_start(ap, fmt);		// Parses The String For Variables
	vsprintf(text, fmt, ap);	// And Converts Symbols To Actual Numbers
	va_end(ap);			// Results Are Stored In Text

	glPushAttrib(GL_LIST_BIT);	// Pushes The Display List Bits
	glListBase(base - 32);		// Sets The Base Character to 32
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();			// Pops The Display List Bits
}


/* Resize OGL window  */
void resizeGLScene(unsigned int width, unsigned int height)
{
    if (height == 0)    height = 1;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0f, (GLfloat)width / (GLfloat)height, 1.0f, 500.0f);
    glMatrixMode(GL_MODELVIEW);
}

/*  Render State Initialisation    */
int initGL(void)
{
    glShadeModel(GL_SMOOTH);

    glClearColor(0.7f, 0.80f, 1.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  /* okreslenie funkcji przezroczystosci */
    glPolygonOffset(1.0,0.0);

    Duck=new Mesh();

    return 1;
}

/* Scene Rendering */
int drawGLScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glRotatef(float(iMy)+90.f,1.f,0.f,0.f);
    glRotatef(float(iMx),0.f,1.f,0.f);

    glDisable(GL_DEPTH_TEST); // blue horizon rendering
    glColor4f(0.4f,0.5f,0.9f,0.40f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0,-2,0);
        glVertex3f(-100,0,-100);
        glVertex3f(-100,0, 100);
        glVertex3f( 100,0, 100);
        glVertex3f( 100,0,-100);
        glVertex3f(-100,0,-100);
    glEnd();
    glEnable(GL_DEPTH_TEST);


    glLoadIdentity();
        glTranslatef(X,Y,Z-15); // shift camera
        glRotatef(float(iMy)+90.f,1.f,0.f,0.f);
        glRotatef(float(iMx),0.f,1.f,0.f);

        glTranslatef(-Duck->vPosition.x,-Duck->vPosition.y,-Duck->vPosition.z);     // hold camera on model position

        Asix();

        Duck->Render(1); // rendering Model

glEnable(GL_BLEND);

    glPushMatrix();
    glScalef(1,0,1);
        glDepthMask(false);
          Duck->Render(0);     // rendering Model Shadow
        glDepthMask(true);
    glPopMatrix();

    // Drawing blue bacground (medium boundary)
    glColor4f(0.4f,0.5f,0.9f,0.40f);
    glBegin(GL_QUADS);
        glVertex3f(-100+Duck->vPosition.x,0,-100+Duck->vPosition.z);
        glVertex3f(-100+Duck->vPosition.x,0, 100+Duck->vPosition.z);
        glVertex3f( 100+Duck->vPosition.x,0, 100+Duck->vPosition.z);
        glVertex3f( 100+Duck->vPosition.x,0,-100+Duck->vPosition.z);
    glEnd();

    int iPointX=(int)(Duck->vPosition.x/5.0)*5;
    int iPointZ=(int)(Duck->vPosition.z/5.0)*5;

    glPointSize(5.0);
    glColor3f(0.2f,0.2f,0.8f);
    glBegin(GL_POINTS);
        for (float f_x=-100.f;f_x<100.f;f_x+=5.f)
            for (float f_z=-100.f;f_z<100.f;f_z+=5.f)  glVertex3f(f_x+iPointX,0.f,f_z+iPointZ);
    glEnd();

glDisable(GL_BLEND);


    glFlush();

    return 1;
}




void Asix(void)
{
	glBegin(GL_LINES);

	glColor3f(1,0,0);
	glVertex3f(0,0,0);
	glVertex3f(1,0,0);
	glVertex3f(1,-0.25,0.25);
	glVertex3f(1,0.25,-0.25);
	glVertex3f(1,-0.25,-0.25);
	glVertex3f(1,0.25,0.25);


	glColor3f(0,1,0);
	glVertex3f(0,0,0);
	glVertex3f(0,1,0);
	glVertex3f(0,1,0);
	glVertex3f(0.25,1,0);
	glVertex3f(0,1,0);
	glVertex3f(-0.25,1,0.25);
	glVertex3f(0,1,0);
	glVertex3f(-0.25,1,-0.25);


	glColor3f(0,0,1);
	glVertex3f(0,0,0);
	glVertex3f(0,0,1);
	glVertex3f(-0.25,-0.25,1);
	glVertex3f(0.25,0.25,1);
	glVertex3f(-0.25,-0.25,1);
	glVertex3f(0.25,-0.25,1);
	glVertex3f(-0.25,0.25,1);
	glVertex3f(0.25,0.25,1);

	glEnd();
}

