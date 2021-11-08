#include "MQService_Hdr.h"

XHTHREAD CALLBACK MessageQueue_WebsocketThread(LPVOID lParam)
{
	int nThreadPos = *(int*)lParam;
	nThreadPos++;
	while (bIsRun)
	{
		if (!RfcComponents_WSPacket_WaitEventEx(xhWSPacket, nThreadPos))
		{
			continue;
		}
		int nListCount = 0;
		RFCCOMPONENTS_WSPKT_CLIENT** ppSst_ListAddr;

		RfcComponents_WSPacket_GetPoolEx(xhWSPacket, nThreadPos, &ppSst_ListAddr, &nListCount);
		for (int i = 0; i < nListCount; i++)
		{
			for (int j = 0; j < ppSst_ListAddr[i]->nPktCount; j++)
			{
				int nMsgLen = 0;
				TCHAR* ptszMsgBuffer = NULL;
				ENUM_XENGINE_RFCOMPONENTS_WEBSOCKET_OPCODE enOPCode;
				if (!RfcComponents_WSPacket_GetMemoryEx(xhWSPacket, ppSst_ListAddr[i]->tszClientAddr, &ptszMsgBuffer, &nMsgLen, &enOPCode))
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
	if (ENUM_XENGINE_RFCOMPONENTS_WEBSOCKET_OPCODE_TEXT == enOPCode)
	{
		int nPLen = 0;
		TCHAR* ptszMsgBuffer = NULL;
		XENGINE_PROTOCOLHDR st_ProtocolHdr;
		memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));

		ProtocolModule_Parse_Http(lpszMsgBuffer, nMsgLen, &st_ProtocolHdr, &ptszMsgBuffer, &nPLen);
		MessageQueue_TCP_Handle(&st_ProtocolHdr, lpszClientAddr, ptszMsgBuffer, nPLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
		BaseLib_OperatorMemory_FreeCStyle((XPPMEM)&ptszMsgBuffer);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("Websocket客户端:%s,协议错误"), lpszClientAddr);
	}
	return TRUE;
}