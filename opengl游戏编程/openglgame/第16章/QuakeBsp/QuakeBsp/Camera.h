//========================================================
/**
*  @file      Camera.h
*
*  项目描述： Quake室内场景实例
*  文件描述:  摄像机类  
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-08-06	
*  修改日期： 2007-09-01
*
*/     
//========================================================
#ifndef _CAMERA_H
#define _CAMERA_H

/** 摄像机类 */
class CCamera {

public:

	CCamera();	

	///获得摄像机属性的方法
	CVector3 Position() {	return m_vPosition;		}
	CVector3 View()		{	return m_vView;			}
	CVector3 UpVector() {	return m_vUpVector;		}
	CVector3 Strafe()	{	return m_vStrafe;		}

	/** 设置摄像机属性的方法 */
	void SetPosition(CVector3 vPosition)	{	m_vPosition = vPosition;	}
	void SetView(CVector3 vView)			{	m_vView = vView;			}
	void SetUpVector(CVector3 vUpVector)	{	m_vUpVector = vUpVector;	}
	
	/** 放置摄像机 */
	void PositionCamera(float positionX, float positionY, float positionZ,
			 		    float viewX,     float viewY,     float viewZ,
						float upVectorX, float upVectorY, float upVectorZ);

	/** 旋转摄像机 */
	void RotateView(float angle, float X, float Y, float Z);

	/** 根据鼠标旋转摄像机 */
	void SetViewByMouse(); 

	void RotateAroundPoint(CVector3 vCenter, float X, float Y, float Z);

	/** 左右移动摄像机 */
	void StrafeCamera(float speed);

	/** 前后移动摄像机 */
	void MoveCamera(float speed);

	/** 检查键盘动作 */
	void CheckForMovement();

	/** 摄像机更新 */
	void Update();

	/** 设置视口 */
	void Look();

private:

	/** 摄像机位置 */
	CVector3 m_vPosition;					

	/** 摄像机视点 */
	CVector3 m_vView;						

	/** 向上向量 */
	CVector3 m_vUpVector;		
	
	/** 平移量 */
	CVector3 m_vStrafe;	
};

#endif
