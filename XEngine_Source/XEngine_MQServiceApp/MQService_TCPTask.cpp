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
		int nMsgLen = 4096;
		int nListCount = 0;
		TCHAR tszMsgBuffer[4096];
		XENGINE_PROTOCOLHDR st_ProtocolHdr;;
		HELPCOMPONENT_PACKET_CLIENT** ppSst_ListAddr;

		memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
		memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
		
		HelpComponents_Datas_GetPoolEx(xhTCPPacket, nThreadPos, &ppSst_ListAddr, &nListCount);
		for (int i = 0; i < nListCount; i++)
		{
			for (int j = 0; j < ppSst_ListAddr[i]->nPktCount; j++)
			{
				if (!HelpComponents_Datas_GetEx(xhTCPPacket, ppSst_ListAddr[i]->tszClientAddr, tszMsgBuffer, &nMsgLen, &st_ProtocolHdr))
				{
					DWORD dwRet = Packets_GetLastError();
					if (ERROR_HELPCOMPONENTS_PACKETS_PROTOCOL_GET_ISNULL == dwRet)
					{
						XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("获取消息队列协议失败，获取数据包失败，错误：%lX"), dwRet);
					}
					continue;
				}
				MessageQueue_TCP_Handle(&st_ProtocolHdr, ppSst_ListAddr[i]->tszClientAddr, tszMsgBuffer, nMsgLen);
			}
		}
		BaseLib_OperatorMemory_Free((XPPPMEM)&ppSst_ListAddr, nListCount);
	}
	return 0;
}
BOOL MessageQueue_TCP_Handle(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, LPCTSTR lpszClientAddr, LPCTSTR lpszMsgBuffer, int nMsgLen)
{
	int nSDLen = 2048;
	int nRVLen = 2048;
	TCHAR tszSDBuffer[2048];
	TCHAR tszRVBuffer[2048];

	memset(tszSDBuffer, '\0', sizeof(tszSDBuffer));
	memset(tszRVBuffer, '\0', sizeof(tszRVBuffer));

	if (ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ == pSt_ProtocolHdr->unOperatorType)
	{
		XENGINE_PROTOCOL_XMQ st_MQProtocol;
		memset(&st_MQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

		memcpy(&st_MQProtocol, lpszMsgBuffer, sizeof(XENGINE_PROTOCOL_XMQ));

		if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQPOST == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPPOST;
			if (!XMQModule_Packet_Post(&st_MQProtocol, lpszMsgBuffer + sizeof(XENGINE_PROTOCOL_XMQ), nMsgLen - sizeof(XENGINE_PROTOCOL_XMQ)))
			{
				if (pSt_ProtocolHdr->byIsReply)
				{
					pSt_ProtocolHdr->wReserve = 701;
					ProtocolModule_Packet_TCPCommon(pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_TCP);
				}
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("TCP消息端:%s,主题:%s,序列:%lld,投递数据报失败,无法继续,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, XMQModule_GetLastError());
				return FALSE;
			}
			if (pSt_ProtocolHdr->byIsReply)
			{
				pSt_ProtocolHdr->wReserve = 0;
				ProtocolModule_Packet_TCPCommon(pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_TCP);
			}
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("TCP消息端:%s,主题:%s,序列:%lld,投递数据到消息队列成功"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQGET == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPGET;
			if (!XMQModule_Packet_Get(&st_MQProtocol, tszRVBuffer, &nRVLen))
			{
				if (pSt_ProtocolHdr->byIsReply)
				{
					pSt_ProtocolHdr->wReserve = 703;
					ProtocolModule_Packet_TCPCommon(pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_TCP);
				}
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("TCP消息端:%s,主题:%s,序列:%lld,获取消息数据失败,无法继续,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, XMQModule_GetLastError());
				return FALSE;
			}
			if (pSt_ProtocolHdr->byIsReply)
			{
				pSt_ProtocolHdr->wReserve = 0;
				ProtocolModule_Packet_TCPCommon(pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen, tszRVBuffer, nRVLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_TCP);
			}
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("TCP消息端:%s,主题:%s,序列:%lld,获取消息数据成功,消息大小:%d"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, nRVLen);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQDEL == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPDEL;
			if (!XMQModule_Packet_Del(&st_MQProtocol))
			{
				if (pSt_ProtocolHdr->byIsReply)
				{
					pSt_ProtocolHdr->wReserve = 705;
					ProtocolModule_Packet_TCPCommon(pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_TCP);
				}
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("TCP消息端:%s,主题:%s,序列:%lld,删除消息队列中的数据失败，错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, XMQModule_GetLastError());
				return FALSE;
			}
			if (pSt_ProtocolHdr->byIsReply)
			{
				pSt_ProtocolHdr->wReserve = 0;
				ProtocolModule_Packet_TCPCommon(pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_TCP);
			}
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("TCP消息端:%s,主题:%s,序列:%lld,删除消息队列中的数据成功"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQCREATE == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPCREATE;
			if (!XMQModule_Packet_TopicCreate(st_MQProtocol.tszMQKey))
			{
				if (pSt_ProtocolHdr->byIsReply)
				{
					pSt_ProtocolHdr->wReserve = 708;
					ProtocolModule_Packet_TCPCommon(pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_TCP);
				}
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("TCP消息端:%s,创建主题失败,主题名称:%s,无法继续,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, XMQModule_GetLastError());
				return FALSE;
			}
			if (pSt_ProtocolHdr->byIsReply)
			{
				pSt_ProtocolHdr->wReserve = 0;
				ProtocolModule_Packet_TCPCommon(pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_TCP);
			}
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("TCP消息端:%s,主题:%s,创建主题成功"), lpszClientAddr, st_MQProtocol.tszMQKey);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQDELETE == pSt_ProtocolHdr->unOperatorCode)
		{
			pSt_ProtocolHdr->unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPDELETE;
			if (!XMQModule_Packet_TopicDelete(st_MQProtocol.tszMQKey))
			{
				if (pSt_ProtocolHdr->byIsReply)
				{
					pSt_ProtocolHdr->wReserve = 708;
					ProtocolModule_Packet_TCPCommon(pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_TCP);
				}
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("TCP消息端:%s,删除主题失败,主题名称:%s,无法继续,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, XMQModule_GetLastError());
				return FALSE;
			}
			if (pSt_ProtocolHdr->byIsReply)
			{
				pSt_ProtocolHdr->wReserve = 0;
				ProtocolModule_Packet_TCPCommon(pSt_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_TCP);
			}
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("TCP消息端:%s,主题:%s,删除主题成功"), lpszClientAddr, st_MQProtocol.tszMQKey);
		}
		else
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("TCP消息端:%s,子协议错误，子协议：%x"), lpszClientAddr, pSt_ProtocolHdr->unOperatorCode);
		}
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("TCP消息端:%s,主协议错误，协议：%x"), lpszClientAddr, pSt_ProtocolHdr->unOperatorType);
	}
	return TRUE;
}