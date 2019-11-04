//========================================================
/**
*  @file      GUIMgr.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  GUI界面管理类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#include "stdafx.h"
#include "guimgr.h"
#include "inifile.h"
#include "timer.h"
#include "input.h"
#include "message.h"

GUIMgr* GUIMgr::m_pGUIMgr=NULL;

GUIMgr::GUIMgr(void):m_bMainMenu(FALSE),m_fFadeAlpha(0.f)
{
	m_pGUIMgr=this;
}

GUIMgr::~GUIMgr(void)
{
}


/**\brief
 * 从INI文件载入光标数据
 */
BOOL GUIMgr::LoadCurIni(char* _filename,char* _index)
{
	if(!m_cCursor.LoadIni(_filename,_index))return FALSE;
	bottom.LoadIni("ini\\bottom.ini","bottom");

	m_cChatList.SetRect(SRect(114,53,528,52));
	m_cChatList.SetColor(SColor(0.8f,0.5f,0.6f,0.5f));
	m_cChatList.SetBorderWidth(0);
	m_cChatList.SetBlend(TRUE);
	m_cChatList.SetItemColor(SColor(1.f,1.f,1.f,0.f));

	m_cChatEdit.SetRect(SRect(214,31,415,40));
	m_cChatEdit.SetBlend(TRUE);
	m_cChatEdit.SetColor(SColor(1.f,1.f,1.f,0.f));
	m_cChatEdit.SetInputNamePos(SPoint2(3,130));
	m_cChatEdit.SetFontColor(SColor(1.f,1.f,1.f,1.f));
	m_cChatEdit.SetCaretColor(SColor(1.f,1.f,1.f,1.f));

	m_cMainMenu.LoadIni("ini\\MainMenu.ini","MainMenu");
	m_cTopMenu.LoadIni("ini\\topmenu.ini","menu");
	m_cTopAni.LoadIni("ini\\topmenu.ini","ani");

	m_cMapView.LoadIni("ini\\MapView.ini","map01");

	m_cRoleLife.LoadIni("ini\\life.ini","role");
	m_cMonsterLife.LoadIni("ini\\life.ini","monster");

	m_cPropertyWnd.LoadIni("ini\\property.ini","dialog");
	m_cBagWnd.LoadIni("ini\\bag.ini","dialog");

	m_cLifeTex.InitTexture("interface\\life.bmp",TRUE);
	m_cMagicTex.InitTexture("interface\\magic.bmp",TRUE);
	return TRUE;
}

/**\brief
 * 淡入淡出
 */
void GUIMgr::Fade(void)
{
	if(m_fFadeAlpha<=0.1f)return;

	int width=SYS_WIDTH;
	int height=SYS_HEIGHT;

	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER , 0.0f);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	static float dtime=0.f;
	dtime-=CTimer::GetSysTimer()->m_fTime;
	if(dtime<=0.0f) { m_fFadeAlpha-=0.03f;dtime=0.03f;}

	glColor4f(0.f,0.f,0.f,m_fFadeAlpha);
	glBegin(GL_QUADS);
		glVertex2i(0,0);
		glVertex2i(width,0);
		glVertex2i(width,height);
		glVertex2i(0,height);
	glEnd();

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
}


/**\brief
 * 界面的渲染
 */
void GUIMgr::Render(void)
{
	char str[25];
	BeginOrtho(CInput::GetSysInput()->GetMouse()->m_iWidth,CInput::GetSysInput()->GetMouse()->m_iHeight);

	///////////////////////////////////////////////////////
	//----------------游戏主场景时的界面-------------------
	if(SceneMgr::GetSysSceneMgr()->GetScene()==SCE_OPENAIR||SceneMgr::GetSysSceneMgr()->GetScene()==SCE_ROOM)
	{
		//------------生命球和魔法球--------
		bottom.draw();
		Ball(m_cLifeTex,25.f,10.f,CRole::GetRolePointer()->GetLife());
		Ball(m_cMagicTex,696.f,10.f,CRole::GetRolePointer()->energy);

		//------------聊天面板--------------
		m_cChatList.draw();
		m_cChatEdit.draw();
		if(m_cChatEdit.IsFocus()&&m_cChatEdit.GetReturnState())
		{
			char* str=m_cChatEdit.GetText();
			if(strlen(str)>0)
			{
				m_cChatList.InsertItem(-1,str);
				m_cChatList.GetScrollBar()->SetPos(1.f);
			}
			m_cChatEdit.SetReturnState(FALSE);
		}
		if(m_cChatList.GetItemNum()>5)m_cChatList.DeleteItem(0);
		//----------------------------------

		//--------------血槽----------------
		m_cRoleLife.SetPos(CRole::GetRolePointer()->stamina);
		m_cRoleLife.draw();

		//--------------提示的显示----------
		if(m_cRoleLife.IsActive())
		{
			SYS_FONT->SetColor(SColor(1.f,1.f,1.f,1.f));
			sprintf(str,"体力值：%.2f",CRole::GetRolePointer()->stamina);
			SYS_FONT->Format(str);
			SYS_FONT->PrintAt(200,30);
		}
		SRect r;
		r=SRect(25,10,81,90);
		if(r.PointIn(SYS_MOUSE->GetMousePos()))
		{
			SYS_FONT->SetColor(SColor(1.f,1.f,1.f,1.f));
			sprintf(str,"生命值：%.2f",CRole::GetRolePointer()->GetLife());
			SYS_FONT->Format(str);
			SYS_FONT->PrintAt(25,130);
		}
		r=SRect(696,10,81,90);
		if(r.PointIn(SYS_MOUSE->GetMousePos()))
		{
			SYS_FONT->SetColor(SColor(1.f,1.f,1.f,1.f));
			sprintf(str,"魔法值：%.2f",CRole::GetRolePointer()->energy);
			SYS_FONT->Format(str);
			SYS_FONT->PrintAt(696,130);
		}
		//----------------------------------

		char buf[16];
		float life;
		if(CRole::GetRolePointer()->GetMonsterInfo(buf,life))
		{
			m_cMonsterLife.SetPos(life);
			m_cMonsterLife.draw();
			SYS_FONT->Format(buf);
			SYS_FONT->PrintAt(300,570);
		}
		//----------------------------------

		//-------------顶部菜单-------------
		m_cTopMenu.draw();
		m_cTopAni.draw();
		if(m_cTopMenu.GetState()==1)Property();
		if(m_cTopMenu.GetState()==2)Bag();
		if(m_cTopMenu.GetState()==5)m_bMainMenu=TRUE;
		//----------------------------------

		//----------主菜单的弹出------------
		static BOOL t_bKeyDown=FALSE;
		if(SYS_KEYBOARD->KeyDown(DIK_ESCAPE))t_bKeyDown=TRUE;
		if(t_bKeyDown&&!SYS_KEYBOARD->KeyDown(DIK_ESCAPE))
		{
			m_bMainMenu=!m_bMainMenu;
			t_bKeyDown=FALSE;
		}
		if(m_bMainMenu)
		{
			m_cMainMenu.draw();
			if(m_cMainMenu.GetState()==1)m_bMainMenu=FALSE;
			//if(m_cMainMenu.GetState()==5)
			//{
			//	SYS_MESSAGE->Insert(SMessage(MSG_PLAY_MUSIC,0,0,"sound/title.mid",NULL,NULL));
			//	SYS_MESSAGE->Insert(SMessage(MSG_FADE,0,0,NULL,NULL,NULL));
			//	SYS_MESSAGE->Insert(SMessage(MSG_TITLE,0,0,"ini\\title.ini","menu",NULL));
			//	m_bMainMenu=FALSE;
			//}
			if(m_cMainMenu.GetState()==5)SYS_MESSAGE->Insert(SMessage(MSG_EXIT,0,0,NULL,NULL,NULL));
			if(m_cMainMenu.GetState()!=0)m_cTopMenu.ClearState();		
			m_cMainMenu.ClearState();
		}
		//----------------------------------

		if(SYS_KEYBOARD->KeyDown(DIK_TAB))m_cMapView.draw();
	}
	////////////////////////////////////////////////////

	//-----------帧率显示---------------
	if(SYS_KEYBOARD->KeyDown(DIK_F8))
	{
		char str[60];
		sprintf(str,"FPS:%.2f,cursor(%d,%d)",CTimer::GetSysTimer()->m_fFPS,SYS_MOUSE->GetMousePos().x,SYS_MOUSE->GetMousePos().y);
		SYS_FONT->Format(str);
		SYS_FONT->PrintAt(250,SYS_HEIGHT-20);
	}
	//----------------------------------

	//----------光标的绘制--------------
	if(SceneMgr::GetSysSceneMgr()->GetScene()!=SCE_AVI)
	{
		SPoint2 p=SYS_MOUSE->GetMousePos();
		m_cCursor.rect.x=p.x;
		m_cCursor.rect.y=p.y-m_cCursor.rect.height;//(SYS_HEIGHT-p.y)-m_cCursor.rect.height;
		m_cCursor.draw();
	}
	//----------------------------------

	Fade();//用来控制淡入淡出的函数

	if(bottom.IsActive()
		||(m_cTopMenu.GetState()==1&&m_cPropertyWnd.IsActive())
		||(m_cTopMenu.GetState()==2&&m_cBagWnd.IsActive())
		||(m_bMainMenu&&m_cMainMenu.IsActive())
		||m_cTopMenu.IsActive())
		CRole::GetRolePointer()->SetActive(FALSE);
	else if(!CRole::GetRolePointer()->GetActive())
		CRole::GetRolePointer()->SetActive(TRUE);

	EndOrtho();
}


/**\brief
 * 英雄的属性
 */
void GUIMgr::Property(void)
{
	CWindow win(m_cPropertyWnd.GetRect());
	win.SetBlend(TRUE);
	win.SetColor(SColor(0.9f,0.5f,0.5f,0.5f));
	win.draw();
	m_cPropertyWnd.draw();
	int x=m_cPropertyWnd.GetRect().x;
	int y=m_cPropertyWnd.GetRect().y;
	SYS_FONT->SetColor(SColor(1.f,0.f,0.f,1.f));
	SYS_FONT->Format("经验");SYS_FONT->PrintAt(x+88,y+350);
	SYS_FONT->Format("力量");SYS_FONT->PrintAt(x+88,y+315);
	SYS_FONT->Format("敏捷");SYS_FONT->PrintAt(x+88,y+260);
	SYS_FONT->Format("体力");SYS_FONT->PrintAt(x+88,y+190);
	SYS_FONT->Format("精力");SYS_FONT->PrintAt(x+88,y+140);

	if(m_cPropertyWnd.GetState()==1)
	{
		m_cTopMenu.ClearState();
		m_cPropertyWnd.ClearState();
	}
}


/**\brief
 * 英雄的物品栏
 */
void GUIMgr::Bag(void)
{
	CWindow win(m_cBagWnd.GetRect());
	win.SetBlend(TRUE);
	win.SetColor(SColor(0.9f,0.5f,0.5f,0.5f));
	win.draw();
	m_cBagWnd.draw();

	if(m_cBagWnd.GetState()==1)
	{
		m_cTopMenu.ClearState();
		m_cBagWnd.ClearState();
	}
}


/**\brief
 * 生命球和魔法球的绘制
 */
void GUIMgr::Ball(CTexture tex,float x,float y,float pos)
{
	int w=81,h=90;
	glEnable(GL_TEXTURE_2D);
	tex.Bind(0);
	glEnable(GL_ALPHA_TEST);
	glBegin(GL_QUADS);
		glTexCoord2f(0.f,0.f);glVertex2f(x,y);
		glTexCoord2f(1.f,0.f);glVertex2f(x+w,y);
		glTexCoord2f(1.f,pos);glVertex2f(x+w,(y+h*pos));
		glTexCoord2f(0.f,pos);glVertex2f(x,(y+h*pos));
	glEnd();
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_TEXTURE_2D);
}