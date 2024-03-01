#include "MQService_Hdr.h"

XHTHREAD CALLBACK MessageQueue_TCPThread(XPVOID lParam)
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
		XENGINE_MANAGEPOOL_TASKEVENT** ppSst_ListAddr;

		memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));

		HelpComponents_Datas_GetPoolEx(xhTCPPacket, nThreadPos, &ppSst_ListAddr, &nListCount);
		for (int i = 0; i < nListCount; i++)
		{
			for (int j = 0; j < ppSst_ListAddr[i]->nPktCount; j++)
			{
				int nMsgLen = 0;
				XCHAR* ptszMsgBuffer = NULL;
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
bool MessageQueue_TCP_Handle(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, LPCXSTR lpszClientAddr, LPCXSTR lpszMsgBuffer, int nMsgLen, int nNetType)
{
	int nSDLen = 10240;
	LPCXSTR lpszClientType;
	XCHAR tszSDBuffer[10240];
	memset(tszSDBuffer, '\0', sizeof(tszSDBuffer));

	if (XENGINE_MQAPP_NETTYPE_TCP == nNetType)
	{
		lpszClientType = _X("TCP");
	}
	else
	{
		lpszClientType = _X("WEBSOCKET");
	}

	if (ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_HEARTBEAT == pSt_ProtocolHdr->unOperatorType)
	{
		if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_HB_SYN == pSt_ProtocolHdr->unOperatorCode)
		{
			//如果设置了标志位或者是HTTP请求,那么返回消息
			if (pSt_ProtocolHdr->byIsReply)
			{
				pSt_ProtocolHdr->unPacketSize = 0;
				pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_HB_ACK;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			}
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_DEBUG, _X("%s客户端：%s，处理心跳协议成功，回复标志位：%d"), lpszClientType, lpszClientAddr, pSt_ProtocolHdr->byIsReply);
		}
		else
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s客户端：%s，处理心跳子协议失败，协议类型没有找到：%d"), lpszClientType, lpszClientAddr, pSt_ProtocolHdr->unOperatorCode);
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
			if (SessionModule_Client_GetAddr(st_ProtocolAuth.tszUserName))
			{
				pSt_ProtocolHdr->wReserve = 700;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s客户端:%s,请求登录到服务失败,因为用户已经登录了"), lpszClientType, lpszClientAddr);
				return false;
			}
			_tcsxcpy(st_UserInfo.tszUserName, st_ProtocolAuth.tszUserName);
			_tcsxcpy(st_UserInfo.tszUserPass, st_ProtocolAuth.tszUserPass);

			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUSERLOG;
			if (_tcsxlen(st_ServiceCfg.st_XPass.tszPassLogin) > 0)
			{
				int nRVLen = 0;
				int nHTTPCode = 0;
				XCHAR* ptszSDBuffer = NULL;
				XCLIENT_APIHTTP st_HTTPParament;

				memset(&st_HTTPParament, '\0', sizeof(XCLIENT_APIHTTP));

				st_HTTPParament.nTimeConnect = 2;
				ProtocolModule_Packet_PassAuth(&st_ProtocolAuth, tszSDBuffer, &nSDLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERLOG);
				APIClient_Http_Request(_X("POST"), st_ServiceCfg.st_XPass.tszPassLogin, tszSDBuffer, &nHTTPCode, &ptszSDBuffer, &nSDLen, NULL, NULL, &st_HTTPParament);
				if (200 != nHTTPCode)
				{
					pSt_ProtocolHdr->wReserve = 701;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s客户端:%s,请求远程验证失败,HTTPCode:%d"), lpszClientType, lpszClientAddr, nHTTPCode);
					return false;
				}
				ProtocolModule_Parse_Websocket(ptszSDBuffer, nSDLen, NULL, (XCHAR*)&st_UserInfo, &nRVLen);
				BaseLib_OperatorMemory_FreeCStyle((XPPMEM)&ptszSDBuffer);
			}
			else
			{
				if (!DBModule_MQUser_UserQuery(&st_UserInfo))
				{
					pSt_ProtocolHdr->wReserve = 701;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s客户端:%s,请求本地验证失败,用户或者密码不正确,错误:%lX"), lpszClientType, lpszClientAddr, SessionModule_GetLastError());
					return false;
				}
				st_UserInfo.nUserState = 1;
				DBModule_MQUser_UserUPDate(&st_UserInfo);
			}
			pSt_ProtocolHdr->wReserve = 0;
			SessionModule_Client_Create(lpszClientAddr, &st_UserInfo, nNetType);
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("%s客户端:%s,请求验证成功,用户名:%s,密码:%s"), lpszClientType, lpszClientAddr, st_ProtocolAuth.tszUserName, st_ProtocolAuth.tszUserPass);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSEROUT == pSt_ProtocolHdr->unOperatorCode)
		{
			XCHAR tszUserName[MAX_PATH] = {};
			XENGINE_PROTOCOL_USERINFO st_ProtocolInfo = {};

			if (!SessionModule_Client_GetUser(lpszClientAddr, tszUserName))
			{
				pSt_ProtocolHdr->wReserve = 711;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s客户端:%s,请求登出失败,用户没有登录"), lpszClientType, lpszClientAddr);
				return false;
			}
			SessionModule_Client_GetInfoByUser(tszUserName, &st_ProtocolInfo);
			//是否需要通知
			if (_tcsxlen(st_ServiceCfg.st_XPass.tszPassLogout) > 0)
			{
				int nSDLen = 0;
				int nHTTPCode = 0;
				XCHAR tszSDBuffer[1024] = {};
				XCLIENT_APIHTTP st_HTTPParament = {};
				XENGINE_PROTOCOL_USERAUTH st_ProtocolAuth = {};

				_tcsxcpy(st_ProtocolAuth.tszUserName, st_ProtocolInfo.tszUserName);
				_tcsxcpy(st_ProtocolAuth.tszUserPass, st_ProtocolInfo.tszUserPass);

				st_HTTPParament.nTimeConnect = 2;
				ProtocolModule_Packet_PassAuth(&st_ProtocolAuth, tszSDBuffer, &nSDLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSEROUT);
				APIClient_Http_Request(_X("POST"), st_ServiceCfg.st_XPass.tszPassLogout, tszSDBuffer, &nHTTPCode, NULL, NULL, NULL, NULL, &st_HTTPParament);
				if (200 == nHTTPCode)
				{
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("%s客户端:%s,请求远程注销成功"), lpszClientType, lpszClientAddr);
				}
				else
				{
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s客户端:%s,请求远程注销失败,HTTPCode:%d"), lpszClientType, lpszClientAddr, nHTTPCode);
				}
			}
			st_ProtocolInfo.nUserState = 0;
			DBModule_MQUser_UserUPDate(&st_ProtocolInfo);
			SessionModule_Client_Delete(lpszClientAddr);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("%s客户端:%s,用户登出成功,用户名:%s"), lpszClientType, lpszClientAddr, tszUserName);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERREG == pSt_ProtocolHdr->unOperatorCode)
		{
			XENGINE_PROTOCOL_USERINFO st_UserInfo;
			memset(&st_UserInfo, '\0', sizeof(XENGINE_PROTOCOL_USERINFO));

			memcpy(&st_UserInfo, lpszMsgBuffer, sizeof(XENGINE_PROTOCOL_USERINFO));
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUSERREG;
			//是否需要代理处理
			if (_tcsxlen(st_ServiceCfg.st_XPass.tszPassRegister) > 0)
			{
				int nHTTPCode = 0;
				XCLIENT_APIHTTP st_HTTPParament;
				memset(&st_HTTPParament, '\0', sizeof(XCLIENT_APIHTTP));

				st_HTTPParament.nTimeConnect = 2;
				ProtocolModule_Packet_PassUser(&st_UserInfo, tszSDBuffer, &nSDLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERREG);
				APIClient_Http_Request(_X("POST"), st_ServiceCfg.st_XPass.tszPassRegister, tszSDBuffer, &nHTTPCode, NULL, NULL, NULL, NULL, &st_HTTPParament);
				if (200 != nHTTPCode)
				{
					pSt_ProtocolHdr->wReserve = 701;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s客户端:%s,请求远程注册失败,HTTPCode:%d"), lpszClientType, lpszClientAddr, nHTTPCode);
					return false;
				}
			}
			else
			{
				if (DBModule_MQUser_UserQuery(&st_UserInfo))
				{
					pSt_ProtocolHdr->wReserve = 721;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s客户端:%s,请求用户注册失败,用户已经存在,错误:%lX"), lpszClientType, lpszClientAddr, SessionModule_GetLastError());
					return false;
				}
				if (!DBModule_MQUser_UserInsert(&st_UserInfo))
				{
					pSt_ProtocolHdr->wReserve = 722;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s客户端:%s,请求用户注册失败,插入数据库失败,错误:%lX"), lpszClientType, lpszClientAddr, SessionModule_GetLastError());
					return false;
				}
			}
			pSt_ProtocolHdr->wReserve = 0;
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("%s客户端:%s,请求用户注册成功,用户名:%s,密码:%s"), lpszClientType, lpszClientAddr, st_UserInfo.tszUserName, st_UserInfo.tszUserPass);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERDEL == pSt_ProtocolHdr->unOperatorCode)
		{
			XENGINE_PROTOCOL_USERINFO st_UserInfo;
			memset(&st_UserInfo, '\0', sizeof(XENGINE_PROTOCOL_USERINFO));

			memcpy(&st_UserInfo, lpszMsgBuffer, sizeof(XENGINE_PROTOCOL_USERINFO));
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUSERDEL;

			if (_tcsxlen(st_ServiceCfg.st_XPass.tszPassUNReg) > 0)
			{
				int nHTTPCode = 0;
				XCLIENT_APIHTTP st_HTTPParament;
				memset(&st_HTTPParament, '\0', sizeof(XCLIENT_APIHTTP));

				st_HTTPParament.nTimeConnect = 2;

				ProtocolModule_Packet_PassUser(&st_UserInfo, tszSDBuffer, &nSDLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERDEL);
				APIClient_Http_Request(_X("POST"), st_ServiceCfg.st_XPass.tszPassUNReg, tszSDBuffer, &nHTTPCode, NULL, NULL, NULL, NULL, &st_HTTPParament);
				if (200 != nHTTPCode)
				{
					pSt_ProtocolHdr->wReserve = 701;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s客户端:%s,请求远程注销失败,错误:%lX,HTTPCode:%d"), lpszClientType, lpszClientAddr, nHTTPCode);
					return false;
				}
			}
			else
			{
				if (!DBModule_MQUser_UserDelete(&st_UserInfo))
				{
					pSt_ProtocolHdr->wReserve = 721;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s客户端:%s,请求用户删除失败,删除数据库失败,错误:%lX"), lpszClientType, lpszClientAddr, SessionModule_GetLastError());
					return false;
				}
			}
			pSt_ProtocolHdr->wReserve = 0;
			XENGINE_DBUSERKEY st_UserKey;
			XENGINE_DBTOPICOWNER st_DBOwner;

			memset(&st_UserKey, '\0', sizeof(XENGINE_DBUSERKEY));
			memset(&st_DBOwner, '\0', sizeof(XENGINE_DBTOPICOWNER));

			_tcsxcpy(st_UserKey.tszUserName, st_UserInfo.tszUserName);
			_tcsxcpy(st_DBOwner.tszUserName, st_UserInfo.tszUserName);

			DBModule_MQUser_KeyDelete(&st_UserKey);
			DBModule_MQUser_OwnerDelete(&st_DBOwner);
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, NULL, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("%s客户端:%s,请求用户删除成功,用户名:%s"), lpszClientType, lpszClientAddr, st_UserInfo.tszUserName);
		}
	}
	else if (ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ == pSt_ProtocolHdr->unOperatorType)
	{
		XENGINE_PROTOCOL_XMQ st_MQProtocol;
		XCHAR tszUserName[MAX_PATH];

		memset(&st_MQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));
		memset(tszUserName, '\0', MAX_PATH);

		if (!SessionModule_Client_GetUser(lpszClientAddr, tszUserName))
		{
			pSt_ProtocolHdr->wReserve = 700;
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s客户端:%s,请求失败,用户没有通过验证"), lpszClientType, lpszClientAddr);
			return false;
		}
		memcpy(&st_MQProtocol, lpszMsgBuffer, sizeof(XENGINE_PROTOCOL_XMQ));
		//如果没有填充消息,那就使用默认
		if (0 == _tcsxlen(st_MQProtocol.tszMQKey))
		{
			_tcsxcpy(st_MQProtocol.tszMQKey, st_ServiceCfg.tszTopic);
		}
		if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQPOST == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPPOST;
			XENGINE_DBMESSAGEQUEUE st_DBQueue;
			XENGINE_DBMESSAGEQUEUE st_DBIndex;

			memset(&st_DBQueue, '\0', sizeof(XENGINE_DBMESSAGEQUEUE));
			memset(&st_DBIndex, '\0', sizeof(XENGINE_DBMESSAGEQUEUE));
			
			st_DBQueue.byMsgType = pSt_ProtocolHdr->byVersion;
			st_DBQueue.nQueueSerial = st_MQProtocol.nSerial;
			st_DBQueue.nMsgLen = nMsgLen - sizeof(XENGINE_PROTOCOL_XMQ);
			_tcsxcpy(st_DBQueue.tszUserName, tszUserName);
			_tcsxcpy(st_DBQueue.tszUserBelong, st_MQProtocol.tszMQUsr);
			_tcsxcpy(st_DBQueue.tszQueueName, st_MQProtocol.tszMQKey);
			memcpy(st_DBQueue.tszMsgBuffer, lpszMsgBuffer + sizeof(XENGINE_PROTOCOL_XMQ), st_DBQueue.nMsgLen);
			memcpy(&st_DBQueue.nMsgAttr, &st_MQProtocol.st_MSGAttr, sizeof(XENGINE_PROTOCOL_MSGATTR));
		
			if (st_MQProtocol.nKeepTime > 0)
			{
				XENGINE_LIBTIMER st_LibTime;
				memset(&st_LibTime, '\0', sizeof(XENGINE_LIBTIMER));

				time_t nTimeStart = time(NULL);
				time_t nTimeEnd = nTimeStart + st_MQProtocol.nKeepTime;
				BaseLib_OperatorTimeSpan_CalForTime(nTimeStart, nTimeEnd, &st_LibTime);
				BaseLib_OperatorTime_TimeToStr(st_DBQueue.tszQueueLeftTime, NULL, true, &st_LibTime);
			}
			//处理序列号
			if (st_DBQueue.nQueueSerial > 0)
			{
				//序列号大于0,序列号是否存在
				st_DBIndex.nQueueSerial = st_DBQueue.nQueueSerial;
				_tcsxcpy(st_DBIndex.tszQueueName, st_DBQueue.tszQueueName);

				if (DBModule_MQData_Query(&st_DBIndex))
				{
					//找到了返回错误
					if (pSt_ProtocolHdr->byIsReply)
					{
						pSt_ProtocolHdr->wReserve = 701;
						ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
						XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					}
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s客户端:%s,主题:%s,序列:%lld,序列主题已经存在,插入数据库失败"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial);
					return false;
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
				_tcsxcpy(st_DBTime.tszQueueName, st_DBQueue.tszQueueName);

				BaseLib_OperatorTime_TTimeToStuTime(st_MQProtocol.nPubTime, &st_LibTime);
				BaseLib_OperatorTime_TimeToStr(st_DBQueue.tszQueuePublishTime, NULL, true, &st_LibTime);
				DBModule_MQUser_TimeInsert(&st_DBTime);
			}
			else if (0 == st_MQProtocol.nPubTime)
			{
				//设置为0,不是定时发布,即时通知
				if (1 == st_MQProtocol.st_MSGAttr.byAttrAll)
				{
					int nListCount = 0;
					XCHAR** pptszListAddr;
					SessionModule_Client_GetListAddr(&pptszListAddr, &nListCount);
					for (int i = 0; i < nListCount; i++)
					{
						//跳过自己
						if (0 == _tcsxncmp(lpszClientAddr, pptszListAddr[i], _tcsxlen(lpszClientAddr)) && (0 == st_MQProtocol.st_MSGAttr.byAttrSelf))
						{
							continue;
						}
						nSDLen = 0;
						int nClientType = 0;
						memset(tszSDBuffer, '\0', sizeof(tszSDBuffer));

						pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_MSGNOTIFY;

						SessionModule_Client_GetType(pptszListAddr[i], &nClientType);
						ProtocolModule_Packet_Common(nClientType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen, lpszMsgBuffer + sizeof(XENGINE_PROTOCOL_XMQ), nMsgLen - sizeof(XENGINE_PROTOCOL_XMQ));
						XEngine_MQXService_Send(pptszListAddr[i], tszSDBuffer, nSDLen, nClientType);
						BaseLib_OperatorMemory_Free((XPPPMEM)&pptszListAddr, nListCount);
					}
				}
				else
				{
					int nListCount = 0;
					XENGINE_DBUSERKEY** ppSt_ListUser;
					DBModule_MQUser_KeyList(NULL, st_MQProtocol.tszMQKey, &ppSt_ListUser, &nListCount);

					for (int i = 0; i < nListCount; i++)
					{
						//跳过自己
						if (0 == _tcsxncmp(tszUserName, ppSt_ListUser[i]->tszUserName, _tcsxlen(tszUserName)) && (0 == st_MQProtocol.st_MSGAttr.byAttrSelf))
						{
							continue;
						}
						nSDLen = 0;
						int nClientType = 0;
						XCHAR tszUserAddr[128];
						memset(tszUserAddr, '\0', sizeof(tszUserAddr));
						memset(tszSDBuffer, '\0', sizeof(tszSDBuffer));

						pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_MSGNOTIFY;
						//只有在线用户才需要即时通知
						if (SessionModule_Client_GetExist(NULL, tszUserName))
						{
							//如果发送指定用户被指定.
							if ((_tcsxlen(st_MQProtocol.tszMQUsr) > 0) && (0 != _tcsxnicmp(st_MQProtocol.tszMQUsr, tszUserName, _tcsxlen(st_MQProtocol.tszMQUsr))))
							{
								break;
							}
							SessionModule_Client_GetAddr(ppSt_ListUser[i]->tszUserName, tszUserAddr);
							SessionModule_Client_GetType(tszUserAddr, &nClientType);
							ProtocolModule_Packet_Common(nClientType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen, lpszMsgBuffer + sizeof(XENGINE_PROTOCOL_XMQ), nMsgLen - sizeof(XENGINE_PROTOCOL_XMQ));
							XEngine_MQXService_Send(tszUserAddr, tszSDBuffer, nSDLen, nClientType);
						}
					}
					BaseLib_OperatorMemory_Free((XPPPMEM)&ppSt_ListUser, nListCount);
				}
				_xstprintf(st_DBQueue.tszQueuePublishTime, _X("0"));
			}
			else
			{
				//用户获取
				_xstprintf(st_DBQueue.tszQueuePublishTime, _X("-1"));
			}
			//插入数据库
			if (!DBModule_MQData_Insert(&st_DBQueue))
			{
				if (pSt_ProtocolHdr->byIsReply)
				{
					pSt_ProtocolHdr->wReserve = 702;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				}
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s客户端:%s,主题:%s,序列:%lld,投递数据报失败,插入数据库失败,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, DBModule_GetLastError());
				return false;
			}
			//返回成功没如果需要
			if (pSt_ProtocolHdr->byIsReply)
			{
				pSt_ProtocolHdr->wReserve = 0;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			}
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("%s消息端:%s,主题:%s,序列:%lld,投递数据到消息队列成功,发布时间:%s"), lpszClientType, lpszClientAddr, st_DBQueue.tszQueueName, st_DBQueue.nQueueSerial, st_DBQueue.tszQueuePublishTime);
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
				_tcsxcpy(st_MessageQueue.tszQueueName, st_MQProtocol.tszMQKey);

				if (!DBModule_MQData_Query(&st_MessageQueue))
				{
					pSt_ProtocolHdr->wReserve = 722;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s消息端:%s,主题:%s,获取消息数据失败,获取指定消息序列:%lld 失败,错误:%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, DBModule_GetLastError());
					return false;
				}
				//获得此消息属性
				XENGINE_PROTOCOL_MSGATTR st_MSGAttr = {};
				memcpy(&st_MSGAttr, &st_MessageQueue.nMsgAttr, sizeof(XENGINE_PROTOCOL_MSGATTR));
				//所属用户
				if (_tcsxlen(st_MessageQueue.tszUserBelong) > 0)
				{
					if (1 != st_MSGAttr.byAttrActive)
					{
						pSt_ProtocolHdr->wReserve = 723;
						ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
						XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
						XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s消息端:%s,主题:%s,获取消息数据失败,获取指定消息序列:%lld 失败,错误:此消息不属于此用户"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial);
						return false;
					}
				}
				//过期任务
				if (_tcsxlen(st_MessageQueue.tszQueueLeftTime) > 2)
				{
					//有过期时间,判断是否过期
					__int64x nTimeRet = 0;
					XCHAR tszTimeStr[MAX_PATH] = {};
					BaseLib_OperatorTime_TimeToStr(tszTimeStr);
					BaseLib_OperatorTimeSpan_GetForStr(st_MessageQueue.tszQueueLeftTime, tszTimeStr, &nTimeRet, 3);
					//如果超时并且不允许主动获取,返回错误
					if ((nTimeRet < 0) && (1 != st_MSGAttr.byAttrActive))
					{
						pSt_ProtocolHdr->wReserve = 724;
						ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
						XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
						XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s消息端:%s,主题:%s,获取消息数据失败,获取指定消息序列:%lld 失败,错误:消息超时"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial);
						return false;
					}
				}
				//定时发布任务
				if (_tcsxlen(st_MessageQueue.tszQueuePublishTime) > 0)
				{
					if (1 != st_MSGAttr.byAttrActive)
					{
						pSt_ProtocolHdr->wReserve = 725;
						ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
						XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
						XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s消息端:%s,主题:%s,获取消息数据失败,获取指定消息序列:%lld 失败,错误:此为定时发布任务"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial);
						return false;
					}
				}
			}
			else
			{
				//查询绑定
				_tcsxcpy(st_UserKey.tszKeyName, st_MQProtocol.tszMQKey);
				_tcsxcpy(st_UserKey.tszUserName, tszUserName);
				if (!DBModule_MQUser_KeyQuery(&st_UserKey))
				{
					pSt_ProtocolHdr->wReserve = 723;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s消息端:%s,主题:%s,获取消息数据失败,获取绑定信息失败,错误:%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, DBModule_GetLastError());
					return false;
				}
				//得到消息
				while (true)
				{
					st_MessageQueue.nQueueSerial = st_UserKey.nKeySerial;
					_tcsxcpy(st_MessageQueue.tszQueueName, st_MQProtocol.tszMQKey);
					if (!DBModule_MQData_Query(&st_MessageQueue))
					{
						pSt_ProtocolHdr->wReserve = 724;
						ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
						XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
						XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s消息端:%s,主题:%s,序列:%lld,获取消息数据失败,无法继续,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, DBModule_GetLastError());
						return false;
					}
					//只有-1的未指定投递任务才能获取
					if (-1 != _ttxoi(st_MessageQueue.tszQueuePublishTime))
					{
						st_UserKey.nKeySerial++;
						continue;
					}
					//跳过过期任务
					if (_tcsxlen(st_MessageQueue.tszQueueLeftTime) > 0)
					{
						__int64x nTimeDiff = 0;
						XCHAR tszTimeEnd[128];
						memset(tszTimeEnd, '\0', sizeof(tszTimeEnd));

						BaseLib_OperatorTime_TimeToStr(tszTimeEnd);
						BaseLib_OperatorTimeSpan_GetForStr(st_MessageQueue.tszQueueLeftTime, tszTimeEnd, &nTimeDiff, 3);
						if (nTimeDiff > 0)
						{
							st_UserKey.nKeySerial++;
							continue;
						}
					}
					//判断是否指定了用户
					if (_tcsxlen(st_MessageQueue.tszUserBelong) > 0)
					{
						if (0 != _tcsxnicmp(st_MessageQueue.tszUserBelong, tszUserName, _tcsxlen(tszUserName)))
						{
							st_UserKey.nKeySerial++;
							continue;
						}
					}
					//是不是自己发布的
					XENGINE_PROTOCOL_MSGATTR st_MSGAttr;
					memcpy(&st_MSGAttr, &st_MessageQueue.nMsgAttr, sizeof(XENGINE_PROTOCOL_MSGATTR));
					//如果不能发送自己并且是自己的消息,那么就跳过
					if (0 == st_MSGAttr.byAttrSelf && (0 == _tcsxnicmp(st_MessageQueue.tszUserName, tszUserName, _tcsxlen(tszUserName))))
					{
						st_UserKey.nKeySerial++;
						continue;
					}
					break;
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
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("%s消息端:%s,主题:%s,序列:%lld,获取消息数据成功,消息大小:%d"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MessageQueue.nQueueSerial, st_MessageQueue.nMsgLen);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICCREATE == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICCREATE;
			//创建表
			if (!DBModule_MQData_CreateTable(st_MQProtocol.tszMQKey))
			{
				if (pSt_ProtocolHdr->byIsReply)
				{
					pSt_ProtocolHdr->wReserve = 751;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				}
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s消息端:%s,创建主题失败,创建表失败,主题名称:%s,无法继续,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, DBModule_GetLastError());
				return false;
			}
			//插入所有者
			XENGINE_DBTOPICOWNER st_DBOwner;
			memset(&st_DBOwner, '\0', sizeof(XENGINE_DBTOPICOWNER));

			_tcsxcpy(st_DBOwner.tszUserName, tszUserName);
			_tcsxcpy(st_DBOwner.tszQueueName, st_MQProtocol.tszMQKey);

			if (!DBModule_MQUser_OwnerInsert(&st_DBOwner))
			{
				if (pSt_ProtocolHdr->byIsReply)
				{
					pSt_ProtocolHdr->wReserve = 752;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				}
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s消息端:%s,创建主题失败,插入所有者失败,主题名称:%s,无法继续,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, DBModule_GetLastError());
				return false;
			}
			//回复
			if (pSt_ProtocolHdr->byIsReply)
			{
				pSt_ProtocolHdr->wReserve = 0;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			}
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("%s消息端:%s,主题:%s,创建主题成功"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICDELETE == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICDELETE;
			//清理所有者
			XENGINE_DBTOPICOWNER st_DBOwner;
			XENGINE_DBUSERKEY st_UserKey;
			XENGINE_DBTIMERELEASE st_DBInfo;

			memset(&st_DBOwner, '\0', sizeof(XENGINE_DBTOPICOWNER));
			memset(&st_UserKey, '\0', sizeof(XENGINE_DBUSERKEY));
			memset(&st_DBInfo, '\0', sizeof(XENGINE_DBTIMERELEASE));

			_tcsxcpy(st_DBOwner.tszUserName, tszUserName);
			_tcsxcpy(st_DBOwner.tszQueueName, st_MQProtocol.tszMQKey);
			_tcsxcpy(st_UserKey.tszKeyName, st_MQProtocol.tszMQKey);
			_tcsxcpy(st_DBInfo.tszQueueName, st_MQProtocol.tszMQKey);

			if (!DBModule_MQUser_OwnerDelete(&st_DBOwner))
			{
				if (pSt_ProtocolHdr->byIsReply)
				{
					pSt_ProtocolHdr->wReserve = 761;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				}
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s消息端:%s,删除主题失败,删除所有者失败,主题名称:%s,无法继续,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, DBModule_GetLastError());
				return false;
			}
			//清楚数据库
			DBModule_MQData_DeleteTable(st_MQProtocol.tszMQKey);
			DBModule_MQUser_KeyDelete(&st_UserKey);
			DBModule_MQUser_TimeDelete(&st_DBInfo);
			if (pSt_ProtocolHdr->byIsReply)
			{
				pSt_ProtocolHdr->wReserve = 0;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			}
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("%s消息端:%s,主题:%s,删除主题成功"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICBIND == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->wReserve = 0;
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICBIND;

			XENGINE_DBUSERKEY st_Userkey;
			memset(&st_Userkey, '\0', sizeof(XENGINE_DBUSERKEY));

			_tcsxcpy(st_Userkey.tszUserName, tszUserName);
			_tcsxcpy(st_Userkey.tszKeyName, st_MQProtocol.tszMQKey);
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
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s消息端:%s,设置消息队列主题更新失败,主题名称:%s,序列号:%lld,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, DBModule_GetLastError());
					return false;
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
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s消息端:%s,设置消息队列主题创建失败,主题名称:%s,序列号:%lld,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, DBModule_GetLastError());
					return false;
				}
			}
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("%s消息端:%s,请求设置序列号成功,主题名称:%s,序列号:%lld"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICUNBIND == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->wReserve = 0;
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICUNBIND;

			XENGINE_DBUSERKEY st_Userkey;
			memset(&st_Userkey, '\0', sizeof(XENGINE_DBUSERKEY));

			_tcsxcpy(st_Userkey.tszUserName, tszUserName);
			_tcsxcpy(st_Userkey.tszKeyName, st_MQProtocol.tszMQKey);
	
			if (!DBModule_MQUser_KeyDelete(&st_Userkey))
			{
				pSt_ProtocolHdr->wReserve = 721;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s消息端:%s,解除消息绑定订阅失败,可能没有找到主题用户,主题名称:%s,用户名称:%s,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, tszUserName, DBModule_GetLastError());
				return false;
			}
			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("%s消息端:%s,解除消息绑定成功,主题名称:%s,序列号:%lld"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey);
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
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s消息端:%s,获取消息队列序列属性失败,主题名称:%s,序列号:%lld,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nKeepTime == 1 ? "顺序" : "倒序", st_MQProtocol.nSerial, DBModule_GetLastError());
				return false;
			}
			pSt_ProtocolHdr->wReserve = 0;
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPNUMBER;

			st_MQNumber.nFirstNumber = st_DBStart.nQueueSerial;
			st_MQNumber.nLastNumber = st_DBEnd.nQueueSerial;
			_tcsxcpy(st_MQNumber.tszMQKey, st_MQProtocol.tszMQKey);
			ProtocolModule_Packet_MQNumber(pSt_ProtocolHdr, &st_MQNumber, tszSDBuffer, &nSDLen, nNetType);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("%s消息端:%s,获取主题序列编号成功,主题名称:%s,队列个数:%lld,开始编号:%lld,结尾编号:%lld"), lpszClientType, lpszClientAddr, st_MQNumber.tszMQKey, st_MQNumber.nCount, st_MQNumber.nFirstNumber, st_MQNumber.nLastNumber);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICMODIFY == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICMODIFY;
			XENGINE_MQTOPIC st_MQTopic;
			memset(&st_MQTopic, '\0', sizeof(XENGINE_MQTOPIC));

			memcpy(&st_MQTopic, lpszMsgBuffer + sizeof(XENGINE_PROTOCOL_XMQ), sizeof(XENGINE_MQTOPIC));
			//验证所有者
			XENGINE_DBTOPICOWNER st_DBOwner;
			memset(&st_DBOwner, '\0', sizeof(XENGINE_DBTOPICOWNER));

			_tcsxcpy(st_DBOwner.tszUserName, tszUserName);
			_tcsxcpy(st_DBOwner.tszQueueName, st_MQProtocol.tszMQKey);

			if (!DBModule_MQUser_OwnerQuery(&st_DBOwner))
			{
				if (pSt_ProtocolHdr->byIsReply)
				{
					pSt_ProtocolHdr->wReserve = 781;
					ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				}
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s消息端:%s,修改主题失败,可能用户不拥有主题,主题名称:%s,无法继续,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, DBModule_GetLastError());
				return false;
			}
			//修改主题
			if (!DBModule_MQData_ModifyTable(st_MQProtocol.tszMQKey, st_MQTopic.tszMQKey))
			{
				pSt_ProtocolHdr->wReserve = 782;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s消息端:%s,修改主题名称失败,原名称:%s,目标名:%s,错误：%lX"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQTopic.tszMQKey, DBModule_GetLastError());
				return false;
			}
			pSt_ProtocolHdr->wReserve = 0;
			DBModule_MQUser_KeyTopicUPDate(st_MQProtocol.tszMQKey, st_MQTopic.tszMQKey);
			DBModule_MQUser_TimeTopicUPDate(st_MQProtocol.tszMQKey, st_MQTopic.tszMQKey);
			DBModule_MQUser_OwnerTopicUPDate(st_MQProtocol.tszMQKey, st_MQTopic.tszMQKey);

			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("%s消息端:%s,修改主题名称成功,原名称:%s,目标名:%s"), lpszClientType, lpszClientAddr, st_MQProtocol.tszMQKey, st_MQTopic.tszMQKey);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQMSGMODIFY == pSt_ProtocolHdr->unOperatorCode)
		{
			XENGINE_DBMESSAGEQUEUE st_DBQueue;
			memset(&st_DBQueue, '\0', sizeof(XENGINE_DBMESSAGEQUEUE));

			st_DBQueue.byMsgType = pSt_ProtocolHdr->byVersion;
			st_DBQueue.nQueueSerial = st_MQProtocol.nSerial;
			st_DBQueue.nMsgLen = nMsgLen - sizeof(XENGINE_PROTOCOL_XMQ);

			_tcsxcpy(st_DBQueue.tszUserName, tszUserName);
			_tcsxcpy(st_DBQueue.tszQueueName, st_MQProtocol.tszMQKey);
			memcpy(st_DBQueue.tszMsgBuffer, lpszMsgBuffer + sizeof(XENGINE_PROTOCOL_XMQ), st_DBQueue.nMsgLen);

			if (st_MQProtocol.nKeepTime > 0)
			{
				XENGINE_LIBTIMER st_LibTime;
				memset(&st_LibTime, '\0', sizeof(XENGINE_LIBTIMER));

				time_t nTimeStart = time(NULL);
				time_t nTimeEnd = nTimeStart + st_MQProtocol.nKeepTime;
				BaseLib_OperatorTimeSpan_CalForTime(nTimeStart, nTimeEnd, &st_LibTime);
				_xstprintf(st_DBQueue.tszQueueLeftTime, _X("%04d-%02d-%02d %02d:%02d:%02d"), st_LibTime.wYear, st_LibTime.wMonth, st_LibTime.wDay, st_LibTime.wHour, st_LibTime.wMinute, st_LibTime.wSecond);
			}
			if (st_MQProtocol.nPubTime > 0)
			{
				XENGINE_DBTIMERELEASE st_DBTime;
				XENGINE_LIBTIMER st_LibTime;

				memset(&st_DBTime, '\0', sizeof(XENGINE_DBTIMERELEASE));
				memset(&st_LibTime, '\0', sizeof(XENGINE_LIBTIMER));

				st_DBTime.nIDMsg = st_DBQueue.nQueueSerial;
				st_DBTime.nIDTime = st_MQProtocol.nPubTime;
				_tcsxcpy(st_DBTime.tszQueueName, st_DBQueue.tszQueueName);

				BaseLib_OperatorTime_TTimeToStuTime(st_MQProtocol.nPubTime, &st_LibTime);
				BaseLib_OperatorTime_TimeToStr(st_DBQueue.tszQueuePublishTime, NULL, true, &st_LibTime);
				DBModule_MQUser_TimeInsert(&st_DBTime);
			}
			if (!DBModule_MQData_Modify(&st_DBQueue))
			{
				pSt_ProtocolHdr->wReserve = 791;
				ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s消息端:%s,修改消息:%s,序列号:%lld,失败,错误：%lX"), lpszClientType, lpszClientAddr, st_DBQueue.tszQueueName, st_DBQueue.nQueueSerial, DBModule_GetLastError());
				return false;
			}
			pSt_ProtocolHdr->wReserve = 0;
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPMSGMODIFY;

			ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("%s消息端:%s,修改消息:%s,序列号:%lld,成功"), lpszClientType, lpszClientAddr, st_DBQueue.tszQueueName, st_DBQueue.nQueueSerial);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUNREAD == pSt_ProtocolHdr->unOperatorCode)
		{
			int nListCount = 0;
			XENGINE_DBUSERKEY** ppSt_UserKey;
			DBModule_MQUser_KeyList(tszUserName, st_MQProtocol.tszMQKey, &ppSt_UserKey, &nListCount);

			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUNREAD;
			XHANDLE xhUNRead = ProtocolModule_Packet_UNReadCreate(pSt_ProtocolHdr, XENGINE_MQAPP_NETTYPE_TCP == nNetType ? ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_BIN : ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_JSON);
			//查找具体主题列表
			for (int i = 0; i < nListCount; i++)
			{
				int nDBCount = 0;
				XENGINE_DBUSERKEY st_UserKey;
				XENGINE_DBMESSAGEQUEUE** ppSt_DBMessage;

				memset(&st_UserKey, '\0', sizeof(XENGINE_DBUSERKEY));

				DBModule_MQData_List(ppSt_UserKey[i]->tszKeyName, ppSt_UserKey[i]->nKeySerial, &ppSt_DBMessage, &nDBCount);
				if (nDBCount > 0)
				{
					//更新用户KEY
					st_UserKey.nKeySerial = ppSt_UserKey[i]->nKeySerial + nDBCount;
					_tcsxcpy(st_UserKey.tszUserName, tszUserName);
					_tcsxcpy(st_UserKey.tszKeyName, ppSt_UserKey[i]->tszKeyName);
					DBModule_MQUser_KeyUPDate(&st_UserKey);
					ProtocolModule_Packet_UNReadInsert(xhUNRead, &ppSt_DBMessage, nDBCount, tszUserName);
					BaseLib_OperatorMemory_Free((XPPPMEM)&ppSt_DBMessage, nDBCount);
				}
			}
			ProtocolModule_Packet_UNReadDelete(xhUNRead, tszSDBuffer, &nSDLen);
			BaseLib_OperatorMemory_Free((XPPPMEM)&ppSt_UserKey, nListCount);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, nNetType);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("%s消息端:%s,请求未读消息成功,用户名:%s,发送未读消息成功,发送的主题个数:%d"), lpszClientType, lpszClientAddr, tszUserName, nListCount);
		}
		else
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("%s消息端:%s,子协议错误，子协议：%x"), lpszClientType, lpszClientAddr, pSt_ProtocolHdr->unOperatorCode);
		}
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("%s消息端:%s,主协议错误，协议：%x"), lpszClientType, lpszClientAddr, pSt_ProtocolHdr->unOperatorType);
	}
	return true;
}