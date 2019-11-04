//========================================================
/**
*  @file      Vector.cpp
*
*  项目描述： 碰撞检测演示
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
#include "Tray.h"

/** 返回两条射线的中点 */
Vector3::Vector3(const TRay &ray1, const TRay &ray2) 
{

	Vector3 point1, point2;

	if (ray1.adjacentPoints(ray2, point1, point2))

		*this = (point1 + point2)*0.5;

	else

		*this = Vector3();

}

/** 单位化 */
Vector3 &Vector3::unit() {

	if (isDefault()) {

		double rep = mag();

		if (rep < ZERO) {

			x = 0.0;

			y = 0.0;

			z = 0.0;

		} 
		else 
		{

			double temp = 1.0 / rep;

			x *= temp;

			y *= temp;

			z *= temp;

		}

		status = UNIT;

	}

	return *this;

}

Vector3 &Vector3::Default() {

	if (isUnit())

		status = DEFAULT;

	return *this;

}


/** 得到相反向量 */
Vector3 &Vector3::invert(const Vector3 &v1, Vector3 &result) {

	if (v1.isValid()) 
	{

		result.x = -v1.x;

		result.y = -v1.y;

		result.z = -v1.z;

		result.status = v1.status;

	} 
	else

		result = Vector3();

	return result;

}

/** 向量相加 */
Vector3 &Vector3::add(const Vector3 &v1, const Vector3 &v2, Vector3 &result) 
{

	if (v1.isValid() && v2.isValid()) {

		result.x = v1.x + v2.x;

		result.y = v1.y + v2.y;

		result.z = v1.z + v2.z;

		result.status = DEFAULT;

	} 
	else

		result = Vector3();

	return result;

}


/** 向量相减 */
Vector3 &Vector3::subtract(const Vector3 &v1, const Vector3 &v2, Vector3 &result) 
{

	if (v1.isValid() && v2.isValid()) 
	{

		result.x = v1.x - v2.x;

		result.y = v1.y - v2.y;

		result.z = v1.z - v2.z;

		result.status = DEFAULT;

	} 
	else

		result = Vector3();

	return result;

}


/** 向量叉乘 */
Vector3 &Vector3::cross(const Vector3 &v1, const Vector3 &v2, Vector3 &result) 
{

	if (v1.isValid() && v2.isValid()) 
	{

		result.x = v1.y * v2.z - v1.z * v2.y;

		result.y = v1.z * v2.x - v1.x * v2.z;

		result.z = v1.x * v2.y - v1.y * v2.x;

		result.status = DEFAULT;

	} 
	else

		result = Vector3();

	return result;

}


/** 向量缩放 */
Vector3 &Vector3::multiply(const Vector3 &v1, const double &scale, Vector3 &result) {

	if (v1.isValid()) 
	{

		result.x = v1.x * scale;

		result.y = v1.y * scale;

		result.z = v1.z * scale;

		result.status = DEFAULT;

	} 
	else

		result = Vector3();

	return result;

}









