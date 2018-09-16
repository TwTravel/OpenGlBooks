// GLViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GLView.h"
#include "GLViewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLViewDlg dialog

CGLViewDlg::CGLViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGLViewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGLViewDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGLViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGLViewDlg)
	DDX_Control(pDX, IDC_LST_EXT2, m_lbWGLExtensions);
	DDX_Control(pDX, IDC_LST_EXT, m_lbGLExtensions);
	DDX_Control(pDX, IDC_LIST, m_ListBox);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGLViewDlg, CDialog)
	//{{AFX_MSG_MAP(CGLViewDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLViewDlg message handlers

BOOL CGLViewDlg::OnInitDialog()
	{
	PIXELFORMATDESCRIPTOR pfd;		// Pixel format descriptor
	char cBuffer[64];				// String buffer 
	char cOutput[128];
	int nTabs[] = { 16, 40, 64, 95, 159, 195, 222 };
	
	CDialog::OnInitDialog();
	m_ListBox.SetTabStops(7, nTabs);

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
		{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
			{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
			}
		}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	WNDCLASS wndClass;

	wndClass.style = CS_OWNDC;
	wndClass.lpfnWndProc = AfxWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = AfxGetInstanceHandle();
	wndClass.hIcon = NULL;
	wndClass.hCursor = NULL;
	wndClass.hbrBackground = NULL;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = "OpenGLClass";

	// Register the window class
	AfxRegisterClass(&wndClass);


	// Get the number of pixel formats
	// Will need a device context
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	HDC hDC = ::GetDC(this->m_hWnd);
	int nFormatCount = DescribePixelFormat(hDC, 1, 0, NULL);

	// Retrieve each pixel format and store a pointer to it in the list box
	// Parse it apart and put some relevant data in the list box columns
	for(int i = 1; i <= nFormatCount; i++)
		{
		// Get description of pixel format
		DescribePixelFormat(hDC, i, pfd.nSize, &pfd);
		
		// Pixel format identifier
		wsprintf(cOutput,"%d\t",i);

		// Driver type /////////////////////////////////
		if(!(pfd.dwFlags & PFD_GENERIC_FORMAT))
			strcat(cOutput,"ICD\t");
		else
			{
			if(pfd.dwFlags & PFD_GENERIC_ACCELERATED)
				strcat(cOutput,"MCD\t");
			else
				strcat(cOutput,"GEN\t");
			}

		
		// Double or single buffer
		if(pfd.dwFlags & PFD_DOUBLEBUFFER)
			strcat(cOutput,"DBL\t");
		else
			strcat(cOutput,"SGL\t");

		// RGBA or CI
		if(pfd.iPixelType == PFD_TYPE_RGBA)
			strcat(cOutput,"RGBA\t");
		else
			strcat(cOutput,"CI\t");


		// Color
		wsprintf(cBuffer,"%d (%d-%d-%d-%d)\t",pfd.cColorBits, pfd.cRedBits, 
			pfd.cGreenBits, pfd.cBlueBits, pfd.cAlphaBits);
		strcat(cOutput, cBuffer);

		// Depth, alpha, stencil, Accumulation
		wsprintf(cBuffer,"%d\t%d\t%d(%d-%d-%d-%d)",pfd.cDepthBits,
			pfd.cStencilBits,pfd.cAccumBits,
			pfd.cAccumRedBits, pfd.cAccumGreenBits, pfd.cAccumBlueBits,
			pfd.cAccumAlphaBits);
		strcat(cOutput, cBuffer);

		// Finally, add to list box
		m_ListBox.AddString(cOutput);
		}


	m_ListBox.SetCurSel(0);

	// Free the Device Context
	::ReleaseDC(this->m_hWnd, hDC);


	CRect glRect;
	CRect dlgRect;
	CRect clRect;

	GetWindowRect(&dlgRect);
	GetClientRect(&clRect);

	// Create the storm direction window control
	GetDlgItem(IDC_TEST_FRAME)->GetWindowRect(&glRect);

	newRect.top = glRect.top - dlgRect.top - (dlgRect.Height() - clRect.Height()) +20;
	newRect.bottom = glRect.bottom - dlgRect.top - (dlgRect.Height() - clRect.Height()) -3;
	newRect.left = glRect.left - dlgRect.left - (dlgRect.Width() - clRect.Width()) +9;
	newRect.right = glRect.right - dlgRect.left  - (dlgRect.Width() - clRect.Width()) -1 ;


	// Create initial window
	OnSelchangeList();

	return TRUE;  // return TRUE  unless you set the focus to a control
	}

void CGLViewDlg::OnSysCommand(UINT nID, LPARAM lParam)
	{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
		{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
		}
	else
		{
		CDialog::OnSysCommand(nID, lParam);
		}
	}	

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGLViewDlg::OnPaint() 
	{
	if (IsIconic())
		{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
		}
	else
		{
		CDialog::OnPaint();
		}
	}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGLViewDlg::OnQueryDragIcon()
	{
	return (HCURSOR) m_hIcon;
	}

// Whenever the selection in the list box changes, we must destroy the current
// window if it exists, and create a new one
void CGLViewDlg::OnSelchangeList() 
	{
	// Destroy the window and reset the pixel format
	m_glWnd.DestroyWindow();
	m_glWnd.m_nPixelFormat = m_ListBox.GetCurSel() + 1;

	m_glWnd.Create("OpenGLClass",NULL,WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			newRect,this,901,NULL);
	
	m_glWnd.Invalidate(FALSE);

    m_lbWGLExtensions.ResetContent();
    m_lbGLExtensions.ResetContent();

	// Update String info
	SetDlgItemText(IDC_VENDOR,(const char *)glGetString(GL_VENDOR));
	SetDlgItemText(IDC_RENDERER, (const char *)glGetString(GL_RENDERER));
	SetDlgItemText(IDC_VERSION, (const char *)glGetString(GL_VERSION));

	// Display GLU Driver/Implementation info.
	SetDlgItemText(IDC_GLUVERSION, (const char *)gluGetString(GLU_VERSION));

	//////////////////////////////////////////////////////////////////////
	// Put all GL Extensions in the list box
	char *szTemp = NULL;
	char *szStringTemp = NULL;
	m_lbGLExtensions.ResetContent();
	if(glGetString(GL_EXTENSIONS) != NULL)
		{
		szStringTemp = strdup((const char *)glGetString(GL_EXTENSIONS));

		szTemp = strtok(szStringTemp," ");
		while(szTemp != NULL)
			{
			m_lbGLExtensions.AddString(szTemp);
			szTemp = strtok(NULL," ");
			}
		
		free(szStringTemp);


		// Put all wgl extensions in the list box
		m_lbWGLExtensions.ResetContent();
		if(strstr((char *)glGetString(GL_EXTENSIONS), "WGL_EXT_extensions_string") != NULL)
			{
			// Get function pointer to wglExtension string
			PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
			wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");

			if(wglGetExtensionsStringARB != NULL)
				{
				szStringTemp = strdup((const char *)wglGetExtensionsStringARB(m_glWnd.m_hDC));
				
				szTemp = strtok(szStringTemp," ");
				while(szTemp != NULL)
					{
					m_lbWGLExtensions.AddString(szTemp);
					szTemp = strtok(NULL," ");
					}
				
				free(szStringTemp);
			
				}
			}
		}

	}
