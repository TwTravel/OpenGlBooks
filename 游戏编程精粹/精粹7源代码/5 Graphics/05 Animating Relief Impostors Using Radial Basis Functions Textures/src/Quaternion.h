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

#ifndef QUATERNION_H_
#define QUATERNION_H_

/**
 * Quaternion used to mouse interactions with the model
 */
class Quaternion {
	
public:
	// x,y,z is the vector;
	float x; 
	float y; 
	float z;
	// w is the scalar.
	float w;

	double length() {
  		return sqrt(x * x + y * y + z * z + w * w);
	}
	
	Quaternion normalize() {
		Quaternion quat;
  		double L = length();

		quat.x /= L;
  		quat.y /= L;
  		quat.z /= L;
  		quat.w /= L;

  		return quat;
	}
	
	Quaternion conjugate() {
		Quaternion quat;
  		quat.x = -x;
  		quat.y = -y;
  		quat.z = -z;
  		quat.w = w;
  		return quat;
	}

	float * toArray() {
		float * arr = new float[4];
		arr[0] = x;
		arr[1] = y;
		arr[2] = z;
		arr[3] = w;
		return arr;
	}

	// unifies two rotations. 
	Quaternion mult(Quaternion outro) {
  		Quaternion C;
		// Cross product
  		C.x = w*outro.x + x*outro.w + y*outro.z - z*outro.y;
  		C.y = w*outro.y - x*outro.z + y*outro.w + z*outro.x;
  		C.z = w*outro.z + x*outro.y - y*outro.x + z*outro.w;
  		C.w = w*outro.w - x*outro.x - y*outro.y - z*outro.z;
  		return C;
	}
	
	// Rotaciona o quartenion no rotor. 
	// Este quaternion representa um vetor, equanto o rotor
	// representa um eixo mais um angulo de rotacao. 
	Quaternion rotate(Quaternion rotor) {
		// W = R * V * R' where R' is the conjugate of R.
		return rotor.mult(*this).mult(rotor.conjugate()); 
	}
	
	float * toMatrix4x4() {
		float xx = x * x;
    	float xy = x * y;
    	float xz = x * z;
    	float xw = x * w;

    	float yy = y * y;
    	float yz = y * z;
    	float yw = y * w;

    	float zz = z * z;
    	float zw = z * w;

		float * mat = new float[16]; 

    	mat[0]  = 1 - 2 * ( yy + zz );
    	mat[1]  =     2 * ( xy - zw );
    	mat[2]  =     2 * ( xz + yw );

	    mat[4]  =     2 * ( xy + zw );
	    mat[5]  = 1 - 2 * ( xx + zz );
	    mat[6]  =     2 * ( yz - xw );

	    mat[8]  =     2 * ( xz - yw );
	    mat[9]  =     2 * ( yz + xw );
	    mat[10] = 1 - 2 * ( xx + yy );

	    mat[3]  = mat[7] = mat[11] = mat[12] = mat[13] = mat[14] = 0;
	    mat[15] = 1;
	    
	    return mat;
	}

	void setVector(double _x, double _y, double _z) {
		x = _x;
  		y = _y;
  		z = _z;
  		w = 0;
	}

	void setVector(Vector3f & v) {
		x = v[0];
  		y = v[1];
  		z = v[2];
  		w = 0;
	}
	
	void loadIdentity() {
		setRotor(0,0,1,0);
		//x = 0;
  		//y = 0;
  		//z = 0;
  		//w = 0;
	}

	void setRotor(double angle, double _x, double _y, double _z) {
		x = _x * sin(angle/2);
  		y = _y * sin(angle/2);
  		z = _z * sin(angle/2);
  		w = cos(angle/2);
	}

	Quaternion() {
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}
	
	Quaternion(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
		w = 0;
	}
	
	Quaternion(float _x, float _y, float _z, float _w) {
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}
	
    std::string print() {
    	std::stringstream stream;
		stream << "[ ";
		stream << x << ", " << y << ", " << z << ", " << w;
        stream << "] ";
        
        return stream.str();
    } 	
	
	virtual ~Quaternion() {}
};

#endif /*QUTERNION_H_*/
