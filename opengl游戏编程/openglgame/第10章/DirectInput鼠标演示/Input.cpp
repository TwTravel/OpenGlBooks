//======================================================================
/**
*  @file      Input.cpp
*
*  项目描述： DirectInput输入系统
*   
*  文件描述:  输入系统类CInputSystem
*             其中,类CKeyboard、CMouse和CJoystick分别用来处理键盘、鼠标
*             和游戏手柄，CInputSystem是输入设备的管理类。
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-24	
*  修改日期： 2007-07-24
*/																			
//======================================================================

#include "Input.h"

//************** 键盘类CKeyboard的实现 **************************/
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
			return false;

		if (FAILED(m_pDIDev->GetDeviceState(sizeof(m_keys),(LPVOID)m_keys)))
			return false;
	}

	return TRUE;
}

//************** 鼠标类CMouse的实现 **************************/
/** 构造函数 */
CMouse::CMouse(LPDIRECTINPUT8 pDI, HWND hwnd, bool isExclusive)
{
	/** 得到当前窗口的大小 */
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

		/** 更新鼠标位置 */
		if(m_diData[i].dwOfs == DIMOFS_X)
			m_iMousePos.x += m_diData[i].dwData;

		if(m_diData[i].dwOfs == DIMOFS_Y)
			m_iMousePos.y += m_diData[i].dwData;
		
		/** 限制鼠标在窗口范围 */
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


//************** 游戏手柄CJoystick的实现 *******************************/

CJoystick::CJoystick(LPDIRECTINPUT8 pDI,HWND hwnd,HINSTANCE appInstance)
:m_iMin(-100),m_iMax(100),m_iDeadZone(20)
{
	HRESULT hr;
	m_pDI = pDI;

	/** 枚举已安装的所有游戏杆 */
	/**hr = m_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL,(LPDIENUMDEVICESCALLBACKA)EnumJSCallback,&m_pDIDev,DIEDFL_ATTACHEDONLY);
	if(FAILED(hr))
	{
		MessageBox(NULL,"枚举游戏杆失败!","错误",MB_OK);
		exit(-1);
	}*/
	if(m_pDIDev == NULL)
	{
		MessageBox(NULL,"没有安装游戏杆!","错误",MB_OK);
		exit(-1);
	}

	/** 设置游戏杆的数据格式 */
	hr = m_pDIDev->SetDataFormat(&c_dfDIJoystick2);
	if(FAILED(hr))
		MessageBox(NULL,"设置游戏杆数据格式失败！","错误",MB_OK);

	/** 设置游戏杆的协作层级 */
	hr = m_pDIDev->SetCooperativeLevel(hwnd,DISCL_BACKGROUND|DISCL_NONEXCLUSIVE);
	if(FAILED(hr))
		MessageBox(NULL,"设置游戏杆协作层级失败！","错误",MB_OK);

	/** 设置游戏杆的轴范围 */
	DIPROPRANGE  dipROPRANGE;
	dipROPRANGE.diph.dwSize = sizeof(DIPROPRANGE);
	dipROPRANGE.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipROPRANGE.diph.dwObj = 0;
	dipROPRANGE.diph.dwHow = DIPH_DEVICE;
	dipROPRANGE.lMin = m_iMin;
	dipROPRANGE.lMax = m_iMax;

	hr = m_pDIDev->SetProperty(DIPROP_RANGE,&dipROPRANGE.diph);
	if(FAILED(hr))
		MessageBox(NULL,"设置游戏杆轴范围失败！","错误",MB_OK);

	/** 设置游戏杆的死区范围 */
	DIPROPDWORD  dipROPDWORD;
	dipROPDWORD.diph.dwSize = sizeof(DIPROPDWORD);
	dipROPDWORD.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipROPDWORD.diph.dwObj = 0;
	dipROPDWORD.diph.dwHow = DIPH_DEVICE;
	dipROPDWORD.dwData = 100 * m_iDeadZone;

	hr = m_pDIDev->SetProperty(DIPROP_DEADZONE,&dipROPDWORD.diph);
	if(FAILED(hr))
		MessageBox(NULL,"设置游戏杆死区范围失败！","错误",MB_OK);

	/** 初始化数据区 */
	ZeroMemory(&m_diJoyState2,sizeof(DIJOYSTATE2));
    

}

/** 回调函数 */
bool CALLBACK CJoystick::EnumJSCallback(LPCDIDEVICEINSTANCE  pDIDeviceInstance, LPVOID pvRef)
{
	HRESULT hr;
	LPDIRECTINPUTDEVICE8* ppDID = (LPDIRECTINPUTDEVICE8* )pvRef;
	hr = m_pDI->CreateDevice(pDIDeviceInstance->guidInstance,ppDID,NULL);
	if(FAILED(hr))
		return DIENUM_CONTINUE;
	else
		return DIENUM_STOP;
}

/** 更新，获取输入 */
bool CJoystick::Update()
{
	/** 获取游戏杆的访问权 */
	HRESULT hr;
	hr = m_pDIDev->Acquire();
	if(FAILED(hr))
	{
		MessageBox(NULL,"获得游戏杆访问权失败！","错误",MB_OK);
		return false;
	}

	/** 检测游戏杆 */
	hr = m_pDIDev->Poll();
	if(FAILED(hr))
	{
		MessageBox(NULL,"检测游戏杆失败！","错误",MB_OK);
		return false;
	}

	/** 读取游戏杆的输入 */
	hr = m_pDIDev->GetDeviceState(sizeof(DIJOYSTATE2),&m_diJoyState2);
	
	/** 设备丢失 */
	if(hr == DIERR_INPUTLOST)
	{
		m_pDIDev->Acquire();  /**< 重新获得设备使用权 */
		m_pDIDev->GetDeviceState(sizeof(DIJOYSTATE2),&m_diJoyState2);
		if(FAILED(hr))
		{
			MessageBox(NULL,"获取游戏杆数据失败！","错误",MB_OK);
			return false;
		}

	}
	return true;

}

CJoystick::~CJoystick()
{
	if (m_pDI)
		m_pDI->Release();


	if (m_pDIDev)
	{
		m_pDIDev->Unacquire();
		m_pDIDev->Release();
	}
}


//************** 输入系统类CInputSystem的实现 **************************/

CInputSystem* CInputSystem::m_pInput = NULL;
HWND CInputSystem::m_hWnd = NULL;

/** 初始化设备 */
bool CInputSystem::Init(HWND hwnd,HINSTANCE appInstance,
						bool isExclusive,DWORD flags)
{
	m_pKeyboard = NULL; 
	m_pMouse = NULL;
	m_pJoystick = NULL;

	m_hWnd = hwnd;

	/** 创建DirectInput接口对象 */
	if (FAILED(DirectInput8Create(appInstance,
								DIRECTINPUT_VERSION,
								IID_IDirectInput8,
								(void **)&m_pDI,NULL)))
	{
		MessageBox(NULL,"创建DirectInput接口对象失败!","错误",MB_OK);
		return false;
	}
    
	/** 创建键盘 */
	if (flags & IS_USEKEYBOARD)
	{
		m_pKeyboard = new CKeyboard(m_pDI, hwnd);
	}

	/** 创建鼠标 */
	if (flags & IS_USEMOUSE)
	{
		m_pMouse = new CMouse(m_pDI, hwnd, true);
	}
	
	/** 创建游戏杆 */
	if (flags & IS_USEJOYSTICK)
	{
		m_pJoystick = new CJoystick(m_pDI,hwnd,appInstance);
	}

	return true;

}

/** 更新输入设备的参数 */
void CInputSystem::Update()
{
	/** 键盘更新 */
	if (m_pKeyboard)	
		m_pKeyboard->Update();
	
	/** 鼠标更新 */
	if (m_pMouse)		
		m_pMouse->Update();
	
	/** 游戏杆更新 */
	if (m_pJoystick)		
		m_pJoystick->Update();

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

    if (m_pMouse)
	{
	   delete m_pMouse;
	   m_pMouse = NULL;
	}

	if (m_pJoystick)
	{
	   delete m_pJoystick;
	   m_pJoystick = NULL;
	}
    
	/** 释放接口对象 */
	if (FAILED(m_pDI->Release()))
		return false;

     return true;
}

/** 获得光标的3D坐标 */
Vector3  CInputSystem::GetMousePosition()
{
	float depth[2];
	Point2 p;

	/** 得到鼠标的当前2D位置 */
	p = m_pMouse->GetMousePos();

	GLdouble x,y,z;

	/** 得到像素的深度值 */
	glReadPixels(p.x,p.y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,depth);
	
	/** 得到模型矩阵和投影矩阵 */
	GLdouble model[16],project[16];
	glGetDoublev(GL_MODELVIEW_MATRIX,model);
	glGetDoublev(GL_PROJECTION_MATRIX,project);

	/** 计算该点的世界坐标 */
	GLint viewport[4]={0,0,m_pMouse->m_iWidth,m_pMouse->m_iHeight};
	gluUnProject(p.x,p.y,depth[0],model,project,viewport,&x,&y,&z);

	return Vector3((float)x,(float)y,(float)z);
}


