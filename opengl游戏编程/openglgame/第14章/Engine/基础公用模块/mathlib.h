//========================================================
/**
*  @file      mathlib.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  数学计算用的类的结构  
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/     
//========================================================


#ifndef _MATH_LIB_H__
#define _MATH_LIB_H__

#include <math.h>
#include <assert.h>

#ifndef EPSILON
#define EPSILON 0.0001f
#endif

#ifndef PI
#define PI 3.1415926f
#endif



//////////////////////////////////////////////////////
/**\brief
 * vector2d 2D矢量
 */
struct vector2d
{
	//----------------------------------
	union
	{
		struct{float x , y;};
		float vector2[2];
	};
	//----------------------------------

	vector2d(){}
	~vector2d(){}

	vector2d(float _x ,float _y):x(_x),y(_y){}
	vector2d(float *f):x(f[0]),y(f[1])		{}
	vector2d(const vector2d& vector):x(vector.x),y(vector.y){}

	float mag()		{return (float)sqrt(x*x+y*y);}
	float mag_sqr()	{return x*x + y*y;}

	/**\brief
	 * 两点之间的距离
	 */
	float dist(vector2d& vector);
	float dot(vector2d& vector);

	/**\brief
	 * 得到二维向量的垂直向量
	 */
	void change(){float m;m = x;x = -y;y = m;}

	/**\brief
	 * 点到直线的距离
	 */
	float dist(vector2d&p1 , vector2d&p2);

	/**\brief
	 * 判断点到直线的投影是否在线段内
	 */
	BOOL bPtInLine(vector2d p1 , vector2d p2);

	float dist_sqr(vector2d& vector);

	void set(float _x , float _y){x=_x;y=_y;}

	void normalize(){float d=mag();x/=d;y/=d;}

	void operator  =(vector2d vector) {x = vector.x;y = vector.y;}
	void operator +=(vector2d& vector){x+= vector.x;y+= vector.y;}
	void operator -=(vector2d& vector){x-= vector.x;y-= vector.y;}

	vector2d operator +(vector2d& vector);
	vector2d operator -(vector2d& vector);

	vector2d operator *(float s);
	vector2d operator /(float& s);


	/**\brief
	 * 得到向量的弧度
	 */
	float getradian()
	{
		if(fabs(x)<=EPSILON&&y>0)return 90.0f;
		if(fabs(x)<=EPSILON&&y<0)return 270.0f;
		if(fabs(y)<=EPSILON&&x>0)return 0.0f;
		if(fabs(y)<=EPSILON&&x<0)return 180.0f;

		float rel = asinf(fabsf(y));
		rel = rel*180.0f / PI;
		if(x>0&&y>0)return rel;
		if(x<0&&y>0)return 180.0f - rel;
		if(x<0&&y<0)return 180.0f + rel;
		if(x>0&&y<0)return 360.0f - rel;

		return 0;
	}


	/**\brief
	 * 旋转向量
	 */
	void rotate(float s);
};



///////////////////////////////////////////////////////////
/**\brief
 * 3D矢量结构
 */
struct vector3d
{
	//-------------------------------------
	union
	{
		struct{float x,y,z;};
		float v[3];
	};
	//-------------------------------------

	vector3d()	{}
	~vector3d()	{}

	vector3d(float _x,float _y,float _z):x(_x),y(_y),z(_z)	{}
	vector3d(float f[]):x(f[0]),y(f[1]),z(f[2])				{}

	void set(float _x,float _y,float _z){x=_x;y=_y;z=_z;}

	float mag(){return sqrtf(x*x + y*y + z*z);}
	float mag_sqr(){return x*x + y*y + z*z;}

	float dist(vector3d& vector);
	float dist_sqr(vector3d& vector);

	vector3d cross(const vector3d& p);


	/**\brief
	 * 矢量的规格化
	 */
	void normalize();

	void operator  =(const vector3d& p);
	void operator +=(const vector3d& p);
	void operator -=(const vector3d& p);
	void operator *=(const float& s);
	void operator /=(const float& s);

	vector3d operator+(const vector3d& _v);
	vector3d operator-(const vector3d& _v);
	vector3d operator*(const float& s);
	
	static float dot(const vector3d& p1,const vector3d& p2);
};



///////////////////////////////////////////////////////
/**\brief
 * 2D空间中的线
 */
struct line2d
{
	vector2d p1;
	vector2d p2;

	BOOL IsIntersect(line2d& line);

	vector2d GetIntersectPoint(line2d& line);
};



//////////////////////////////////////////////////////
/**\brief
 * 圆结构
 */
struct SCircle
{
	//----------------------------------
	//! 圆心位置
	vector2d	pos;
	//! 半径
	float		radius;
	//----------------------------------

	SCircle()	{}
	~SCircle()	{}

	/**\brief
	 * 判断线段和圆是否相交
	 */
	BOOL isIntersect(vector2d& p1 , vector2d& p2);
	void Set(vector2d &v , float& r);

	/**\brief
	 * 得到圆相对于p1的切点
	 */
	vector2d getpoint(vector2d& p1 , vector2d& p2);

	/**\brief
	 * 圆的赋值运算
	 */
	void operator =(SCircle c);
};




////////////////////////////////////////////////////////
/**\brief
 * 盒结构
 */
class CBox  
{
public:
	CBox(){}
	~CBox(){}

public:
	vector3d min;
	vector3d max;

	float xsize()		{return max.x - min.x;}
	float ysize()		{return max.y - min.y;}
	float zsize()		{return max.z - min.z;}

	vector3d size()		{return max - min;}
	vector3d center()	{return (min+max)*0.5f;}


	/**\brief
	 * 坐标为v的点是否在此立方体中
	 */
	BOOL PtInBox(vector3d& v);

	vector3d closestPointTo(vector3d& v);

	BOOL IntersectSphere(vector3d& vcenter , float radius);

	float IntersectRay(vector3d& start , vector3d&end);

	vector3d corner(int i);

};




//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//<<<<<<<<<<<<<<<<<<<<下面是inline函数的实现>>>>>>>>>>>>>>>>>>>>



////////////////////////////////////////////////////////////
//-----------------------vector2d---------------------------
/**\brief
 * 点到直线的距离
 */
inline float vector2d::dist(vector2d&p1 , vector2d&p2)
{
	vector2d p(p2.x - p1.x , p2.y - p1.y);
	p.normalize();
	p.change();
	return p.dot(vector2d(x-p1.x,y-p1.y));
}

/**\brief
 * 旋转向量
 */
inline void vector2d::rotate(float s)
{
	x = x*(float)cos(s) - y*(float)sin(s);
	y = x*(float)sin(s) + y*(float)cos(s);
}

inline float vector2d::dist_sqr(vector2d& vector)
{
	return (x-vector.x)*(x-vector.x)+(y-vector.y)*(y-vector.y);
}

/**\brief
 * 两点之间的距离
 */
inline float vector2d::dist(vector2d& vector)
{
	return (float)sqrt(  (x-vector.x)*(x-vector.x)
						+(y-vector.y)*(y-vector.y)
						);
}

inline float vector2d::dot(vector2d& vector)
{
	return x*vector.x + y*vector.y;
}

inline vector2d vector2d::operator +(vector2d& vector)
{
	return vector2d(x+vector.x ,y + vector.y);
}

inline vector2d vector2d::operator -(vector2d& vector)
{
	return vector2d(x-vector.x , y-vector.y);
}

inline vector2d vector2d::operator *(float s)
{
	return vector2d(x*s,y*s);
}

inline vector2d vector2d::operator /(float& s)
{
	return vector2d(x/s,y/s);
}



///////////////////////////////////////////////////////////
//-----------------------vector3d--------------------------

inline float vector3d::dist(vector3d& vector)
{
	return (float)sqrt(	 (x-vector.x)*(x-vector.x)
						+(y-vector.y)*(y-vector.y)
						+(z-vector.z)*(z-vector.z)
						);
}


inline float vector3d::dist_sqr(vector3d& vector)
{
	return ( (x-vector.x)*(x-vector.x)
			+(y-vector.y)*(y-vector.y)
			+(z-vector.z)*(z-vector.z)
			);
}


inline vector3d vector3d::cross(const vector3d& p)
{
	return(vector3d(y*p.z-z*p.y,z*p.x-x*p.z,x*p.y-y*p.x));
}


/**\brief
 * 矢量的规格化
 */
inline void vector3d::normalize()
{
	float invmag =1.0f/mag();
	x *= invmag;
	y *= invmag;
	z *= invmag;
}

inline void vector3d::operator  =(const vector3d& p)
{
	x = p.x;y = p.y;z = p.z;
}

inline void vector3d::operator +=(const vector3d& p)
{
	x += p.x;y += p.y;z += p.z;
}

inline void vector3d::operator -=(const vector3d& p)
{
	x -= p.x;y -= p.y;z -= p.z;
}

inline void vector3d::operator *=(const float& s)
{
	x *= s;y *= s;z *= s;
}

inline void vector3d::operator /=(const float& s)
{
	x /= s;y /= s;z /= s;
}	

inline vector3d vector3d::operator+(const vector3d& _v)
{
	return vector3d(x+_v.x,y+_v.y,z+_v.z);
}

inline vector3d vector3d::operator-(const vector3d& _v)
{
	return vector3d(x-_v.x,y-_v.y,z-_v.z);
}

inline vector3d vector3d::operator*(const float& s)
{
	return vector3d(x*s,y*s,z*s);
}

inline float vector3d::dot(const vector3d& p1 , const vector3d& p2)
{
	return (p1.x * p2.x + p1.y * p2.y + p1.z * p2.z);
}




//////////////////////////////////////////////////////////
//-----------------------SCircle--------------------------
/**\brief
 * 圆的赋值运算
 */
inline void SCircle::operator =(SCircle c)
{
	pos = c.pos;
	radius = c.radius;
}



//////////////////////////////////////////////////////////
//------------------------CBox----------------------------

inline BOOL CBox::PtInBox(vector3d&v)
{
	return (v.x >= min.x)&&(v.x <= max.x)&&
		   (v.y >= min.y)&&(v.y <= max.y)&&
		   (v.z >= min.z)&&(v.z <= max.z);
}



#endif