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

#ifndef MOVINGCUBE_H_
#define MOVINGCUBE_H_

/**
 * Render a Cube. 
 */
class MovingCube : public Moving {
public:
	
	MovingCube() : Moving() {}
	virtual ~MovingCube() {}

	virtual void draw(float size, int invNormal) {
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);

		 invNormal = - invNormal;

		glBegin(GL_QUADS);

	    glNormal3f(-invNormal,+invNormal,-invNormal);glTexCoord2i(1,1);glVertex3f(-size,+size,-size);
	    glNormal3f(+invNormal,+invNormal,-invNormal);glTexCoord2i(0,1);glVertex3f(+size,+size,-size);
	    glNormal3f(+invNormal,-invNormal,-invNormal);glTexCoord2i(0,0);glVertex3f(+size,-size,-size);
	    glNormal3f(-invNormal,-invNormal,-invNormal);glTexCoord2i(1,0);glVertex3f(-size,-size,-size);

	    glNormal3f(-invNormal,+invNormal,+invNormal);glTexCoord2i(0,1);glVertex3f(-size,+size,+size);
	    glNormal3f(-invNormal,-invNormal,+invNormal);glTexCoord2i(0,0);glVertex3f(-size,-size,+size);
	    glNormal3f(+invNormal,-invNormal,+invNormal);glTexCoord2i(1,0);glVertex3f(+size,-size,+size);
	    glNormal3f(+invNormal,+invNormal,+invNormal);glTexCoord2i(1,1);glVertex3f(+size,+size,+size);

		glNormal3f(+invNormal,-invNormal,+invNormal);glTexCoord2i(0,0);glVertex3f(+size,-size,+size);
	    glNormal3f(+invNormal,-invNormal,-invNormal);glTexCoord2i(1,0);glVertex3f(+size,-size,-size);
	    glNormal3f(+invNormal,+invNormal,-invNormal);glTexCoord2i(1,1);glVertex3f(+size,+size,-size);
	   	glNormal3f(+invNormal,+invNormal,+invNormal);glTexCoord2i(0,1);glVertex3f(+size,+size,+size);

	    glNormal3f(-invNormal,-invNormal,-invNormal);glTexCoord2i(0,0);glVertex3f(-size,-size,-size);
	    glNormal3f(-invNormal,-invNormal,+invNormal);glTexCoord2i(1,0);glVertex3f(-size,-size,+size);
	    glNormal3f(-invNormal,+invNormal,+invNormal);glTexCoord2i(1,1);glVertex3f(-size,+size,+size);
	    glNormal3f(-invNormal,+invNormal,-invNormal);glTexCoord2i(0,1);glVertex3f(-size,+size,-size);
		
	    glNormal3f(-invNormal,+invNormal,-invNormal);glTexCoord2i(1,0);glVertex3f(-size,+size,-size);
	    glNormal3f(+invNormal,+invNormal,-invNormal);glTexCoord2i(1,1);glVertex3f(+size,+size,-size);
	    glNormal3f(+invNormal,+invNormal,+invNormal);glTexCoord2i(0,1);glVertex3f(+size,+size,+size);
	    glNormal3f(-invNormal,+invNormal,+invNormal);glTexCoord2i(0,0);glVertex3f(-size,+size,+size);

	    glNormal3f(-invNormal,-invNormal,-invNormal);glTexCoord2i(1,0);glVertex3f(-size,-size,-size);
	    glNormal3f(+invNormal,-invNormal,-invNormal);glTexCoord2i(1,1);glVertex3f(+size,-size,-size);
	    glNormal3f(+invNormal,-invNormal,+invNormal);glTexCoord2i(0,1);glVertex3f(+size,-size,+size);
	    glNormal3f(-invNormal,-invNormal,+invNormal);glTexCoord2i(0,0);glVertex3f(-size,-size,+size);

		glEnd();
	}
};

#endif /*MOVINGCUBE_H_*/

