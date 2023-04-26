#include "MQService_Hdr.h"

XHTHREAD CALLBACK MessageQueue_HttpThread(XPVOID lParam)
{
	int nThreadPos = *(int*)lParam;
	nThreadPos++;
	while (bIsRun)
	{
		if (!HttpProtocol_Server_EventWaitEx(xhHTTPPacket, nThreadPos))
		{
			continue;
		}
		int nListCount = 0;
		RFCCOMPONENTS_HTTP_PKTCLIENT** ppSst_ListAddr;

		HttpProtocol_Server_GetPoolEx(xhHTTPPacket, nThreadPos, &ppSst_ListAddr, &nListCount);
		for (int i = 0; i < nListCount; i++)
		{
			for (int j = 0; j < ppSst_ListAddr[i]->nPktCount; j++)
			{
				int nMsgLen = 0;
				int nHdrCount = 0;
				XCHAR** ppszHdrList;
				XCHAR* ptszMsgBuffer = NULL;
				RFCCOMPONENTS_HTTP_REQPARAM st_HTTPReqparam;

				memset(&st_HTTPReqparam, '\0', sizeof(RFCCOMPONENTS_HTTP_REQPARAM));

				if (HttpProtocol_Server_GetMemoryEx(xhHTTPPacket, ppSst_ListAddr[i]->tszClientAddr, &ptszMsgBuffer, &nMsgLen, &st_HTTPReqparam, &ppszHdrList, &nHdrCount))
				{
					MessageQueue_Http_Handle(&st_HTTPReqparam, ppSst_ListAddr[i]->tszClientAddr, ptszMsgBuffer, nMsgLen, ppszHdrList, nHdrCount);
					BaseLib_OperatorMemory_FreeCStyle((VOID**)&ptszMsgBuffer);
					BaseLib_OperatorMemory_Free((XPPPMEM)&ppszHdrList, nHdrCount);
				}
			}
		}
		BaseLib_OperatorMemory_Free((XPPPMEM)&ppSst_ListAddr, nListCount);
	}
	return 0;
}
bool MessageQueue_Http_Handle(RFCCOMPONENTS_HTTP_REQPARAM* pSt_HTTPParam, LPCXSTR lpszClientAddr, LPCXSTR lpszMsgBuffer, int nMsgLen, XCHAR** pptszListHdr, int nHdrCount)
{
	LPCXSTR lpszPostMethod = _X("POST");
	LPCXSTR lpszGetMethod = _X("GET");
	if (0 == _tcsxnicmp(lpszPostMethod, pSt_HTTPParam->tszHttpMethod, _tcsxlen(lpszPostMethod)))
	{
		int nPLen = 0;
		XCHAR tszMsgBuffer[4096];
		XENGINE_PROTOCOLHDR st_ProtocolHdr;

		memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
		memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));

		ProtocolModule_Parse_Http(lpszMsgBuffer, nMsgLen, &st_ProtocolHdr, tszMsgBuffer, &nPLen);
		MessageQueue_TCP_Handle(&st_ProtocolHdr, lpszClientAddr, tszMsgBuffer, nPLen, XENGINE_MQAPP_NETTYPE_HTTP);
	}
	else if (0 == _tcsxnicmp(lpszGetMethod, pSt_HTTPParam->tszHttpMethod, _tcsxlen(lpszGetMethod)))
	{
		//http://127.0.0.1:5201/api?function=get&token=112&key=xengine&serial=1
		HttpProtocol_ServerHelp_GetParament(pSt_HTTPParam->tszHttpUri, &pptszListHdr, &nHdrCount);
		if (4 != nHdrCount)
		{
			int nPKTLen = 8196;
			XCHAR tszPKTBuffer[8196];
			RFCCOMPONENTS_HTTP_HDRPARAM st_HTTPHdr;

			memset(tszPKTBuffer, '\0', sizeof(tszPKTBuffer));
			memset(&st_HTTPHdr, '\0', sizeof(RFCCOMPONENTS_HTTP_HDRPARAM));

			st_HTTPHdr.nHttpCode = 400;
			st_HTTPHdr.bIsClose = true;

			HttpProtocol_Server_SendMsgEx(xhHTTPPacket, tszPKTBuffer, &nPKTLen, &st_HTTPHdr);
			NetCore_TCPXCore_SendEx(xhHTTPSocket, lpszClientAddr, tszPKTBuffer, nPKTLen);
			return false;
		}
		XCHAR tszKey[128];
		XCHAR tszValue[128];
		XENGINE_PROTOCOLHDR st_ProtocolHdr;
		XENGINE_PROTOCOL_XMQ st_MQProtocol;

		memset(tszKey, '\0', sizeof(tszKey));
		memset(tszValue, '\0', sizeof(tszValue));
		memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
		memset(&st_MQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

		BaseLib_OperatorString_GetKeyValue(pptszListHdr[1], "=", tszKey, tszValue);

		st_ProtocolHdr.xhToken = _ttxoll(tszValue);
		st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
		st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
		st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQGET;
		st_ProtocolHdr.xhToken = sizeof(XENGINE_PROTOCOL_XMQ);
		st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

		BaseLib_OperatorString_GetKeyValue(pptszListHdr[2], "=", tszKey, st_MQProtocol.tszMQKey);
		BaseLib_OperatorString_GetKeyValue(pptszListHdr[3], "=", tszKey, tszValue);
		st_MQProtocol.nSerial = _ttxoll(tszValue);

		MessageQueue_TCP_Handle(&st_ProtocolHdr, lpszClientAddr, (LPCXSTR)&st_MQProtocol, sizeof(XENGINE_PROTOCOL_XMQ), XENGINE_MQAPP_NETTYPE_HTTP);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("HTTP消息端:%s,协议错误"), lpszClientAddr);
	}
	return true;
}