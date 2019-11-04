//========================================================
/**
*  @file      ListView.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  列表框类
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
#include "listview.h"

ListView::ListView(void):m_nCurrItem(0),m_iBorderWidth(4)
{
	m_sItemColor=SColor(0.f,0.f,1.f,1.f);
	m_sTextColor=SColor(0.0f,1.0f,0.0f,1.0f);
	m_cScroll.style=FALSE;
	m_cScroll.width=16;
}


/**\brief
 * 取得列表项上的字符串
 */
char* ListView::GetItemText(unsigned int _nWhere)
{
	if(_nWhere>=m_vItems.size())return NULL;
	else return m_vItems[_nWhere].GetText();
}


/**\brief
 * 插入一个列表项
 *
 *\param	_iWhere	插入的位置，为-1时插到表尾
 *\param	_str	插入项上的内容
 *\return	BOOL	插入是否成功
 */
BOOL ListView::InsertItem(unsigned int _iWhere,char* str)
{
	CButton t_cItem;
	t_cItem.SetTextColor(m_sTextColor);
	t_cItem.SetTextStyle(BTN_TEXT_LEFT);
	t_cItem.SetBlend(TRUE);
	t_cItem.SetText(str);

	if(_iWhere=-1){m_vItems.push_back(t_cItem);return TRUE;}
	else if(_iWhere>m_vItems.size())return FALSE;
	else
	{
		m_vItems.insert(m_vItems.begin()+_iWhere,t_cItem);
		return TRUE;
	}

	return TRUE;
}


/**\brief 
 * 删除一个列表项，默认参数时岀除表尾元素
 */
BOOL ListView::DeleteItem(unsigned int _iWhere)
{
	if(m_vItems.size()<=0)return FALSE;

	if(_iWhere==-1){m_vItems.pop_back();return TRUE;}
	else if(_iWhere>=m_vItems.size())return FALSE;
	{
		m_vItems.erase(m_vItems.begin()+_iWhere);
		return TRUE;
	}

	return TRUE;
}


/**\brief
 * 绘制列表
 */
void ListView::OnDraw(void)
{
	// 检测并调整高度
	if((m_sRect.height-m_iBorderWidth*2)%ITEM_HEIGHT!=0)
		m_sRect.height-=(m_sRect.height-m_iBorderWidth*2)%ITEM_HEIGHT-ITEM_HEIGHT;

	// 绘制滚动条
	m_cScroll.x=m_sRect.x+m_sRect.width-m_cScroll.width-m_iBorderWidth;
	m_cScroll.y=m_sRect.y+m_iBorderWidth;
	m_cScroll.MaxLength=m_sRect.height-m_iBorderWidth*2;
	m_cScroll.draw();

	//一些成员赋值
	m_nMaxItemNum	=(m_sRect.height-m_iBorderWidth*2)/ITEM_HEIGHT;
	m_iItemWidth	=m_sRect.width-m_iBorderWidth*2-m_cScroll.width-1;
	m_iItemX		=m_sRect.x+m_iBorderWidth;
	m_iFirstY		=m_sRect.y+m_sRect.height-ITEM_HEIGHT-m_iBorderWidth;

	// 绘制列表项
	unsigned int t_iNum;	//要渲染项的个数
	unsigned int t_iWhich;	//从哪一项开始渲染
	if(m_vItems.size()<=m_nMaxItemNum){t_iNum=m_vItems.size();t_iWhich=0;}
	else
	{
		t_iNum=m_nMaxItemNum;
		if(IsActive())
		{
			if(CInput::GetSysInput()->GetMouse()->GetWheelState()<0)
				m_cScroll.SetPos(m_cScroll.GetPos()+0.1f);
			if(CInput::GetSysInput()->GetMouse()->GetWheelState()>0)
				m_cScroll.SetPos(m_cScroll.GetPos()-0.1f);
		}
		t_iWhich=(unsigned int)((m_vItems.size()-m_nMaxItemNum)*m_cScroll.GetPos());
	}

	for(unsigned int i=t_iWhich;i<(t_iNum+t_iWhich);++i)
	{
		m_vItems[i].SetNormalColor(m_sItemColor);
		m_vItems[i].SetRect(SRect(m_iItemX,m_iFirstY-(i-t_iWhich)*ITEM_HEIGHT,m_iItemWidth,ITEM_HEIGHT-1));
		m_vItems[i].draw();
		if(m_vItems[i].GetState()==BUTTON_CLICK)m_nCurrItem=i;
	}

}
