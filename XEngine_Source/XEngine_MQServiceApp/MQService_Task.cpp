#include "MQService_Hdr.h"

XHTHREAD CALLBACK MessageQueue_TCPThread(LPVOID lParam)
{
	int* pInt_Pos = (int*)lParam;

	while (bIsRun)
	{
		if (!HelpComponents_Datas_WaitEventEx(xhTCPPacket, *pInt_Pos))
		{
			continue;
		}
		int nListCount = 0;
		HELPCOMPONENT_PACKET_CLIENT** ppSst_ListAddr;
		NETENGIEN_MQTHREADPOOL st_ThreadPool;

		memset(&st_ThreadPool, '\0', sizeof(NETENGIEN_MQTHREADPOOL));

		st_ThreadPool.nIPProto = 1;
		st_ThreadPool.nMsgLen = 10240;
		
		HelpComponents_Datas_GetPoolEx(xhTCPPacket, *pInt_Pos, &ppSst_ListAddr, &nListCount);
		for (int i = 0; i < nListCount; i++)
		{
			for (int j = 0; j < ppSst_ListAddr[i]->nPktCount; j++)
			{
				if (!HelpComponents_Datas_GetEx(xhTCPPacket, ppSst_ListAddr[i]->tszClientAddr, st_ThreadPool.tszMsgBuffer, &st_ThreadPool.nMsgLen, &st_ThreadPool.st_ProtocolHdr))
				{
					DWORD dwRet = Packets_GetLastError();
					if (ERROR_HELPCOMPONENTS_PACKETS_PROTOCOL_GET_ISNULL == dwRet)
					{
						XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("获取消息队列协议失败，获取数据包失败，错误：%lX"), dwRet);
					}
					continue;
				}
				_tcscpy(st_ThreadPool.tszClientAddr, ppSst_ListAddr[i]->tszClientAddr);
				MessageQueue_Callback_ThreadPool(&st_ThreadPool);
			}
		}
		BaseLib_OperatorMemory_Free((XPPPMEM)&ppSst_ListAddr, nListCount);
	}
	return 0;
}
BOOL MessageQueue_Callback_ThreadPool(NETENGIEN_MQTHREADPOOL* pSt_ThreadPool)
{
	int nSLen = 10240;
	int nPkgLen = 10240;
	TCHAR tszSendMsg[10240];
	TCHAR tszPkgMsg[10240];

	memset(tszSendMsg, '\0', sizeof(tszSendMsg));
	memset(tszPkgMsg, '\0', sizeof(tszPkgMsg));

	if (ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_AUTH == pSt_ThreadPool->st_ProtocolHdr.unOperatorType)
	{
		if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_AUTH_REQLOGIN == pSt_ThreadPool->st_ProtocolHdr.unOperatorCode)
		{
			XENGINE_PROTOCOL_USERAUTH st_UserAuth;
			memset(&st_UserAuth, '\0', sizeof(XENGINE_PROTOCOL_USERAUTH));

			memcpy(&st_UserAuth, pSt_ThreadPool->tszMsgBuffer, sizeof(XENGINE_PROTOCOL_USERAUTH));
			XMQService_User_Insert(pSt_ThreadPool->tszClientAddr, &st_UserAuth);

			XMQService_Protocol_REPLogin(tszPkgMsg, &nPkgLen);
			NetEngine_MQXService_Send(pSt_ThreadPool->tszClientAddr, tszPkgMsg, nPkgLen, pSt_ThreadPool->nIPProto);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("消息端:%s,用户：%s,类型%d,插入到用户管理器成功!"), pSt_ThreadPool->tszClientAddr, st_UserAuth.tszUserName, pSt_ThreadPool->nIPProto);
		}
		else
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("消息端:%s,类型%d,协议错误,无法继续,子协议类型:%d"), pSt_ThreadPool->tszClientAddr, pSt_ThreadPool->nIPProto, pSt_ThreadPool->st_ProtocolHdr.unOperatorCode);
		}
	}
	else if (ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_NORMAL == pSt_ThreadPool->st_ProtocolHdr.unOperatorType)
	{
		if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_LEAVE == pSt_ThreadPool->st_ProtocolHdr.unOperatorCode)
		{
			HelpComponents_Datas_DeleteEx(xhTCPPacket, pSt_ThreadPool->tszClientAddr);
			XMQService_User_Delete(pSt_ThreadPool->tszClientAddr);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("UDP客户端离开，TCP客户端地址：%s"), pSt_ThreadPool->tszClientAddr);
		}
		else
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("消息端:%s,类型%d,协议错误,无法继续,子协议类型:%d"), pSt_ThreadPool->tszClientAddr, pSt_ThreadPool->nIPProto, pSt_ThreadPool->st_ProtocolHdr.unOperatorCode);
		}
	}
	else if (ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ == pSt_ThreadPool->st_ProtocolHdr.unOperatorType)
	{
		XENGINE_MQ_USERINFO st_UserInfo;
		if (!XMQService_User_GetInfo(pSt_ThreadPool->tszClientAddr, &st_UserInfo))
		{
			XMQService_Protocol_REPPacket(tszPkgMsg, &nPkgLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPPOST, 1);
			NetEngine_MQXService_Send(pSt_ThreadPool->tszClientAddr, tszPkgMsg, nPkgLen, pSt_ThreadPool->nIPProto);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("消息端:%s,类型%d,此客户端没有登录,无法继续,错误：%lX"), pSt_ThreadPool->tszClientAddr, pSt_ThreadPool->nIPProto, XMQService_GetLastError());
			return FALSE;
		}
		if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQPOST == pSt_ThreadPool->st_ProtocolHdr.unOperatorCode)
		{
			if (!XMQService_Packet_Post(&pSt_ThreadPool->st_ProtocolHdr, pSt_ThreadPool->tszMsgBuffer))
			{
				XMQService_Protocol_REPPacket(tszPkgMsg, &nPkgLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPPOST, 1);
				NetEngine_MQXService_Send(pSt_ThreadPool->tszClientAddr, tszPkgMsg, nPkgLen, pSt_ThreadPool->nIPProto);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("消息端:%s,用户：%s,类型%d,投递数据报失败,无法继续,错误：%lX"), pSt_ThreadPool->tszClientAddr, st_UserInfo.st_AuthProtocol.tszUserName, pSt_ThreadPool->nIPProto, XMQService_GetLastError());
				return FALSE;
			}
			if (pSt_ThreadPool->st_ProtocolHdr.byIsReply)
			{
				XMQService_Protocol_REPPacket(tszPkgMsg, &nPkgLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPPOST);
				NetEngine_MQXService_Send(pSt_ThreadPool->tszClientAddr, tszPkgMsg, nPkgLen, pSt_ThreadPool->nIPProto);
			}
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("消息端:%s,用户：%s,类型%d,投递数据到消息队列成功"), pSt_ThreadPool->tszClientAddr, st_UserInfo.st_AuthProtocol.tszUserName, pSt_ThreadPool->nIPProto);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQGET == pSt_ThreadPool->st_ProtocolHdr.unOperatorCode)
		{
			if (!XMQService_Packet_Get(&pSt_ThreadPool->st_ProtocolHdr, pSt_ThreadPool->tszMsgBuffer, tszSendMsg, &nSLen, &st_UserInfo.nSerial))
			{
				XMQService_Protocol_REPPacket(tszPkgMsg, &nPkgLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPGET, 2);
				NetEngine_MQXService_Send(pSt_ThreadPool->tszClientAddr, tszPkgMsg, nPkgLen, pSt_ThreadPool->nIPProto);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("消息端:%s,用户：%s,类型%d,获取消息数据失败,无法继续,错误：%lX"), pSt_ThreadPool->tszClientAddr, st_UserInfo.st_AuthProtocol.tszUserName, pSt_ThreadPool->nIPProto, XMQService_GetLastError());
				return FALSE;
			}
			XMQService_Protocol_REPPacket(tszPkgMsg, &nPkgLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPGET, 0, tszSendMsg, nSLen);
			NetEngine_MQXService_Send(pSt_ThreadPool->tszClientAddr, tszPkgMsg, nPkgLen, pSt_ThreadPool->nIPProto);
			XMQService_User_SetSerial(pSt_ThreadPool->tszClientAddr, st_UserInfo.nSerial);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("消息端:%s,用户：%s,类型%d,序列号:%d,获取消息数据成功"), pSt_ThreadPool->tszClientAddr, st_UserInfo.st_AuthProtocol.tszUserName, pSt_ThreadPool->nIPProto, st_UserInfo.nSerial);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQDEL == pSt_ThreadPool->st_ProtocolHdr.unOperatorCode)
		{
			if (!XMQService_Packet_Del(&pSt_ThreadPool->st_ProtocolHdr, pSt_ThreadPool->tszMsgBuffer))
			{
				XMQService_Protocol_REPPacket(tszPkgMsg, &nPkgLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPDEL, 3);
				NetEngine_MQXService_Send(pSt_ThreadPool->tszClientAddr, tszPkgMsg, nPkgLen, pSt_ThreadPool->nIPProto);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("删除消息队列中的数据失败，客户端地址：%s，错误：%lX"), pSt_ThreadPool->tszClientAddr, XMQService_GetLastError());
				return FALSE;
			}
			if (pSt_ThreadPool->st_ProtocolHdr.byIsReply)
			{
				XMQService_Protocol_REPPacket(tszPkgMsg, &nPkgLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPDEL);
				NetEngine_MQXService_Send(pSt_ThreadPool->tszClientAddr, tszPkgMsg, nPkgLen, pSt_ThreadPool->nIPProto);
			}
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("消息端:%s,用户：%s,类型%d,删除消息队列中的数据成功"), pSt_ThreadPool->tszClientAddr, st_UserInfo.st_AuthProtocol.tszUserName, pSt_ThreadPool->nIPProto);
		}
		else
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("消息端:%s,用户：%s,类型%d,子协议错误，子协议：%x"), pSt_ThreadPool->tszClientAddr, st_UserInfo.st_AuthProtocol.tszUserName, pSt_ThreadPool->nIPProto, pSt_ThreadPool->st_ProtocolHdr.unOperatorCode);
		}
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("消息端:%s,类型%d,主协议错误，协议：%x"), pSt_ThreadPool->tszClientAddr, pSt_ThreadPool->nIPProto, pSt_ThreadPool->st_ProtocolHdr.unOperatorType);
	}
	return TRUE;
}