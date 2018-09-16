#if !defined(AFX_GLWINDOW_H__048D92C6_6DF2_11D3_8A92_00A0CC2492EC__INCLUDED_)
#define AFX_GLWINDOW_H__048D92C6_6DF2_11D3_8A92_00A0CC2492EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GLWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGLWindow window

class CGLWindow : public CWnd
{
// Construction
public:
	CGLWindow();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLWindow)
	//}}AFX_VIRTUAL

// Implementation
public:
	int m_nPixelFormat;
	HGLRC m_hRC;
	HDC m_hDC;
	virtual ~CGLWindow();

	// Generated message map functions
protected:
	GLuint m_Sides[6];
	//{{AFX_MSG(CGLWindow)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLWINDOW_H__048D92C6_6DF2_11D3_8A92_00A0CC2492EC__INCLUDED_)
