#include "MQService_Hdr.h"

XHTHREAD CALLBACK MessageQueue_TCPThread(LPVOID lParam)
{
	int nThreadPos = *(int*)lParam;
	nThreadPos++;
	while (bIsRun)
	{
		if (!HelpComponents_Datas_WaitEventEx(xhTCPPacket, nThreadPos))
		{
			continue;
		}
		int nListCount = 0;
		XENGINE_PROTOCOLHDR st_ProtocolHdr;;
		HELPCOMPONENT_PACKET_CLIENT** ppSst_ListAddr;

		memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));

		HelpComponents_Datas_GetPoolEx(xhTCPPacket, nThreadPos, &ppSst_ListAddr, &nListCount);
		for (int i = 0; i < nListCount; i++)
		{
			for (int j = 0; j < ppSst_ListAddr[i]->nPktCount; j++)
			{
				int nMsgLen = 0;
				TCHAR* ptszMsgBuffer = NULL;
				if (!HelpComponents_Datas_GetMemoryEx(xhTCPPacket, ppSst_ListAddr[i]->tszClientAddr, &ptszMsgBuffer, &nMsgLen, &st_ProtocolHdr))
				{
					DWORD dwRet = Packets_GetLastError();
					if (ERROR_HELPCOMPONENTS_PACKETS_PROTOCOL_GET_ISNULL == dwRet)
					{
						XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("获取消息队列协议失败，获取数据包失败，错误：%lX"), dwRet);
					}
					continue;
				}
				MessageQueue_TCP_Handle(&st_ProtocolHdr, ppSst_ListAddr[i]->tszClientAddr, ptszMsgBuffer, nMsgLen, XENGINE_MQAPP_NETTYPE_TCP);
				BaseLib_OperatorMemory_FreeCStyle((VOID**)&ptszMsgBuffer);
			}
		}
		BaseLib_OperatorMemory_Free((XPPPMEM)&ppSst_ListAddr, nListCount);
	}
	return 0;
}
BOOL MessageQueue_TCP_Handle(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, LPCTSTR lpszClientAddr, LPCTSTR lpszMsgBuffer, int nMsgLen, int nNetType)
{
	int nSDLen = 2048;
	int nRVLen = 2048;
	LPCTSTR lpszClientType;
	TCHAR tszSDBuffer[2048];
	TCHAR tszRVBuffer[2048];

	memset(tszSDBuffer, '\0', sizeof(tszSDBuffer));
	memset(tszRVBuffer, '\0', sizeof(tszRVBuffer));

	if (XENGINE_MQAPP_NETTYPE_TCP == nNetType)
	{
		lpszClientType = _T("TCP");
	}
	else if (XENGINE_MQAPP_NETTYPE_HTTP == nNetType)
	{
		lpszClientType = _T("HTTP");
	}
	else
	{
		lpszClientType = _T("WEBSOCKET");
	}

	if (ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_AUTH == pSt_ProtocolHdr->unOperatorType)
	{
		if (!st_ServiceCfg.st_XSql.bEnable)
		{
			pSt_ProtocolHdr->wReserve = 200;
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端:%s,请求验证失败,服务器没有开启验证"), lpszClientType, lpszClientAddr);
			return FALSE;
		}
		if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERLOG == pSt_ProtocolHdr->unOperatorCode)
		{
			XENGINE_PROTOCOL_USERINFO st_UserInfo;
			XENGINE_PROTOCOL_USERAUTH st_ProtocolAuth;

			memset(&st_UserInfo, '\0', sizeof(XENGINE_PROTOCOL_USERINFO));
			memset(&st_ProtocolAuth, '\0', sizeof(XENGINE_PROTOCOL_USERAUTH));

			memcpy(&st_ProtocolAuth, lpszMsgBuffer, sizeof(XENGINE_PROTOCOL_USERAUTH));
			_tcscpy(st_UserInfo.tszUserName, st_ProtocolAuth.tszUserName);
			_tcscpy(st_UserInfo.tszUserPass, st_ProtocolAuth.tszUserPass);

			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUSERLOG;
			if (!DBModule_MQUser_UserQuery(&st_UserInfo))
			{
				pSt_ProtocolHdr->wReserve = 701;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端:%s,请求本地验证失败,用户或者密码不正确,错误:%lX"), lpszClientType, lpszClientAddr, SessionModule_GetLastError());
				return FALSE;
			}
			pSt_ProtocolHdr->wReserve = 0;
			SessionModule_Client_SetAuth(lpszClientAddr);
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s客户端:%s,请求验证成功,用户名:%s,密码:%s"), lpszClientType, lpszClientAddr, st_ProtocolAuth.tszUserName, st_ProtocolAuth.tszUserPass);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERREG == pSt_ProtocolHdr->unOperatorCode)
		{
			XENGINE_PROTOCOL_USERINFO st_UserInfo;
			memset(&st_UserInfo, '\0', sizeof(XENGINE_PROTOCOL_USERINFO));

			memcpy(&st_UserInfo, lpszMsgBuffer, sizeof(XENGINE_PROTOCOL_USERINFO));
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUSERREG;
			if (!DBModule_MQUser_UserInsert(&st_UserInfo))
			{
				pSt_ProtocolHdr->wReserve = 711;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端:%s,请求用户注册失败,插入数据库失败,错误:%lX"), lpszClientType, lpszClientAddr, SessionModule_GetLastError());
				return FALSE;
			}
			pSt_ProtocolHdr->wReserve = 0;
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s客户端:%s,请求用户注册成功,用户名:%s,密码:%s"), lpszClientType, lpszClientAddr, st_UserInfo.tszUserName, st_UserInfo.tszUserPass);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERDEL == pSt_ProtocolHdr->unOperatorCode)
		{
			XENGINE_PROTOCOL_USERINFO st_UserInfo;
			memset(&st_UserInfo, '\0', sizeof(XENGINE_PROTOCOL_USERINFO));

			memcpy(&st_UserInfo, lpszMsgBuffer, sizeof(XENGINE_PROTOCOL_USERINFO));
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUSERDEL;
			if (!DBModule_MQUser_UserDelete(&st_UserInfo))
			{
				pSt_ProtocolHdr->wReserve = 721;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端:%s,请求用户删除失败,删除数据库失败,错误:%lX"), lpszClientType, lpszClientAddr, SessionModule_GetLastError());
				return FALSE;
			}
			pSt_ProtocolHdr->wReserve = 0;
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s客户端:%s,请求用户删除成功,用户名:%s"), lpszClientType, lpszClientAddr, st_UserInfo.tszUserName);
		}
	}
	else if (ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ == pSt_ProtocolHdr->unOperatorType)
	{
		BOOL bAuth = FALSE;
		XENGINE_PROTOCOL_XMQ st_MQProtocol;
		XENGINE_PROTOCOL_XMQ st_MQClient;

		memset(&st_MQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));
		memset(&st_MQClient, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

		memcpy(&st_MQProtocol, lpszMsgBuffer, sizeof(XENGINE_PROTOCOL_XMQ));

		SessionModule_Client_Get(lpszClientAddr, &st_MQClient, &bAuth);
		if (st_ServiceCfg.st_XSql.bEnable && !bAuth)
		{
			pSt_ProtocolHdr->wReserve = 700;
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端:%s,请求失败,用户没有通过验证"), lpszClientType, lpszClientAddr);
			return FALSE;
		}
		if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQPOST == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPPOST;
			if (!XMQModule_Packet_Post(&st_MQProtocol, lpszMsgBuffer + sizeof(XENGINE_PROTOCOL_XMQ), nMsgLen - sizeof(XENGINE_PROTOCOL_XMQ)))
			{
				pSt_ProtocolHdr->wReserve = 701;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端:%s,主题:%s,序列:%lld,投递数据报失败,无法继续,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, XMQModule_GetLastError());
				return FALSE;
			}
			pSt_ProtocolHdr->wReserve = 0;
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);

			int nListCount = 0;
			SESSION_NOTIFYCLIENT** ppSt_ListAddr;
			if (SessionModule_Notify_GetList(st_MQProtocol.tszMQKey, &ppSt_ListAddr, &nListCount))
			{
				int nTCPLen = 0;
				TCHAR tszTCPBuffer[4096];
				memset(tszTCPBuffer, '\0', sizeof(tszTCPBuffer));

				pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_MSGNOTIFY;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszTCPBuffer, &nTCPLen, lpszMsgBuffer + sizeof(XENGINE_PROTOCOL_XMQ), nMsgLen - sizeof(XENGINE_PROTOCOL_XMQ));
				for (int i = 0; i < nListCount; i++)
				{
					XEngine_MQXService_Send(ppSt_ListAddr[i]->tszNotifyAddr, tszTCPBuffer, nTCPLen, nNetType);
				}
				BaseLib_OperatorMemory_Free((XPPPMEM)&ppSt_ListAddr, nListCount);
			}
			if (st_ServiceCfg.st_XSql.bEnable)
			{
				XENGINE_DBMESSAGEQUEUE st_DBQueue;
				memset(&st_DBQueue, '\0', sizeof(XENGINE_DBMESSAGEQUEUE));

				st_DBQueue.nQueueSerial = st_MQProtocol.nSerial;
				st_DBQueue.nQueueGetTime = st_MQProtocol.nGetTimer;
				st_DBQueue.nMsgLen = nMsgLen - sizeof(XENGINE_PROTOCOL_XMQ);
				_tcscpy(st_DBQueue.tszQueueName, st_MQProtocol.tszMQKey);
				memcpy(st_DBQueue.tszMsgBuffer, lpszMsgBuffer + sizeof(XENGINE_PROTOCOL_XMQ), st_DBQueue.nMsgLen);
				if (st_MQProtocol.nPubTime > 0)
				{
					XENGINE_LIBTIMER st_LibTimer;
					memset(&st_LibTimer, '\0', sizeof(XENGINE_LIBTIMER));
					BaseLib_OperatorTime_TTimeToStuTime(st_MQProtocol.nPubTime, &st_LibTimer);
				}
				if (st_MQProtocol.nKeepTime > 0)
				{
					XENGINE_LIBTIMER st_LibTime;
					memset(&st_LibTime, '\0', sizeof(XENGINE_LIBTIMER));

					time_t nTimeStart = time(NULL);
					time_t nTimeEnd = nTimeStart + st_MQProtocol.nKeepTime;
					BaseLib_OperatorTimeSpan_CalForTime(nTimeStart, nTimeEnd, &st_LibTime);
					_stprintf_s(st_DBQueue.tszQueueLeftTime, _T("%04d-%02d-%02d %02d:%02d:%02d"), st_LibTime.wYear, st_LibTime.wMonth, st_LibTime.wDay, st_LibTime.wHour, st_LibTime.wMinute, st_LibTime.wSecond);
				}
				if (!DBModule_MQData_Insert(&st_DBQueue))
				{
					pSt_ProtocolHdr->wReserve = 702;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端:%s,主题:%s,序列:%lld,投递数据报失败,插入数据库失败,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, DBModule_GetLastError());
					return FALSE;
				}
			}
			else
			{

			}
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s消息端:%s,主题:%s,序列:%lld,投递数据到消息队列成功,通知客户端个数:%d"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, nListCount);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQGET == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPGET;
			if (XENGINE_MQAPP_NETTYPE_HTTP == nNetType)
			{
				//HTTP不能有为0的情况
				if (st_MQProtocol.nSerial <= 0)
				{
					pSt_ProtocolHdr->wReserve = 721;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,主题:%s,获取消息数据失败,HTTP必须设置消息序列"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey);
					return FALSE;
				}
			}
			else
			{
				if ((st_MQProtocol.nSerial > 0) || (0 != _tcsnicmp(st_MQClient.tszMQKey, st_MQProtocol.tszMQKey, _tcslen(st_MQProtocol.tszMQKey))))
				{
					//如果序列号和主题有修改
					if (0 == st_MQProtocol.nSerial)
					{
						st_MQProtocol.nSerial = 1; //不能以0为序列号
					}
					SessionModule_Client_Set(lpszClientAddr, &st_MQProtocol);
				}
				else
				{
					st_MQProtocol.nSerial = st_MQClient.nSerial;
				}
			}
			//得到消息
			if (!XMQModule_Packet_Get(&st_MQProtocol, tszRVBuffer, &nRVLen))
			{
				pSt_ProtocolHdr->wReserve = 722;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,主题:%s,序列:%lld,获取消息数据失败,无法继续,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, XMQModule_GetLastError());
				return FALSE;
			}
			pSt_ProtocolHdr->wReserve = 0;
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen, tszRVBuffer, nRVLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			SessionModule_Client_ADDDelSerial(lpszClientAddr);       //移动序列号
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s消息端:%s,主题:%s,序列:%lld,获取消息数据成功,消息大小:%d"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, nRVLen);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQDEL == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPDEL;
			if (!XMQModule_Packet_Del(&st_MQProtocol))
			{
				pSt_ProtocolHdr->wReserve = 741;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,主题:%s,序列:%lld,删除消息队列中的数据失败，错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, XMQModule_GetLastError());
				return FALSE;
			}
			pSt_ProtocolHdr->wReserve = 0;
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s消息端:%s,主题:%s,序列:%lld,删除消息队列中的数据成功"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQCREATE == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPCREATE;
			if (st_ServiceCfg.st_XSql.bEnable)
			{
				if (!DBModule_MQData_CreateTable(st_MQProtocol.tszMQKey))
				{
					pSt_ProtocolHdr->wReserve = 761;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,创建主题失败,主题名称:%s,无法继续,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, DBModule_GetLastError());
					return FALSE;
				}
			}
			if (!XMQModule_Packet_TopicCreate(st_MQProtocol.tszMQKey))
			{
				pSt_ProtocolHdr->wReserve = 762;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				DWORD dwRet = XMQModule_GetLastError();
				if (ERROR_MQ_MODULE_PACKET_CREATE_EXIST == dwRet)
				{
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s消息端:%s,创建主题名称:%s,已经存在.无法继续创建"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey);
				}
				else
				{
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,创建主题失败,主题名称:%s,无法继续,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, XMQModule_GetLastError());
				}
				return FALSE;
			}
			pSt_ProtocolHdr->wReserve = 0;
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			SessionModule_Notify_Create(st_MQProtocol.tszMQKey);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s消息端:%s,主题:%s,创建主题成功"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQDELETE == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPDELETE;
			if (!XMQModule_Packet_TopicDelete(st_MQProtocol.tszMQKey))
			{
				pSt_ProtocolHdr->wReserve = 781;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,删除主题失败,主题名称:%s,无法继续,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, XMQModule_GetLastError());
				return FALSE;
			}
			pSt_ProtocolHdr->wReserve = 0;
			DBModule_MQData_DeleteTable(st_MQProtocol.tszMQKey);
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			SessionModule_Notify_Destory(st_MQProtocol.tszMQKey);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s消息端:%s,主题:%s,删除主题成功"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQNOTIFY == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPNOTIFY;

			if (XENGINE_MQAPP_NETTYPE_HTTP == nNetType)
			{
				pSt_ProtocolHdr->wReserve = 711;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,主题:%s,订阅失败,HTTP不支持消息订阅"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey);
				return FALSE;
			}
			if (0 == pSt_ProtocolHdr->wReserve)
			{
				SessionModule_Notify_Delete(st_MQProtocol.tszMQKey, lpszClientAddr, ENUM_MQCORE_SESSION_CLIENT_TYPE_TCP);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s消息端:%s,取消订阅成功,主题名称:%s"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey);
			}
			else
			{
				if (!SessionModule_Notify_Insert(st_MQProtocol.tszMQKey, lpszClientAddr, ENUM_MQCORE_SESSION_CLIENT_TYPE_TCP))
				{
					pSt_ProtocolHdr->wReserve = 710;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,订阅主题失败,主题名称:%s,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, SessionModule_GetLastError());
					return FALSE;
				}
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s消息端:%s,订阅主题成功,主题名称:%s"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey);
			}
			pSt_ProtocolHdr->wReserve = 0;
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQSERIAL == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->wReserve = 0;
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPSERIAL;

			if (0 == st_MQProtocol.nSerial)
			{
				__int64x nFirstNumber = 0;
				__int64x nLastNumber = 0;
				if (!XMQModule_Packet_GetSerial(st_MQProtocol.tszMQKey, &nFirstNumber, &nLastNumber))
				{
					pSt_ProtocolHdr->wReserve = 712;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,获取主题序列编号失败,主题名称:%s,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, SessionModule_GetLastError());
					return FALSE;
				}
				st_MQProtocol.nKeepTime == 1 ? st_MQProtocol.nSerial = nFirstNumber : st_MQProtocol.nSerial = nLastNumber;
			}
			if (!SessionModule_Client_SetOrder(lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nKeepTime, st_MQProtocol.nSerial))
			{
				pSt_ProtocolHdr->wReserve = 712;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,设置消息队列读取模式失败,主题名称:%s,读取模式:%s,序列号:%lld,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nKeepTime == 1 ? "顺序" : "倒序", st_MQProtocol.nSerial, SessionModule_GetLastError());
				return FALSE;
			}
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s消息端:%s,请求设置序列号成功,主题名称:%s,读取模式:%s,序列号:%lld"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nKeepTime == 1 ? "顺序" : "倒序", st_MQProtocol.nSerial);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQNUMBER == pSt_ProtocolHdr->unOperatorCode)
		{
			XENGINE_MQNUMBER st_MQNumber;
			memset(&st_MQNumber, '\0', sizeof(XENGINE_MQNUMBER));
			pSt_ProtocolHdr->wReserve = 0;
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPNUMBER;

			_tcscpy(st_MQNumber.tszMQKey, st_MQProtocol.tszMQKey);
			if (!XMQModule_Packet_GetSerial(st_MQNumber.tszMQKey, &st_MQNumber.nFirstNumber, &st_MQNumber.nLastNumber))
			{
				pSt_ProtocolHdr->wReserve = 714;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,获取主题序列编号失败,主题名称:%s,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, SessionModule_GetLastError());
				return FALSE;
			}
			XMQModule_Packet_GetCount(st_MQNumber.tszMQKey, &st_MQNumber.nCount);
			ProtocolModule_Packet_MQNumber(pSt_ProtocolHdr, &st_MQNumber, tszSDBuffer, &nSDLen, nNetType);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s消息端:%s,获取主题序列编号成功,主题名称:%s,队列个数:%lld,开始编号:%lld,结尾编号:%lld"), lpszClientType, lpszClientAddr, st_MQNumber.tszMQKey, st_MQNumber.nCount, st_MQNumber.nFirstNumber, st_MQNumber.nLastNumber);
		}
		else
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("%s消息端:%s,子协议错误，子协议：%x"), lpszClientType, lpszClientAddr, pSt_ProtocolHdr->unOperatorCode);
		}
	}
	else if (ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XDDS == pSt_ProtocolHdr->unOperatorType)
	{
		MQService_DDSTask_Handle(pSt_ProtocolHdr, lpszClientAddr, lpszMsgBuffer, nMsgLen);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("%s消息端:%s,主协议错误，协议：%x"), lpszClientType, lpszClientAddr, pSt_ProtocolHdr->unOperatorType);
	}
	return TRUE;
}