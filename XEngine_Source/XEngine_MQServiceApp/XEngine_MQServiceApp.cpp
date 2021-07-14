#include "MQService_Hdr.h"

BOOL bIsRun = FALSE;
XLOG xhLog = NULL;
XNETHANDLE xhTCPSocket = 0;
XNETHANDLE xhTCPPacket = 0;
XNETHANDLE xhTCPHeart = 0;
XNETHANDLE xhHTTPSocket = 0;
XHANDLE xhHTTPPacket = 0;
XNETHANDLE xhTCPPool = 0;
XNETHANDLE xhHttpPool = 0;
XENGINE_SERVERCONFIG st_ServiceCfg;

void ServiceApp_Stop(int signo)
{
	if (bIsRun)
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("服务器退出..."));
		bIsRun = FALSE;

		HelpComponents_Datas_Destory(xhTCPPacket);
		RfcComponents_HttpServer_DestroyEx(xhHTTPPacket);
		NetCore_TCPXCore_DestroyEx(xhTCPSocket);
		NetCore_TCPXCore_DestroyEx(xhHTTPSocket);
		SocketOpt_HeartBeat_DestoryEx(xhTCPHeart);
		ManagePool_Thread_NQDestroy(xhTCPPool);
		ManagePool_Thread_NQDestroy(xhHttpPool);
		XMQModule_Packet_Destory();
		SessionModule_Client_Destory();
		HelpComponents_XLog_Destroy(xhLog);
	}
#ifdef _WINDOWS
	WSACleanup();
#endif
	exit(0);
}

int main(int argc, char** argv)
{
#ifdef _WINDOWS
	WSADATA st_WSAData;
	WSAStartup(MAKEWORD(2, 2), &st_WSAData);
#endif
	bIsRun = TRUE;
	LPCTSTR lpszHTTPMime = _T("./XEngine_Config/HttpMime.types");
	LPCTSTR lpszHTTPCode = _T("./XEngine_Config/HttpCode.types");
	TCHAR tszStringMsg[2048];
	LPCTSTR lpszLogFile = _T("./XEngine_Log/XEngine_MQServiceApp.Log");
	HELPCOMPONENTS_XLOG_CONFIGURE st_XLogConfig;
	THREADPOOL_PARAMENT** ppSt_ListTCPParam;
	THREADPOOL_PARAMENT** ppSt_ListHTTPParam;

	memset(tszStringMsg, '\0', sizeof(tszStringMsg));
	memset(&st_XLogConfig, '\0', sizeof(HELPCOMPONENTS_XLOG_CONFIGURE));
	memset(&st_ServiceCfg, '\0', sizeof(XENGINE_SERVERCONFIG));

	st_XLogConfig.XLog_MaxBackupFile = 10;
	st_XLogConfig.XLog_MaxSize = 1024000;
	_tcscpy(st_XLogConfig.tszFileName, lpszLogFile);

	signal(SIGINT, ServiceApp_Stop);
	signal(SIGTERM, ServiceApp_Stop);
	signal(SIGABRT, ServiceApp_Stop);

	if (!MQ_Service_Parament(argc, argv, &st_ServiceCfg))
	{
		return -1;
	}
	xhLog = HelpComponents_XLog_Init(HELPCOMPONENTS_XLOG_OUTTYPE_STD | HELPCOMPONENTS_XLOG_OUTTYPE_FILE, &st_XLogConfig);
	if (NULL == xhLog)
	{
		printf("启动服务器失败，启动日志失败，错误：%lX", XLog_GetLastError());
		goto NETSERVICEEXIT;
	}
	HelpComponents_XLog_SetLogPriority(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO);

	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，初始化日志系统成功"));

	if (st_ServiceCfg.bDeamon)
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("初始化守护进程..."));
	}
	
	if (!HelpComponents_Datas_Init(&xhTCPPacket, st_ServiceCfg.st_XMax.nMaxQueue, 0, st_ServiceCfg.st_XMax.nTCPThread))
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("初始化TCP组包器失败，错误：%lX"), Packets_GetLastError());
		goto NETSERVICEEXIT;
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，启动TCP组包器成功"));

	xhHTTPPacket = RfcComponents_HttpServer_InitEx(lpszHTTPCode, lpszHTTPMime, st_ServiceCfg.st_XMax.nHttpThread);
	if (NULL == xhHTTPPacket)
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("启动服务器中，初始化HTTP服务失败，错误：%lX"), HttpServer_GetLastError());
		goto NETSERVICEEXIT;
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，初始化HTTP服务成功，IO线程个数:%d"), st_ServiceCfg.st_XMax.nHttpThread);
	
	if (!SessionModule_Client_Init())
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("初始化客户端会话管理器失败，错误：%lX"), SessionModule_GetLastError());
		goto NETSERVICEEXIT;
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，初始化客户端会话管理器成功"));

	if (!XMQModule_Packet_Init(st_ServiceCfg.tszTopic, st_ServiceCfg.st_XMax.nMaxQueue))
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("初始化消息队列服务失败，错误：%lX"), XMQModule_GetLastError());
		goto NETSERVICEEXIT;
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，初始化消息队列服务成功"));

	if (st_ServiceCfg.st_XTime.bHBTime)
	{
		if (!SocketOpt_HeartBeat_InitEx(&xhTCPHeart, st_ServiceCfg.st_XTime.nTCPTimeOut, st_ServiceCfg.st_XTime.nTimeCheck, MessageQueue_Callback_TCPHeart))
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("初始化TCP心跳服务失败，错误：%lX"), XMQModule_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，初始化TCP心跳服务成功,句柄:%llu,时间:%d,次数:%d"), xhTCPHeart, st_ServiceCfg.st_XTime.nTCPTimeOut, st_ServiceCfg.st_XTime.nTimeCheck);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("启动服务中，TCP心跳服务被设置为不启用"));
	}

	if (!NetCore_TCPXCore_StartEx(&xhTCPSocket, st_ServiceCfg.nTCPPort, st_ServiceCfg.st_XMax.nMaxClient, st_ServiceCfg.st_XMax.nIOThread))
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("启动TCP网络服务器失败，错误：%lX"), NetCore_GetLastError());
		goto NETSERVICEEXIT;
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，启动TCP网络服务器成功,TCP端口:%d,IO:%d"), st_ServiceCfg.nTCPPort, st_ServiceCfg.st_XMax.nIOThread);
	NetCore_TCPXCore_RegisterCallBackEx(xhTCPSocket, MessageQueue_Callback_TCPLogin, MessageQueue_Callback_TCPRecv, MessageQueue_Callback_TCPLeave);
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，注册TCP网络事件成功"));

	if (!NetCore_TCPXCore_StartEx(&xhHTTPSocket, st_ServiceCfg.nHttpPort, st_ServiceCfg.st_XMax.nMaxClient, st_ServiceCfg.st_XMax.nIOThread))
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("启动HTTP网络服务器失败，错误：%lX"), NetCore_GetLastError());
		goto NETSERVICEEXIT;
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，启动HTTP网络服务器成功,HTTP端口:%d,IO:%d"), st_ServiceCfg.nHttpPort, st_ServiceCfg.st_XMax.nIOThread);
	NetCore_TCPXCore_RegisterCallBackEx(xhHTTPSocket, MessageQueue_Callback_HttpLogin, MessageQueue_Callback_HttpRecv, MessageQueue_Callback_HttpLeave);
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，注册HTTP网络事件成功"));

	BaseLib_OperatorMemory_Malloc((XPPPMEM)&ppSt_ListTCPParam, st_ServiceCfg.st_XMax.nTCPThread, sizeof(THREADPOOL_PARAMENT));
	for (int i = 0; i < st_ServiceCfg.st_XMax.nTCPThread; i++)
	{
		int* pInt_Pos = new int;

		*pInt_Pos = i;
		ppSt_ListTCPParam[i]->lParam = pInt_Pos;
		ppSt_ListTCPParam[i]->fpCall_ThreadsTask = MessageQueue_TCPThread;
	}
	if (!ManagePool_Thread_NQCreate(&xhTCPPool, &ppSt_ListTCPParam, st_ServiceCfg.st_XMax.nTCPThread))
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("启动TCP线程池服务失败，错误：%lX"), ManagePool_GetLastError());
		goto NETSERVICEEXIT;
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，启动TCP线程池服务成功,启动个数:%d"), st_ServiceCfg.st_XMax.nTCPThread);

	BaseLib_OperatorMemory_Malloc((XPPPMEM)&ppSt_ListHTTPParam, st_ServiceCfg.st_XMax.nHttpThread, sizeof(THREADPOOL_PARAMENT));
	for (int i = 0; i < st_ServiceCfg.st_XMax.nHttpThread; i++)
	{
		int* pInt_Pos = new int;

		*pInt_Pos = i;
		ppSt_ListHTTPParam[i]->lParam = pInt_Pos;
		ppSt_ListHTTPParam[i]->fpCall_ThreadsTask = MessageQueue_HttpThread;
	}
	if (!ManagePool_Thread_NQCreate(&xhHttpPool, &ppSt_ListHTTPParam, st_ServiceCfg.st_XMax.nHttpThread))
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("启动HTTP线程池服务失败，错误：%lX"), ManagePool_GetLastError());
		goto NETSERVICEEXIT;
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，启动HTTP线程池服务成功,启动个数:%d"), st_ServiceCfg.st_XMax.nHttpThread);
	
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("所有服务成功启动，服务运行中。。。"));
	while (bIsRun)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
NETSERVICEEXIT:

	if (bIsRun)
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("有服务启动失败，服务器退出..."));
		bIsRun = FALSE;

		HelpComponents_Datas_Destory(xhTCPPacket);
		RfcComponents_HttpServer_DestroyEx(xhHTTPPacket);
		NetCore_TCPXCore_DestroyEx(xhTCPSocket);
		NetCore_TCPXCore_DestroyEx(xhHTTPSocket);
		SocketOpt_HeartBeat_DestoryEx(xhTCPHeart);
		ManagePool_Thread_NQDestroy(xhTCPPool);
		ManagePool_Thread_NQDestroy(xhHttpPool);
		XMQModule_Packet_Destory();
		SessionModule_Client_Destory();
		HelpComponents_XLog_Destroy(xhLog);
	}
#ifdef _WINDOWS
	WSACleanup();
#endif
	return 0;
}
