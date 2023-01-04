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
				if (HelpComponents_Datas_GetMemoryEx(xhTCPPacket, ppSst_ListAddr[i]->tszClientAddr, &ptszMsgBuffer, &nMsgLen, &st_ProtocolHdr))
				{
					MessageQueue_TCP_Handle(&st_ProtocolHdr, ppSst_ListAddr[i]->tszClientAddr, ptszMsgBuffer, nMsgLen, XENGINE_MQAPP_NETTYPE_TCP);
					BaseLib_OperatorMemory_FreeCStyle((VOID**)&ptszMsgBuffer);
				}
			}
		}
		BaseLib_OperatorMemory_Free((XPPPMEM)&ppSst_ListAddr, nListCount);
	}
	return 0;
}
BOOL MessageQueue_TCP_Handle(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, LPCTSTR lpszClientAddr, LPCTSTR lpszMsgBuffer, int nMsgLen, int nNetType)
{
	int nSDLen = 2048;
	LPCTSTR lpszClientType;
	TCHAR tszSDBuffer[2048];
	TCHAR tszSessionStr[MAX_PATH];

	memset(tszSessionStr, '\0', MAX_PATH);
	memset(tszSDBuffer, '\0', sizeof(tszSDBuffer));

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

	if (ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_HEARTBEAT == pSt_ProtocolHdr->unOperatorType)
	{
		if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_HB_SYN == pSt_ProtocolHdr->unOperatorCode)
		{
			if (XENGINE_MQAPP_NETTYPE_HTTP == nNetType)
			{
				_stprintf(tszSessionStr, _T("%lld"), pSt_ProtocolHdr->xhToken);
			}
			else
			{
				_tcscpy(tszSessionStr, lpszClientAddr);
			}
			SessionModule_Client_Heart(tszSessionStr);
			//如果设置了标志位或者是HTTP请求,那么返回消息
			if (pSt_ProtocolHdr->byIsReply || (XENGINE_MQAPP_NETTYPE_HTTP == nNetType))
			{
				pSt_ProtocolHdr->unPacketSize = 0;
				pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_HB_ACK;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			}
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_DEBUG, _T("%s客户端：%s，处理心跳协议成功，回复标志位：%d"), lpszClientType, lpszClientAddr, pSt_ProtocolHdr->byIsReply);
		}
		else
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端：%s，处理心跳子协议失败，协议类型没有找到：%d"), lpszClientType, lpszClientAddr, pSt_ProtocolHdr->unOperatorCode);
		}
	}
	else if (ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_AUTH == pSt_ProtocolHdr->unOperatorType)
	{
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
			if (_tcslen(st_ServiceCfg.st_XPass.tszPassLogin) > 0)
			{
				int nRVLen = 0;
				int nHTTPCode = 0;
				TCHAR* ptszSDBuffer = NULL;
				APIHELP_HTTPPARAMENT st_HTTPParament;

				memset(&st_HTTPParament, '\0', sizeof(APIHELP_HTTPPARAMENT));

				st_HTTPParament.nTimeConnect = 2;

				ProtocolModule_Packet_PassAuth(&st_ProtocolAuth, tszSDBuffer, &nSDLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERLOG);
				APIHelp_HttpRequest_Custom(_T("POST"), st_ServiceCfg.st_XPass.tszPassLogin, tszSDBuffer, &nHTTPCode, &ptszSDBuffer, &nSDLen, NULL, NULL, &st_HTTPParament);
				if (200 != nHTTPCode)
				{
					pSt_ProtocolHdr->wReserve = 701;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端:%s,请求远程验证失败,错误:%lX,HTTPCode:%d"), lpszClientType, lpszClientAddr, APIHelp_GetLastError(), nHTTPCode);
					return FALSE;
				}
				ProtocolModule_Parse_Http(ptszSDBuffer, nSDLen, NULL, (TCHAR*)&st_UserInfo, &nRVLen);
				BaseLib_OperatorMemory_FreeCStyle((XPPMEM)&ptszSDBuffer);
			}
			else
			{
				if (!DBModule_MQUser_UserQuery(&st_UserInfo))
				{
					pSt_ProtocolHdr->wReserve = 701;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端:%s,请求本地验证失败,用户或者密码不正确,错误:%lX"), lpszClientType, lpszClientAddr, SessionModule_GetLastError());
					return FALSE;
				}
				st_UserInfo.nUserState = 1;
				DBModule_MQUser_UserUPDate(&st_UserInfo);
			}
			pSt_ProtocolHdr->wReserve = 0;
			
			if (XENGINE_MQAPP_NETTYPE_HTTP == nNetType)
			{
				//HTTP使用SESSION
				BaseLib_OperatorHandle_Create(&pSt_ProtocolHdr->xhToken);
				_stprintf(tszSessionStr, _T("%lld"), pSt_ProtocolHdr->xhToken);
				SessionModule_Client_Create(tszSessionStr, st_UserInfo.tszUserName, nNetType);
			}
			else
			{
				SessionModule_Client_Create(lpszClientAddr, st_UserInfo.tszUserName, nNetType);
			}
			//是否需要主动推送所有没有处理的包
			if (pSt_ProtocolHdr->wReserve & 0x02)
			{
				int nListCount = 0;
				XENGINE_DBUSERKEY** ppSt_UserKey;
				DBModule_MQUser_KeyList(st_UserInfo.tszUserName, &ppSt_UserKey, &nListCount);

				XHANDLE xhUNRead = ProtocolModule_Packet_UNReadCreate(pSt_ProtocolHdr, XENGINE_MQAPP_NETTYPE_TCP == nNetType ? ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_BIN : ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_JSON);
				//查找具体主题列表
				for (int i = 0; i < nListCount; i++)
				{
					int nDBCount = 0;
					XENGINE_DBMESSAGEQUEUE** ppSt_DBMessage;
					DBModule_MQData_List(ppSt_UserKey[i]->tszKeyName, ppSt_UserKey[i]->nKeySerial, &ppSt_DBMessage, &nDBCount);
					for (int j = 0; j < nDBCount; j++)
					{
						ProtocolModule_Packet_UNReadInsert(xhUNRead, &ppSt_DBMessage, nDBCount);
					}
				}
				ProtocolModule_Packet_UNReadDelete(xhUNRead, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s客户端:%s,请求验证成功,用户名:%s,密码:%s,发送未读消息成功"), lpszClientType, lpszClientAddr, st_ProtocolAuth.tszUserName, st_ProtocolAuth.tszUserPass);
			}
			else
			{
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s客户端:%s,请求验证成功,用户名:%s,密码:%s"), lpszClientType, lpszClientAddr, st_ProtocolAuth.tszUserName, st_ProtocolAuth.tszUserPass);
			}
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSEROUT == pSt_ProtocolHdr->unOperatorCode)
		{
			XENGINE_PROTOCOL_USERAUTH st_ProtocolAuth;
			memset(&st_ProtocolAuth, '\0', sizeof(XENGINE_PROTOCOL_USERAUTH));

			memcpy(&st_ProtocolAuth, lpszMsgBuffer, sizeof(XENGINE_PROTOCOL_USERAUTH));

			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s客户端:%s,用户登出成功,暂时没有作用,用户名:%s,密码:%s"), lpszClientType, lpszClientAddr, st_ProtocolAuth.tszUserName, st_ProtocolAuth.tszUserPass);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERREG == pSt_ProtocolHdr->unOperatorCode)
		{
			XENGINE_PROTOCOL_USERINFO st_UserInfo;
			memset(&st_UserInfo, '\0', sizeof(XENGINE_PROTOCOL_USERINFO));

			memcpy(&st_UserInfo, lpszMsgBuffer, sizeof(XENGINE_PROTOCOL_USERINFO));
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUSERREG;
			//是否需要代理处理
			if (_tcslen(st_ServiceCfg.st_XPass.tszPassRegister) > 0)
			{
				int nHTTPCode = 0;
				APIHELP_HTTPPARAMENT st_HTTPParament;
				memset(&st_HTTPParament, '\0', sizeof(APIHELP_HTTPPARAMENT));

				st_HTTPParament.nTimeConnect = 2;

				ProtocolModule_Packet_PassUser(&st_UserInfo, tszSDBuffer, &nSDLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERREG);
				APIHelp_HttpRequest_Custom(_T("POST"), st_ServiceCfg.st_XPass.tszPassRegister, tszSDBuffer, &nHTTPCode, NULL, NULL, NULL, NULL, &st_HTTPParament);
				if (200 != nHTTPCode)
				{
					pSt_ProtocolHdr->wReserve = 701;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端:%s,请求远程注册失败,错误:%lX,HTTPCode:%d"), lpszClientType, lpszClientAddr, APIHelp_GetLastError(), nHTTPCode);
					return FALSE;
				}
			}
			else
			{
				if (DBModule_MQUser_UserQuery(&st_UserInfo))
				{
					pSt_ProtocolHdr->wReserve = 721;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端:%s,请求用户注册失败,用户已经存在,错误:%lX"), lpszClientType, lpszClientAddr, SessionModule_GetLastError());
					return FALSE;
				}
				if (!DBModule_MQUser_UserInsert(&st_UserInfo))
				{
					pSt_ProtocolHdr->wReserve = 722;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端:%s,请求用户注册失败,插入数据库失败,错误:%lX"), lpszClientType, lpszClientAddr, SessionModule_GetLastError());
					return FALSE;
				}
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

			if (_tcslen(st_ServiceCfg.st_XPass.tszPassUNReg) > 0)
			{
				int nHTTPCode = 0;
				APIHELP_HTTPPARAMENT st_HTTPParament;
				memset(&st_HTTPParament, '\0', sizeof(APIHELP_HTTPPARAMENT));

				st_HTTPParament.nTimeConnect = 2;

				ProtocolModule_Packet_PassUser(&st_UserInfo, tszSDBuffer, &nSDLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERDEL);
				APIHelp_HttpRequest_Custom(_T("POST"), st_ServiceCfg.st_XPass.tszPassUNReg, tszSDBuffer, &nHTTPCode, NULL, NULL, NULL, NULL, &st_HTTPParament);
				if (200 != nHTTPCode)
				{
					pSt_ProtocolHdr->wReserve = 701;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端:%s,请求远程注销失败,错误:%lX,HTTPCode:%d"), lpszClientType, lpszClientAddr, APIHelp_GetLastError(), nHTTPCode);
					return FALSE;
				}
			}
			else
			{
				if (!DBModule_MQUser_UserDelete(&st_UserInfo))
				{
					pSt_ProtocolHdr->wReserve = 721;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端:%s,请求用户删除失败,删除数据库失败,错误:%lX"), lpszClientType, lpszClientAddr, SessionModule_GetLastError());
					return FALSE;
				}
			}

			pSt_ProtocolHdr->wReserve = 0;
			XENGINE_DBUSERKEY st_UserKey;
			memset(&st_UserKey, '\0', sizeof(XENGINE_DBUSERKEY));

			_tcscpy(st_UserKey.tszUserName, st_UserInfo.tszUserName);
			DBModule_MQUser_KeyDelete(&st_UserKey);
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s客户端:%s,请求用户删除成功,用户名:%s"), lpszClientType, lpszClientAddr, st_UserInfo.tszUserName);
		}
	}
	else if (ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ == pSt_ProtocolHdr->unOperatorType)
	{
		XENGINE_PROTOCOL_XMQ st_MQProtocol;
		TCHAR tszUserName[MAX_PATH];

		memset(&st_MQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));
		memset(tszUserName, '\0', MAX_PATH);

		if (XENGINE_MQAPP_NETTYPE_HTTP == nNetType)
		{
			_stprintf(tszSessionStr, _T("%lld"), pSt_ProtocolHdr->xhToken);
			if (!SessionModule_Client_GetUser(tszSessionStr, tszUserName))
			{
				pSt_ProtocolHdr->wReserve = 700;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端:%s,请求失败,用户没有通过验证,Session:%s"), lpszClientType, lpszClientAddr, tszSessionStr);
				return FALSE;
			}
		}
		else
		{
			if (!SessionModule_Client_GetUser(lpszClientAddr, tszUserName))
			{
				pSt_ProtocolHdr->wReserve = 700;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端:%s,请求失败,用户没有通过验证"), lpszClientType, lpszClientAddr);
				return FALSE;
			}
		}
		memcpy(&st_MQProtocol, lpszMsgBuffer, sizeof(XENGINE_PROTOCOL_XMQ));
		
		if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQPOST == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPPOST;
			XENGINE_DBMESSAGEQUEUE st_DBQueue;
			XENGINE_DBMESSAGEQUEUE st_DBIndex;

			memset(&st_DBQueue, '\0', sizeof(XENGINE_DBMESSAGEQUEUE));
			memset(&st_DBIndex, '\0', sizeof(XENGINE_DBMESSAGEQUEUE));

			st_DBQueue.byMsgType = pSt_ProtocolHdr->byVersion;
			st_DBQueue.nQueueSerial = st_MQProtocol.nSerial;
			st_DBQueue.nQueueGetTime = st_MQProtocol.nGetTimer;
			st_DBQueue.nMsgLen = nMsgLen - sizeof(XENGINE_PROTOCOL_XMQ);
			_tcscpy(st_DBQueue.tszQueueName, st_MQProtocol.tszMQKey);
			memcpy(st_DBQueue.tszMsgBuffer, lpszMsgBuffer + sizeof(XENGINE_PROTOCOL_XMQ), st_DBQueue.nMsgLen);
		
			if (st_MQProtocol.nKeepTime > 0)
			{
				XENGINE_LIBTIMER st_LibTime;
				memset(&st_LibTime, '\0', sizeof(XENGINE_LIBTIMER));

				time_t nTimeStart = time(NULL);
				time_t nTimeEnd = nTimeStart + st_MQProtocol.nKeepTime;
				BaseLib_OperatorTimeSpan_CalForTime(nTimeStart, nTimeEnd, &st_LibTime);
				_stprintf_s(st_DBQueue.tszQueueLeftTime, _T("%04d-%02d-%02d %02d:%02d:%02d"), st_LibTime.wYear, st_LibTime.wMonth, st_LibTime.wDay, st_LibTime.wHour, st_LibTime.wMinute, st_LibTime.wSecond);
			}
			//处理序列号
			if (st_DBQueue.nQueueSerial > 0)
			{
				//序列号大于0,序列号是否存在
				st_DBIndex.nQueueSerial = st_DBQueue.nQueueSerial;
				_tcscpy(st_DBIndex.tszQueueName, st_DBQueue.tszQueueName);

				if (DBModule_MQData_Query(&st_DBIndex))
				{
					//找到了返回错误
					if (pSt_ProtocolHdr->byIsReply || (XENGINE_MQAPP_NETTYPE_HTTP == nNetType))
					{
						pSt_ProtocolHdr->wReserve = 701;
						ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
						XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					}
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端:%s,主题:%s,序列:%lld,序列主题已经存在,插入数据库失败"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial);
					return FALSE;
				}
			}
			else
			{
				//序列号为0,自加
				if (DBModule_MQData_GetSerial(st_DBQueue.tszQueueName, NULL, NULL, &st_DBIndex))
				{
					st_DBQueue.nQueueSerial = st_DBIndex.nQueueSerial + 1;
				}
				else
				{
					//可能为空表
					st_DBQueue.nQueueSerial = 1;
				}
			}
			//是否被设置定时发布
			if (st_MQProtocol.nPubTime > 0)
			{
				XENGINE_DBTIMERELEASE st_DBTime;
				XENGINE_LIBTIMER st_LibTime;

				memset(&st_DBTime, '\0', sizeof(XENGINE_DBTIMERELEASE));
				memset(&st_LibTime, '\0', sizeof(XENGINE_LIBTIMER));

				st_DBTime.nIDMsg = st_DBQueue.nQueueSerial;
				st_DBTime.nIDTime = st_MQProtocol.nPubTime;
				_tcscpy(st_DBTime.tszQueueName, st_DBQueue.tszQueueName);

				BaseLib_OperatorTime_TTimeToStuTime(st_MQProtocol.nPubTime, &st_LibTime);
				BaseLib_OperatorTime_TimeToStr(st_DBQueue.tszQueuePublishTime, NULL, TRUE, &st_LibTime);
				DBModule_MQUser_TimeInsert(&st_DBTime);
			}
			//插入数据库
			if (!DBModule_MQData_Insert(&st_DBQueue))
			{
				if (pSt_ProtocolHdr->byIsReply || (XENGINE_MQAPP_NETTYPE_HTTP == nNetType))
				{
					pSt_ProtocolHdr->wReserve = 702;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				}
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端:%s,主题:%s,序列:%lld,投递数据报失败,插入数据库失败,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, DBModule_GetLastError());
				return FALSE;
			}
			//返回成功没如果需要
			if (pSt_ProtocolHdr->byIsReply || (XENGINE_MQAPP_NETTYPE_HTTP == nNetType))
			{
				pSt_ProtocolHdr->wReserve = 0;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			}
			//是否需要通知
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
					//跳过自己
					if (0 == _tcsncmp(lpszClientAddr, ppSt_ListAddr[i]->tszNotifyAddr, _tcslen(lpszClientAddr)))
					{
						continue;
					}
					XEngine_MQXService_Send(ppSt_ListAddr[i]->tszNotifyAddr, tszTCPBuffer, nTCPLen, nNetType);
				}
				BaseLib_OperatorMemory_Free((XPPPMEM)&ppSt_ListAddr, nListCount);
			}
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s消息端:%s,主题:%s,序列:%lld,投递数据到消息队列成功,通知客户端个数:%d"), lpszClientType, lpszClientAddr, st_DBQueue.tszQueueName, st_DBQueue.nQueueSerial, nListCount);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQGET == pSt_ProtocolHdr->unOperatorCode)
		{
			XENGINE_DBMESSAGEQUEUE st_MessageQueue;
			XENGINE_DBUSERKEY st_UserKey;

			memset(&st_MessageQueue, '\0', sizeof(XENGINE_DBMESSAGEQUEUE));
			memset(&st_UserKey, '\0', sizeof(XENGINE_DBUSERKEY));
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPGET;
			if (st_MQProtocol.nSerial > 0)
			{
				st_MessageQueue.nQueueSerial = st_MQProtocol.nSerial;
				_tcscpy(st_MessageQueue.tszQueueName, st_MQProtocol.tszMQKey);

				if (!DBModule_MQData_Query(&st_MessageQueue))
				{
					pSt_ProtocolHdr->wReserve = 722;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,主题:%s,获取消息数据失败,获取指定消息序列:%lld 失败,错误:%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, DBModule_GetLastError());
					return FALSE;
				}
			}
			else
			{
				//查询绑定
				_tcscpy(st_UserKey.tszKeyName, st_MQProtocol.tszMQKey);
				_tcscpy(st_UserKey.tszUserName, tszUserName);
				if (!DBModule_MQUser_KeyQuery(&st_UserKey))
				{
					pSt_ProtocolHdr->wReserve = 723;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,主题:%s,获取消息数据失败,获取绑定信息失败,错误:%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, DBModule_GetLastError());
					return FALSE;
				}
				//得到消息
				st_MessageQueue.nQueueSerial = st_UserKey.nKeySerial;
				_tcscpy(st_MessageQueue.tszQueueName, st_MQProtocol.tszMQKey);
				if (!DBModule_MQData_Query(&st_MessageQueue))
				{
					pSt_ProtocolHdr->wReserve = 724;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,主题:%s,序列:%lld,获取消息数据失败,无法继续,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, DBModule_GetLastError());
					return FALSE;
				}
				st_MQProtocol.nSerial = st_UserKey.nKeySerial;
				//移动序列号
				st_UserKey.nKeySerial++;
				DBModule_MQUser_KeyUPDate(&st_UserKey);
			}
			pSt_ProtocolHdr->wReserve = 0;
			pSt_ProtocolHdr->byVersion = st_MessageQueue.byMsgType;
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen, st_MessageQueue.tszMsgBuffer, st_MessageQueue.nMsgLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s消息端:%s,主题:%s,序列:%lld,获取消息数据成功,消息大小:%d"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MessageQueue.nQueueSerial, st_MessageQueue.nMsgLen);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQCREATE == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPCREATE;
			if (!DBModule_MQData_CreateTable(st_MQProtocol.tszMQKey))
			{
				if (pSt_ProtocolHdr->byIsReply || (XENGINE_MQAPP_NETTYPE_HTTP == nNetType))
				{
					pSt_ProtocolHdr->wReserve = 761;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				}
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,创建主题失败,主题名称:%s,无法继续,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, DBModule_GetLastError());
				return FALSE;
			}
			SessionModule_Notify_Create(st_MQProtocol.tszMQKey);
			if (pSt_ProtocolHdr->byIsReply || (XENGINE_MQAPP_NETTYPE_HTTP == nNetType))
			{
				pSt_ProtocolHdr->wReserve = 0;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			}
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s消息端:%s,主题:%s,创建主题成功"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQDELETE == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPDELETE;
			
			if (pSt_ProtocolHdr->byIsReply || (XENGINE_MQAPP_NETTYPE_HTTP == nNetType))
			{
				pSt_ProtocolHdr->wReserve = 0;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			}
			DBModule_MQData_DeleteTable(st_MQProtocol.tszMQKey);
			SessionModule_Notify_Destory(st_MQProtocol.tszMQKey);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s消息端:%s,主题:%s,删除主题成功"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQNOTIFY == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPNOTIFY;

			if (XENGINE_MQAPP_NETTYPE_HTTP == nNetType)
			{
				if (pSt_ProtocolHdr->byIsReply || (XENGINE_MQAPP_NETTYPE_HTTP == nNetType))
				{
					pSt_ProtocolHdr->wReserve = 711;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				}
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
					if (pSt_ProtocolHdr->byIsReply || (XENGINE_MQAPP_NETTYPE_HTTP == nNetType))
					{
						pSt_ProtocolHdr->wReserve = 710;
						ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
						XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					}
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,订阅主题失败,主题名称:%s,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, SessionModule_GetLastError());
					return FALSE;
				}
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s消息端:%s,订阅主题成功,主题名称:%s"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey);
			}
			if (pSt_ProtocolHdr->byIsReply || (XENGINE_MQAPP_NETTYPE_HTTP == nNetType))
			{
				pSt_ProtocolHdr->wReserve = 0;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			}
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQSERIAL == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->wReserve = 0;
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPSERIAL;

			XENGINE_DBUSERKEY st_Userkey;
			memset(&st_Userkey, '\0', sizeof(XENGINE_DBUSERKEY));

			_tcscpy(st_Userkey.tszUserName, tszUserName);
			_tcscpy(st_Userkey.tszKeyName, st_MQProtocol.tszMQKey);
			//先查询有没有
			if (DBModule_MQUser_KeyQuery(&st_Userkey))
			{
				//有就更新
				st_Userkey.nKeySerial = st_MQProtocol.nSerial;
				if (!DBModule_MQUser_KeyUPDate(&st_Userkey))
				{
					pSt_ProtocolHdr->wReserve = 710;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,设置消息队列主题更新失败,主题名称:%s,序列号:%lld,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, DBModule_GetLastError());
					return FALSE;
				}
			}
			else
			{
				//没有就创建
				st_Userkey.nKeySerial = st_MQProtocol.nSerial;
				if (!DBModule_MQUser_KeyInsert(&st_Userkey))
				{
					pSt_ProtocolHdr->wReserve = 711;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,设置消息队列主题创建失败,主题名称:%s,序列号:%lld,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, DBModule_GetLastError());
					return FALSE;
				}
			}
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s消息端:%s,请求设置序列号成功,主题名称:%s,序列号:%lld"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQNUMBER == pSt_ProtocolHdr->unOperatorCode)
		{
			XENGINE_DBMESSAGEQUEUE st_DBStart;
			XENGINE_DBMESSAGEQUEUE st_DBEnd;
			XENGINE_MQNUMBER st_MQNumber;

			memset(&st_DBStart, '\0', sizeof(XENGINE_DBMESSAGEQUEUE));
			memset(&st_DBEnd, '\0', sizeof(XENGINE_DBMESSAGEQUEUE));
			memset(&st_MQNumber, '\0', sizeof(XENGINE_MQNUMBER));
			if (!DBModule_MQData_GetSerial(st_MQProtocol.tszMQKey, &st_MQNumber.nCount, &st_DBStart, &st_DBEnd))
			{
				pSt_ProtocolHdr->wReserve = 741;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,获取消息队列序列属性失败,主题名称:%s,序列号:%lld,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nKeepTime == 1 ? "顺序" : "倒序", st_MQProtocol.nSerial, DBModule_GetLastError());
				return FALSE;
			}
			pSt_ProtocolHdr->wReserve = 0;
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPNUMBER;

			st_MQNumber.nFirstNumber = st_DBStart.nQueueSerial;
			st_MQNumber.nLastNumber = st_DBEnd.nQueueSerial;
			_tcscpy(st_MQNumber.tszMQKey, st_MQProtocol.tszMQKey);
			ProtocolModule_Packet_MQNumber(pSt_ProtocolHdr, &st_MQNumber, tszSDBuffer, &nSDLen, nNetType);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s消息端:%s,获取主题序列编号成功,主题名称:%s,队列个数:%lld,开始编号:%lld,结尾编号:%lld"), lpszClientType, lpszClientAddr, st_MQNumber.tszMQKey, st_MQNumber.nCount, st_MQNumber.nFirstNumber, st_MQNumber.nLastNumber);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICMODIFY == pSt_ProtocolHdr->unOperatorCode)
		{
			XENGINE_MQTOPIC st_MQTopic;
			memset(&st_MQTopic, '\0', sizeof(XENGINE_MQTOPIC));

			memcpy(&st_MQTopic, lpszMsgBuffer + sizeof(XENGINE_PROTOCOL_XMQ), sizeof(XENGINE_MQTOPIC));
			if (!DBModule_MQData_ModifyTable(st_MQProtocol.tszMQKey, st_MQTopic.tszMQKey))
			{
				pSt_ProtocolHdr->wReserve = 781;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,修改主题名称失败,原名称:%s,目标名:%s,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQTopic.tszMQKey, DBModule_GetLastError());
				return FALSE;
			}
			pSt_ProtocolHdr->wReserve = 0;
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICMODIFY;

			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s消息端:%s,修改主题名称成功,原名称:%s,目标名:%s"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQTopic.tszMQKey);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQMSGMODIFY == pSt_ProtocolHdr->unOperatorCode)
		{
			XENGINE_DBMESSAGEQUEUE st_DBQueue;
			memset(&st_DBQueue, '\0', sizeof(XENGINE_DBMESSAGEQUEUE));

			st_DBQueue.byMsgType = pSt_ProtocolHdr->byVersion;
			st_DBQueue.nQueueSerial = st_MQProtocol.nSerial;
			st_DBQueue.nQueueGetTime = st_MQProtocol.nGetTimer;
			st_DBQueue.nMsgLen = nMsgLen - sizeof(XENGINE_PROTOCOL_XMQ);
			_tcscpy(st_DBQueue.tszQueueName, st_MQProtocol.tszMQKey);
			memcpy(st_DBQueue.tszMsgBuffer, lpszMsgBuffer + sizeof(XENGINE_PROTOCOL_XMQ), st_DBQueue.nMsgLen);

			if (st_MQProtocol.nKeepTime > 0)
			{
				XENGINE_LIBTIMER st_LibTime;
				memset(&st_LibTime, '\0', sizeof(XENGINE_LIBTIMER));

				time_t nTimeStart = time(NULL);
				time_t nTimeEnd = nTimeStart + st_MQProtocol.nKeepTime;
				BaseLib_OperatorTimeSpan_CalForTime(nTimeStart, nTimeEnd, &st_LibTime);
				_stprintf_s(st_DBQueue.tszQueueLeftTime, _T("%04d-%02d-%02d %02d:%02d:%02d"), st_LibTime.wYear, st_LibTime.wMonth, st_LibTime.wDay, st_LibTime.wHour, st_LibTime.wMinute, st_LibTime.wSecond);
			}
			if (st_MQProtocol.nPubTime > 0)
			{
				XENGINE_DBTIMERELEASE st_DBTime;
				XENGINE_LIBTIMER st_LibTime;

				memset(&st_DBTime, '\0', sizeof(XENGINE_DBTIMERELEASE));
				memset(&st_LibTime, '\0', sizeof(XENGINE_LIBTIMER));

				st_DBTime.nIDMsg = st_DBQueue.nQueueSerial;
				st_DBTime.nIDTime = st_MQProtocol.nPubTime;
				_tcscpy(st_DBTime.tszQueueName, st_DBQueue.tszQueueName);

				BaseLib_OperatorTime_TTimeToStuTime(st_MQProtocol.nPubTime, &st_LibTime);
				BaseLib_OperatorTime_TimeToStr(st_DBQueue.tszQueuePublishTime, NULL, TRUE, &st_LibTime);
				DBModule_MQUser_TimeInsert(&st_DBTime);
			}
			if (!DBModule_MQData_Modify(&st_DBQueue))
			{
				pSt_ProtocolHdr->wReserve = 791;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s消息端:%s,修改消息:%s,序列号:%lld,失败,错误：%lX"), lpszClientType, lpszClientAddr, st_DBQueue.tszQueueName, st_DBQueue.nQueueSerial, DBModule_GetLastError());
				return FALSE;
			}
			pSt_ProtocolHdr->wReserve = 0;
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPMSGMODIFY;

			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s消息端:%s,修改消息:%s,序列号:%lld,成功"), lpszClientType, lpszClientAddr, st_DBQueue.tszQueueName, st_DBQueue.nQueueSerial);
		}
		else
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("%s消息端:%s,子协议错误，子协议：%x"), lpszClientType, lpszClientAddr, pSt_ProtocolHdr->unOperatorCode);
		}
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("%s消息端:%s,主协议错误，协议：%x"), lpszClientType, lpszClientAddr, pSt_ProtocolHdr->unOperatorType);
	}
	return TRUE;
}