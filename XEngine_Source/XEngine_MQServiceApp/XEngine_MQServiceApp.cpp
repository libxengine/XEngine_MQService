#include "MQService_Hdr.h"

bool bIsRun = true;
bool bIsTest = false;
XHANDLE xhLog = NULL;
XHANDLE xhTCPSocket = NULL;
XHANDLE xhHTTPSocket = NULL;
XHANDLE xhWSSocket = NULL;
XHANDLE xhMQTTSocket = NULL;

XHANDLE xhTCPPacket = NULL;
XHANDLE xhHTTPPacket = NULL;
XHANDLE xhWSPacket = NULL;

XHANDLE xhTCPPool = NULL;
XHANDLE xhHttpPool = NULL;
XHANDLE xhWSPool = NULL;
XHANDLE xhMQTTPool = NULL;

XENGINE_SERVERCONFIG st_ServiceCfg;
MESSAGEQUEUE_DBCONFIG st_DBConfig;

void ServiceApp_Stop(int signo)
{
	if (bIsRun)
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("服务器退出..."));
		bIsRun = false;

		HelpComponents_Datas_Destory(xhTCPPacket);
		HttpProtocol_Server_DestroyEx(xhHTTPPacket);
		RfcComponents_WSPacket_DestoryEx(xhWSPacket);
		MQTTProtocol_Parse_Destory();

		NetCore_TCPXCore_DestroyEx(xhTCPSocket);
		NetCore_TCPXCore_DestroyEx(xhHTTPSocket);
		NetCore_TCPXCore_DestroyEx(xhWSSocket);
		NetCore_TCPXCore_DestroyEx(xhMQTTSocket);

		ManagePool_Thread_NQDestroy(xhTCPPool);
		ManagePool_Thread_NQDestroy(xhHttpPool);
		ManagePool_Thread_NQDestroy(xhWSPool);
		ManagePool_Thread_NQDestroy(xhMQTTPool);

		DBModule_MQData_Destory();
		DBModule_MQUser_Destory();
		MemoryCache_DBData_Destory();
		MemoryCache_DBUser_Destory();

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
#ifdef _MSC_BUILD
LONG WINAPI Coredump_ExceptionFilter(EXCEPTION_POINTERS* pExceptionPointers)
{
	static int i = 0;
	XCHAR tszFileStr[MAX_PATH] = {};
	XCHAR tszTimeStr[128] = {};
	BaseLib_Time_TimeToStr(tszTimeStr);
	_xstprintf(tszFileStr, _X("./XEngine_Coredump/dumpfile_%s_%d.dmp"), tszTimeStr, i++);
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_FATAL, _X("主程序:软件崩溃,写入dump:%s"), tszFileStr);
	HANDLE hDumpFile = CreateFileA(tszFileStr, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != hDumpFile)
	{
		MINIDUMP_EXCEPTION_INFORMATION st_DumpInfo = {};
		st_DumpInfo.ExceptionPointers = pExceptionPointers;
		st_DumpInfo.ThreadId = GetCurrentThreadId();
		st_DumpInfo.ClientPointers = TRUE;
		// 写入 dump 文件
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &st_DumpInfo, NULL, NULL);
		CloseHandle(hDumpFile);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}
#endif

int main(int argc, char** argv)
{
#ifdef _WINDOWS
	WSADATA st_WSAData;
	WSAStartup(MAKEWORD(2, 2), &st_WSAData);

	SetUnhandledExceptionFilter(Coredump_ExceptionFilter);
#ifndef _DEBUG
	if (setlocale(LC_ALL, ".UTF8") == NULL)
	{
		fprintf(stderr, "Error setting locale.\n");
		return 1;
	}
#endif
#endif
	int nRet = -1;
	LPCXSTR lpszHTTPMime = _X("./XEngine_Config/HttpMime.types");
	LPCXSTR lpszHTTPCode = _X("./XEngine_Config/HttpCode.types");
	LPCXSTR lpszDBConfig = _X("./XEngine_Config/XEngine_DBConfig.json");

	HELPCOMPONENTS_XLOG_CONFIGURE st_XLogConfig;
	THREADPOOL_PARAMENT** ppSt_ListTCPParam;
	THREADPOOL_PARAMENT** ppSt_ListHTTPParam;
	THREADPOOL_PARAMENT** ppSt_ListWSParam;
	THREADPOOL_PARAMENT** ppSt_ListMQTTParam;

	memset(&st_XLogConfig, '\0', sizeof(HELPCOMPONENTS_XLOG_CONFIGURE));
	memset(&st_ServiceCfg, '\0', sizeof(XENGINE_SERVERCONFIG));

	if (!MQ_Service_Parament(argc, argv, &st_ServiceCfg))
	{
		return -1;
	}
	st_XLogConfig.XLog_MaxBackupFile = st_ServiceCfg.st_XLog.nMaxCount;
	st_XLogConfig.XLog_MaxSize = st_ServiceCfg.st_XLog.nMaxSize;
	_tcsxcpy(st_XLogConfig.tszFileName, st_ServiceCfg.st_XLog.tszLOGFile);

	xhLog = HelpComponents_XLog_Init(st_ServiceCfg.st_XLog.nLogType, &st_XLogConfig);
	if (NULL == xhLog)
	{
		printf("启动服务器失败，启动日志失败，错误：%lX", XLog_GetLastError());
		goto NETSERVICEEXIT;
	}
	HelpComponents_XLog_SetLogPriority(xhLog, st_ServiceCfg.st_XLog.nLogLeave);
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，初始化日志系统成功"));

	if (!Config_Json_DBFile(lpszDBConfig, &st_DBConfig))
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("启动服务中，初始化数据库配置失败，错误：%lX"), Config_GetLastError());
		goto NETSERVICEEXIT;
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，初始化数据库配置成功"));

	if (st_ServiceCfg.bDeamon)
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("初始化守护进程..."));
		ServiceApp_Deamon(1);
	}

	signal(SIGINT, ServiceApp_Stop);
	signal(SIGTERM, ServiceApp_Stop);
	signal(SIGABRT, ServiceApp_Stop);
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，初始化服务器信号管理成功"));

	if (st_ServiceCfg.st_XMemory.bDataQueryEnable || st_ServiceCfg.st_XMemory.bDataInsertEnable)
	{
		if (!MemoryCache_DBData_Init(st_ServiceCfg.st_XMemory.nTimeLast, st_ServiceCfg.st_XMemory.nTimeCount, MessageQueue_CBTask_MemoryCache))
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("启动服务中，初始化消息内容高速缓存服务失败，错误：%lX"), MemoryCache_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，初始化消息内容高速缓存服务成功,更新最大时间:%d,存储最大时间:%d"), st_ServiceCfg.st_XMemory.nTimeLast, st_ServiceCfg.st_XMemory.nTimeCount);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("启动服务中，检测到没有启用消息内容高速缓存服务"), st_ServiceCfg.st_XMemory.nTimeLast, st_ServiceCfg.st_XMemory.nTimeCount);
	}
	if (st_ServiceCfg.st_XMemory.bUserQueryEnable)
	{
		if (!MemoryCache_DBUser_Init(st_ServiceCfg.st_XMemory.nTimeLast, st_ServiceCfg.st_XMemory.nTimeCount, MessageQueue_CBTask_MemoryCache))
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("启动服务中，初始化用户信息高速缓存服务失败，错误：%lX"), MemoryCache_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，初始化用户信息高速缓存服务成功,更新最大时间:%d,存储最大时间:%d"), st_ServiceCfg.st_XMemory.nTimeLast, st_ServiceCfg.st_XMemory.nTimeCount);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("启动服务中，检测到没有启用用户信息高速缓存服务"), st_ServiceCfg.st_XMemory.nTimeLast, st_ServiceCfg.st_XMemory.nTimeCount);
	}

	if (!bIsTest)
	{
		if (!DBModule_MQData_Init((DATABASE_MYSQL_CONNECTINFO*)&st_ServiceCfg.st_XSql, st_ServiceCfg.st_XMemory.bDataQueryEnable, st_ServiceCfg.st_XMemory.bDataInsertEnable))
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("启动服务中，初始化消息数据数据库失败，错误：%lX"), DBModule_GetLastError());
			goto NETSERVICEEXIT;
		}
		if (!DBModule_MQUser_Init((DATABASE_MYSQL_CONNECTINFO*)&st_ServiceCfg.st_XSql, st_ServiceCfg.st_XMemory.bUserQueryEnable, MessageQueue_CBTask_TimePublish))
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("启动服务中，初始化消息用户数据库失败，错误：%lX"), DBModule_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，初始化数据库服务成功"));
	}

	if (!SessionModule_Client_Init())
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("初始化客户端会话管理器失败，错误：%lX"), SessionModule_GetLastError());
		goto NETSERVICEEXIT;
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，初始化客户端会话管理器成功"));
	//TCP消息服务
	if (st_ServiceCfg.nTCPPort > 0)
	{
		//组包器
		xhTCPPacket = HelpComponents_Datas_Init(st_ServiceCfg.st_XMax.nMaxQueue, st_ServiceCfg.st_XMax.nTCPThread);
		if (NULL == xhTCPPacket)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("初始化TCP组包器失败，错误：%lX"), Packets_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，启动TCP组包器成功"));
		//启动网络
		xhTCPSocket = NetCore_TCPXCore_StartEx(st_ServiceCfg.nTCPPort, st_ServiceCfg.st_XMax.nMaxClient, st_ServiceCfg.st_XMax.nIOThread);
		if (NULL == xhTCPSocket)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("启动TCP网络服务器失败，错误：%lX"), NetCore_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，启动TCP网络服务器成功,TCP端口:%d,IO:%d"), st_ServiceCfg.nTCPPort, st_ServiceCfg.st_XMax.nIOThread);
		NetCore_TCPXCore_RegisterCallBackEx(xhTCPSocket, MessageQueue_Callback_TCPLogin, MessageQueue_Callback_TCPRecv, MessageQueue_Callback_TCPLeave);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，注册TCP网络事件成功"));
		//任务池
		BaseLib_Memory_Malloc((XPPPMEM)&ppSt_ListTCPParam, st_ServiceCfg.st_XMax.nTCPThread, sizeof(THREADPOOL_PARAMENT));
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
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("启动TCP线程池服务失败，错误：%lX"), ManagePool_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，启动TCP线程池服务成功,启动个数:%d"), st_ServiceCfg.st_XMax.nTCPThread);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("启动服务中，TCP消息服务没有被启用"));
	}
	//HTTP查询服务
	if (st_ServiceCfg.nHttpPort > 0)
	{
		xhHTTPPacket = HttpProtocol_Server_InitEx(lpszHTTPCode, lpszHTTPMime, st_ServiceCfg.st_XMax.nHttpThread);
		if (NULL == xhHTTPPacket)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("启动服务器中，初始化HTTP组包失败，错误：%lX"), HttpProtocol_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，初始化HTTP组包成功，IO线程个数:%d"), st_ServiceCfg.st_XMax.nHttpThread);

		xhHTTPSocket = NetCore_TCPXCore_StartEx(st_ServiceCfg.nHttpPort, st_ServiceCfg.st_XMax.nMaxClient, st_ServiceCfg.st_XMax.nIOThread);
		if (NULL == xhHTTPSocket)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("启动HTTP网络服务器失败，错误：%lX"), NetCore_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，启动HTTP网络服务器成功,HTTP端口:%d,IO:%d"), st_ServiceCfg.nHttpPort, st_ServiceCfg.st_XMax.nIOThread);
		NetCore_TCPXCore_RegisterCallBackEx(xhHTTPSocket, MessageQueue_Callback_HttpLogin, MessageQueue_Callback_HttpRecv, MessageQueue_Callback_HttpLeave);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，注册HTTP网络事件成功"));

		BaseLib_Memory_Malloc((XPPPMEM)&ppSt_ListHTTPParam, st_ServiceCfg.st_XMax.nHttpThread, sizeof(THREADPOOL_PARAMENT));
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
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("启动HTTP线程池服务失败，错误：%lX"), ManagePool_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，启动HTTP线程池服务成功,启动个数:%d"), st_ServiceCfg.st_XMax.nHttpThread);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("启动服务中，HTTP消息服务没有被启用"));
	}
	//WEBSOCKET消息服务
	if (st_ServiceCfg.nWSPort > 0)
	{
		xhWSPacket = RfcComponents_WSPacket_InitEx(st_ServiceCfg.st_XMax.nMaxClient, 0, st_ServiceCfg.st_XMax.nWSThread);
		if (NULL == xhWSPacket)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("启动服务器中，初始化Websocket组包失败，错误：%lX"), WSFrame_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，初始化Websocket组包成功，IO线程个数:%d"), st_ServiceCfg.st_XMax.nWSThread);

		xhWSSocket = NetCore_TCPXCore_StartEx(st_ServiceCfg.nWSPort, st_ServiceCfg.st_XMax.nMaxClient, st_ServiceCfg.st_XMax.nIOThread);
		if (NULL == xhWSSocket)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("启动Websocket网络服务器失败，错误：%lX"), NetCore_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，启动Websocket网络服务器成功,Websocket端口:%d,IO:%d"), st_ServiceCfg.nWSPort, st_ServiceCfg.st_XMax.nIOThread);
		NetCore_TCPXCore_RegisterCallBackEx(xhWSSocket, MessageQueue_Callback_WSLogin, MessageQueue_Callback_WSRecv, MessageQueue_Callback_WSLeave);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，注册Websocket网络事件成功"));

		BaseLib_Memory_Malloc((XPPPMEM)&ppSt_ListWSParam, st_ServiceCfg.st_XMax.nWSThread, sizeof(THREADPOOL_PARAMENT));
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
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("启动Websocket线程池服务失败，错误：%lX"), ManagePool_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，启动Websocket线程池服务成功,启动个数:%d"), st_ServiceCfg.st_XMax.nWSThread);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("启动服务中，Websocket消息服务没有被启用"));
	}
	//MQTT服务
	if (st_ServiceCfg.nMQTTPort > 0)
	{
		if (!MQTTProtocol_Parse_Init(st_ServiceCfg.st_XMax.nMQTTThread))
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("启动服务器中，初始化MQTT组包失败，错误：%lX"), MQTTProtocol_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，初始化MQTT组包成功，IO线程个数:%d"), st_ServiceCfg.st_XMax.nMQTTThread);

		xhMQTTSocket = NetCore_TCPXCore_StartEx(st_ServiceCfg.nMQTTPort, st_ServiceCfg.st_XMax.nMaxClient, st_ServiceCfg.st_XMax.nMQTTThread);
		if (NULL == xhMQTTSocket)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("启动MQTT网络服务器失败，错误：%lX"), NetCore_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，启动MQTT网络服务器成功,MQTT端口:%d,IO:%d"), st_ServiceCfg.nMQTTPort, st_ServiceCfg.st_XMax.nMQTTThread);
		NetCore_TCPXCore_RegisterCallBackEx(xhMQTTSocket, MessageQueue_Callback_MQTTLogin, MessageQueue_Callback_MQTTRecv, MessageQueue_Callback_MQTTLeave);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，注册MQTT网络事件成功"));

		BaseLib_Memory_Malloc((XPPPMEM)&ppSt_ListMQTTParam, st_ServiceCfg.st_XMax.nMQTTThread, sizeof(THREADPOOL_PARAMENT));
		for (int i = 0; i < st_ServiceCfg.st_XMax.nMQTTThread; i++)
		{
			int* pInt_Pos = new int;

			*pInt_Pos = i;
			ppSt_ListMQTTParam[i]->lParam = pInt_Pos;
			ppSt_ListMQTTParam[i]->fpCall_ThreadsTask = MessageQueue_MQTTThread;
		}
		xhWSPool = ManagePool_Thread_NQCreate(&ppSt_ListMQTTParam, st_ServiceCfg.st_XMax.nMQTTThread);
		if (NULL == xhWSPool)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("启动MQTT线程池服务失败，错误：%lX"), ManagePool_GetLastError());
			goto NETSERVICEEXIT;
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，启动MQTT线程池服务成功,启动个数:%d"), st_ServiceCfg.st_XMax.nMQTTThread);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("启动服务中，MQTT消息服务没有被启用"));
	}
	//发送信息报告
	if (st_ServiceCfg.st_XReport.bEnable && !bIsTest)
	{
		if (InfoReport_APIMachine_Send(st_ServiceCfg.st_XReport.tszAPIUrl, st_ServiceCfg.st_XReport.tszServiceName))
		{
			__int64x nTimeCount = 0;
			if (InfoReport_APIMachine_GetTime(st_ServiceCfg.st_XReport.tszAPIUrl, st_ServiceCfg.st_XReport.tszServiceName, &nTimeCount))
			{
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("启动服务中，启动信息报告给API服务器:%s 成功,报告次数:%lld"), st_ServiceCfg.st_XReport.tszAPIUrl, nTimeCount);
			}
			else
			{
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("启动服务中，启动信息报告给API服务器:%s 成功,获取报告次数失败,错误:%lX"), st_ServiceCfg.st_XReport.tszAPIUrl, InfoReport_GetLastError());
			}
		}
		else
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("启动服务中，启动信息报告给API服务器:%s 失败，错误：%lX"), st_ServiceCfg.st_XReport.tszAPIUrl, InfoReport_GetLastError());
		}
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("启动服务中，信息报告给API服务器没有启用"));
	}

	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("所有服务成功启动，服务运行中，XEngine版本:%s%s,发行版本次数:%d,当前运行版本：%s。。。"), BaseLib_Version_XNumberStr(), BaseLib_Version_XTypeStr(), st_ServiceCfg.st_XVer.pStl_ListStorage->size(), st_ServiceCfg.st_XVer.pStl_ListStorage->front().c_str());

	while (true)
	{
		if (bIsTest)
		{
			nRet = 0;
			break;
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
NETSERVICEEXIT:

	bIsRun = false;
	if (bIsTest && 0 == nRet)
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("服务启动完毕，测试程序退出..."));
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("服务启动失败，服务器退出..."));
	}
	
	HelpComponents_Datas_Destory(xhTCPPacket);
	HttpProtocol_Server_DestroyEx(xhHTTPPacket);
	RfcComponents_WSPacket_DestoryEx(xhWSPacket);
	MQTTProtocol_Parse_Destory();

	NetCore_TCPXCore_DestroyEx(xhTCPSocket);
	NetCore_TCPXCore_DestroyEx(xhHTTPSocket);
	NetCore_TCPXCore_DestroyEx(xhWSSocket);
	NetCore_TCPXCore_DestroyEx(xhMQTTSocket);

	ManagePool_Thread_NQDestroy(xhTCPPool);
	ManagePool_Thread_NQDestroy(xhHttpPool);
	ManagePool_Thread_NQDestroy(xhWSPool);
	ManagePool_Thread_NQDestroy(xhMQTTPool);

	DBModule_MQData_Destory();
	DBModule_MQUser_Destory();
	MemoryCache_DBData_Destory();
	MemoryCache_DBUser_Destory();

	SessionModule_Client_Destory();
	HelpComponents_XLog_Destroy(xhLog);
#ifdef _WINDOWS
	WSACleanup();
#endif
	
	return nRet;
}
