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



  方塊主內容



/*///////////////////////////////////////////
void appTetrisRun();
class Tetris : public appProc
{
private :
public:
	void DrawText();
	void ListDelIs();
	void Rotation();
	void Move();
	BOOL IsInRect( int gx , int gy );
	BOOL IsDown( int nx , int ny );
	void BoxDown();
	void DrawNowBox();
	void GetBox();
	void DrawBox( int gx , int gy , int index );
	void Updata();
	void Run();
	void Render();
	void RenderBack();
};

/*///////////////////////////////////////////



  準備開始



/*///////////////////////////////////////////
void appTetrisIsRun();
class TetrisIs : public appProc
{
public :
	void Run();
	void Render();
};

/*///////////////////////////////////////////



  遊戲結束



/*///////////////////////////////////////////
void appTetrisGameOverRun();
class TetrisGameOver : public appProc
{
public :
	void Run();
	void Render();
};


#endif // !defined(AFX_APPPROC_H__FE858DC1_2FD3_11D8_B47F_0002441FF68C__INCLUDED_)
