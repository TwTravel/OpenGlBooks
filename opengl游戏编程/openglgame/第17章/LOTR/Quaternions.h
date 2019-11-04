#ifndef _QUATERNIONS_H
#define _QUATERNIONS_H

#define M_PI 3.1415926539

#include <math.h>
#include "linear.h"

/**********************************************
*quaternions  
**********************************************
*A quaternion class
**********************************************/
class quaternions
{
	public:
		float x,y,z,w;

		/************************************************
		*Function: Default constructor
		*Input: None
		*Return: None
		*Description: sets parameters to 0
		************************************************/
		quaternions()
			{x=y=z=w=0;}

		/************************************************
		*Function: Overloaded constructor
		*Input: Four floats
		*Return: None
		*Description: sets parameters(x,y,z,w) to (xt,yt,zt,wt)
		************************************************/
		quaternions(float xt, float yt, float zt, float wt)
			{x=xt; y=yt; z=zt; w=wt;}

		/************************************************
		*Function: Overloaded constructor
		*Input: A vector and a float
		*Return: None
		*Description: sets the vector part of the quaternion
		*             to v and w to wt
		************************************************/
		quaternions(vector v, float wt)
			{x=v.x; y=v.y; z=v.z; w=wt;}

		/************************************************
		*Function: Get Vector
		*Input: None
		*Return: vector
		*Description: Returns the vector part
		************************************************/
		vector getvector()
		{ 
			vector v(x,y,z);
			return v;
		}

		/************************************************
		*Function: Get Scalar
		*Input: None
		*Return: float
		*Description: Returns the scalar part
		************************************************/
		float getscalar()
		{
			return w;
		}

		/************************************************
		*Function: Set Vector
		*Input: Three floats
		*Return: None
		*Description: Setes (x,y,z) to (xt,yt,zt)
		************************************************/
		void setvector(float xt, float yt, float zt)
			{x=xt; y=yt; z=zt;}

		/************************************************
		*Function: Set Vector
		*Input: vector
		*Return: None
		*Description: Sets the vector part to v
		************************************************/
		void setvector(vector v)
			{x=v.x; y=v.y; z=v.z;}

		/************************************************
		*Function: Conjugate
		*Input: None
		*Return: None
		*Description: Calculates the conjugate of the quaternion
		************************************************/
		void conjugate(void)
			{x=-x; y=-y; z=-z;}

		/************************************************
		*Function: Magnitude
		*Input: None
		*Return: None
		*Description: Calculates the magnitude
		************************************************/
		float magnitude(void)
			{ return sqrt(w*w+x*x+y*y+z*z);}

		/************************************************
		*Function: Normalize
		*Input: None
		*Return: None
		*Description: Normalizes the quaternion
		************************************************/
		void normalize(void)
		{
			float mag=magnitude();
			x/=mag;
			y/=mag;
			z/=mag;
			w/=mag;
		}

		/************************************************
		*Function: Multiply
		*Input: A quaternions
		*Return: None
		*Description: Multiplies the quaternions with another
		*             and returns the result
		************************************************/
		quaternions multiply(quaternions q)
		{
			quaternions dest;
			
			vector v1=getvector();
			vector v2=q.getvector();
			dest.w = (w*q.w)-v1.dot(v2);
		    vector v3=v1.cross(v2);
			v1.scale(q.w);
			v2.scale(w);
			v3.add(v1);
			v3.add(v2);
			dest.setvector(v3);

		    dest.normalize();
			return dest;
		}

		/************************************************
		*Function: Get rotation Matrix
		*Input: None
		*Return: A 4*4 array of floats
		*Description: Returns the rotation matrix associated
		*             to the quaternion
		************************************************/
		void getrotationmatrix(double* mat)
		{
		    float xx      = x * x;
			float xy      = x * y;
			float xz      = x * z;
		    float xw      = x * w;

		    float yy      = y * y;
		    float yz      = y * z;
		    float yw      = y * w;

		    float zz      = z * z;
			float zw      = z * w;

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

		}

		/************************************************
		*Function: Axis Angle to Quaternion
		*Input: Four floats
		*Return: None
		*Description: Creates a quaternion from a axis and
		*             a rotation angle around that axis
		************************************************/
		void axisangletoquaternions(float xt, float yt, float zt, float an)
		{
			float sin_a = sin( (an*(M_PI/180.0)) / 2.0 );
			float cos_a = cos( (an*(M_PI/180.0)) / 2.0 );

			x    = xt * sin_a;
			y    = yt * sin_a;
			z    = zt * sin_a;
			w    = cos_a;

			normalize();
    
		}

		/************************************************
		*Function: Euler to Quaternions
		*Input: Three float
		*Return: None
		*Description: Creates a quaternion from three euler angles
		************************************************/
		void eulertoquaternions( float pitch,float yaw,float roll )
		{
			quaternions qx, qy, qz,qt, q;

			qx.axisangletoquaternions(1,0,0,pitch);
			qy.axisangletoquaternions(0,1,0,yaw);
			qz.axisangletoquaternions(0,0,1,roll);
			
			qt=qx.multiply(qy);
			q=qt.multiply(qz);
			
			x = q.x;
			y = q.y;
			z = q.z;
			w = q.w;

			//normalization was done in the multiply step
		}
};

/**********************************************
*CRU
**********************************************
*Cummulative Rotation Units
*Three orthogonal vectors and a quaternion representing
*a local system of vectors
**********************************************/
class CRU //Cummulative rotation units
{
	public:
		vector dir,up,right;
		quaternions q;
		
		/************************************************
		*Function: Default constructor
		*Input: None
		*Return: None
		*Description: sets parameters to 0
		************************************************/
		CRU()
		{
			q.eulertoquaternions(0,0,0);
			dir.set(0,1,0);
			up.set(0,0,1);
			right.set(1,0,0);
		}
		
	/************************************************
	*Function: rotation
	*Input: Ten floats
	*Return: None
	*Description: Calculates the rotation of vector A
	*             towards B around C.
	************************************************/
	void rotation(float &Ax,float &Ay,float &Az,float &Bx,float &By,float &Bz,float Cx,float Cy,float Cz,float angle)
	{
		//A towards B around C by angle
		float OAx=Ax;
		float OAy=Ay;
		float OAz=Az;

		float theta=angle*M_PI/180;

		float c=cos(theta);
		float s=sin(theta);

		float den=(Bx*Cy*OAz+By*OAx*Cz-By*Cx*OAz-Bx*Cz*OAy-Bz*OAx*Cy+Bz*Cx*OAy);
		Ax=(Cz*c*By-Cz*s*OAy-Cy*c*Bz+s*Cy*OAz)/den;
		Ay=-(s*OAz*Cx-s*OAx*Cz-Cx*c*Bz+c*Bx*Cz)/den;
		Az=(-c*By*Cx+Cx*s*OAy-Cy*s*OAx+c*Bx*Cy)/den;

		float OBx=Bx;
		float OBy=By;
		float OBz=Bz;

		theta=-angle*M_PI/180;
		c=cos(theta);
		s=sin(theta);

		den=(OAx*Cy*OBz+OAy*OBx*Cz-OAy*Cx*OBz-OAx*Cz*OBy-OAz*OBx*Cy+OAz*Cx*OBy);
		Bx=(Cz*c*OAy-Cz*s*OBy-Cy*c*OAz+s*Cy*OBz)/den;
		By=-(s*OBz*Cx-s*OBx*Cz-Cx*c*OAz+c*OAx*Cz)/den;
		Bz=(-c*OAy*Cx+Cx*s*OBy-Cy*s*OBx+c*OAx*Cy)/den;
	}

	/************************************************
	*Function: Pitch
	*Input: float
	*Return: None
	*Description: Rotates the system around the right vector
	************************************************/
	void pitch(float angle)
	{
		rotation(dir.x,dir.y,dir.z,up.x,up.y,up.z,right.x,right.y,right.z,angle);
		quaternions temp;
		temp.axisangletoquaternions(right.x,right.y,right.z,-angle);
		q=q.multiply(temp);
	}

	/************************************************
	*Function: Yaw
	*Input: float
	*Return: None
	*Description: Rotates the system around the up vector
	************************************************/
	void yaw(float angle)
	{
		rotation(right.x,right.y,right.z,dir.x,dir.y,dir.z,up.x,up.y,up.z,angle);
		quaternions temp;
		temp.axisangletoquaternions(up.x,up.y,up.z,-angle);
		q=q.multiply(temp);
	}

	/************************************************
	*Function: Roll
	*Input: float
	*Return: None
	*Description: Rotates the system around the Direction vector
	************************************************/
	void roll(float angle)
	{
		rotation(up.x,up.y,up.z,right.x,right.y,right.z,dir.x,dir.y,dir.z,angle);
		quaternions temp;
		temp.axisangletoquaternions(dir.x,dir.y,dir.z,-angle);
		q=q.multiply(temp);
	}

	/************************************************
	*Function: Overloaded constructor
	*Input: Three floats
	*Return: None
	*Description: Sets the systems to the coresponding angles
	************************************************/
	CRU(float p,float y, float r)
	{
		pitch(p);
		yaw(y);
		roll(r);
	}
	//Missing constructor (dir,up)

	/************************************************
	*Function: Set
	*Input: Three floats
	*Return: None
	*Description: sets parameters the CRU to the euler angles (p,y,r)
	************************************************/
	void set(float p,float y, float r)
	{
	
		q.eulertoquaternions(0,0,0);
		dir.set(0,1,0);
		up.set(0,0,1);
		right.set(1,0,0);
		pitch(p);
		yaw(y);
		roll(r);
	}

	/************************************************
	*Function: SetVectorstoQuaternion
	*Input: None
	*Return: None
	*Description: sets the 3 vectors to the current quaternion
	************************************************/
	void SetVectorstoQuaternion(void)
	{
		dir.set(0,1,0);
		up.set(0,0,1);
		right.set(1,0,0);
		double mat[16];
		q.getrotationmatrix(mat);

		//Special Case, since vector are aligned
		dir.set(mat[1]*dir.y,mat[5]*dir.y,mat[9]*dir.y);
		right.set(mat[0]*right.x,mat[4]*right.x,mat[8]*right.x);
		up.set(mat[2]*up.z,mat[6]*up.z,mat[10]*up.z);

	}

	vector project(vector v)
	{
		vector res;
		res=dir*v.z+right*v.x+up*v.y;
		return res;
	}
};
#endif