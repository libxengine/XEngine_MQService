#pragma once
#ifdef _WINDOWS
#include <Windows.h>
#include <tchar.h>
#include <io.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <signal.h>
#include <errno.h>
#include <thread>
using namespace std;
#include <XEngine_Include/XEngine_CommHdr.h>
#include <XEngine_Include/XEngine_Types.h>
#include <XEngine_Include/XEngine_ProtocolHdr.h>
#include <XEngine_Include/XEngine_BaseLib/BaseLib_Define.h>
#include <XEngine_Include/XEngine_BaseLib/BaseLib_Error.h>
#include <XEngine_Include/XEngine_Core/NetCore_Define.h>
#include <XEngine_Include/XEngine_Core/NetCore_Error.h>
#include <XEngine_Include/XEngine_Core/ManagePool_Define.h>
#include <XEngine_Include/XEngine_Core/ManagePool_Error.h>
#include <XEngine_Include/XEngine_HelpComponents/XLog_Define.h>
#include <XEngine_Include/XEngine_HelpComponents/XLog_Error.h>
#include <XEngine_Include/XEngine_HelpComponents/Packets_Define.h>
#include <XEngine_Include/XEngine_HelpComponents/Packets_Error.h>
#include <XEngine_Include/XEngine_MQCore/XMQService_Define.h>
#include <XEngine_Include/XEngine_MQCore/XMQService_Error.h>

typedef struct tag_NetEngine_MQServiceCfg
{
    int nDeamon;
    int nThreadCount;
    struct
    {
        int nTCPPort;
        int nUDPPort;
    }st_XMQ;
}NETENGIEN_MQSERVICECFG;
typedef struct tag_NetEngine_MQThreadPool
{
    XENGINE_PROTOCOLHDR st_ProtocolHdr;
    TCHAR tszMsgBuffer[10240];
    TCHAR tszClientAddr[64];
    int nMsgLen;
    int nIPProto;
}NETENGIEN_MQTHREADPOOL;

extern BOOL bIsRun;
extern XLOG xhLog;
extern XNETHANDLE xhTCPSocket;
extern XNETHANDLE xhUDPSocket;
extern XNETHANDLE xhTCPPacket;
extern XNETHANDLE xhPool;
extern NETENGIEN_MQSERVICECFG st_ServiceCfg;

void ServiceApp_Stop(int signo);

#include "MQService_Config.h"
#include "MQService_Net.h"
#include "MQService_Task.h"


#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib,"x86/XEngine_BaseLib/XEngine_BaseLib.lib")
#pragma comment(lib,"x86/XEngine_Core/XEngine_Core.lib")
#pragma comment(lib,"x86/XEngine_Core/XEngine_ManagePool.lib")
#pragma comment(lib,"x86/XEngine_HelpComponents/HelpComponents_XLog.lib")
#pragma comment(lib,"x86/XEngine_HelpComponents/HelpComponents_Packets.lib")
#pragma comment(lib,"x86/XEngine_MQCore/MQCore_XMQService.lib")
#else
#pragma comment(lib,"x64/XEngine_BaseLib/XEngine_BaseLib.lib")
#pragma comment(lib,"x64/XEngine_Core/XEngine_Core.lib")
#pragma comment(lib,"x64/XEngine_Core/XEngine_ManagePool.lib")
#pragma comment(lib,"x64/XEngine_HelpComponents/HelpComponents_XLog.lib")
#pragma comment(lib,"x64/XEngine_HelpComponents/HelpComponents_Packets.lib")
#pragma comment(lib,"x64/XEngine_MQCore/MQCore_XMQService.lib")
#endif
#pragma comment(lib,"Ws2_32.lib")
#endif