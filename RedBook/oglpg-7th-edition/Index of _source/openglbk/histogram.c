/*
 * Copyright (c) 1993-1999, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED 
 * Permission to use, copy, modify, and distribute this software for 
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that 
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission. 
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * US Government Users Restricted Rights 
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
 */

/*
 *  histogram.c
 *  Compute the histogram of the image.  This program illustrates the
 *  use of the glHistogram() function.
 */

#include <GL/glut.h>
#include <stdlib.h>

#define HISTOGRAM_SIZE  256   /* Must be a power of 2 */

extern GLubyte*  readImage(const char*, GLsizei*, GLsizei*);

GLubyte  *pixels;
GLsizei   width, height;


void init(void)
{
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glClearColor(0.0, 0.0, 0.0, 0.0);

   glHistogram(GL_HISTOGRAM, HISTOGRAM_SIZE, GL_RGB, GL_FALSE);
   glEnable(GL_HISTOGRAM);
}

void display(void)
{
   int i;
   GLushort values[HISTOGRAM_SIZE][3];
   
   glClear(GL_COLOR_BUFFER_BIT);
   glRasterPos2i(1, 1);
   glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
   
   glGetHistogram(GL_HISTOGRAM, GL_TRUE, GL_RGB, GL_UNSIGNED_SHORT, values);

   /* Plot histogram */
   
   glBegin(GL_LINE_STRIP);
   glColor3f(1.0, 0.0, 0.0);
   for ( i = 0; i < HISTOGRAM_SIZE; i++ )
       glVertex2s(i, values[i][0]);
   glEnd();

   glBegin(GL_LINE_STRIP);
   glColor3f(0.0, 1.0, 0.0);
   for ( i = 0; i < HISTOGRAM_SIZE; i++ )
       glVertex2s(i, values[i][1]);
   glEnd();

   glBegin(GL_LINE_STRIP);
   glColor3f(0.0, 0.0, 1.0);
   for ( i = 0; i < HISTOGRAM_SIZE; i++ )
       glVertex2s(i, values[i][2]);
   glEnd();
   glFlush();
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, 256, 0, 10000, -1.0, 1.0);
   glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
   static GLboolean sink = GL_FALSE;
    
   switch (key) {
      case 's' :
	  sink = !sink;
	  glHistogram(GL_HISTOGRAM, HISTOGRAM_SIZE, GL_RGB, sink);
	  break;
	  
      case 27:
         exit(0);
   }
   glutPostRedisplay();
   
}

/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
   pixels = readImage("Data/leeds.bin", &width, &height);    

   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize(width, height);
   glutInitWindowPosition(100, 100);
   glutCreateWindow(argv[0]);
   init();
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutDisplayFunc(display);
   glutMainLoop();
   return 0;
}
