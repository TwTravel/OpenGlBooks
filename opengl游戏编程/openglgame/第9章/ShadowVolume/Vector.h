//========================================================
/**
*  @file      Vector.h
*
*  项目描述： 阴影体
*  文件描述:  向量类  
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gail.com
*  创建日期： 2006-12-06	
*  修改日期： 2006-12-19
*
*/     
//========================================================

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "stdafx.h"

/** 向量类 */
class Vector3
{
public:
	/** 构造函数 */
	Vector3()  { x = 0.0; y = 0.0; z = 0.0; }
	Vector3( float xx, float yy, float zz)
	{
		x = xx;
		y = yy;
		z = zz;
	}
	Vector3(const Vector3& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
	}
    
	/** 成员函数 */
	inline float length();                       /**< 计算向量长度 */
	Vector3 normalize();                         /**< 单位化向量 */
	float dotProduct(const Vector3 &v );          /**< 计算点积 */
	Vector3 crossProduct(const Vector3 &v);     /**< 计算叉积 */

	/** 重载操作符 */
	Vector3 operator + (const Vector3& v);
	Vector3 operator += (const Vector3& v);
	Vector3 operator - (const Vector3& v);
	Vector3 operator -= (const Vector3& v);
	Vector3 operator * (const Vector3& v);
	Vector3 operator / (const Vector3& v);
	Vector3 operator * (float scale);
	Vector3 operator / (float scale);
	Vector3 operator - ();
	
public:
	  float x,y,z;

};

/** 四维向量类 */
class CVector4
{
   public:
	  ///构造函数
      CVector4();                                        
      CVector4(float X, float Y, float Z);               
      CVector4(float X, float Y, float Z, float W);      
      CVector4(const CVector4 &v);   
	   
	  /** 操作符重载 */
	  void operator=(CVector4 v);
	  CVector4 operator-(CVector4 v);                   
      CVector4 operator+(CVector4 v);                    
      CVector4 operator*(CVector4 v);                   
      CVector4 operator/(CVector4 v);                  
      CVector4 operator+(float f);                     
      CVector4 operator-(float f);                     
      CVector4 operator*(float f);                      
      CVector4 operator/(float f);                      

	  /** 将当前点沿某个方向延伸 */
	  void ExtendVertexPos(CVector4 currentVertex,       
                           CVector4 lightPos,            
                           float Extend);

public:
	float x,y,z,w;
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