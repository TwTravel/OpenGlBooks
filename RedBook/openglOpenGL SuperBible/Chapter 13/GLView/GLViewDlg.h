// GLViewDlg.h : header file
//

#if !defined(AFX_GLVIEWDLG_H__D85222E9_6DEE_11D3_8A92_00A0CC2492EC__INCLUDED_)
#define AFX_GLVIEWDLG_H__D85222E9_6DEE_11D3_8A92_00A0CC2492EC__INCLUDED_

#include "GLWindow.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CGLViewDlg dialog

class CGLViewDlg : public CDialog
{
// Construction
public:
	CGLViewDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CGLViewDlg)
	enum { IDD = IDD_GLVIEW_DIALOG };
	CListBox	m_lbWGLExtensions;
	CListBox	m_lbGLExtensions;
	CListBox	m_ListBox;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CGLWindow m_glWnd;
	HICON m_hIcon;
	CRect newRect;

	// Generated message map functions
	//{{AFX_MSG(CGLViewDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLVIEWDLG_H__D85222E9_6DEE_11D3_8A92_00A0CC2492EC__INCLUDED_)
