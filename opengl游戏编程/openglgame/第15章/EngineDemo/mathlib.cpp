//========================================================
/**
*  @file      mathlib.cpp
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


#include "stdafx.h"
#include "mathlib.h"

//////////////////////////////////////////////////////
/**\brief
 * 判断点到直线的投影是否在线段内
 */
BOOL vector2d::bPtInLine(vector2d p1 , vector2d p2)
{
	vector2d p;
	float d = dist(p1,p2);
	
	p.x = p2.x - p1.x;
	p.y = p2.y - p1.y;
	float x1 = p.x;
	p.normalize();
	p.change();
	p.x *= d;
	p.y *= d;
	float x2;
	x2 = x - p1.x - p.x;
	float t;
	t = x2 / x1;
	if((t>0)&&(t<1)) return TRUE;
	return FALSE;
}






///////////////////////////////////////////////////////////////

BOOL line2d::IsIntersect(line2d& line)
{
	float a1 , b1 , c1;
	float a2 , b2 , c2;
	a1 = p2.x - p1.x;
	b1 = line.p1.x - line.p2.x;
	c1 = line.p1.x - p1.x;

	a2 = p2.y - p1.y;
	b2 = line.p1.y - line.p2.y;
	c2 = line.p1.y - p1.y;
	if(fabs(a2*b1 - a1*b2)>0.0001f)
		return TRUE;
	else
		return FALSE;
}

vector2d line2d::GetIntersectPoint(line2d& line)
{
	float a1 , b1 , c1;
	float a2 , b2 , c2;
	a1 = p2.x - p1.x;
	b1 = line.p1.x - line.p2.x;
	c1 = line.p1.x - p1.x;

	a2 = p2.y - p1.y;
	b2 = line.p1.y - line.p2.y;
	c2 = line.p1.y - p1.y;
	vector2d rel;
	float t;
	t = (a2 * c1 - a1 * c2) / (a2 * b1 - a1 * b2);
	rel = line.p1 + (line.p2 - line.p1) * t;
	return rel;
}


////////////////////////////////////////////////////////////////


void SCircle::Set(vector2d &v, float& r)
{
}

vector2d SCircle::getpoint(vector2d&p1 , vector2d&p2)
{
	float d = pos.dist(p1,p2);
	float l = pos.dist(p1);
	l = radius / l;
	if(l > 1.0f)l= 1.0f;
	if(l < -1.0f)l=-1.0f;
	float s = (float)asin(l);
	if(d > 0) s = -s;
	vector2d p;
	p = pos - p1;
	p.normalize();
	p.rotate(s);
	d = sqrtf(fabsf(pos.dist(p1)*pos.dist(p1) - radius * radius));
	p = p1 + vector2d(p.x*d,p.y*d);
	return p;
}

BOOL SCircle::isIntersect(vector2d&p1, vector2d&p2)
{
	vector2d p(pos.x,pos.y);		
	float d;
    d = fabsf(p.dist(p1,p2));
	if(d > radius)
		return FALSE;
	else
	{
		if(pos.bPtInLine(p1,p2))
			return TRUE;
		else
			return FALSE;
	}
}


////////////////////////////////////////////////////////
vector3d CBox::corner(int i)
{
	if(i < 0) i = 0;
	if(i > 7) i = 7;
	return vector3d((i & 1) ? max.x :min.x , 
					(i & 2) ? max.y : min.y,
					(i & 3) ? max.z : min.z
					);
}


vector3d CBox::closestPointTo(vector3d& v)
{
	vector3d rel;
	if(v.x < min.x)
		rel.x = min.x;
	else 
		if(v.x > max.x)
			rel.x = max.x;
		else
			rel.x = v.x;

	if(v.y < min.y)
		rel.y = min.y;
	else 
		if(v.y > max.y)
			rel.y = max.y;
		else
			rel.y = v.y;

	if(v.z < min.z)
		rel.z = min.z;
	else 
		if(v.z > max.z)
			rel.z = max.z;
		else
			rel.z = v.z;

	return rel;
}

BOOL CBox::IntersectSphere(vector3d&vcenter , float radius)
{
	vector3d closestpoint = closestPointTo(vcenter);
	return vcenter.dist_sqr(closestpoint) < radius * radius;
}


//the return value must be from 0 to 1
//if it is 2 , ray isn't intersect with box
float CBox::IntersectRay(vector3d&start , vector3d&end)
{
	BOOL inside = TRUE;
	vector3d delta = end - start;
	

	float xt , xn;
	if(start.x < min.x)
	{
		xt = min.x - start.x;
		if(xt > delta.x)return 2.0f;
		xt /= delta.x;
		inside =FALSE;
		xn = -1.0f;
	}
	else if(start.x > max.x)
	{
		xt = max.x - start.x;
		if(xt < delta.x)return 2.0f;
		xt /= delta.x;
		inside = FALSE;
		xn = 1.0f;
	}
	else{xt = -1.0f;}


	float yt , yn;
	if(start.y < min.y)
	{
		yt = min.y - start.y;
		if(yt > delta.y)return 2.0f;
		yt /= delta.y;
		inside = FALSE;
		yn = -1.0f;
	}
	else if(start.y > max.y)
	{
		yt = max.y - start.y;
		if(yt < delta.y)return 2.0f;
		yt /= delta.y;
		inside = FALSE;
		yn = 1.0f;
	}
	else{yt = -1.0f;}


	float zt , zn;
	if(start.z < min.z)
	{
		zt = min.z - start.z;
		if(zt > delta.z)return 2.0f;
		zt /= delta.z;
		inside = FALSE;
		zn = -1.0f;
	}
	else if(start.z > max.z)
	{
		zt = max.z - start.z;
		if(zt < delta.z)return 2.0f;
		zt /= delta.z;
		inside = FALSE;
		zn = 1.0f;
	}
	else{zt = -1.0f;}

	if(inside)return 2.0f;

	int which = 0;
	float t = xt;
	if(yt > t){which = 1;t = yt;}
	if(zt > t){which = 2;t = zt;}


	switch(which)
	{
	case 0:
		{
			float y = start.y + delta.y * t;
			if(y < min.y || y > max.y)return 2.0f;

			float z = start.z + delta.z * t;
			if(z < min.z || z > max.z)return 2.0f;
		}
		break;


	case 1:
		{
			float x = start.x + delta.x * t;
			if(x < min.x || x > max.x)return 2.0f;

			float z = start.z + delta.z * t;
			if(z < min.z || z > max.z)return 2.0f;
		}
		break;


	case 2:
		{
			float x = start.x + delta.x * t;
			if(x < min.x || x > max.x)return 2.0f;

			float y = start.y + delta.y * t;
			if(y < min.y || y > max.y)return 2.0f;
		}
		break;
	}


	return t;
}