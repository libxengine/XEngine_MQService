#include "MQService_Hdr.h"

XHTHREAD CALLBACK MessageQueue_WebsocketThread(LPVOID lParam)
{
	int nThreadPos = *(int*)lParam;
	nThreadPos++;
	while (bIsRun)
	{
		if (!RfcComponents_WSPacket_WaitEvent(nThreadPos))
		{
			continue;
		}
		int nMsgLen = 4096;
		int nListCount = 0;
		TCHAR tszMsgBuffer[4096];
		RFCCOMPONENTS_WSPKT_CLIENT** ppSst_ListAddr;

		memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));

		RfcComponents_WSPacket_GetPool(nThreadPos, &ppSst_ListAddr, &nListCount);
		for (int i = 0; i < nListCount; i++)
		{
			for (int j = 0; j < ppSst_ListAddr[i]->nPktCount; j++)
			{
				ENUM_XENGINE_RFCOMPONENTS_WEBSOCKET_OPCODE enOPCode;
				if (!RfcComponents_WSPacket_Get(ppSst_ListAddr[i]->tszClientAddr, tszMsgBuffer, &nMsgLen, &enOPCode))
				{
					DWORD dwRet = WSFrame_GetLastError();
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("Websocket服务器获取消息失败，获取数据包失败，错误：%lX"), dwRet);
					continue;
				}
				MessageQueue_Websocket_Handle(ppSst_ListAddr[i]->tszClientAddr, tszMsgBuffer, nMsgLen, enOPCode);
			}
		}
		BaseLib_OperatorMemory_Free((XPPPMEM)&ppSst_ListAddr, nListCount);
	}
	return 0;
}
BOOL MessageQueue_Websocket_Handle(LPCTSTR lpszClientAddr, LPCTSTR lpszMsgBuffer, int nMsgLen, ENUM_XENGINE_RFCOMPONENTS_WEBSOCKET_OPCODE enOPCode)
{
	int nSDLen = 2048;
	int nRVLen = 2048;
	int nMsgType = 0;
	TCHAR tszSDBuffer[2048];
	TCHAR tszRVBuffer[2048];

	memset(tszSDBuffer, '\0', sizeof(tszSDBuffer));
	memset(tszRVBuffer, '\0', sizeof(tszRVBuffer));

	if (ENUM_XENGINE_RFCOMPONENTS_WEBSOCKET_OPCODE_TEXT == enOPCode)
	{
		XENGINE_PROTOCOLHDR st_ProtocolHdr;
		XENGINE_PROTOCOL_XMQ st_MQProtocol;
		XENGINE_PROTOCOL_XMQ st_MQClient;

		memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
		memset(&st_MQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));
		memset(&st_MQClient, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

		if (!SessionModule_Client_Get(lpszClientAddr, &st_MQClient))
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("TCP消息端:%s,没有找到指定会话地址,无法继续,错误：%lX"), lpszClientAddr, SessionModule_GetLastError());
			return FALSE;
		}
		ProtocolModule_Parse_Http(lpszMsgBuffer, nMsgLen, &st_ProtocolHdr, &st_MQProtocol, tszRVBuffer, &nRVLen, &nMsgType);
		if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQPOST == st_ProtocolHdr.unOperatorCode)
		{
			st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPPOST;
			if (!XMQModule_Packet_Post(&st_MQProtocol, tszRVBuffer, nRVLen))
			{
				st_ProtocolHdr.wReserve = 701;
				ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("Websocket客户端:%s,主题:%s,序列:%lld,投递数据报失败,无法继续,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, XMQModule_GetLastError());
				return FALSE;
			}
			st_ProtocolHdr.wReserve = 0;
			ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);

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
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("Websocket客户端:%s,主题:%s,序列:%lld,投递数据到消息队列成功,通知客户端个数:%d"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, nListCount);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQGET == st_ProtocolHdr.unOperatorCode)
		{
			int nMsgLen = 8196;
			TCHAR tszMsgBuffer[8196];
			memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));

			st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPGET;
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
			if (!XMQModule_Packet_Get(&st_MQProtocol, tszMsgBuffer, &nMsgLen))
			{
				st_ProtocolHdr.wReserve = 722;
				ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("Websocket客户端:%s,主题:%s,序列:%lld,获取消息数据失败,无法继续,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, XMQModule_GetLastError());
				return FALSE;
			}
			st_ProtocolHdr.wReserve = 0;
			ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen, tszMsgBuffer, nMsgLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
			SessionModule_Client_ADDSerial(lpszClientAddr);       //移动序列号
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("Websocket客户端:%s,主题:%s,序列:%lld,获取消息数据成功,消息大小:%d"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, nRVLen);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQDEL == st_ProtocolHdr.unOperatorCode)
		{
			st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPDEL;
			if (!XMQModule_Packet_Del(&st_MQProtocol))
			{
				st_ProtocolHdr.wReserve = 741;
				ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("Websocket客户端:%s,主题:%s,序列:%lld,删除消息队列中的数据失败，错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, XMQModule_GetLastError());
				return FALSE;
			}
			st_ProtocolHdr.wReserve = 0;
			ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("Websocket客户端:%s,主题:%s,序列:%lld,删除消息队列中的数据成功"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQCREATE == st_ProtocolHdr.unOperatorCode)
		{
			st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPCREATE;
			if (!XMQModule_Packet_TopicCreate(st_MQProtocol.tszMQKey))
			{
				st_ProtocolHdr.wReserve = 761;
				ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
				DWORD dwRet = XMQModule_GetLastError();
				if (ERROR_MQ_MODULE_PACKET_CREATE_EXIST == dwRet)
				{
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("Websocket客户端:%s,创建主题名称:%s,已经存在.无法继续创建"), lpszClientAddr, st_MQProtocol.tszMQKey);
				}
				else
				{
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("Websocket客户端:%s,创建主题失败,主题名称:%s,无法继续,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, XMQModule_GetLastError());
				}
				return FALSE;
			}
			st_ProtocolHdr.wReserve = 0;
			ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
			SessionModule_Notify_Create(st_MQProtocol.tszMQKey);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("Websocket客户端:%s,主题:%s,创建主题成功"), lpszClientAddr, st_MQProtocol.tszMQKey);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQDELETE == st_ProtocolHdr.unOperatorCode)
		{
			st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPDELETE;
			if (!XMQModule_Packet_TopicDelete(st_MQProtocol.tszMQKey))
			{
				st_ProtocolHdr.wReserve = 781;
				ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("Websocket客户端:%s,删除主题失败,主题名称:%s,无法继续,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, XMQModule_GetLastError());
				return FALSE;
			}
			st_ProtocolHdr.wReserve = 0;
			ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
			SessionModule_Notify_Destory(st_MQProtocol.tszMQKey);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("Websocket客户端:%s,主题:%s,删除主题成功"), lpszClientAddr, st_MQProtocol.tszMQKey);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQNOTIFY == st_ProtocolHdr.unOperatorCode)
		{
			st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPNOTIFY;
			if (0 == st_ProtocolHdr.wReserve)
			{
				SessionModule_Notify_Delete(st_MQProtocol.tszMQKey, lpszClientAddr);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("Websocket客户端:%s,取消订阅成功,主题名称:%s"), lpszClientAddr, st_MQProtocol.tszMQKey);
			}
			else
			{
				if (!SessionModule_Notify_Insert(st_MQProtocol.tszMQKey, lpszClientAddr))
				{
					st_ProtocolHdr.wReserve = 710;
					ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("Websocket客户端:%s,订阅主题失败,主题名称:%s,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, SessionModule_GetLastError());
					return FALSE;
				}
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("Websocket客户端:%s,订阅主题成功,主题名称:%s"), lpszClientAddr, st_MQProtocol.tszMQKey);
			}
			st_ProtocolHdr.wReserve = 0;
			ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_TCP);
		}
		else
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("Websocket客户端:%s,子协议错误，子协议：%x"), lpszClientAddr, st_ProtocolHdr.unOperatorCode);
		}
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("Websocket客户端:%s,协议错误"), lpszClientAddr);
	}
	return TRUE;
}