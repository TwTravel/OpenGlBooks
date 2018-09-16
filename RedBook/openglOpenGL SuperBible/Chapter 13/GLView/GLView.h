// GLView.h : main header file for the GLVIEW application
//

#if !defined(AFX_GLVIEW_H__D85222E7_6DEE_11D3_8A92_00A0CC2492EC__INCLUDED_)
#define AFX_GLVIEW_H__D85222E7_6DEE_11D3_8A92_00A0CC2492EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CGLViewApp:
// See GLView.cpp for the implementation of this class
//

class CGLViewApp : public CWinApp
{
public:
	CGLViewApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLViewApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGLViewApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLVIEW_H__D85222E7_6DEE_11D3_8A92_00A0CC2492EC__INCLUDED_)
