// ver. 0.1
#include "quaternion.h"


quaternion::quaternion(float x,float y,float z,float w)
	{
	this->x=x;
	this->y=y;
	this->z=z;
	this->w=w;
	}


quaternion::quaternion()
	{
	x=0;
	y=0;
	z=0;
	w=1;
	}


quaternion quaternion::operator*(quaternion q)
	{
    double rx, ry, rz, rw;		// temp result

	rw	= q.w*w - q.x*x - q.y*y - q.z*z;

	rx	= q.w*x + q.x*w + q.y*z - q.z*y;
	ry	= q.w*y + q.y*w + q.z*x - q.x*z;
	rz	= q.w*z + q.z*w + q.x*y - q.y*x;

	return quaternion((float)rx,(float)ry,(float)rz,(float)rw);
	}


quaternion quaternion::AsixAngle()
{
vect3 v(x,y,z);

       if (v.l()==0) return quaternion(0,0,1,0);
       else
       {
            v=v.n();
            return quaternion(v.x,v.y,v.z,2.0f*acos(w));
       }
}

quaternion quaternion::operator~ ()
{
       	return quaternion(-x,-y,-z,w);
}

quaternion quaternion:: operator+= (quaternion q)	// operator+= is used to add another Vector3D to this Vector3D.
{
	x += q.x;
	y += q.y;
	z += q.z;
	w += q.w;
        return *this;
}

quaternion quaternion:: operator+ (quaternion q)	// operator+= is used to add another Vector3D to this Vector3D.
{
        return quaternion(x+q.x,y+q.y,z+q.z,w+q.w);
}

quaternion quaternion::n()
{
float length;

	length=sqrt(x*x+y*y+z*z+w*w);
	if (length==0) return quaternion(0,0,0,1);
	    else return quaternion(x/length,y/length,z/length,w/length);
}

float quaternion::l()
{
	return sqrt(x*x+y*y+z*z+w*w);
}

quaternion quaternion::operator* (float value)
{
	return quaternion(x*value,y*value,z*value,w*value);
}

quaternion quaternion::operator/ (float value)
{
	return quaternion(x/value,y/value,z/value,w/value);
}


quaternion AsixToQuat(vect3 v, float angle)
{
float x,y,z,w;			// temp vars of vector
double rad, scale;		// temp vars

	if (v.l()==0) return quaternion();

	v=v.n();
	rad=angle/2.0;
	scale= sin(rad);


	w = (float)cos(rad);

	x = float(v.x * scale);
	y = float(v.y * scale);
	z = float(v.z * scale);

	return quaternion(x,y,z,w);
}

