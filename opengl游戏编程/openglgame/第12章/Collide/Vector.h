//========================================================
/**
*  @file      Vector.h
*
*  项目描述： 碰撞检测演示
*  文件描述:  向量类  
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     BrightXu
*  电子邮件:  huoxini@hotmail.com
*  创建日期： 2006-12-06	
*  修改日期： 2006-12-19
*
*/     
//========================================================

#ifndef __VECTOR_H__
#define __VECTOR_H__

#define ZERO 1.0e-8
#define M_PI 3.1415926535
#include <math.h>

class TRay;

/** 向量类 */
class Vector3
{
public:
	enum Status { INVALID, DEFAULT, UNIT };
    double   x,y,z;
    Status  status;

public:
	/** 构造函数 */
	Vector3():x(0.0), y(0.0), z(0.0), status(INVALID) {}
	Vector3(double xx, double yy, double zz):x(xx),y(yy),z(zz),status(DEFAULT) {}
	Vector3(double xx, double yy, double zz,Status s):x(xx),y(yy),z(zz),status(s) {}

	/** 拷贝构造函数 */
	Vector3(const Vector3& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
		status = vec.status;
	}

	/** 返回成员变量 */
	int isUnit() const    { return status==UNIT; }
	int isDefault() const { return status==DEFAULT; }
	int isValid() const   { return status!=INVALID; }
    
	/** 返回两条射线的中点 */
	Vector3(const TRay &line1, const TRay &line2);

	/** 返回单位向量 */
	Vector3 &unit();
	static Vector3 &unit(const Vector3 &v, Vector3 &result) { result = v; return result.unit(); }
	static Vector3 unit(const Vector3 &v) { return Vector3(v).unit(); }

	Vector3 &Default();
	static Vector3 Default(const Vector3 &v, Vector3 &result) { result = v; return result.Default(); }
	static Vector3 Default(const Vector3 &v) { return Vector3(v).Default(); }

	/// 返回向量的模
	double mag() const { return (isValid() ? (isUnit() ? 1.0 : sqrt( x*x + y*y + z*z )) : 0.0); }
	double magSqr() const { return (isValid() ? (isUnit() ? 1.0 : x*x + y*y + z*z) : 0.0); }

	/// 返回两个向量的点乘
	double dot(const Vector3 &v) const { return ((isValid() && v.isValid()) ? (x*v.x + y*v.y + z*v.z) : 0.0); }
	static double dot(const Vector3 &v1, const Vector3 &v2) { return v1.dot(v2); }

	/// 返回两个向量的距离
	double dist(const Vector3 &v) const { return (*this-v).mag(); }
	double distSqr(const Vector3 &v) const { return (*this-v).magSqr(); }

	/// 函数表示的向量运算
	static Vector3 &add(const Vector3 &v1, const Vector3 &v2, Vector3 &result);
	static Vector3 &subtract(const Vector3 &v1, const Vector3 &v2, Vector3 &result);
	static Vector3 &cross(const Vector3 &v1, const Vector3 &v2, Vector3 &result);
	static Vector3 &invert(const Vector3 &v1, Vector3 &result);
	static Vector3 &multiply(const Vector3 &v1, const double &scale, Vector3 &result);

	/// 向量运算
	Vector3 operator-() const { return invert(*this, Vector3()); }
	Vector3 &operator+=(const Vector3 &v) { return add(*this, v, *this); }
	Vector3 &operator-=(const Vector3 &v) { return subtract(*this, v, *this); }
	Vector3 &operator*=(const Vector3 &v) { Vector3 tv(*this); return cross(tv, v, *this); }
	Vector3 &operator*=(const double &scale) { return multiply(*this, scale, *this); }
	Vector3 operator+(const Vector3 &v) const { Vector3 tv; return add(*this, v, tv); }
	Vector3 operator-(const Vector3 &v) const { Vector3 tv; return subtract(*this, v, tv); }
	Vector3 operator*(const Vector3 &v) const { Vector3 tv; return cross(*this, v, tv); }
	Vector3 operator*(const double &scale) const { Vector3 tv; return multiply(*this, scale, tv); }

};

/** 2D平面上的点 */
struct Point2
{
	int x,y;

	Point2():x(0),y(0) {}
	Point2(int _x,int _y):x(_x),y(_y) {}
	const Point2 &operator=(const Point2 &p)	
	{  
		x = p.x; 
	    y = p.y;
	    return *this;
	}
};

#endif //__VECTOR_H__