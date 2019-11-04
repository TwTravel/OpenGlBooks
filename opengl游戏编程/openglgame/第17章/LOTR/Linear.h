#ifndef _LINEAR_H
#define _LINEAR_H

#define M_PI 3.1415926539

#include <math.h>

/** 三维向量类 */
class vector
{
	public:
		float x,y,z;

		vector()
			{x=y=z=0;}

		vector(float xt, float yt, float zt)
			{x=xt; y=yt; z=zt;}

		void set(float xt, float yt, float zt)
			{x=xt; y=yt; z=zt;}

		void scale(float sc)
			{x*=sc; y*=sc; z*=sc;}

		void add(vector v)
			{x+=v.x; y+=v.y; z+=v.z;}

		float dot(vector v)
			{ return (x * v.x) + (y * v.y) + (z * v.z);}

		vector cross(vector v)
		{
			vector dest;
			dest.x = (y * v.z) - (z * v.y);
			dest.y = (z * v.x) - (x * v.z);
			dest.z = (x * v.y) - (y * v.x);
			return dest;
		}

		float magnitude()
		{
			return (float)sqrt(x*x+y*y+z*z);
		}

		float magnitude2()
		{
			return (float)(x*x+y*y+z*z);
		}


		void normalize()
		{
			float mag=magnitude();

			x/=mag;
			y/=mag;
			z/=mag;
		}
		
		vector project(vector v1)
		{
			vector temp(x,y,z);
			temp.scale(v1.dot(temp));
			return temp;
		}

		vector operator +(vector v1)
		{
			vector temp(0,0,0);
			temp.x=x+v1.x;
			temp.y=y+v1.y;
			temp.z=z+v1.z;
			return temp;
		}

		vector operator -(vector v1)
		{
			vector temp(0,0,0);
			temp.x=x-v1.x;
			temp.y=y-v1.y;
			temp.z=z-v1.z;
			return temp;
		}

		vector operator *(float s)
		{
			vector temp(x,y,z);
			temp.scale(s);
			return temp;
		}

		vector operator *(vector v1)
		{
			vector temp(x,y,z);
			temp.cross(v1);
			return temp;
		}
};

/*The following classes are not used in Whiplash*/
class line
{
	public:
		vector d; //direction of the line
		vector o; //origin of the line

		vector CalcPoint(float t)
		{
			vector temp;
			temp.x= o.x + d.x*t;
			temp.y = o.y + d.y*t;
			temp.z = o.z + d.z*t;
			return temp;
		}

		float Gett(vector v)
		{
			if(d.x!=0)
				return (v.x-o.x)/d.x;
			else if(d.y!=0)
				return (v.y-o.y)/d.y;

			return (v.z-o.z)/d.z;
		}

};

class plane
{
	public:
		vector n; //plane's normal
		vector o; //planes origin point
		float c; //plane equation constant

		void CalcC(void)
		{
			c=-(n.x * o.x + n.y * o.y + n.z * o.z);
		}

		void Create(vector ot,vector p1, vector p2)
		{
			o=ot;
			vector temp1,temp2;
			temp1=(p1-o);
			temp2=(p2-o);
			n=temp1.cross(temp2);
			n.normalize();
			CalcC();
		}

		int intersect(line ray,vector *v)
		{
	
	        float t = - (n.x*ray.o.x + n.y*ray.o.y + n.z*ray.o.z + c)/(n.x*ray.d.x + n.y*ray.d.y + n.z*ray.d.z);

			vector temp=ray.CalcPoint(t);

			if(t>0)
			{
				v->set(temp.x,temp.y,temp.z);
				return 1;
			}
			return 0;
		}
};

/** 以Z轴为中轴的圆柱体结构 */
class zcylinder 
{
	public:
		float radius;

		zcylinder()
		{
			radius=0;
		}

		zcylinder(float rt)
		{
			radius=rt;
		}

		int intersect(line ray, vector *v)
		{

			float a;
			float b;
			float c;

			a=ray.d.x*ray.d.x+ray.d.y*ray.d.y;
			b=2*(ray.o.x*ray.d.x+ray.o.y*ray.d.y);
			c=(ray.o.x*ray.o.x)+(ray.o.y*ray.o.y)-(radius*radius);

			float sq=b*b-4*a*c;
			float t1=-1,t2=-1;

			if(sq<0)
				return 0;

			t1=(-b+sqrt(sq))/(2*a);
			t2=(-b-sqrt(sq))/(2*a);
			
			if(fabs(t1)>fabs(t2))
			{
				float t3=t1;
				t1=t2;
				t2=t3;
			}

			float t;
			if(t1>0)
				t=t1;
			else if(t2>0)
				t=t2;
			else 
				return 0;

			vector temp=ray.CalcPoint(t);

			if(t>0)
			{
				v->set(temp.x,temp.y,temp.z);
				return 1;
			}
			return 0;
		}

		int intersect(line ray, vector *v1, vector *v2)
		{

			float a;
			float b;
			float c;

			a=ray.d.x*ray.d.x+ray.d.y*ray.d.y;
			b=2*(ray.o.x*ray.d.x+ray.o.y*ray.d.y);
			c=(ray.o.x*ray.o.x)+(ray.o.y*ray.o.y)-(radius*radius);

			float sq=b*b-4*a*c;
			float t1=-1,t2=-1;

			if(sq<0)
				return 0;

			t1=(-b+sqrt(sq))/(2*a);
			t2=(-b-sqrt(sq))/(2*a);
			
		
			vector temp=ray.CalcPoint(t1);

	//		if(t1>0)
	//			temp.set(0,0,0);
				v1->set(temp.x,temp.y,temp.z);
			

			vector temp1=ray.CalcPoint(t2);

	//		if(t2>0)
	//			temp1.set(0,0,0);
			//if(t>0)
			//{
			v2->set(temp1.x,temp1.y,temp1.z);
			
				return 1;
			//}
			return 0;
		}
};
#endif