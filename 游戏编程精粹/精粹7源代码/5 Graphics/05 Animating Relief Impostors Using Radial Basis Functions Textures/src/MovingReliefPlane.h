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

#ifndef MOVINGRELIEFPLANE_H_
#define MOVINGRELIEFPLANE_H_

/**
 * Draw Two planes with opposite normals.
 * 
 * This kind of object is commonlly used with Relief Maps 
 */
class MovingReliefPlane : public Moving {
public:

	MovingReliefPlane() : Moving() {}
	virtual ~MovingReliefPlane() {}

	void draw(float size, int invNormal) {
		glEnable(GL_BLEND);
		glFrontFace(GL_CCW);

		Moving::draw(size, invNormal);

		size *= 2;

		glBegin(GL_QUADS);
		
	    glNormal3f(0,0,-invNormal);glTexCoord2f(-0.5,-0.5);glVertex3f(-size,-size,0);
	    glNormal3f(0,0,-invNormal);glTexCoord2f(1.5,-0.5);glVertex3f(+size,-size,0);
	    glNormal3f(0,0,-invNormal);glTexCoord2f(1.5,1.5);glVertex3f(+size,+size,0);
	    glNormal3f(0,0,-invNormal);glTexCoord2f(-0.5,1.5);glVertex3f(-size,+size,0);

		glEnd();

		glFrontFace(GL_CW);
		
		glBegin(GL_QUADS);

	    glNormal3f(0,0,invNormal);glTexCoord2f(-0.5,-0.5);glVertex3f(-size,-size,0);
	    glNormal3f(0,0,invNormal);glTexCoord2f(1.5,-0.5);glVertex3f(+size,-size,0);
	    glNormal3f(0,0,invNormal);glTexCoord2f(1.5,1.5);glVertex3f(+size,+size,0);
	    glNormal3f(0,0,invNormal);glTexCoord2f(-0.5,1.5);glVertex3f(-size,+size,0);
		
		glEnd();

		glFrontFace(GL_CCW);
	}
};

#endif /*MOVINGCUBE_H_*/

