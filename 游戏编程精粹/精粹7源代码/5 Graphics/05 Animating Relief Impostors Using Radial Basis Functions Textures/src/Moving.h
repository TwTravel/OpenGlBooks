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

#ifndef MOVING_H_
#define MOVING_H_

/**
 * Moving is the base class for any object that are in movement in the scene.
 * There, we put all camera and model logic to render. 
 * 
 * This object holds a simple logic of rotating in Y axis
 */
class Moving : public Rotationable {
private:
 	bool isCullFaceEnable;
 	bool isToMove;
 	
 	float x; 
 	float y;
 	float z;

 	float objX; 
 	float objY;
 	float objZ;
 	
 	Vector3f view; 
	Vector3f up; 	
	Vector3f viewVector;
public:

	Moving() : Rotationable() { 
		isToMove=false; 
		x = 0.0f; 
		y = 0.0f; 
		z=  0.0f;
		objX = 0.0f;
		objY = 0.0f; 
		objZ = 0.0f;
		
		up = Vector3f(0,1,0);
		view = Vector3f(-x,-y,-z);
		viewVector = Vector3f(0,0,1);
	}
	virtual ~Moving() {}
	
	void setCameraPosition(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
		
		view = Vector3f(-x,-y,-z);
	}
	
	void setObjPosition(float _x, float _y, float _z) {
		objX = _x;
		objY = _y;
		objZ = _z;
	}	
	
	void prepare(float size, float width, float height) {
		isCullFaceEnable = glIsEnabled(GL_CULL_FACE);
		
		if (!isCullFaceEnable) {
			glEnable(GL_CULL_FACE);
		}
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0f,(GLfloat)width/(GLfloat)height,0.1,size*10);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();				
		gluLookAt(x,y,z, 0,0,0, 0,1,0);		
		
		glColor3f(1.0f, 1.0f, 1.0f);	

		glTranslatef(objX, objY, objZ);

		loadRotation();
	}

	void getEyePosition(float * array4, float size) {
		array4[0] = 0.0;
		array4[1] = size*2;
		array4[2] = size*4.5;
		array4[1] = 1.0;
	}
	
	void toggleIsToMove() {
		isToMove = !isToMove;
	}
	
	virtual void draw(float size, int invNormal) {
		
	}
	
	void mouseMove(double fromX, double toX, double fromY, double toY) {
		 Rotationable::mouseMove(fromX, toX, fromY, toY, viewVector, up);		 	
	}
	
	void unprepare() {
		if (isToMove) {
			rotate(0.5f,0.0f,1.0f,0.0f);
		}
		
		if (!isCullFaceEnable) {
			glDisable(GL_CULL_FACE);
		}			
	}
};


#endif
