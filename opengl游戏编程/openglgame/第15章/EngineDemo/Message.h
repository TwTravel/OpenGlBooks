//========================================================
/**
*  @file      Message.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  消息处理类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <deque>
using namespace std;


#define SYS_MESSAGE (Message::GetSysMessage())

/*\brief
 * 消息类型的定义
 */
typedef enum MESSAGE_
{
	MSG_NULL=0,			//空消息

	MSG_PLAYAVI,		//播放视频动画(str1=filename,str2=index)
	MSG_TITLE,			//进入字幕场景(str1=filename,str2=index)
	MSG_SELCHAR,		//进入角色选择场景
	MSG_OPENAIR,		//进入室外场景(str1=filename,str2=index)
	MSG_EXIT,			//退出

	MSG_LOADMAP,		//读入地图
	MSG_SAVEMAP,		//存储地图

	MSG_MOVE,			//行走			(p1=x, p2=y)
	MSG_SETDIR,			//设置人物方向	(p1=方向)
	MSG_SETPOSITION,	//设置人物位置	(p1=x, p2=y)
	MSG_PLAYERSHOW,		//设置主角显示与否 (p1=bShow)
	MSG_LOADPLAYER,		//从ini文件读入主角(p1=rolenum s1=filename s2=index)

	MSG_ADDMONEY,		//改变金钱		(p1=money)
	MSG_ADDOBJECT,		//加入物体		(s1=物体名 p1=num)
	MSG_DELOBJECT,		//删除物体		(s1=物体名 p1=num)
	MSG_ADDMAGIC,		//加入魔法		(s1=魔法名)
	MSG_DELMAGIC,		//删除魔法      (s1=魔法名)

	MSG_MOVENPCTO,		//NPC移动到		(s1=Npc名字, p1=x, p2=y)
	MSG_MOVENPC,		//NPC移动		(s1=Npc名字, p1=x, p2=y)
	MSG_SETNPCPOS,		//设置NPC位置	(p1=x, p2=y)
	MSG_SETNPCDIR,		//设置NPC方向	(p1=方向);
	MSG_SETNPCSTEP,		//设置NPC步伐	(p1=步伐);
	MSG_ADDNPC,			//加入NPC		(s1=filename p1=id, p2=x, p3=y)
	MSG_DELNPC,			//删除NPC		(s1=Npc名字)
	MSG_SETNPCACTIVE,	//设置NPC活动与否	(s1=Npc名字, p1=bActive)
	MSG_SETNPCSHOW,		//设置NPC显示与否	(s1=Npc名字, p1=bShow)
	MSG_SETNPCSCRIPT,	//设置NPC脚本		(s1=Npc名字, s2=脚本文件)

	MSG_DELAY,			//延时			(p1=time)
	MSG_FADE,			//淡入淡出		(p1=1淡出，p1=0淡入)
	MSG_PLAYSOUND,		//播放音效		(s1=File)
	MSG_STOPSOUND,		//停止播放
	MSG_PLAY_MUSIC,		//播放音乐		(s1=filename)
	MSG_STOP_MUSIC,		//停止播放音乐
	MSG_PAUSE_MUSIC,	//暂停播放
	MSG_RESUME_MUSIC,	//继续播放
	MSG_BREAK,			//本次不执行

} MESSAGE;


/**\brief
 * 消息传递数据
 */
struct SMessage
{
	//! 消息类型
	MESSAGE msg;

	//! 参数
	int param1,param2;

	//! 字符指针参数
	char *str1,*str2,*str3;

	SMessage():param1(0),param2(0),str1(NULL),str2(NULL),str3(NULL){}
	SMessage(MESSAGE _msg,int p1,int p2,char* s1,char* s2,char* s3)
		:msg(_msg),param1(p1),param2(p2),str1(s1),str2(s2),str3(s3){}
	~SMessage(){}
};


/**\brief
 * 消息处理类
 */
class Message
{
public:
	Message(void);
	~Message(void);


	/**\brief
	 * 在队尾插入一条消息
	 */
	BOOL Insert(SMessage _msg){m_dQueue.push_back(_msg);return TRUE;}


	/**\brief
	 * 在队头删除一条消息
	 */
	BOOL Delete(){if(m_dQueue.size()<=0)return FALSE;else{m_dQueue.pop_front();return TRUE;}}


	/**\brief
	 * 清空消息队列
	 */
	void Clear(){m_dQueue.clear();}


	/**\brief
	 * 消息队列中消息的个数
	 */
	unsigned int GetMsgNum(){return m_dQueue.size();}


	/**\brief
	 * 执行当前消息
	 */
	void Run();


	/**\brief
	 * 取得当前的消息处理对象
	 */
	static Message* GetSysMessage(){return m_pMessage;}


private:
	//! 消息队列
	deque<SMessage> m_dQueue;

	//! 消息处理对象的指针
	static Message* m_pMessage;

};



#endif