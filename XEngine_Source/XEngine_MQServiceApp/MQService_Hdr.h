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
#include <list>
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
#include "../MQCore_ConfigModule/Config_Define.h"
#include "../MQCore_ConfigModule/Config_Error.h"
#include "../MQCore_XMQModule/XMQModule_Define.h"
#include "../MQCore_XMQModule/XMQModule_Error.h"
#include "../MQCore_ProtocolModule/Protocol_Define.h"
#include "../MQCore_ProtocolModule/Protocol_Error.h"
#include "../MQCore_SessionModule/Session_Define.h"
#include "../MQCore_SessionModule/Session_Error.h"

#define XENGINE_MQAPP_NETTYPE_TCP 1
#define XENGINE_MQAPP_NETTYPE_HTTP 2

extern BOOL bIsRun;
extern XLOG xhLog;
extern XNETHANDLE xhTCPSocket;
extern XNETHANDLE xhTCPPacket;
extern XNETHANDLE xhTCPHeart;
extern XNETHANDLE xhPool;
extern XENGINE_SERVERCONFIG st_ServiceCfg;

void ServiceApp_Stop(int signo);

#include "MQService_Config.h"
#include "MQService_Net.h"
#include "MQService_TCPTask.h"


#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib,"../Debug/MQCore_ConfigModule.lib")
#pragma comment(lib,"../Debug/MQCore_ProtocolModule.lib")
#pragma comment(lib,"../Debug/MQCore_XMQModule.lib")
#pragma comment(lib,"../Debug/MQCore_SessionModule.lib")
#pragma comment(lib,"x86/XEngine_BaseLib/XEngine_BaseLib.lib")
#pragma comment(lib,"x86/XEngine_Core/XEngine_Core.lib")
#pragma comment(lib,"x86/XEngine_Core/XEngine_ManagePool.lib")
#pragma comment(lib,"x86/XEngine_HelpComponents/HelpComponents_XLog.lib")
#pragma comment(lib,"x86/XEngine_HelpComponents/HelpComponents_Packets.lib")
#else
#pragma comment(lib,"../x64/Release/MQCore_ConfigModule.lib")
#pragma comment(lib,"../x64/Release/MQCore_ProtocolModule.lib")
#pragma comment(lib,"../x64/Release/MQCore_XMQModule.lib")
#pragma comment(lib,"../x64/Release/MQCore_SessionModule.lib")
#pragma comment(lib,"x64/XEngine_BaseLib/XEngine_BaseLib.lib")
#pragma comment(lib,"x64/XEngine_Core/XEngine_Core.lib")
#pragma comment(lib,"x64/XEngine_Core/XEngine_ManagePool.lib")
#pragma comment(lib,"x64/XEngine_HelpComponents/HelpComponents_XLog.lib")
#pragma comment(lib,"x64/XEngine_HelpComponents/HelpComponents_Packets.lib")
#endif
#pragma comment(lib,"Ws2_32.lib")
#endif