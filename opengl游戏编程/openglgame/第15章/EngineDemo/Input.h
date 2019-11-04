//======================================================================
/**
*  @file      Input.h
*
*  项目描述： 构造游戏引擎
*   
*  文件描述:  输入系统类CInputSystem
*             其中,类CKeyboard、CMouse、CJoystick分别用来处理键盘、鼠标
*			  和游戏手柄，CInputSystem是输入设备的管理类。
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-24	
*  修改日期： 2007-07-24
*/																			
//======================================================================


#ifndef __INPUT_H__
#define __INPUT_H__

#define DIRECTINPUT_VERSION  0x0800
#include "stdafx.h"

#include <dinput.h>
#include "mathlib.h"

#define IS_USEKEYBOARD  1
#define IS_USEMOUSE     2
#define IS_USEJOYSTICK  4


/////////////////////////////////////////
/**\brief
 *  键盘类
 */
class CKeyboard
{
public:
	CKeyboard(LPDIRECTINPUT8 pDI, HWND hwnd);
	~CKeyboard();


	/**\brief
	 * key键是否按下
	 */
	BOOL KeyDown(int key);


	/**\brief
	 * 更新键盘数据，放在主循环中
	 */
	BOOL Update();


	/**\brief
	 * 对键盘数据清零
	 */
	void Clear();


	/**\brief
	 * 取键盘数据
	 */
	BOOL Acquire();


	/**\brief
	 * 停止取数据
	 */
	BOOL Unacquire();


private:
	//! 键盘设备
	LPDIRECTINPUTDEVICE8  m_pDIDev;

	//! 键盘数据区
	char    m_keys[256];
};




////////////////////////////////////////
/**\brief
 *  鼠标类
 */
class CMouse
{
public:
	CMouse(LPDIRECTINPUT8 pDI,HWND hwnd,BOOL isExclusive=TRUE);
	~CMouse();


	/**\brief
	 * 鼠标某键是否按下，0-左键，1-右键
	 */
	BOOL	ButtonDown(int button);
	
	
	/**\brief
	 * 获得光标坐标变化
	 */
	void	GetMovement(int &dx, int &dy);
	
	
	/**\brief
	 * 获得光标位置
	 */
	SPoint2	GetMousePos();


	/**\brief
	 * 获得鼠标滚轮状态,(<0,0,>0)
	 */
	int		GetWheelState();


	/**\brief
	 * 更新鼠标参数
	 */
	BOOL	Update();


	/**\beirf
	 * 取鼠标数据
	 */
	BOOL	Acquire();
	
	
	/**\brief
	 * 停止取数据
	 */
	BOOL	Unacquire();


	//! 窗口宽
	int		m_iWidth;

	//! 窗口高
	int		m_iHeight;


private:
	//! 鼠标设备
	LPDIRECTINPUTDEVICE8	m_pDIDev;

	//! 鼠标状态
	DIMOUSESTATE			m_state;

	//! 光标位置
	SPoint2					m_sMousePos;
};




/////////////////////////////////////////
/**\brief
 *  游戏手柄
 */
class CJoystick
{
public:
	CJoystick(LPDIRECTINPUT8 pDI, HINSTANCE appInstance);
	~CJoystick();


	/**\brief
	 * 更新手柄参数
	 */
	BOOL Update(){ return TRUE;}


	/**\brief
	 * 取手柄数据
	 */
	BOOL Acquire();


	/**\brief
	 * 停上取手柄数据
	 */
	BOOL Unacquire();


private:
	//! 手柄设备
	LPDIRECTINPUTDEVICE8  m_pDIDev;
};




////////////////////////////////////////
/**\brief
 *  输入系统类
 */
class CInput
{
public:
	CInput():bClick(FALSE),bFirstMDownL(FALSE){m_pInput=this;}
	~CInput(){Shutdown();}

	
	/**\brief
	 * 初始化输入设备
	 */
	BOOL  Init(HWND hwnd,HINSTANCE appInstance,BOOL isExclusive,DWORD flags=0);
	
	
	/**\brief
	 * 释放输入设备资源
	 */
	BOOL  Shutdown();


	/**\brief
	 * 取输入设备的数据
	 */
	void  AcquireAll();
	
	
	/**\brief
	 * 停上取输入设备的数据
	 */
	void  UnacquireAll();


	/**\brief
	 * 获得键盘
	 */
	CKeyboard  *GetKeyboard(){return m_pKeyboard;}
	
	
	/**\brief
	 * 获得鼠标
	 */
	CMouse     *GetMouse(){return m_pMouse;}
	
	
	/**\brief
	 * 获得游戏手柄
	 */
	CJoystick  *GetJoystick(){return m_pJoystick;}


	/**\brief
	 * 更新输入设备的参数
	 */
	void	Update();


	/**\brief
	 * 获得光标的3D坐标
	 */
	vector3d GetMousePosition();


	/**\brief
	 * 鼠标是否是第一次按下
	 */
	BOOL	FirstMouseDownL(){return bFirstMDownL;}


	/**\brief
	 * 是否单击了鼠标
	 */
	BOOL	Clicked(){return bClick;}
	

	/**\brief
	 * 得到当前的输入设备
	 */
	static CInput* GetSysInput(){return m_pInput;}


	/**\brief
	 * 得到当前的窗口句柄
	 */
	static HWND GetSysHWnd(){return m_hWnd;}


private:
	//! 输入系统
	static CInput* m_pInput;

	//! 窗口句柄
	static HWND m_hWnd;

	//! 键盘
	CKeyboard  *m_pKeyboard;

	//! 鼠标
	CMouse     *m_pMouse;

	//! 游戏手柄
	CJoystick  *m_pJoystick;

	//! 输入设备
	LPDIRECTINPUT8 m_pDI;

	//鼠标左键按下后抬起时为TRUE
	BOOL		bClick;

	//鼠标按下会有个时间段，是不是刚开始按下
	BOOL		bFirstMDownL;
	
};







///////////////////////////////////////////////////
//------------CKeyboard inline函数实现-------------

/**\brief
 * key键是否按下
 */
inline BOOL CKeyboard::KeyDown(int key)
{
	return (m_keys[key]&0x80)?TRUE:FALSE;
}

/**\brief
 * 对键盘数据清零
 */
inline void CKeyboard::Clear()
{
	ZeroMemory(m_keys,256*sizeof(char));
}

/**\brief
 * 取键盘数据
 */
inline BOOL CKeyboard::Acquire()
{
	Clear();
	return (!FAILED(m_pDIDev->Acquire()));
}

/**\brief
 * 停止取数据
 */
inline BOOL CKeyboard::Unacquire()
{
	Clear();
	return (!FAILED(m_pDIDev->Unacquire()));
}



///////////////////////////////////////////////////
//------------CMouse inline函数实现----------------

/**\brief
 * 鼠标某键是否按下，0-左键，1-右键
 */
inline BOOL CMouse::ButtonDown(int button)
{
	return (m_state.rgbButtons[button]&0x80)?TRUE:FALSE;
}

/**\brief
 * 获得光标坐标变化
 */
inline void CMouse::GetMovement(int &dx, int &dy)
{
	dx = m_state.lX;
	dy = m_state.lY;
}

/**\brief
 * 获得光标位置
 */
inline SPoint2 CMouse::GetMousePos()
{
	return m_sMousePos;
}

/**\brief
 * 获得鼠标滚轮状态 (<0,0,>0)
 */
inline int	CMouse::GetWheelState()
{
	return m_state.lZ;
}

/**\beirf
 * 取鼠标数据
 */
inline BOOL CMouse::Acquire()
{
	return (!FAILED(m_pDIDev->Acquire()));
}

/**\brief
 * 停止取数据
 */
inline BOOL CMouse::Unacquire()
{
	return (!FAILED(m_pDIDev->Unacquire()));
}



///////////////////////////////////////////////////
//-----------CJoystick inline函数实现--------------
/**\brief
 * 取手柄数据
 */
inline BOOL CJoystick::Acquire()
{
	return (!FAILED(m_pDIDev->Acquire()));
}

/**\brief
 * 停止取数据
 */
inline BOOL CJoystick::Unacquire()
{
	return (!FAILED(m_pDIDev->Unacquire()));
}



///////////////////////////////////////////////////
//-------------CInput inline函数实现---------------

/**\brief
 * 取输入设备的数据
 */
inline void CInput::AcquireAll()
{
	if (m_pKeyboard)	m_pKeyboard->Acquire();
	if (m_pMouse)		m_pMouse->Acquire();
	if (m_pJoystick)	m_pJoystick->Acquire();
}

/**\brief
 * 停止取数据
 */
inline void CInput::UnacquireAll()
{
	if (m_pKeyboard)	m_pKeyboard->Unacquire();
	if (m_pMouse)		m_pMouse->Unacquire();
	if (m_pJoystick)	m_pJoystick->Unacquire();
}



#endif