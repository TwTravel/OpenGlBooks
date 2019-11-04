/*
    Imagedit - Demo for Animating Relief Impostors using Radial Basis Functions Textures
    Copyright (C) 2007 Vitor Fernando Pamplona (vitor@vitorpamplona.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GENERALVIEWER_
#define GENERALVIEWER_

void mouseGLUT(int button, int state, int x, int y);
void motionGLUT(int x, int y);
void keypressedGLUT(unsigned char key, int x, int y);
void displayGLUT(void);
void idleGLUT(void);
void reshapeGLUT(int w, int h);

/**
 * Basic class to Window control with GLUT
 * 
 * Responsibilities: 
 * - Initialize a basic GLUT window.
 * - Get events of mouse and keyboard 
 */ 
class GeneralViewer {
	
public:
	 
	GeneralViewer() {
	} 
	
	virtual ~GeneralViewer() {}
	 
	int width;
	int height;	
	
	void initGlut(char * title) {
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA);
		glutInitWindowSize(610, 630);
		glutInitWindowPosition(100, 100);
		glutCreateWindow(title);
		
		reshape(610,630);
		
  		glutDisplayFunc(displayGLUT);
  		glutMotionFunc(motionGLUT);   
  		glutMouseFunc(mouseGLUT);
		glutIdleFunc(idleGLUT);
    	glutReshapeFunc(reshapeGLUT);    
    	glutKeyboardFunc(keypressedGLUT);
	}
	
	virtual void init() {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-100.0, 100.0, -100.0, 100.0, -100., 100.);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glClearColor(0.0, 0.0, 0.0, 0.0);
	} 	

	virtual void reshape(int w, int h) {
	 	if (h==0)					
			h=1;
		width=w;
		height=h;
	
  		glViewport(0, 0, w, h);
  		glEnable(GL_TEXTURE_2D);
  		glMatrixMode(GL_PROJECTION);
  		glLoadIdentity();
  		glOrtho(0.0, w, h, 0.0, 0.0, 100.0);
	}
	
			
	virtual void takeControl() {
		glutMainLoop();
	}
	
	void mouse(int button, int state, int x, int y) {
		bool shift = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) == GLUT_ACTIVE_SHIFT;
		bool ctrl = (glutGetModifiers() & GLUT_ACTIVE_CTRL) == GLUT_ACTIVE_CTRL;
		bool alt = (glutGetModifiers() & GLUT_ACTIVE_ALT) == GLUT_ACTIVE_ALT;

		if (button == GLUT_LEFT_BUTTON) {
		   	if (state == GLUT_DOWN) {
				mousePressed(x, y, shift, ctrl, alt);		   		
		   	} else {
		   		mouseReleased(x, y, shift, ctrl, alt); 
		   	}
		} 
		if (button == GLUT_RIGHT_BUTTON) {
		   	if (state == GLUT_DOWN) {
				mouseRightPressed(x, y, shift, ctrl, alt);		   		
		   	} else {
		   		mouseRightReleased(x, y, shift, ctrl, alt); 
		   	}
		}		
	}
	
	void motion(int x, int y) {
		mouseMotion(x, y);
	}
	
	void key(unsigned char key, int x, int y) {
		keyPressed(key);
	}
	
	virtual void mousePressed(int x, int y, bool shift, bool ctrl, bool alt) {}
	virtual void mouseReleased(int x, int y, bool shift, bool ctrl, bool alt) {}
	virtual void mouseRightPressed(int x, int y, bool shift, bool ctrl, bool alt) {}
	virtual void mouseRightReleased(int x, int y, bool shift, bool ctrl, bool alt) {}
	virtual void mouseMotion(int x, int y) {}
	virtual void keyPressed(unsigned char key) {}
	
	virtual void display() {
		glMatrixMode(GL_MODELVIEW);	
		glLoadIdentity();	
		
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	  	glClearDepth(0.0f);
	  	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	  	glColor3f(1.0f, 1.0f, 1.0f);
	  	
	  	glutSwapBuffers();
	}
	
};

GeneralViewer * glutViewer;

void mouseGLUT(int button, int state, int x, int y) {
	glutViewer->mouse(button, state, x, y);
}

void keypressedGLUT(unsigned char key, int x, int y) {
	glutViewer->key(key, x, y);
}

void displayGLUT(void) {
	glutViewer->display();  	
} 

void reshapeGLUT(int w, int h) {
	glutViewer->reshape(w, h);
}

void motionGLUT(int x, int y) {
	glutViewer->motion(x, y);
}

void idleGLUT(void) {
	glutViewer->display(); 
}

#endif /*GENERALVIEWER_*/

