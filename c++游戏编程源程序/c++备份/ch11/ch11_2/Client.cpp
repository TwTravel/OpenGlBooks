#define INITGUID
#define _WIN32_DCOM
#include <stdio.h>
#include <dplay8.h>



struct HOST_NODE
{
    DPN_APPLICATION_DESC*   pAppDesc;
    IDirectPlay8Address*    pHostAddress;
    WCHAR*                  pwszSessionName;

    HOST_NODE*              pNext;
};


IDirectPlay8Client*                 g_pDPClient         = NULL;
IDirectPlay8Address*                g_pDeviceAddress    = NULL;
IDirectPlay8Address*                g_pHostAddress      = NULL;
BOOL                                g_bHost;
HOST_NODE*                          g_pHostList         = NULL;
CRITICAL_SECTION                    g_csHostList;


GUID g_guidApp = { 0x1ad4ca3b, 0xac68, 0x4d9b, { 0x95, 0x22, 0xbe, 0x59, 0xcd, 0x48, 0x52, 0x76 } };


HRESULT WINAPI DirectPlayMessageHandler(PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer);
BOOL    IsServiceProviderValid(const GUID* pGuidSP);
HRESULT InitDirectPlay();
HRESULT CreateDeviceAddress();
HRESULT CreateHostAddress(WCHAR* pwszHost);
HRESULT EnumDirectPlayHosts();
HRESULT ConnectToSession();
HRESULT SendDirectPlayMessage();
void    CleanupDirectPlay();


#define SAFE_DELETE(p)          {if(p) {delete (p);     (p)=NULL;}}
#define SAFE_DELETE_ARRAY(p)    {if(p) {delete[] (p);   (p)=NULL;}}
#define SAFE_RELEASE(p)         {if(p) {(p)->Release(); (p)=NULL;}}

#define USER_HOST       1
#define USER_CONNECT    2
#define USER_EXIT       1
#define USER_SEND       2




int main(int argc, char* argv[], char* envp[])
{
    HRESULT                     hr;
    int                         iUserChoice;

    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    if( FAILED( hr = InitDirectPlay() ) )
    {
        printf("DirectPlay初始化失敗:  0x%X\n", hr);
        goto LCleanup;
    }

    InitializeCriticalSection(&g_csHostList);

    if( FAILED( hr = CreateDeviceAddress() ) )
    {
        printf("建立裝置位址失敗:  0x%X\n", hr);
        goto LCleanup;
    }

    if( FAILED( hr = EnumDirectPlayHosts() ) )
    {
        printf("列舉本機失敗:  0x%X\n", hr);
        goto LCleanup;
    }

    if( FAILED( hr = ConnectToSession() ) )
    {
        printf("建立連線失敗:  0x%X\n", hr);
        goto LCleanup;
    }
    else
    {
        printf("\n建立連線成功.\n");
    }

    do
    {
        printf("請選擇功能表.\n1.  離開\n2.  傳送訊息\n");
        scanf("%d", &iUserChoice);

        if( iUserChoice == USER_SEND )
        {
            if( FAILED( hr = SendDirectPlayMessage() ) )
            {
                printf("傳送訊息失敗:  0x%X\n", hr);
                goto LCleanup;
            }
        }
    } while (iUserChoice != USER_EXIT);

LCleanup:
    CleanupDirectPlay();

    CoUninitialize();

    return 0;
}

HRESULT InitDirectPlay()
{
    HRESULT     hr = S_OK;

    if( FAILED( hr = CoCreateInstance(CLSID_DirectPlay8Client, NULL, 
                                    CLSCTX_INPROC_SERVER,
                                    IID_IDirectPlay8Client, 
                                    (LPVOID*) &g_pDPClient ) ) )
    {
        printf("建立 IDirectPlay8Client 物件失敗:  0x%X\n", hr);
        goto LCleanup;
    }

    if( FAILED( hr = g_pDPClient->Initialize(NULL, DirectPlayMessageHandler, 0 ) ) )
    {
        printf("初始化DirectPlay失敗:  0x%X\n", hr);
        goto LCleanup;
    }
    
    if( FALSE == IsServiceProviderValid(&CLSID_DP8SP_TCPIP ) )
    {
        hr = E_FAIL;
        printf("建立 TCPIP 失敗");
        goto LCleanup;
    }

LCleanup:
    return hr;
}


BOOL IsServiceProviderValid(const GUID* pGuidSP)
{
    HRESULT                     hr;
    DPN_SERVICE_PROVIDER_INFO*  pdnSPInfo = NULL;
    DWORD                       dwItems = 0;
    DWORD                       dwSize = 0;

    hr = g_pDPClient->EnumServiceProviders(&CLSID_DP8SP_TCPIP, NULL, NULL, &dwSize, &dwItems, 0);

    if( hr != DPNERR_BUFFERTOOSMALL)
    {
        printf("列舉伺服器失敗:  0x%x\n", hr);
        goto LCleanup;
    }

    pdnSPInfo = (DPN_SERVICE_PROVIDER_INFO*) new BYTE[dwSize];

    if( FAILED( hr = g_pDPClient->EnumServiceProviders(&CLSID_DP8SP_TCPIP, NULL, pdnSPInfo, &dwSize, &dwItems, 0 ) ) )
    {
        printf("列舉TCP/IP失敗:  0x%x\n", hr);
        goto LCleanup;
    }

    if( dwItems == 0)
    {
        hr = E_FAIL;
    }

LCleanup:
    SAFE_DELETE_ARRAY(pdnSPInfo);
    if( SUCCEEDED(hr) )
        return TRUE;
    else
        return FALSE;
}



HRESULT WINAPI DirectPlayMessageHandler(PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer)
{
    HRESULT     hr = S_OK;

    switch (dwMessageId)
    {
        case DPN_MSGID_ENUM_HOSTS_RESPONSE:
        {
            PDPNMSG_ENUM_HOSTS_RESPONSE     pEnumHostsResponseMsg;
            const DPN_APPLICATION_DESC*     pAppDesc;
            HOST_NODE*                      pHostNode = NULL;
            WCHAR*                          pwszSession = NULL;

            pEnumHostsResponseMsg = (PDPNMSG_ENUM_HOSTS_RESPONSE) pMsgBuffer;
            pAppDesc = pEnumHostsResponseMsg->pApplicationDescription;

            EnterCriticalSection(&g_csHostList);

            for (pHostNode = g_pHostList; pHostNode; pHostNode = pHostNode->pNext)
            {
                if( pAppDesc->guidInstance == pHostNode->pAppDesc->guidInstance)
                {
                    pHostNode = NULL;
                    goto Break_ENUM_HOSTS_RESPONSE;
                }
            }

            pHostNode = new HOST_NODE;
            if( pHostNode == NULL)
            {
                goto Break_ENUM_HOSTS_RESPONSE;
            }

            ZeroMemory(pHostNode, sizeof(HOST_NODE));

            if( FAILED( pEnumHostsResponseMsg->pAddressSender->Duplicate(&pHostNode->pHostAddress ) ) )
            {
                goto Break_ENUM_HOSTS_RESPONSE;
            }

            pHostNode->pAppDesc = new DPN_APPLICATION_DESC;

            if( pHostNode == NULL)
            {
                goto Break_ENUM_HOSTS_RESPONSE;
            }

            ZeroMemory(pHostNode->pAppDesc, sizeof(DPN_APPLICATION_DESC));
            memcpy(pHostNode->pAppDesc, pAppDesc, sizeof(DPN_APPLICATION_DESC));

            pHostNode->pAppDesc->pwszSessionName = NULL;
            pHostNode->pAppDesc->pwszPassword = NULL;
            pHostNode->pAppDesc->pvReservedData = NULL;
            pHostNode->pAppDesc->dwReservedDataSize = 0;
            pHostNode->pAppDesc->pvApplicationReservedData = NULL;
            pHostNode->pAppDesc->dwApplicationReservedDataSize = 0;
            
            if( pAppDesc->pwszSessionName)
            {
                pwszSession = new WCHAR[wcslen(pAppDesc->pwszSessionName) + 1];
                
                if( pwszSession)
                {
                    wcscpy(pwszSession, pAppDesc->pwszSessionName);
                }
            }

            pHostNode->pwszSessionName = pwszSession;

            pHostNode->pNext = g_pHostList ? g_pHostList->pNext : NULL;
            g_pHostList = pHostNode;
            pHostNode = NULL;

Break_ENUM_HOSTS_RESPONSE:
            LeaveCriticalSection(&g_csHostList);

            if( pHostNode)
            {
                SAFE_RELEASE(pHostNode->pHostAddress);

                SAFE_DELETE(pHostNode->pAppDesc);

                delete pHostNode;
            }

            break;
        }
    
        case DPN_MSGID_TERMINATE_SESSION:
        {
            PDPNMSG_TERMINATE_SESSION   pTermSessionMsg;

            pTermSessionMsg = (PDPNMSG_TERMINATE_SESSION) pMsgBuffer;

            printf("\n伺服器離連了!\n");
            break;
        }
    
        case DPN_MSGID_RECEIVE:
        {
            PDPNMSG_RECEIVE     pMsg;

            pMsg = (PDPNMSG_RECEIVE) pMsgBuffer;

            printf("\n接收到的訊息:  %S\n", (WCHAR*)pMsg->pReceiveData);
            break;
        }
    }

    return hr;
}



HRESULT EnumDirectPlayHosts()
{
    HRESULT                 hr = S_OK;
    WCHAR                   wszHost[128];
    DPN_APPLICATION_DESC    dpAppDesc;
    WCHAR*                  pwszURL = NULL;

    printf("\n輸入遠端主機位址:\n");
    wscanf(L"%ls", wszHost);

    if( FAILED( hr = CreateHostAddress(wszHost ) ) )
    {
        printf("建立遠端主機失敗:  0x%X\n", hr);
        goto LCleanup;
    }


    ZeroMemory(&dpAppDesc, sizeof(DPN_APPLICATION_DESC));
    dpAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
    dpAppDesc.guidApplication = g_guidApp;

    if( FAILED( hr = g_pDPClient->EnumHosts(&dpAppDesc,     
                                        g_pHostAddress,     
                                        g_pDeviceAddress,   
                                        NULL, 0,            
                                        4,                  
                                        0,                  
                                        0,                  
                                        NULL,               
                                        NULL,               
                                        DPNENUMHOSTS_SYNC ) ) )
    {
        printf("列舉伺服器失敗:  0x%X\n", hr);
        goto LCleanup;
    }

LCleanup:
    return hr;
}


HRESULT CreateDeviceAddress()
{
    HRESULT         hr = S_OK;

    if( FAILED( hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL,
                                    CLSCTX_INPROC_SERVER,
                                    IID_IDirectPlay8Address,
                                    (LPVOID*) &g_pDeviceAddress ) ) )
    {
        printf("建立 IDirectPlay8Address 物件失敗:  0x%X\n", hr);
        goto LCleanup;
    }
    
    if( FAILED( hr = g_pDeviceAddress->SetSP(&CLSID_DP8SP_TCPIP ) ) )
    {
        printf("列舉TCP/IP失敗:  0x%X\n", hr);
        goto LCleanup;
    }

LCleanup:
    return hr;
}


HRESULT CreateHostAddress(WCHAR* pwszHost)
{
    HRESULT         hr = S_OK;

    if( FAILED( hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL,
                                    CLSCTX_INPROC_SERVER,
                                    IID_IDirectPlay8Address,
                                    (LPVOID*) &g_pHostAddress ) ) )
    {
        printf("建立 IDirectPlay8Address 物件失敗:  0x%X\n", hr);
        goto LCleanup;
    }
    
    if( FAILED( hr = g_pHostAddress->SetSP(&CLSID_DP8SP_TCPIP ) ) )
    {
        printf("設定TCP/IP失敗:  0x%X\n", hr);
        goto LCleanup;
    }

    if( FAILED( hr = g_pHostAddress->AddComponent(DPNA_KEY_HOSTNAME, pwszHost,
                                                    2*(wcslen(pwszHost) + 1), /*bytes*/
                                                    DPNA_DATATYPE_STRING ) ) )
    {
        printf("設定本機資訊失敗:  0x%X\n", hr);
        goto LCleanup;
    }

LCleanup:
    return hr;
}


HRESULT ConnectToSession()
{
    HRESULT                     hr = E_FAIL;
    DPN_APPLICATION_DESC        dpnAppDesc;
    IDirectPlay8Address*        pHostAddress = NULL;

    ZeroMemory(&dpnAppDesc, sizeof(DPN_APPLICATION_DESC));
    dpnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
    dpnAppDesc.guidApplication = g_guidApp;

    EnterCriticalSection(&g_csHostList);

    if( g_pHostList && SUCCEEDED(hr = g_pHostList->pHostAddress->Duplicate(&pHostAddress ) ) )
    {
        hr = g_pDPClient->Connect(&dpnAppDesc,  
                            pHostAddress,       
                            g_pDeviceAddress,   
                            NULL,               
                            NULL,               
                            NULL, 0,            
                            NULL,               
                            NULL,               
                            DPNCONNECT_SYNC);   

        if( FAILED( hr))
            printf("連線失敗:  0x%x\n", hr);
    }
    else
    {
        printf("本機位址失敗:  0x%x\n", hr);
    }

    LeaveCriticalSection(&g_csHostList);

    SAFE_RELEASE(pHostAddress);
    return hr;
}


HRESULT SendDirectPlayMessage()
{
    HRESULT         hr = S_OK;
    DPN_BUFFER_DESC dpnBuffer;
    WCHAR           wszData[256];

    printf("\n請輸入字串.\n");
    wscanf(L"%ls", wszData);

    dpnBuffer.pBufferData = (BYTE*) wszData;
    dpnBuffer.dwBufferSize = 2 * (wcslen(wszData) + 1);

    if( FAILED( hr = g_pDPClient->Send(&dpnBuffer,             
                                    1,                         
                                    0,                         
                                    NULL,                      
                                    NULL,                      
                                    DPNSEND_SYNC |
                                    DPNSEND_NOLOOPBACK ) ) )   
    {
        printf("傳送字串失敗:  0x%x\n", hr);
    }
    return hr;
}



void CleanupDirectPlay()
{
    HOST_NODE*                  pHostNode = NULL;
    HOST_NODE*                  pHostNodetmp = NULL;

    if( g_pDPClient)
        g_pDPClient->Close(0);

    EnterCriticalSection(&g_csHostList);
    
    pHostNode = g_pHostList;
    while( pHostNode != NULL )
    {       
        SAFE_RELEASE(pHostNode->pHostAddress);
        SAFE_DELETE(pHostNode->pAppDesc);
        SAFE_DELETE(pHostNode->pwszSessionName);

        pHostNodetmp = pHostNode;
        pHostNode    = pHostNode->pNext;
        SAFE_DELETE(pHostNodetmp);
    }

    LeaveCriticalSection(&g_csHostList);

    SAFE_RELEASE(g_pDeviceAddress);
    SAFE_RELEASE(g_pHostAddress);
    SAFE_RELEASE(g_pDPClient);

    DeleteCriticalSection(&g_csHostList);
}







