#include "MQService_Hdr.h"

XHTHREAD XCALLBACK MessageQueue_WebsocketThread(XPVOID lParam)
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
		XENGINE_MANAGEPOOL_TASKEVENT** ppSst_ListAddr;

		RfcComponents_WSPacket_GetPoolEx(xhWSPacket, nThreadPos, &ppSst_ListAddr, &nListCount);
		for (int i = 0; i < nListCount; i++)
		{
			for (int j = 0; j < ppSst_ListAddr[i]->nPktCount; j++)
			{
				int nMsgLen = 0;
				XCHAR* ptszMsgBuffer = NULL;
				ENUM_XENGINE_RFCOMPONENTS_WEBSOCKET_OPCODE enOPCode;
				if (RfcComponents_WSPacket_GetMemoryEx(xhWSPacket, ppSst_ListAddr[i]->tszClientAddr, &ptszMsgBuffer, &nMsgLen, &enOPCode))
				{
					MessageQueue_Websocket_Handle(ppSst_ListAddr[i]->tszClientAddr, ptszMsgBuffer, nMsgLen, enOPCode);
					BaseLib_Memory_FreeCStyle((XPPMEM)&ptszMsgBuffer);
				}
			}
		}
		BaseLib_Memory_Free((XPPPMEM)&ppSst_ListAddr, nListCount);
	}
	return 0;
}
bool MessageQueue_Websocket_Handle(LPCXSTR lpszClientAddr, LPCXSTR lpszMsgBuffer, int nMsgLen, ENUM_XENGINE_RFCOMPONENTS_WEBSOCKET_OPCODE enOPCode)
{
	if (ENUM_XENGINE_RFCOMPONENTS_WEBSOCKET_OPCODE_CLOSE == enOPCode)
	{
		int nSDLen = 0;
		XCHAR tszMSGBuffer[XPATH_MAX] = {};
		RfcComponents_WSCodec_EncodeMsg(NULL, tszMSGBuffer, &nSDLen, ENUM_XENGINE_RFCOMPONENTS_WEBSOCKET_OPCODE_CLOSE);
		NetCore_TCPXCore_SendEx(xhWSSocket, lpszClientAddr, tszMSGBuffer, nSDLen);
	}
	else if (ENUM_XENGINE_RFCOMPONENTS_WEBSOCKET_OPCODE_PING == enOPCode)
	{
		int nSDLen = 0;
		XCHAR tszMSGBuffer[XPATH_MAX] = {};
		RfcComponents_WSCodec_EncodeMsg(NULL, tszMSGBuffer, &nSDLen, ENUM_XENGINE_RFCOMPONENTS_WEBSOCKET_OPCODE_PONG);
		NetCore_TCPXCore_SendEx(xhWSSocket, lpszClientAddr, tszMSGBuffer, nSDLen);
	}
	if (ENUM_XENGINE_RFCOMPONENTS_WEBSOCKET_OPCODE_TEXT == enOPCode)
	{
		int nPLen = 0;
		XCHAR tszMsgBuffer[4096];
		XENGINE_PROTOCOLHDR st_ProtocolHdr;

		memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
		memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));

		ProtocolModule_Parse_Websocket(lpszMsgBuffer, nMsgLen, &st_ProtocolHdr, tszMsgBuffer, &nPLen);
		MessageQueue_TCP_Handle(&st_ProtocolHdr, lpszClientAddr, tszMsgBuffer, nPLen, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("Websocket客户端:%s,协议:%d 错误"), lpszClientAddr, enOPCode);
	}
	return true;
}