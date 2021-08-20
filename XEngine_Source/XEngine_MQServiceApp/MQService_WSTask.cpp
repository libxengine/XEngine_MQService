﻿#include "MQService_Hdr.h"

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
		int nListCount = 0;
		RFCCOMPONENTS_WSPKT_CLIENT** ppSst_ListAddr;

		RfcComponents_WSPacket_GetPool(nThreadPos, &ppSst_ListAddr, &nListCount);
		for (int i = 0; i < nListCount; i++)
		{
			for (int j = 0; j < ppSst_ListAddr[i]->nPktCount; j++)
			{
				int nMsgLen = 0;
				TCHAR* ptszMsgBuffer = NULL;
				ENUM_XENGINE_RFCOMPONENTS_WEBSOCKET_OPCODE enOPCode;
				if (!RfcComponents_WSPacket_GetMemory(ppSst_ListAddr[i]->tszClientAddr, &ptszMsgBuffer, &nMsgLen, &enOPCode))
				{
					DWORD dwRet = WSFrame_GetLastError();
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("Websocket服务器获取消息失败，获取数据包失败，错误：%lX"), dwRet);
					continue;
				}
				MessageQueue_Websocket_Handle(ppSst_ListAddr[i]->tszClientAddr, ptszMsgBuffer, nMsgLen, enOPCode);
				BaseLib_OperatorMemory_FreeCStyle((VOID**)&ptszMsgBuffer);
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
	int nJSLen = 0;
	int nMsgType = 0;
	TCHAR tszSDBuffer[2048];
	TCHAR tszRVBuffer[2048];
	TCHAR* ptszMsgBuffer = NULL;

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

		SessionModule_Client_Get(lpszClientAddr, &st_MQClient);
		ProtocolModule_Parse_Http(lpszMsgBuffer, nMsgLen, &st_MQProtocol, &st_ProtocolHdr, &ptszMsgBuffer, &nJSLen, &nMsgType);
		if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQPOST == st_ProtocolHdr.unOperatorCode)
		{
			st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPPOST;
			if (!XMQModule_Packet_Post(&st_MQProtocol, ptszMsgBuffer, nJSLen))
			{
				st_ProtocolHdr.wReserve = 701;
				ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
				BaseLib_OperatorMemory_FreeCStyle((XPPMEM)&ptszMsgBuffer);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("Websocket客户端:%s,主题:%s,序列:%lld,投递数据报失败,无法继续,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, XMQModule_GetLastError());
				return FALSE;
			}
			st_ProtocolHdr.wReserve = 0;
			ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);

			int nListCount = 0;
			SESSION_NOTIFYCLIENT** ppSt_ListAddr;
			if (SessionModule_Notify_GetList(st_MQProtocol.tszMQKey, &ppSt_ListAddr, &nListCount))
			{
				int nTCPLen = 0;
				int nWSLen = 0;
				TCHAR tszTCPBuffer[4096];
				TCHAR tszWSBuffer[4096];

				memset(tszTCPBuffer, '\0', sizeof(tszTCPBuffer));
				memset(tszWSBuffer, '\0', sizeof(tszWSBuffer));

				st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_MSGNOTIFY;
				ProtocolModule_Packet_TCPCommon(&st_ProtocolHdr, &st_MQProtocol, tszTCPBuffer, &nTCPLen, ptszMsgBuffer, nJSLen);
				ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszWSBuffer, &nWSLen, ptszMsgBuffer, nJSLen);
				for (int i = 0; i < nListCount; i++)
				{
					if (ENUM_MQCORE_SESSION_CLIENT_TYPE_TCP == ppSt_ListAddr[i]->enClientType)
					{
						XEngine_MQXService_Send(ppSt_ListAddr[i]->tszNotifyAddr, tszTCPBuffer, nTCPLen, XENGINE_MQAPP_NETTYPE_TCP);
					}
					else if (ENUM_MQCORE_SESSION_CLIENT_TYPE_WEBSOCKET == ppSt_ListAddr[i]->enClientType)
					{
						XEngine_MQXService_Send(ppSt_ListAddr[i]->tszNotifyAddr, tszWSBuffer, nWSLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
					}
				}
				BaseLib_OperatorMemory_Free((XPPPMEM)&ppSt_ListAddr, nListCount);
			}
			BaseLib_OperatorMemory_FreeCStyle((XPPMEM)&ptszMsgBuffer);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("Websocket客户端:%s,主题:%s,序列:%lld,投递数据到消息队列成功,通知客户端个数:%d"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, nListCount);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQGET == st_ProtocolHdr.unOperatorCode)
		{
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
			if (!XMQModule_Packet_Get(&st_MQProtocol, tszMsgBuffer, &nRVLen))
			{
				st_ProtocolHdr.wReserve = 722;
				ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("Websocket客户端:%s,主题:%s,序列:%lld,获取消息数据失败,无法继续,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, XMQModule_GetLastError());
				return FALSE;
			}
			st_ProtocolHdr.wReserve = 0;
			ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen, tszMsgBuffer, nRVLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
			SessionModule_Client_ADDDelSerial(lpszClientAddr);       //移动序列号
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
				SessionModule_Notify_Delete(st_MQProtocol.tszMQKey, lpszClientAddr, ENUM_MQCORE_SESSION_CLIENT_TYPE_WEBSOCKET);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("Websocket客户端:%s,取消订阅成功,主题名称:%s"), lpszClientAddr, st_MQProtocol.tszMQKey);
			}
			else
			{
				if (!SessionModule_Notify_Insert(st_MQProtocol.tszMQKey, lpszClientAddr, ENUM_MQCORE_SESSION_CLIENT_TYPE_WEBSOCKET))
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
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQSERIAL == st_ProtocolHdr.unOperatorCode)
		{
			st_ProtocolHdr.wReserve = 0;
			st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPSERIAL;

			if (0 == st_MQProtocol.nSerial)
			{
				__int64x nFirstNumber = 0;
				__int64x nLastNumber = 0;
				if (!XMQModule_Packet_GetSerial(st_MQProtocol.tszMQKey, &nFirstNumber, &nLastNumber))
				{
					st_ProtocolHdr.wReserve = 712;
					ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
					XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("Websocket消息端:%s,获取主题序列编号失败,主题名称:%s,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, SessionModule_GetLastError());
					return FALSE;
				}
				st_MQProtocol.nKeepTime == 1 ? st_MQProtocol.nSerial = nFirstNumber : st_MQProtocol.nSerial = nLastNumber;
			}
			if (!SessionModule_Client_SetOrder(lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nKeepTime, st_MQProtocol.nSerial))
			{
				st_ProtocolHdr.wReserve = 712;
				ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("Websocket消息端:%s,设置消息队列读取模式失败,主题名称:%s,读取模式:%s,序列号:%lld,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nKeepTime == 1 ? "顺序" : "倒序", st_MQProtocol.nSerial, SessionModule_GetLastError());
				return FALSE;
			}
			ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("Websocket消息端:%s,请求设置序列号成功,主题名称:%s,读取模式:%s,序列号:%lld"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nKeepTime == 1 ? "顺序" : "倒序", st_MQProtocol.nSerial);
		}
		else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQNUMBER == st_ProtocolHdr.unOperatorCode)
		{
			XENGINE_MQNUMBER st_MQNumber;
			memset(&st_MQNumber, '\0', sizeof(XENGINE_MQNUMBER));
			st_ProtocolHdr.wReserve = 0;
			st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPNUMBER;

			_tcscpy(st_MQNumber.tszMQKey, st_MQProtocol.tszMQKey);
			if (!XMQModule_Packet_GetSerial(st_MQNumber.tszMQKey, &st_MQNumber.nFirstNumber, &st_MQNumber.nLastNumber))
			{
				st_ProtocolHdr.wReserve = 714;
				ProtocolModule_Packet_HttpCommon(&st_ProtocolHdr, &st_MQProtocol, tszSDBuffer, &nSDLen);
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("Websocket消息端:%s,获取主题序列编号失败,主题名称:%s,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, SessionModule_GetLastError());
				return FALSE;
			}
			XMQModule_Packet_GetCount(st_MQNumber.tszMQKey, &st_MQNumber.nCount);
			ProtocolModule_Packet_MQNumber(&st_ProtocolHdr, &st_MQNumber, tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("Websocket消息端:%s,获取主题序列编号成功,主题名称:%s,队列个数:%lld,开始编号:%lld,结尾编号:%lld"), lpszClientAddr, st_MQNumber.tszMQKey, st_MQNumber.nCount, st_MQNumber.nFirstNumber, st_MQNumber.nLastNumber);
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