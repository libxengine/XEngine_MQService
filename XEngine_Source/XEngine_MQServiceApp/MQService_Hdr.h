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
#include <string>
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
#include <XEngine_Include/XEngine_RfcComponents/HttpServer_Define.h>
#include <XEngine_Include/XEngine_RfcComponents/HttpServer_Error.h>
#include <XEngine_Include/XEngine_RfcComponents/WSProtocol_Define.h>
#include <XEngine_Include/XEngine_RfcComponents/WSProtocol_Error.h>
#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif
#include "../XQueue_ProtocolHdr.h"
#include "../MQCore_ConfigModule/Config_Define.h"
#include "../MQCore_ConfigModule/Config_Error.h"
#include "../MQCore_XMQModule/XMQModule_Define.h"
#include "../MQCore_XMQModule/XMQModule_Error.h"
#include "../MQCore_DDSMessage/DDSMessage_Define.h"
#include "../MQCore_DDSMessage/DDSMessage_Error.h"
#include "../MQCore_ProtocolModule/Protocol_Define.h"
#include "../MQCore_ProtocolModule/Protocol_Error.h"
#include "../MQCore_SessionModule/Session_Define.h"
#include "../MQCore_SessionModule/Session_Error.h"

extern BOOL bIsRun;
extern XLOG xhLog;
extern XNETHANDLE xhTCPSocket;
extern XNETHANDLE xhHTTPSocket;
extern XNETHANDLE xhWSSocket;

extern XNETHANDLE xhTCPHeart;
extern XNETHANDLE xhWSHeart;

extern XHANDLE xhTCPPacket;
extern XHANDLE xhHTTPPacket;
extern XHANDLE xhWSPacket;

extern XNETHANDLE xhTCPPool;
extern XNETHANDLE xhHttpPool;
extern XNETHANDLE xhWSPool;

extern SOCKET hSDSocket;
extern SOCKET hRVSocket;

extern XENGINE_SERVERCONFIG st_ServiceCfg;

extern shared_ptr<std::thread> pSTDThread;

void ServiceApp_Stop(int signo);

#include "MQService_Config.h"
#include "MQService_Net.h"
#include "MQService_TCPTask.h"
#include "MQService_HttpTask.h"
#include "MQService_WSTask.h"
#include "MQService_DDSTask.h"

#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib,"../Debug/MQCore_ConfigModule.lib")
#pragma comment(lib,"../Debug/MQCore_ProtocolModule.lib")
#pragma comment(lib,"../Debug/MQCore_DDSMessage.lib")
#pragma comment(lib,"../Debug/MQCore_XMQModule.lib")
#pragma comment(lib,"../Debug/MQCore_SessionModule.lib")
#else
#ifdef WIN64
#pragma comment(lib,"../x64/Release/MQCore_ConfigModule.lib")
#pragma comment(lib,"../x64/Release/MQCore_ProtocolModule.lib")
#pragma comment(lib,"../x64/Release/MQCore_DDSMessage.lib")
#pragma comment(lib,"../x64/Release/MQCore_XMQModule.lib")
#pragma comment(lib,"../x64/Release/MQCore_SessionModule.lib")
#else
#pragma comment(lib,"../Release/MQCore_ConfigModule.lib")
#pragma comment(lib,"../Release/MQCore_ProtocolModule.lib")
#pragma comment(lib,"../Release/MQCore_DDSMessage.lib")
#pragma comment(lib,"../Release/MQCore_XMQModule.lib")
#pragma comment(lib,"../Release/MQCore_SessionModule.lib")
#endif
#endif
#pragma comment(lib,"XEngine_BaseLib/XEngine_BaseLib.lib")
#pragma comment(lib,"XEngine_Core/XEngine_Core.lib")
#pragma comment(lib,"XEngine_Core/XEngine_ManagePool.lib")
#pragma comment(lib,"XEngine_HelpComponents/HelpComponents_XLog.lib")
#pragma comment(lib,"XEngine_HelpComponents/HelpComponents_Packets.lib")
#pragma comment(lib,"XEngine_RfcComponents/RfcComponents_HttpServer.lib")
#pragma comment(lib,"XEngine_RfcComponents/RfcComponents_WSProtocol.lib")
#pragma comment(lib,"Ws2_32.lib")
#endif