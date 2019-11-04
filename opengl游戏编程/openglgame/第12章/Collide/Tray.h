//========================================================
/**
*  @file      Tray.h
*
*  项目描述： 碰撞检测演示
*  文件描述:  射线类  
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-12-06	
*  修改日期： 2006-12-19
*
*/     
//========================================================

#ifndef __TRAY_H__
#define __TRAY_H__

#include "Vector.h"

/** 射线类 */
class TRay
{
	private:
		Vector3 _P;      /**< 射线的端点 */
		Vector3 _V;      /**< 射线的方向 */	
	public:
		/** 构造函数 */
		TRay() {}
		TRay(const Vector3 &point1, const Vector3 &point2);
		
		/** 检测两条直线的最近的点，并保存在point1和point2中 */
		bool adjacentPoints(const TRay &ray, Vector3 &point1, Vector3 &point2) const;
		
		/** 反向射线的方向，原点不变 */
		static TRay &invert(const TRay &r, TRay &result) { result._P = r._P; result._V = -r._V; return result; }
		TRay operator-() const { return invert(*this, TRay()); }
		int isValid() const { return V().isUnit() && P().isValid(); }
		
		/** 返回成员变量 */
		Vector3 P() const { return _P; }
		Vector3 V() const { return _V; }
		
		/** 返回射线的距离 */
		double dist(const TRay &ray) const;
		double dist(const Vector3 &point) const;
		
};

#endif

