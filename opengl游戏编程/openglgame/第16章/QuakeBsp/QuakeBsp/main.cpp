#include "main.h"								
#include "Camera.h"								
#include "Quake3Bsp.h"							
#include "Frustum.h"							

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")
#pragma comment(lib, "jpeg.lib")
#pragma comment(lib, "winmm.lib")


CVector3 g_vVelocity = CVector3(0, 0, 0);


CCamera g_Camera;								
bool  g_bFullScreen = true;						
HWND  g_hWnd;									
RECT  g_rRect;									
HDC   g_hDC;									
HGLRC g_hRC;									
HINSTANCE g_hInstance;							

UINT g_Texture[MAX_TEXTURES] = {0};				

bool g_StopUpdate = false;

CQuake3BSP g_Level;

bool g_RenderMode = true;
bool g_bTextures  = true;

/** 多重纹理指针 */
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB		 = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = NULL;

bool g_bLightmaps = true;

float g_Gamma = 10;

CFrustum g_Frustum;

int g_VisibleFaces = 0;


/** 初始化 */
void Init(HWND hWnd)
{
	g_hWnd = hWnd;										
	GetClientRect(g_hWnd, &g_rRect);					
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	

	/**　读入配置文件 */
	ifstream fin("Config.ini");
	string strLevel = "";
	string strTemp = "";

	if(fin.fail())
	{
		MessageBox(g_hWnd, "Could not find Config.ini file!", "Error", MB_OK);
		PostQuitMessage(0);
		return;
	}

	fin >> strLevel >> strLevel;
	
	fin >> strTemp  >> g_Gamma;

	glActiveTextureARB		 = (PFNGLACTIVETEXTUREARBPROC)	 wglGetProcAddress("glActiveTextureARB");
    glClientActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)   wglGetProcAddress("glClientActiveTextureARB");

	if(!glActiveTextureARB || !glClientActiveTextureARB)
	{
		MessageBox(g_hWnd, "Your video card doesn't support multitexturing", "Error", MB_OK);
		PostQuitMessage(0);
	}

	fin.close();

	/** 载入BSP文件 */
	bool bResult = g_Level.LoadBSP(strLevel.c_str());

	if(bResult == false)
	{
		PostQuitMessage(0);
		return;
	}


	g_Camera.PositionCamera( -555, 150, 55,	-555, 150, 155,	0, 1, 0);

	glEnable(GL_DEPTH_TEST);	
	glEnable(GL_TEXTURE_2D);

	glCullFace(GL_FRONT);
 	glEnable(GL_CULL_FACE);
}


WPARAM MainLoop()
{
	MSG msg;

	while(1)											
	{													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        { 
			if(msg.message == WM_QUIT)					
				break;
            TranslateMessage(&msg);						
            DispatchMessage(&msg);						
        }
		else											
		{ 
			g_Camera.Update();							
			RenderScene();								
        } 
	}
	
	DeInit();											

	return(msg.wParam);									
}


void RenderScene() 
{
	int i = 0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glLoadIdentity();									

	
	g_Camera.Look();

	g_Frustum.CalculateFrustum();

	static CVector3 s_pos;
	s_pos = g_Camera.Position();
	if (g_StopUpdate)
	{
		g_Level.RenderLevel(s_pos);
	}
	else
	{
		g_Level.RenderLevel(g_Camera.Position());
	}

	SwapBuffers(g_hDC);	
}



LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG    lRet = 0; 
    PAINTSTRUCT    ps;

    switch (uMsg)
	{ 
    case WM_SIZE:										
		if(!g_bFullScreen)								
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));
			GetClientRect(hWnd, &g_rRect);					
		}
        break; 
 
	case WM_PAINT:										
		BeginPaint(hWnd, &ps);									
		EndPaint(hWnd, &ps);							
		break;

	case WM_KEYDOWN:

		switch(wParam) {								
			case VK_ESCAPE:								
				PostQuitMessage(0);						
				break;

			case VK_F1:									
		
				g_RenderMode = !g_RenderMode;			

				if(g_RenderMode) 				
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	
				}
				else 
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
				}
				break;
			case VK_F2:
				g_StopUpdate = !g_StopUpdate;

				break;


			case VK_SPACE:								
				if(g_Level.IsOnGround())
					g_vVelocity.y = kJumpAcceleration;
				break;

		}
		break;

    case WM_CLOSE:										
        PostQuitMessage(0);								
        break; 
     
    default:											
        lRet = DefWindowProc (hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return lRet;										
}
