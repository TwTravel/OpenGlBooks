//========================================================
/**
*  @file      Vector.cpp
*
*  项目描述： 阴影体
*  文件描述:  向量类  
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-12-06	
*  修改日期： 2006-12-20
*
*/     
//========================================================

#include "Vector.h"                       /**< 包含头文件 */

/** 计算向量的长度 */
inline float Vector3::length()
{
	return (float)( x * x + y * y + z * z );
}

/** 单位化一向量 */
Vector3 Vector3::normalize()
{
	float len = length();                  /**< 计算向量长度 */
	if( len == 0 )
		len = 1;
	x = x / len;
	y = y / len;
	z = z / len;

	return *this;
}

/** 点积 */
float Vector3::dotProduct(const Vector3& v)
{
	return ( x * v.x + y * v.y + z * v.z );
}

/** 叉积 */
 Vector3 Vector3::crossProduct(const Vector3& v)
{
	Vector3 vec;

	vec.x = y * v.z - z * v.y;
	vec.y = z * v.x - x * v.z;
	vec.z = x * v.y - y * v.x;
	
	return vec;
}

/** 操作符 + */
 Vector3 Vector3::operator +(const Vector3& v)
{
	Vector3 vec;
	
	vec.x = x + v.x;
	vec.y = y + v.y;
	vec.z = z + v.z;

	return vec;
}

Vector3 Vector3::operator +=(const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	
	return *this;
}

Vector3 Vector3::operator -=(const Vector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	
	return *this;
}

/** 操作符 - */
 Vector3 Vector3::operator -(const Vector3& v)
{
	Vector3 vec;
	
	vec.x = x - v.x;
	vec.y = y - v.y;
	vec.z = z - v.z;

	return vec;
}

/** 操作符 * */
 Vector3 Vector3::operator *(const Vector3& v)
{
	Vector3 vec(0.0f, 0.0f, 0.0f);
	
	vec.x = x * v.x;
	vec.y = y * v.y;
	vec.z = z * v.z;

	return vec;
}

/** 操作符 / */
 Vector3 Vector3::operator /(const Vector3& v)
{
	Vector3 vec(0.0f, 0.0f, 0.0f);
	
	vec.x = x / v.x;
	vec.y = y / v.y;
	vec.z = z / v.z;

	return vec;
}

/** 操作符 * */
 Vector3 Vector3::operator *(float scale)
{
	x = x * scale;
	y = y * scale;
	z = z * scale;
	
	return *this;
}

/** 操作符 / */
 Vector3 Vector3::operator /(float scale)
{
	if(scale != 0.0)
	{	
	   x = x / scale;
	   y = y / scale;
	   z = z / scale;
	}
	return *this;
}

/** 负号 */
 Vector3 Vector3::operator -()
{
	Vector3 vec( - x,- y, - z);
	return vec;
}


/************ 四维向量类实现 *****/

CVector4::CVector4()
{
  
   x = y = z = 0.0;
}


CVector4::CVector4(float X, float Y, float Z)
{
   
   x = X;
   y = Y;
   z = Z;
}


CVector4::CVector4(float X, float Y, float Z, float W)
{
   x = X;
   y = Y;
   z = Z;
   w = W;
}


CVector4::CVector4(const CVector4 &v)
{

   x = v.x;
   y = v.y;
   z = v.z;
   w = v.w;
}

void CVector4::operator =(CVector4 v)
{
    x = v.x;
   y = v.y;
   z = v.z;
   w = v.w;
}


CVector4 CVector4::operator -(CVector4 v)
{

   return CVector4(x - v.x, y - v.y, z - v.z);
}


CVector4 CVector4::operator +(CVector4 v)
{

   return CVector4(x + v.x, y + v.y, z + v.z);
}


CVector4 CVector4::operator *(CVector4 v)
{

   return CVector4(x * v.x, y * v.y, z * v.z);
}


CVector4 CVector4::operator /(CVector4 v)
{
 
   return CVector4(x / v.x, y / v.y, z / v.z);
}


CVector4 CVector4::operator +(float f)
{
 
   return CVector4(x + f, y + f, z + f);
}


CVector4 CVector4::operator -(float f)
{
  
   return CVector4(x - f, y - f, z - f);
}


CVector4 CVector4::operator *(float f)
{
 
   return CVector4(x * f, y * f, z * f);
}


CVector4 CVector4::operator /(float f)
{
   
   f = 1/f;

   return CVector4(x * f, y * f, z * f);
}
/** 将顶点沿某光源方向延伸 */
void CVector4::ExtendVertexPos(CVector4 currentVertex, CVector4 lightPos, float Extend)
{
    CVector4 lightDir;  /**< 光源位置到顶点的向量 */
    CVector4 newPos;    /**< 延伸后的新顶点 */

   
	lightDir = currentVertex - lightPos;

    /** 计算延伸后的顶点 */
    newPos = lightPos + lightDir * Extend;

	/** 保存该顶点 */
    x = newPos.x;
    y = newPos.y;
    z = newPos.z;
    w = newPos.w;
}











