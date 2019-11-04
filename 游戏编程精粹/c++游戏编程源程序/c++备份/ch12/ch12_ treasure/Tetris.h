// Tetris.h: interface for the Tetris class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TETRIS_H__FE858DC2_2FD3_11D8_B47F_0002441FF68C__INCLUDED_)
#define AFX_TETRIS_H__FE858DC2_2FD3_11D8_B47F_0002441FF68C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "appProc.h"

class Tetris : public appProc  
{
public:
	void Run();
	Tetris();
	virtual ~Tetris();

};

#endif // !defined(AFX_TETRIS_H__FE858DC2_2FD3_11D8_B47F_0002441FF68C__INCLUDED_)
