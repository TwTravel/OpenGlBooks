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

#ifndef ROTATIONABLE_H_
#define ROTATIONABLE_H_

/**
 * This class must be the basic for any model that wanna receive mouse interactions of rotation. 
 */
class Rotationable {
	
	double mouseSensivity;
	Quaternion rotation;
	
public:
	Rotationable() {
		mouseSensivity = 0.5;
		rotation.loadIdentity();
	}
	
	virtual ~Rotationable() {}
	
	virtual void loadRotation() {
		glMultMatrixf(rotation.toMatrix4x4());
	}
	
	
	// angle in degrees 
	virtual void rotate(double angle, double x, double y, double z) {
		Quaternion rotor;
		rotor.setRotor(degToRad(angle), x, y, z);
		rotation = rotation.mult(rotor);
	} 
	
	virtual void mouseMove(double fromX, double toX, double fromY, double toY, Vector3f & view, Vector3f & up) {
		double angleX = (toX - fromX) * mouseSensivity;
		double angleY = (toY - fromY) * mouseSensivity;
		
		Quaternion rotor1;
		Quaternion rotor2;
		
		Vector3f rotY = view.crossProduct(up);
		
		rotor1.setRotor(-degToRad(angleX), up[0], up[1], up[2]);
		rotor2.setRotor(degToRad(angleY), rotY[0], rotY[1], rotY[2]);
	
		rotation = rotation.mult(rotor1.mult(rotor2));
	}
	
};

#endif /*ROTATIONABLEOBJECT_H_*/
