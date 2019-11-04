
//========================================================
/**
*  @file      Camera.cpp
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
#include "main.h"
#include "Camera.h"
#include "Quake3Bsp.h"


extern CVector3 g_vVelocity;

#define kSpeed	200.0f		

double g_FrameInterval = 0.0f;

extern CQuake3BSP g_Level;

/** 计算FPS */
void CalculateFrameRate()
{
	static double framesPerSecond   = 0.0f;		
    static double lastTime			= 0.0f;		
	static char strFrameRate[50] = {0};			

	static double frameTime = 0.0f;				

	
    double currentTime = timeGetTime() * 0.001f;				
 	g_FrameInterval = currentTime - frameTime + 0.005f;

	frameTime = currentTime;

    ++framesPerSecond;

   if( currentTime - lastTime > 1.0f )
    {
		
	    lastTime = currentTime;
		
		sprintf(strFrameRate, "Quake室内场景实例 FPS: %d", int(framesPerSecond));

		SetWindowText(g_hWnd, strFrameRate);  /**< 设置标题 */

		framesPerSecond = 0;
    }
}

/** 计算叉积 */												
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vNormal;	

	
	vNormal.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	vNormal.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	vNormal.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	return vNormal;										 
}

/** 计算向量的模 */
float Magnitude(CVector3 vNormal)
{
	return (float)sqrt( (vNormal.x * vNormal.x) + 
						(vNormal.y * vNormal.y) + 
						(vNormal.z * vNormal.z) );
}


/** 单位化向量 */
CVector3 Normalize(CVector3 vVector)
{
	
	float magnitude = Magnitude(vVector);				

	vVector = vVector / magnitude;		
	
	return vVector;										
}

CCamera::CCamera()
{
	CVector3 vZero = CVector3(0.0, 0.0, 0.0);		
	CVector3 vView = CVector3(0.0, 1.0, 0.5);		
	CVector3 vUp   = CVector3(0.0, 0.0, 1.0);		

	m_vPosition	= vZero;					
	m_vView		= vView;					
	m_vUpVector	= vUp;					
}


void CCamera::PositionCamera(float positionX, float positionY, float positionZ,
				  		     float viewX,     float viewY,     float viewZ,
							 float upVectorX, float upVectorY, float upVectorZ)
{
	CVector3 vPosition	= CVector3(positionX, positionY, positionZ);
	CVector3 vView		= CVector3(viewX, viewY, viewZ);
	CVector3 vUpVector	= CVector3(upVectorX, upVectorY, upVectorZ);

	m_vPosition = vPosition;					
	m_vView     = vView;					
	m_vUpVector = vUpVector;				
}


void CCamera::SetViewByMouse()
{
	POINT mousePos;									
	int middleX = SCREEN_WIDTH  >> 1;				
	int middleY = SCREEN_HEIGHT >> 1;				
	float angleY = 0.0f;							
	float angleZ = 0.0f;							
	static float currentRotX = 0.0f;
	
	
	GetCursorPos(&mousePos);						
	
	
	if( (mousePos.x == middleX) && (mousePos.y == middleY) ) return;

	SetCursorPos(middleX, middleY);							
	
	angleY = (float)( (middleX - mousePos.x) ) / 500.0f;		
	angleZ = (float)( (middleY - mousePos.y) ) / 500.0f;		

	currentRotX -= angleZ;  

	CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
	vAxis = Normalize(vAxis);

	RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
	RotateView(angleY, 0, 1, 0);
}

void CCamera::RotateView(float angle, float x, float y, float z)
{
	CVector3 vNewView;

	CVector3 vView = m_vView - m_vPosition;		

	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);

	vNewView.x  = (cosTheta + (1 - cosTheta) * x * x)		* vView.x;
	vNewView.x += ((1 - cosTheta) * x * y - z * sinTheta)	* vView.y;
	vNewView.x += ((1 - cosTheta) * x * z + y * sinTheta)	* vView.z;

	vNewView.y  = ((1 - cosTheta) * x * y + z * sinTheta)	* vView.x;
	vNewView.y += (cosTheta + (1 - cosTheta) * y * y)		* vView.y;
	vNewView.y += ((1 - cosTheta) * y * z - x * sinTheta)	* vView.z;

	vNewView.z  = ((1 - cosTheta) * x * z - y * sinTheta)	* vView.x;
	vNewView.z += ((1 - cosTheta) * y * z + x * sinTheta)	* vView.y;
	vNewView.z += (cosTheta + (1 - cosTheta) * z * z)		* vView.z;

	m_vView = m_vPosition + vNewView;
}



void CCamera::StrafeCamera(float speed)
{	
	
	m_vPosition.x += m_vStrafe.x * speed;
	m_vPosition.z += m_vStrafe.z * speed;

	m_vView.x += m_vStrafe.x * speed;
	m_vView.z += m_vStrafe.z * speed;
}


void CCamera::MoveCamera(float speed)
{
	CVector3 vVector = m_vView - m_vPosition;
	vVector = Normalize(vVector);

	m_vPosition.x += vVector.x * speed;		
	m_vPosition.z += vVector.z * speed;		
	m_vView.x += vVector.x * speed;			
	m_vView.z += vVector.z * speed;			

}




void CCamera::CheckForMovement()
{	
	float speed = (float)(kSpeed * g_FrameInterval);

	CVector3 vOldPosition = Position();
	CVector3 vOldView = View();

	bool bMovedBack = false;

	g_vVelocity.y -= (float)(kGravity * g_FrameInterval);
	m_vPosition = m_vPosition + g_vVelocity;


	if(GetKeyState(VK_UP) & 0x80 || GetKeyState('W') & 0x80) 
	{				
		MoveCamera(speed);				
	}

	
	if(GetKeyState(VK_DOWN) & 0x80 || GetKeyState('S') & 0x80) 
	{			
		MoveCamera(-speed);	
		bMovedBack = true;
	}
	
	if(GetKeyState(VK_LEFT) & 0x80 || GetKeyState('A') & 0x80) 
	{			

		StrafeCamera(-speed);
	}

	
	if(GetKeyState(VK_RIGHT) & 0x80 || GetKeyState('D') & 0x80)
	{			

		StrafeCamera(speed);
	}	

	CVector3 vCurrentPosition = Position();

	CVector3 vNewPosition = g_Level.TraceBox(vOldPosition, vCurrentPosition,
		                                     CVector3(-20, -50, -20), CVector3(20, 50, 20));

	if(g_Level.Collided() && bMovedBack)
	{
		if(vNewPosition.x == vOldPosition.x || vNewPosition.z == vOldPosition.z)
			m_vView = vOldView;		
	}

	m_vPosition = vNewPosition;

	if(!g_Level.IsOnGround())
		m_vView = m_vView + g_vVelocity;
	else
	{
	   if(g_vVelocity.y < 0)
			g_vVelocity.y = 0;
	}

}

/** 更新摄像机 */
void CCamera::Update() 
{
	CVector3 vCross = Cross(m_vView - m_vPosition, m_vUpVector);

	m_vStrafe = Normalize(vCross);

	SetViewByMouse();

	CheckForMovement();
	

	CalculateFrameRate();
}

void CCamera::Look()
{
	gluLookAt(m_vPosition.x, m_vPosition.y, m_vPosition.z,	
			  m_vView.x,	 m_vView.y,     m_vView.z,	
			  m_vUpVector.x, m_vUpVector.y, m_vUpVector.z);
}
