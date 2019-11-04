//========================================================
/**
*  @file      ActiveObj.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  游戏中的活动实体
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef _ACTIVE_OBJ_H_
#define _ACTIVE_OBJ_H_

#include "object.h"
#include "MD2File.h"
#include "info.h"



/**\brief
 * 游戏中的活动实体
 */
class CActiveObj:public Object
{
public:
	CActiveObj(void):m_bActive(TRUE),property(NULL){}
	virtual ~CActiveObj(void){}

	BOOL Init(char*filename,char*texfilename,float scale,float h);

	void SetObjectState(OBJ_STATE state);
	OBJ_STATE GetObjectState();
	
	float GetLife(void);
	float GetSize(void);
	
	BOOL IsArrive(void);

	virtual void OnRender(void);
	virtual void EndRender(void);

	void SetActive(BOOL _active){m_bActive=_active;}
	BOOL GetActive(){return m_bActive;}


protected:
	BOOL			m_bActive;
	CMD2File		md2;
	SActiveObjPro*	property;
	void vMatMult(float M[16],float v[4]);

protected:
	struct MD2_ANIMATION
	{
		char m_cName[16];
		int m_iStartFrame;
		int m_iEndFrame;
	};
};




//////////////////////////////////////////////////////////
//---------------------inline函数实现---------------------

inline OBJ_STATE CActiveObj::GetObjectState(void)
{
	return property->object_state;
}

inline float CActiveObj::GetLife(void)
{
	return property->GetLife();
}

inline float CActiveObj::GetSize(void)
{
	return md2.GetSize();
}

inline BOOL CActiveObj::IsArrive(void)
{
	return property->bArrive;
}

inline void CActiveObj::EndRender(void)
{
	//property->vPosition=m_sPos;
	property->Update();
	m_sPos=property->vPosition;
}


#endif