#include "MQService_Hdr.h"

BOOL bIsRun = FALSE;
XLOG xhLog = NULL;
XNETHANDLE xhTCPSocket = 0;
XNETHANDLE xhUDPSocket = 0;
XNETHANDLE xhTCPPacket = 0;
XNETHANDLE xhPool = 0;
NETENGIEN_MQSERVICECFG st_ServiceCfg;

void ServiceApp_Stop(int signo)
{
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("服务器退出..."));

	bIsRun = FALSE;

	NetCore_TCPXCore_DestroyEx(xhTCPSocket);
	NetCore_UDPXCore_DestroyEx(xhUDPSocket);
	ManagePool_Thread_Destroy(xhPool);
	HelpComponents_Datas_Destory(xhTCPPacket);
	XMQService_Packet_Destory();
	HelpComponents_XLog_Destroy(xhLog);
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
	TCHAR tszStringMsg[2048];
	LPCTSTR lpszLogFile = _T("./MQService_Log/MQService.Log");
	HELPCOMPONENTS_XLOG_CONFIGURE st_XLogConfig;
	THREADPOOL_PARAMENT** ppSt_ListParam;

	memset(tszStringMsg, '\0', sizeof(tszStringMsg));
	memset(&st_XLogConfig, '\0', sizeof(HELPCOMPONENTS_XLOG_CONFIGURE));
	memset(&st_ServiceCfg, '\0', sizeof(NETENGIEN_MQSERVICECFG));

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

	if (st_ServiceCfg.nDeamon)
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("初始化守护进程..."));
	}

	if (!NetCore_TCPXCore_StartEx(&xhTCPSocket, st_ServiceCfg.st_XMQ.nTCPPort))
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("启动TCP网络服务器失败，错误：%lX"), NetCore_GetLastError());
		goto NETSERVICEEXIT;
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，启动TCP网络服务器成功,TCP端口:%d"), st_ServiceCfg.st_XMQ.nTCPPort);
	NetCore_TCPXCore_RegisterCallBackEx(xhTCPSocket, MessageQueue_Callback_TCPLogin, MessageQueue_Callback_TCPRecv, MessageQueue_Callback_TCPLeave);

	if (!NetCore_UDPXCore_StartEx(&xhUDPSocket, st_ServiceCfg.st_XMQ.nUDPPort))
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("启动UDP网络服务器失败，错误：%lX"), NetCore_GetLastError());
		goto NETSERVICEEXIT;
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，启动UDP网络服务器成功,UDP端口:%d"), st_ServiceCfg.st_XMQ.nUDPPort);
	NetCore_UDPXCore_RegisterCallBackEx(xhUDPSocket, MessageQueue_Callback_UDPRecv);

	if (!HelpComponents_Datas_Init(&xhTCPPacket, 100000, 0, st_ServiceCfg.nThreadCount))
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("初始化TCP组包器失败，错误：%lX"), Packets_GetLastError());
		goto NETSERVICEEXIT;
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，启动TCP组包器成功"));

	if (!XMQService_Packet_Init(10000))
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("初始化消息队列服务失败，错误：%lX"), XMQService_GetLastError());
		goto NETSERVICEEXIT;
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，初始化消息队列服务成功"));

	BaseLib_OperatorMemory_Malloc((XPPPMEM)&ppSt_ListParam, st_ServiceCfg.nThreadCount, sizeof(THREADPOOL_PARAMENT));
	for (int i = 0; i < st_ServiceCfg.nThreadCount; i++)
	{
		int* pInt_Pos = new int;

		*pInt_Pos = i + 1;
		ppSt_ListParam[i]->lParam = pInt_Pos;
		ppSt_ListParam[i]->fpCall_ThreadsTask = MessageQueue_TCPThread;
	}
	if (!ManagePool_Thread_NQCreate(&xhPool, &ppSt_ListParam, st_ServiceCfg.nThreadCount))
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("启动线程池服务失败，错误：%lX"), ManagePool_GetLastError());
		goto NETSERVICEEXIT;
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，启动线程池服务成功,启动个数:%d"), st_ServiceCfg.nThreadCount);

	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("所有服务成功启动，服务运行中。。。"));
	while (TRUE)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

NETSERVICEEXIT:
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("有服务启动失败，服务器退出..."));
	bIsRun = FALSE;

	NetCore_TCPXCore_DestroyEx(xhTCPSocket);
	NetCore_UDPXCore_DestroyEx(xhUDPSocket);
	ManagePool_Thread_Destroy(xhPool);
	HelpComponents_Datas_Destory(xhTCPPacket);
	XMQService_Packet_Destory();
	HelpComponents_XLog_Destroy(xhLog);
#ifdef _WINDOWS
	WSACleanup();
#endif
	return 0;
}
