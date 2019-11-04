
#include "stdafx.h"
#include <dshow.h>

int main()
{
    IGraphBuilder *pGraph;
    IMediaControl *pMediaControl;
	IMediaEvent   *pEvent;

    CoInitialize(NULL);
    
    // 建立Media物件
    CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                        IID_IGraphBuilder, (void **)&pGraph);
    pGraph->QueryInterface(IID_IMediaControl, (void **)&pMediaControl);
	pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
  
    // 讀取"test.avi"檔案
    pGraph->RenderFile(L"test.avi", NULL);

    // 繪製動畫
    pMediaControl->Run();

    // 等待動畫播放結束
    long evCode;
    pEvent->WaitForCompletion(INFINITE, &evCode);

    // 清除物件
	pEvent->Release();
    pMediaControl->Release();
    pGraph->Release();
	
    CoUninitialize();
	return 0;
}

