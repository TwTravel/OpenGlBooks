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

#ifndef SIMPLEGLWRITER_H_
#define SIMPLEGLWRITER_H_

/**
 * Class to write the help menu.
 */
class SimpleGLWriter {
public:
	 
	int width;
	int height; 

	float posX; 
	float posY;

	SimpleGLWriter(int _width, int _height) {
		width = _width;
		height = _height;
		posX = 0;
		posY = 0;
	}
	virtual ~SimpleGLWriter() {}

	void setPosition(float x, float y) {
		posX = x;
		posY = y;
	}
	
	void setPositionFromBotton(float x, int numberOfMessages) {
		posX = x;
		posY = height - 15*numberOfMessages;
	}	

	void renderString(std::stringstream & string, float _posX, float _posY) {
		string << "                        ";
		
		posX = _posX;
		posY = _posY; 
		
		renderString(const_cast<char *>(string.str().c_str()));
	}	
	
	void renderString(char * string, float _posX, float _posY) {
		posX = _posX;
		posY = _posY; 
		
		renderString(string);
	}
	
	void renderString(std::stringstream & string) {
		string << "                        ";
				
		renderString(const_cast<char *>(string.str().c_str()));
	}		
	
	void renderString(char * string) {
		glMatrixMode(GL_PROJECTION);
	 	glPushMatrix();
		glLoadIdentity();
	 	gluOrtho2D(0, width, 0, height);
	 	glScalef(1, -1, 1);
	 	// puts the (0,0) in left-upper.
	 	glTranslatef(0, -height, 0);
	 	glMatrixMode(GL_MODELVIEW);
	
		glPushMatrix();
		glLoadIdentity();
	    glColor3f(0.0f, 1.0f, 0.0f);	
	
	  	char *c;
		glRasterPos2f(posX,posY);  	
	  	for (c=string; *c != '\0'; c++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
	  	}
	
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);  
		
		posY += 15; 		
	}
};

#endif /*SIMPLEGLWRITER_H_*/
