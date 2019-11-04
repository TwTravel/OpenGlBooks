//========================================================
/**
*  @file      Music.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  DirectMusic音乐类
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
#include "music.h"


IDirectMusicPerformance* CMusic::g_pPerf=NULL;
IDirectMusicLoader*	CMusic::g_pLoader=NULL;
bool CMusic::bMusic=false;
bool CMusic::bExist=false;
bool CMusic::bPlay=false;
CMusic* CMusic::m_pMusic=NULL;

CMusic::CMusic():g_pMIDISeg(NULL)
{
	m_pMusic=this;
}

CMusic::~CMusic()
{
	if( bExist )
	{
		if (g_pMIDISeg)
		{
			g_pPerf->Stop(g_pMIDISeg, NULL, 0, 0);
			_RELEASE(g_pMIDISeg);
		}
	}
	bExist=false;
	bMusic=false;
}


/**\brief
 * 创建主接口
 */
IDirectMusicPerformance* CMusic::CreatePerformance(void)
{
    IDirectMusicPerformance* pPerf;
 
    if (FAILED(CoCreateInstance(
            CLSID_DirectMusicPerformance,
            NULL,
            CLSCTX_INPROC, 
            IID_IDirectMusicPerformance,
            (void**)&pPerf
        )))
    {
        pPerf = NULL;
    }
    return pPerf;
}


/**\brief
 * 创建LOADER
 */
IDirectMusicLoader* CMusic::CreateLoader(void)
{
    IDirectMusicLoader* pLoader;
 
    if (FAILED(CoCreateInstance(
            CLSID_DirectMusicLoader,
            NULL,
            CLSCTX_INPROC, 
            IID_IDirectMusicLoader,
            (void**)&pLoader
        )))
    {
        pLoader = NULL;
    }
    return pLoader;
}


/**\brief
 * 读入一段音乐
 */
IDirectMusicSegment* CMusic::LoadMIDISegment(IDirectMusicLoader* pLoader, WCHAR *wszMidiFileName )
{
	if( !bMusic )return NULL;		//没有音乐

	//检测文件是否存在
	FILE *fp;
	char* str = Char(wszMidiFileName);
	fp=fopen(str , "rb");
	_DELETE( str );
	if( fp==NULL )
	{
		ShowMessage("Midi file %s not found!",Char(wszMidiFileName));
		return NULL;
	}
	fclose(fp);

    DMUS_OBJECTDESC ObjDesc; 
    IDirectMusicSegment* pSegment = NULL;

    ObjDesc.guidClass = CLSID_DirectMusicSegment;
    ObjDesc.dwSize = sizeof(DMUS_OBJECTDESC);
    wcscpy(ObjDesc.wszFileName, wszMidiFileName );
    ObjDesc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;
 
    pLoader->GetObject(&ObjDesc,
            IID_IDirectMusicSegment, (void**) &pSegment);		//得到object
 
    pSegment->SetParam(GUID_StandardMIDIFile,
            -1, 0, 0, (void*)g_pPerf);							//设置乐队的音轨
 
    pSegment->SetParam(GUID_Download, -1, 0, 0, (void*)g_pPerf);//允许自动下载
 
    return pSegment;
}


/**\brief
 * 初始化
 */
BOOL CMusic::InitMusic()
{	
	if(bExist)return true;		//主缓冲存在

    if (SUCCEEDED(CoInitialize(NULL)))		//初始化COM
    {
		if( !bExist )						//主缓冲不存在
		{
			bExist=true;					
			g_pPerf = CreatePerformance();	//创建主接口
			if (g_pPerf == NULL)
			{
				MessageBox(NULL,"CreatePerformance\n初始化DirectMusic过程出错！\
					请检查你是否正确的安装了DirectX 7.0以上版本，或有无其它程序正\
					占用声卡？否则你将无法听到背景音乐。","ERROR",MB_OK);
				bMusic=false;
				return false;
			}	

			if (FAILED(g_pPerf->Init(NULL, NULL, NULL)))
			{
				MessageBox(NULL,"Init\nDirectMusic 初始化出错！请安装DirectX 7.0以\
					上版本。或有无其它程序正占用声卡？否则你将无法听到背景音乐。",
					"ERROR",MB_OK);
				bMusic=false;
				return false;
			}	
		
			if (FAILED(g_pPerf->AddPort(NULL)))		//添加一个port
			{	
				MessageBox(NULL,"AddPort\nDirectMusic 初始化出错！请安装DirectX 7.0以\
					上版本。或有无其它程序正占用声卡？否则你将无法听到背景音乐。",
					"ERROR",MB_OK);
				bMusic=false;
				return false;
			}

			g_pLoader = CreateLoader();				//创建一个loader
			if (g_pLoader == NULL)
			{
				MessageBox(NULL,"CreatLoader\nDirectMusic 初始化出错！\
					请安装DirectX 7.0以上版本。或有无其它程序正占用声卡？否则你将无法\
					听到背景音乐。","ERROR",MB_OK);
				bMusic=false;
				return false;
			}
		}
		bMusic=true;
		return true;
	}
	bMusic=false;
	return false;
}


/**\brief
 * 读入
 */
BOOL CMusic::LoadMusic(WCHAR *filename)
{
	if( !bMusic )return true;		//没有音乐

	wcscpy( MusicFile, filename );		
	if (g_pLoader)
	{
		_RELEASE(g_pMIDISeg);

		g_pMIDISeg = LoadMIDISegment(g_pLoader,filename);	//加载一个段
		return true;
	}
	return false;
}

/**\brief
 * 播放
 */
BOOL CMusic::Play(int start)
{
	if( !bMusic )return true;

	IDirectMusicSegmentState* g_pSegState;		//当前段状态
	if (g_pMIDISeg)
	{
		//播放一个段，并跟踪状态
		g_pPerf->PlaySegment(g_pMIDISeg, DMUS_SEGF_REFTIME , start, &g_pSegState);	
	}
	bPlay=true;
	return true;
}


/**\brief
 * 是否正在播放
 */
BOOL CMusic::IsPlay()
{
	if( !bMusic )return true;	//没有音乐

	if (g_pMIDISeg)
	{
		if( (g_pPerf->IsPlaying(g_pMIDISeg,NULL))==S_OK )
			return true;
	}
	return false;
}


/**\brief
 * 停止播放
 */
BOOL CMusic::Stop()
{
	if( !bMusic )return true;

	if (g_pMIDISeg)
	{
		if( (g_pPerf->Stop(g_pMIDISeg, NULL, 0, 0))==S_OK )
		{
			bPlay=false;
			return true;
		}
	}
	return false;
}


/**\brief
 * 暂停播放
 */
BOOL CMusic::Pause()
{
	if( !bMusic )return true;

	if (g_pMIDISeg)
	{
		g_pPerf->GetTime(NULL, &PauseTime);		//播放到了什么位置

		if( (g_pPerf->Stop(g_pMIDISeg, NULL, 0, 0))==S_OK )	//停止
		{
			bPlay=false;
			return true;
		}
		return false;
	}
	return false;
}


/**\brief
 * 暂停后用来继续播放
 */
BOOL CMusic::Resume()
{
	if( !bMusic ) return true;

	if (g_pMIDISeg)
	{
		g_pMIDISeg->SetStartPoint(PauseTime);	//开始的位置
		IDirectMusicSegmentState* g_pSegState;	//跟踪
		g_pPerf->PlaySegment(g_pMIDISeg,DMUS_SEGF_REFTIME,0,&g_pSegState);
		g_pMIDISeg->SetStartPoint(0);					
		bPlay=true;
		return true;
	}
	return false;
}
