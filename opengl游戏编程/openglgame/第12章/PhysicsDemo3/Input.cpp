//======================================================================
/**
*  @file      Input.cpp
*
*  项目描述： 摩擦力作用下运动模拟
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

#include "Input.h"

/** 构造函数 */
CKeyboard::CKeyboard(LPDIRECTINPUT8 pDI, HWND hwnd)
{
	/** 创建设备对象 */
	if (FAILED(pDI->CreateDevice(GUID_SysKeyboard, &m_pDIDev, NULL)))
	{
		MessageBox(NULL,"创建键盘设备对象失败！","错误",MB_OK);
	}

    /** 设置数据格式 */
	if (FAILED(m_pDIDev->SetDataFormat(&c_dfDIKeyboard)))
	{
		MessageBox(NULL,"设置键盘数据格式失败！","错误",MB_OK);
	}

    /** 设置协作层次 */ 
	if (FAILED(m_pDIDev->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{
		MessageBox(NULL,"设置键盘协作层次失败！","错误",MB_OK);
	}

	/** 捕获设置 */ 
	if (FAILED(m_pDIDev->Acquire()))
	{
		MessageBox(NULL,"捕获键盘设备失败！","错误",MB_OK);
	}

	/** 键盘数据清零 */
	Clear();
}

/** 析构函数 */
CKeyboard::~CKeyboard()
{
	if (m_pDIDev)
	{
		m_pDIDev->Unacquire();
		m_pDIDev->Release();
	}
}

/** 更新键盘数据 */
bool CKeyboard::Update()
{
  if (FAILED(m_pDIDev->GetDeviceState(sizeof(m_keys), (LPVOID)m_keys)))
  {
		if (FAILED(m_pDIDev->Acquire()))
			return FALSE;

		if (FAILED(m_pDIDev->GetDeviceState(sizeof(m_keys),(LPVOID)m_keys)))
			return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////
/** 构造函数 */
CMouse::CMouse(LPDIRECTINPUT8 pDI, HWND hwnd, bool isExclusive)
{
	RECT r;
	GetClientRect(hwnd , &r);
	m_iWidth = r.right - r.left;
	m_iHeight = r.bottom - r.top;

	m_iMousePos.x = m_iWidth/2;
	m_iMousePos.y = m_iHeight/2;

	/** 创建设备对象 */
	if (FAILED(pDI->CreateDevice(GUID_SysMouse, &m_pDIDev, NULL)))
	{
		MessageBox(NULL,"创建鼠标设备对象失败！","错误",MB_OK);
	}

	/** 设置数据格式 */
	if (FAILED(m_pDIDev->SetDataFormat(&c_dfDIMouse)))
	{
		MessageBox(NULL,"设置鼠标数据格式失败！","错误",MB_OK);
	}

	DWORD flags;

	if (isExclusive)
		flags = DISCL_FOREGROUND|DISCL_EXCLUSIVE|DISCL_NOWINKEY;
	else
		flags = DISCL_FOREGROUND|DISCL_NONEXCLUSIVE;

	/** 设置协作层次 */
	if (FAILED(m_pDIDev->SetCooperativeLevel(hwnd, flags)))
	{
		MessageBox(NULL,"设置鼠标协作层次失败！","错误",MB_OK);
	}
    
	/** 设置鼠标属性 */
	DIPROPDWORD  dipROPWORD;
	dipROPWORD.diph.dwSize = sizeof(DIPROPDWORD);
	dipROPWORD.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipROPWORD.diph.dwObj = 0;
	dipROPWORD.diph.dwHow = DIPH_DEVICE;
	dipROPWORD.dwData = DINPUT_BUFFERSIZE;
	
	if(FAILED(m_pDIDev->SetProperty(DIPROP_BUFFERSIZE,&dipROPWORD.diph)))
	{
		MessageBox(NULL,"设置鼠标设备属性失败！","错误",MB_OK);
	}
	/** 捕获设备 */
	if (FAILED(m_pDIDev->Acquire()))
	{
		MessageBox(NULL,"捕获鼠标设备失败！","错误",MB_OK);
	}

}

/** 判断鼠标左键是否按下 */
bool CMouse::IsLButtonPressed()
{
	for(int i=0;i<DINPUT_BUFFERSIZE; i++)
	{
		if((m_diData[i].dwOfs == DIMOFS_BUTTON0) && 
		   (m_diData[i].dwData & 0x80))
		   return true;
	}
	return false;
}

/** 判断鼠标右键是否按下 */
bool CMouse::IsRButtonPressed()
{
	for(int i=0;i<DINPUT_BUFFERSIZE; i++)
	{
		if((m_diData[i].dwOfs == DIMOFS_BUTTON1) && 
		   (m_diData[i].dwData & 0x80))
		   return true;
	}
	return false;
}


/** 判断鼠标中键是否按下 */
bool CMouse::IsMButtonPressed()
{
	for(int i=0;i<DINPUT_BUFFERSIZE; i++)
	{
		if((m_diData[i].dwOfs == DIMOFS_BUTTON2) && 
		   (m_diData[i].dwData & 0x80))
		   return true;
	}
	return false;
}

/** 析构函数 */
CMouse::~CMouse()
{
	if (m_pDIDev)
	{
		m_pDIDev->Unacquire();
		m_pDIDev->Release();
	}
}


/** 更新鼠标参数 */
bool CMouse::Update()
{
	HRESULT  hr;
	DWORD  dwReadNum = 1;
	int i;

	/** 清零鼠标缓冲区 */
	ZeroMemory(m_diData,sizeof(DIDEVICEOBJECTDATA) * DINPUT_BUFFERSIZE);

	/**循环读取鼠标数据 */
	for(i=0; i<DINPUT_BUFFERSIZE;i++)
	{
		hr = m_pDIDev->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),
							&m_diData[i],&dwReadNum,0);
		if(hr == DIERR_INPUTLOST)
		{
			m_pDIDev->Acquire();
			hr = m_pDIDev->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),
							&m_diData[i],&dwReadNum,0);
			if(FAILED(hr))
			{
				MessageBox(NULL,"取得鼠标缓冲区数据失败!","错误",MB_OK);
				return false;
			}
		}

		if(m_diData[i].dwOfs == DIMOFS_X)
			m_iMousePos.x += m_diData[i].dwData;

		if(m_diData[i].dwOfs == DIMOFS_Y)
			m_iMousePos.y += m_diData[i].dwData;
		
		if(m_iMousePos.x < 0)
			m_iMousePos.x = 0;
		if(m_iMousePos.x> m_iWidth)
			m_iMousePos.x = m_iWidth;
		if(m_iMousePos.y < 0)
			m_iMousePos.y = 0;
		if(m_iMousePos.y > m_iHeight)
			m_iMousePos.y = m_iHeight;

        		
	}
  return true;
}

///////////////////
CInputSystem* CInputSystem::m_pInput = NULL;
HWND CInputSystem::m_hWnd = NULL;

/** 初始化设备 */
bool CInputSystem::Init(HWND hwnd,HINSTANCE appInstance,
						bool isExclusive,DWORD flags)
{
	m_pKeyboard = NULL; 
	m_pMouse = NULL; 
	m_hWnd = hwnd;

	if (FAILED(DirectInput8Create(appInstance,
								DIRECTINPUT_VERSION,
								IID_IDirectInput8,
								(void **)&m_pDI,NULL)))
		return FALSE;

	if (flags & IS_USEKEYBOARD)
	{
		m_pKeyboard = new CKeyboard(m_pDI, hwnd);
	}
	if (flags & IS_USEMOUSE)
	{
		m_pMouse = new CMouse(m_pDI, hwnd, true);
	}

	return true;

}

/** 更新输入设备的参数 */
void CInputSystem::Update()
{
	if (m_pKeyboard)	m_pKeyboard->Update();
	if (m_pMouse)		m_pMouse->Update();

	//-----用来记录Clicked事件------
	static bool bMouseLDown=false;
	if(m_pMouse->IsLButtonPressed())
	{
		bMouseLDown=true;
		bClick=false;
	}
	else if(bMouseLDown==true && m_pMouse->IsLButtonPressed()==false)
	{
		bMouseLDown=false;
		bClick=true;
	}
	else
	{
		bMouseLDown=false;
		bClick=false;
	}
	//------------------------------

	//--用来记录FirstMouseDownL事件--
	static bool bMouseLUp=true;
	if(m_pMouse->IsLButtonPressed()==false)
		bMouseLUp=true;
	if(m_pMouse->IsLButtonPressed()==true && bMouseLUp==true)
	{
		bFirstMDownL=true;
		bMouseLUp=false;
	}
	else bFirstMDownL=false;
}

/** 释放输入设备资源 */
bool CInputSystem::ShutDown()
{
	UnacquireAll();

    if (m_pKeyboard)
	{  
		delete m_pKeyboard;
		m_pKeyboard  = NULL;
	}
    if (m_pKeyboard)
	{
	   delete m_pMouse;
	   m_pMouse = NULL;
	}

	if (FAILED(m_pDI->Release()))
		return false;

     return true;
}

/** 获得光标的3D坐标 */
Vector3  CInputSystem::GetMousePosition()
{
	float depth[2];
	Point2 p;
	p = m_pMouse->GetMousePos();

	GLdouble x,y,z;
	glReadPixels(p.x,p.y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,depth);
	GLdouble model[16],project[16];
	glGetDoublev(GL_MODELVIEW_MATRIX,model);
	glGetDoublev(GL_PROJECTION_MATRIX,project);
	GLint viewport[4]={0,0,m_pMouse->m_iWidth,m_pMouse->m_iHeight};
	gluUnProject(p.x,p.y,depth[0],model,project,viewport,&x,&y,&z);

	return Vector3((float)x,(float)y,(float)z);
}


