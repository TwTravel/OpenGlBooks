// ver. 1.9.1
#include "mathvect.h"

vect3 vect3_temp;

float f_temp;


vect3::vect3(float x,float y,float z)
	{
	this->x=x;
	this->y=y;
	this->z=z;
	}


vect3::vect3(float a)
	{
	x=a;
	y=a;
	z=a;
	}

vect3::vect3(float *faVect3)
	{
	x=faVect3[0];
	y=faVect3[1];
	z=faVect3[2];
	}

vect3::vect3()
	{
	x=0;
	y=0;
	z=0;
	}

vect3 vect3::operator* (float value)  // operator* is used to scale a Vector3D by a value. This value multiplies the Vector3D's x, y and z.
	{
               	return vect3(x*value,y*value,z*value);
	}


vect3 vect3::operator/ (float value)			// operator* is used to divide a Vector3D by a value. This value multiplies the Vector3D's x, y and z.
	{
                if (value!=0)
                        return vect3(x/value,y/value,z/value);
		else return vect3(x,y,z);
	}

vect3 vect3:: operator+= (vect3 v)			// operator+= is used to add another Vector3D to this Vector3D.
	{
		x += v.x;
		y += v.y;
		z += v.z;
                return *this;
	}

vect3 vect3::operator+ (vect3 v)  // add vec+vec
	{
               return vect3(x+v.x,y+v.y,z+v.z);
	}


vect3 vect3::operator-=(vect3 v)  // operator+= is used to sub another Vector3D to this Vector3D.
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
                return *this;

	}

vect3 vect3::operator- (vect3 v)  // sub vec+vec
	{
               	return vect3(x-v.x,y-v.y,z-v.z);
	}

vect3 vect3::operator- ()  // add vec+vec
	{
              	return vect3(-x,-y,-z);
	}

vect3 vect3::operator*(vect3 v) // CROS PRODUCT
	{
         vect3_temp.x=v.y*z-v.z*y;
         vect3_temp.y=v.z*x-v.x*z;
         vect3_temp.z=v.x*y-v.y*x;
         return vect3_temp;
	}


float vect3::operator^(vect3 v) // DOT PRODUCT
	{
	return v.x*x+v.y*y+v.z*z;
	}


vect3 vect3::Rotate(float alfa,float vnX,float vnY,float vnZ)
{
// alfa - angle in degrees
// vnX,vnY,vnZ - normal vector to arounding our vector
float s; // sinus result
float c; // cosinus result

alfa/=57.29577951f; // from DEG to RAD conversion
s=sin(alfa);
c=cos(alfa);

f_temp =x*(vnX*vnX+c*(1-vnX*vnX));
f_temp+=y*(vnX*vnY*(1-c)-s*vnZ);
f_temp+=z*(vnX*vnZ*(1-c)+s*vnY);
vect3_temp.x=f_temp;

f_temp= x*(vnY*vnX*(1-c)+s*vnZ);
f_temp+=y*(vnY*vnY+c*(1-vnY*vnY));
f_temp+=z*(vnY*vnZ*(1-c)-s*vnX);
vect3_temp.y=f_temp;

f_temp= x*(vnZ*vnX*(1-c)-s*vnY);
f_temp+=y*(vnZ*vnY*(1-c)+s*vnX);
f_temp+=z*(vnZ*vnZ+c*(1-vnZ*vnZ));
vect3_temp.z=f_temp;

return vect3_temp;
}


float vect3::l() // vector lenght
	{
	return sqrt(x*x+y*y+z*z);
	}



vect3 vect3::n() // normal vector
	{
	float length=sqrt(x*x+y*y+z*z);

	if (length==0)
		{
		vect3_temp.x=0.0;
		vect3_temp.y=0.0;
		vect3_temp.z=0.0;
		}
	else
		{
		vect3_temp.x=x/length;
		vect3_temp.y=y/length;
		vect3_temp.z=z/length;
		}
	return vect3_temp;
	}

void vect3::pointer(float vv[])
        {vv[0]=x;
         vv[1]=y;
         vv[2]=z;
        }



