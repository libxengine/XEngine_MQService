#include "MQService_Hdr.h"

XHTHREAD CALLBACK MessageQueue_HttpThread(LPVOID lParam)
{
	int nThreadPos = *(int*)lParam;
	nThreadPos++;
	while (bIsRun)
	{
		if (!RfcComponents_HttpServer_EventWaitEx(xhHTTPPacket, nThreadPos))
		{
			continue;
		}
		int nListCount = 0;
		RFCCOMPONENTS_HTTP_PKTCLIENT** ppSst_ListAddr;
		
		RfcComponents_HttpServer_GetPoolEx(xhHTTPPacket, nThreadPos, &ppSst_ListAddr, &nListCount);
		for (int i = 0; i < nListCount; i++)
		{
			for (int j = 0; j < ppSst_ListAddr[i]->nPktCount; j++)
			{
				int nMsgLen = 0;
				int nHdrCount = 0;
				CHAR** ppszHdrList;
				CHAR* ptszMsgBuffer = NULL;
				RFCCOMPONENTS_HTTP_REQPARAM st_HTTPReqparam;

				memset(&st_HTTPReqparam, '\0', sizeof(RFCCOMPONENTS_HTTP_REQPARAM));

				if (!RfcComponents_HttpServer_GetMemoryEx(xhHTTPPacket, ppSst_ListAddr[i]->tszClientAddr, &ptszMsgBuffer, &nMsgLen, &st_HTTPReqparam, &ppszHdrList, &nHdrCount))
				{
					DWORD dwRet = HttpServer_GetLastError();
					if (ERROR_HELPCOMPONENTS_PACKETS_PROTOCOL_GET_ISNULL == dwRet)
					{
						XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("HTTP服务器获取消息失败，获取数据包失败，错误：%lX"), dwRet);
					}
					continue;
				}
				MessageQueue_Http_Handle(&st_HTTPReqparam, ppSst_ListAddr[i]->tszClientAddr, ptszMsgBuffer, nMsgLen, ppszHdrList, nHdrCount);
				BaseLib_OperatorMemory_FreeCStyle((VOID**)&ptszMsgBuffer);
			}
		}
		BaseLib_OperatorMemory_Free((XPPPMEM)&ppSst_ListAddr, nListCount);
	}
	return 0;
}
BOOL MessageQueue_Http_Handle(RFCCOMPONENTS_HTTP_REQPARAM* pSt_HTTPParam, LPCTSTR lpszClientAddr, LPCTSTR lpszMsgBuffer, int nMsgLen, TCHAR** pptszListHdr, int nHdrCount)
{
	int nSDLen = 2048;
	int nRVLen = 2048;
	int nMsgType = 0;
	TCHAR tszSDBuffer[2048];
	TCHAR tszRVBuffer[2048];
	LPCTSTR lpszMethod = _T("POST");

	memset(tszSDBuffer, '\0', sizeof(tszSDBuffer));
	memset(tszRVBuffer, '\0', sizeof(tszRVBuffer));

	if (0 == _tcsnicmp(lpszMethod, pSt_HTTPParam->tszHttpMethod, _tcslen(lpszMethod)))
	{
		XENGINE_PROTOCOLHDR st_ProtocolHdr;
		XENGINE_PROTOCOL_XMQ st_MQProtocol;

		memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
		memset(&st_MQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

		ProtocolModule_Parse_Http(lpszMsgBuffer, nMsgLen, &st_ProtocolHdr, &st_MQProtocol, tszRVBuffer, &nRVLen, &nMsgType);
		if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQPOST == st_ProtocolHdr.unOperatorCode)
		{
			st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPPOST;
			if (!XMQModule_Packet_Post(&st_MQProtocol, tszRVBuffer, nRVLen))
			{
				st_ProtocolHdr.wReserve = 701;
				ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("HTTP消息端:%s,主题:%s,序列:%lld,投递数据报失败,无法继续,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, XMQModule_GetLastError());
				return FALSE;
			}
			st_ProtocolHdr.wReserve = 0;
			ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);

			int nListCount = 0;
			TCHAR** pptszListAddr;
			if (SessionModule_Notify_GetList(st_MQProtocol.tszMQKey, &pptszListAddr, &nListCount))
			{
				nSDLen = sizeof(tszSDBuffer);
				memset(tszSDBuffer, '\0', sizeof(tszSDBuffer));
				st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_MSGNOTIFY;
				ProtocolModule_Packet_TCPCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen, tszRVBuffer, nRVLen);
				for (int i = 0; i < nListCount; i++)
				{
					XEngine_MQXService_Send(pptszListAddr[i], tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_TCP);
				}
				BaseLib_OperatorMemory_Free((XPPPMEM)&pptszListAddr, nListCount);
			}
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("HTTP消息端:%s,主题:%s,序列:%lld,投递数据到消息队列成功,通知客户端个数:%d"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, nListCount);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQGET == st_ProtocolHdr.unOperatorCode)
		{
			int nMsgLen = 8196;
			TCHAR tszMsgBuffer[8196];
			memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));

			st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPGET;
			if ((st_MQProtocol.nSerial <= 0))
			{
				st_ProtocolHdr.wReserve = 721;
				ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("HTTP消息端:%s,主题:%s,获取消息数据失败,序列号为0,无法继续"), lpszClientAddr, st_MQProtocol.tszMQKey);
				return FALSE;
			}
			if (!XMQModule_Packet_Get(&st_MQProtocol, tszMsgBuffer, &nMsgLen))
			{
				st_ProtocolHdr.wReserve = 722;
				ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("HTTP消息端:%s,主题:%s,序列:%lld,获取消息数据失败,无法继续,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, XMQModule_GetLastError());
				return FALSE;
			}
			st_ProtocolHdr.wReserve = 0;
			ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen, tszMsgBuffer, nMsgLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("HTTP消息端:%s,主题:%s,序列:%lld,获取消息数据成功,消息大小:%d"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, nRVLen);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQDEL == st_ProtocolHdr.unOperatorCode)
		{
			st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPDEL;
			if (!XMQModule_Packet_Del(&st_MQProtocol))
			{
				st_ProtocolHdr.wReserve = 741;
				ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("HTTP消息端:%s,主题:%s,序列:%lld,删除消息队列中的数据失败，错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, XMQModule_GetLastError());
				return FALSE;
			}
			st_ProtocolHdr.wReserve = 0;
			ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("HTTP消息端:%s,主题:%s,序列:%lld,删除消息队列中的数据成功"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQCREATE == st_ProtocolHdr.unOperatorCode)
		{
			st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPCREATE;
			if (!XMQModule_Packet_TopicCreate(st_MQProtocol.tszMQKey))
			{
				st_ProtocolHdr.wReserve = 761;
				ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
				DWORD dwRet = XMQModule_GetLastError();
				if (ERROR_MQ_MODULE_PACKET_CREATE_EXIST == dwRet)
				{
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("HTTP消息端:%s,创建主题名称:%s,已经存在.无法继续创建"), lpszClientAddr, st_MQProtocol.tszMQKey);
				}
				else
				{
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("HTTP消息端:%s,创建主题失败,主题名称:%s,无法继续,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, XMQModule_GetLastError());
				}
				return FALSE;
			}
			st_ProtocolHdr.wReserve = 0;
			ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			SessionModule_Notify_Create(st_MQProtocol.tszMQKey);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("HTTP消息端:%s,主题:%s,创建主题成功"), lpszClientAddr, st_MQProtocol.tszMQKey);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQDELETE == st_ProtocolHdr.unOperatorCode)
		{
			st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPDELETE;
			if (!XMQModule_Packet_TopicDelete(st_MQProtocol.tszMQKey))
			{
				st_ProtocolHdr.wReserve = 781;
				ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("HTTP消息端:%s,删除主题失败,主题名称:%s,无法继续,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, XMQModule_GetLastError());
				return FALSE;
			}
			st_ProtocolHdr.wReserve = 0;
			ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			SessionModule_Notify_Destory(st_MQProtocol.tszMQKey);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("HTTP消息端:%s,主题:%s,删除主题成功"), lpszClientAddr, st_MQProtocol.tszMQKey);
		}
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("HTTP消息端:%s,协议错误"), lpszClientAddr);
	}
	return TRUE;
}