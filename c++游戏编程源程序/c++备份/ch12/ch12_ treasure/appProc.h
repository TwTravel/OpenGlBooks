// appProc.h: interface for the appProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPPROC_H__FE858DC1_2FD3_11D8_B47F_0002441FF68C__INCLUDED_)
#define AFX_APPPROC_H__FE858DC1_2FD3_11D8_B47F_0002441FF68C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class appProc  
{
public:
	virtual void Render();
	virtual void Run();

};

/*///////////////////////////////////////////



  尋寶主內容



/*///////////////////////////////////////////
void appTreasureRun();
class appTreasure : public appProc  
{
public:
	void DrawText();
	void GetProp();
	void DrawProp();
	void AddObj();
	void Move();
	void Render();
	void Run();
};
/*///////////////////////////////////////////

  準備開始

/*///////////////////////////////////////////
void appTreasureStartRun();
class appTreasureStart : public appProc  
{
public:
	void Render();
	void Run();
};

/*///////////////////////////////////////////

  遊戲結束

/*///////////////////////////////////////////
void appTreasureGameoverRun();
class appTreasureGameover : public appProc  
{
public:
	void Render();
	void Run();
};
/*///////////////////////////////////////////

  基底物件宣告

/*///////////////////////////////////////////
class gameObj
{
public ://排序串列
	gameObj * m_Exit ;//串列下一個
	gameObj * m_Next ;//串列上一個
public :
	float m_x , m_y , m_top ;
public :
	virtual void SortDraw();
	void AddSort();
};
/*///////////////////////////////////////////

  娃娃資料宣告

/*///////////////////////////////////////////
//#include "myd3d.h"
class gameRole : public gameObj
{
private :
	int m_GridTime , m_GridNextTime ;//播放時間
	int m_Index ;//目前格位索引
	int m_News ;//方向
public :
	d3dCartoon *m_Cartoon ;//動畫串
	int m_Alpha ;//物件顏色
public :
	void SetCartoon( d3dCartoon * cartoon , int NextTime , int News );
	void Draw();
	void SortDraw();
};

#endif // !defined(AFX_APPPROC_H__FE858DC1_2FD3_11D8_B47F_0002441FF68C__INCLUDED_)
