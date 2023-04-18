#include "MQService_Hdr.h"

bool bIsRun = false;
XHANDLE xhLog = NULL;
XHANDLE xhTCPSocket = NULL;
XHANDLE xhHTTPSocket = NULL;
XHANDLE xhWSSocket = NULL;

XHANDLE xhTCPPacket = NULL;
XHANDLE xhHTTPPacket = NULL;
XHANDLE xhWSPacket = NULL;

XHANDLE xhTCPPool = NULL;
XHANDLE xhHttpPool = NULL;
XHANDLE xhWSPool = NULL;

XENGINE_SERVERCONFIG st_ServiceCfg;

void ServiceApp_Stop(int signo)
{
	if (bIsRun)
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("服务器退出..."));
		bIsRun = false;

		HelpComponents_Datas_Destory(xhTCPPacket);
		HttpProtocol_Server_DestroyEx(xhHTTPPacket);
		RfcComponents_WSPacket_DestoryEx(xhWSPacket);

		NetCore_TCPXCore_DestroyEx(xhTCPSocket);
		NetCore_TCPXCore_DestroyEx(xhHTTPSocket);
		NetCore_TCPXCore_DestroyEx(xhWSSocket);

		ManagePool_Thread_NQDestroy(xhTCPPool);
		ManagePool_Thread_NQDestroy(xhHttpPool);
		ManagePool_Thread_NQDestroy(xhWSPool);

		DBModule_MQData_Destory();
		DBModule_MQUser_Destory();

		SessionModule_Client_Destory();
		HelpComponents_XLog_Destroy(xhLog);
	}
#ifdef _WINDOWS
	WSACleanup();
#endif
	exit(0);
}

static int ServiceApp_Deamon(int wait)
{
#ifndef _MSC_BUILD
	pid_t pid = 0;
	int status;
	pid = fork();
	if (pid > 0)
	{
		exit(0);
	}

	close(2);
	while (1)
	{

		pid = fork();
		if (pid < 0)
			exit(1);
		if (pid == 0)
		{
			return 0;
		}
		waitpid(pid, &status, 0);

		if (wait > 0)
			sleep(wait);
	}
#endif
	return 0;
}

int main(int argc, char** argv)
{
#ifdef _WINDOWS
	WSADATA st_WSAData;
	WSAStartup(MAKEWORD(2, 2), &st_WSAData);
#endif
	bIsRun = true;
	LPCXSTR lpszHTTPMime = _T("./XEngine_Config/HttpMime.types");
	LPCXSTR lpszHTTPCode = _T("./XEngine_Config/HttpCode.types");
	LPCXSTR lpszLogFile = _T("./XEngine_Log/XEngine_MQServiceApp.Log");
	TCHAR tszStringMsg[2048];
	HELPCOMPONENTS_XLOG_CONFIGURE st_XLogConfig;
	THREADPOOL_PARAMENT** ppSt_ListTCPParam;
	THREADPOOL_PARAMENT** ppSt_ListHTTPParam;
	THREADPOOL_PARAMENT** ppSt_ListWSParam;

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
		ServiceApp_Deamon(1);
	}

	if (!DBModule_MQData_Init((DATABASE_MYSQL_CONNECTINFO*)&st_ServiceCfg.st_XSql))
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("启动服务中，初始化消息数据数据库失败，错误：%lX"), DBModule_GetLastError());
		goto NETSERVICEEXIT;
	}
	if (!DBModule_MQUser_Init((DATABASE_MYSQL_CONNECTINFO *)&st_ServiceCfg.st_XSql, MessageQueue_CBTask_TimePublish))
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("启动服务中，初始化消息用户数据库失败，错误：%lX"), DBModule_GetLastError());
		goto NETSERVICEEXIT;
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，初始化数据库服务成功"));

	if (!SessionModule_Client_Init(st_ServiceCfg.st_XTime.nSessionTime, MessageQueue_Callback_Timeout))
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("初始化客户端会话管理器失败，错误：%lX"), SessionModule_GetLastError());
		goto NETSERVICEEXIT;
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，初始化客户端会话管理器成功,会话超时时间:%d 秒"), st_ServiceCfg.st_XTime.nSessionTime);

	if (st_ServiceCfg.nTCPPort > 0)
	{
		//组包器
		xhTCPPacket = HelpComponents_Datas_Init(st_ServiceCfg.st_XMax.nMaxQueue, st_ServiceCfg.st_XMax.nTCPThread);
		if (NULL == xhTCPPacket)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("初始化TCP组包器失败，错误：%lX"), Packets_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，启动TCP组包器成功"));
		//启动网络
		xhTCPSocket = NetCore_TCPXCore_StartEx(st_ServiceCfg.nTCPPort, st_ServiceCfg.st_XMax.nMaxClient, st_ServiceCfg.st_XMax.nIOThread);
		if (NULL == xhTCPSocket)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("启动TCP网络服务器失败，错误：%lX"), NetCore_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，启动TCP网络服务器成功,TCP端口:%d,IO:%d"), st_ServiceCfg.nTCPPort, st_ServiceCfg.st_XMax.nIOThread);
		NetCore_TCPXCore_RegisterCallBackEx(xhTCPSocket, MessageQueue_Callback_TCPLogin, MessageQueue_Callback_TCPRecv, MessageQueue_Callback_TCPLeave);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，注册TCP网络事件成功"));
		//任务池
		BaseLib_OperatorMemory_Malloc((XPPPMEM)&ppSt_ListTCPParam, st_ServiceCfg.st_XMax.nTCPThread, sizeof(THREADPOOL_PARAMENT));
		for (int i = 0; i < st_ServiceCfg.st_XMax.nTCPThread; i++)
		{
			int* pInt_Pos = new int;

			*pInt_Pos = i;
			ppSt_ListTCPParam[i]->lParam = pInt_Pos;
			ppSt_ListTCPParam[i]->fpCall_ThreadsTask = MessageQueue_TCPThread;
		}
		xhTCPPool = ManagePool_Thread_NQCreate(&ppSt_ListTCPParam, st_ServiceCfg.st_XMax.nTCPThread);
		if (NULL == xhTCPPool)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("启动TCP线程池服务失败，错误：%lX"), ManagePool_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，启动TCP线程池服务成功,启动个数:%d"), st_ServiceCfg.st_XMax.nTCPThread);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("启动服务中，TCP消息服务没有被启用"));
	}

	if (st_ServiceCfg.nHttpPort > 0)
	{
		xhHTTPPacket = HttpProtocol_Server_InitEx(lpszHTTPCode, lpszHTTPMime, st_ServiceCfg.st_XMax.nHttpThread);
		if (NULL == xhHTTPPacket)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("启动服务器中，初始化HTTP组包失败，错误：%lX"), HttpProtocol_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，初始化HTTP组包成功，IO线程个数:%d"), st_ServiceCfg.st_XMax.nHttpThread);

		xhHTTPSocket = NetCore_TCPXCore_StartEx(st_ServiceCfg.nHttpPort, st_ServiceCfg.st_XMax.nMaxClient, st_ServiceCfg.st_XMax.nIOThread);
		if (NULL == xhHTTPSocket)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("启动HTTP网络服务器失败，错误：%lX"), NetCore_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，启动HTTP网络服务器成功,HTTP端口:%d,IO:%d"), st_ServiceCfg.nHttpPort, st_ServiceCfg.st_XMax.nIOThread);
		NetCore_TCPXCore_RegisterCallBackEx(xhHTTPSocket, MessageQueue_Callback_HttpLogin, MessageQueue_Callback_HttpRecv, MessageQueue_Callback_HttpLeave);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，注册HTTP网络事件成功"));

		BaseLib_OperatorMemory_Malloc((XPPPMEM)&ppSt_ListHTTPParam, st_ServiceCfg.st_XMax.nHttpThread, sizeof(THREADPOOL_PARAMENT));
		for (int i = 0; i < st_ServiceCfg.st_XMax.nHttpThread; i++)
		{
			int* pInt_Pos = new int;

			*pInt_Pos = i;
			ppSt_ListHTTPParam[i]->lParam = pInt_Pos;
			ppSt_ListHTTPParam[i]->fpCall_ThreadsTask = MessageQueue_HttpThread;
		}
		xhHttpPool = ManagePool_Thread_NQCreate(&ppSt_ListHTTPParam, st_ServiceCfg.st_XMax.nHttpThread);
		if (NULL == xhHttpPool)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("启动HTTP线程池服务失败，错误：%lX"), ManagePool_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，启动HTTP线程池服务成功,启动个数:%d"), st_ServiceCfg.st_XMax.nHttpThread);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("启动服务中，HTTP消息服务没有被启用"));
	}

	if (st_ServiceCfg.nWSPort > 0)
	{
		xhWSPacket = RfcComponents_WSPacket_InitEx(st_ServiceCfg.st_XMax.nMaxClient, 0, st_ServiceCfg.st_XMax.nWSThread);
		if (NULL == xhWSPacket)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("启动服务器中，初始化Websocket组包失败，错误：%lX"), WSFrame_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，初始化Websocket组包成功，IO线程个数:%d"), st_ServiceCfg.st_XMax.nWSThread);

		xhWSSocket = NetCore_TCPXCore_StartEx(st_ServiceCfg.nWSPort, st_ServiceCfg.st_XMax.nMaxClient, st_ServiceCfg.st_XMax.nIOThread);
		if (NULL == xhWSSocket)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("启动Websocket网络服务器失败，错误：%lX"), NetCore_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，启动Websocket网络服务器成功,Websocket端口:%d,IO:%d"), st_ServiceCfg.nWSPort, st_ServiceCfg.st_XMax.nIOThread);
		NetCore_TCPXCore_RegisterCallBackEx(xhWSSocket, MessageQueue_Callback_WSLogin, MessageQueue_Callback_WSRecv, MessageQueue_Callback_WSLeave);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，注册Websocket网络事件成功"));

		BaseLib_OperatorMemory_Malloc((XPPPMEM)&ppSt_ListWSParam, st_ServiceCfg.st_XMax.nWSThread, sizeof(THREADPOOL_PARAMENT));
		for (int i = 0; i < st_ServiceCfg.st_XMax.nWSThread; i++)
		{
			int* pInt_Pos = new int;

			*pInt_Pos = i;
			ppSt_ListWSParam[i]->lParam = pInt_Pos;
			ppSt_ListWSParam[i]->fpCall_ThreadsTask = MessageQueue_WebsocketThread;
		}
		xhWSPool = ManagePool_Thread_NQCreate(&ppSt_ListWSParam, st_ServiceCfg.st_XMax.nWSThread);
		if (NULL == xhWSPool)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("启动Websocket线程池服务失败，错误：%lX"), ManagePool_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("启动服务中，启动Websocket线程池服务成功,启动个数:%d"), st_ServiceCfg.st_XMax.nWSThread);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("启动服务中，Websocket消息服务没有被启用"));
	}
	
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("所有服务成功启动，服务运行中，XEngine版本:%s,发行版本次数:%d,当前运行版本：%s。。。"), BaseLib_OperatorVer_XGetStr(), st_ServiceCfg.st_XVer.pStl_ListStorage->size(), st_ServiceCfg.st_XVer.pStl_ListStorage->front().c_str());

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
NETSERVICEEXIT:

	if (bIsRun)
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("有服务启动失败，服务器退出..."));
		bIsRun = false;

		HelpComponents_Datas_Destory(xhTCPPacket);
		HttpProtocol_Server_DestroyEx(xhHTTPPacket);
		RfcComponents_WSPacket_DestoryEx(xhWSPacket);

		NetCore_TCPXCore_DestroyEx(xhTCPSocket);
		NetCore_TCPXCore_DestroyEx(xhHTTPSocket);
		NetCore_TCPXCore_DestroyEx(xhWSSocket);
		
		ManagePool_Thread_NQDestroy(xhTCPPool);
		ManagePool_Thread_NQDestroy(xhHttpPool);
		ManagePool_Thread_NQDestroy(xhWSPool);
		
		DBModule_MQData_Destory();
		DBModule_MQUser_Destory();

		SessionModule_Client_Destory();
		HelpComponents_XLog_Destroy(xhLog);
	}
#ifdef _WINDOWS
	WSACleanup();
#endif
	return 0;
}
