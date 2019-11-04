#ifndef _FRUSTUM_H
#define _FRUSTUM_H

#include "main.h"

/** 平锥体类 */
class CFrustum {

public:

	void CalculateFrustum();

	/** 判断一点是否在平头体内 */
	bool PointInFrustum(float x, float y, float z);

	bool SphereInFrustum(float x, float y, float z, float radius);

	bool CubeInFrustum(float x, float y, float z, float size);


	bool BoxInFrustum(float x, float y, float z, float sizeX, float sizeY, float sizeZ);


private:
     /** 组成平头体的六个平面方程系数 */
	float m_Frustum[6][4];
};


class CDebug 
{

public:

	void AddDebugLine(CVector3 vPoint1, CVector3 vPoint2);

	void AddDebugBox(CVector3 vCenter, float width, float height, float depth);

	void RenderDebugLines();		

	void Clear();					

private:
	vector<CVector3> m_vLines;		
};


#endif
