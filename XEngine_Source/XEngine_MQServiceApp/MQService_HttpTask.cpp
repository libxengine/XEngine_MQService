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
	LPCXSTR lpszMethod = _X("POST");
	if (0 == _tcsxnicmp(lpszMethod, pSt_HTTPParam->tszHttpMethod, _tcsxlen(lpszMethod)))
	{
		int nPLen = 0;
		XCHAR tszMsgBuffer[4096];
		XENGINE_PROTOCOLHDR st_ProtocolHdr;

		memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
		memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));

		ProtocolModule_Parse_Http(lpszMsgBuffer, nMsgLen, &st_ProtocolHdr, tszMsgBuffer, &nPLen);
		MessageQueue_TCP_Handle(&st_ProtocolHdr, lpszClientAddr, tszMsgBuffer, nPLen, XENGINE_MQAPP_NETTYPE_HTTP);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("HTTP消息端:%s,协议错误"), lpszClientAddr);
	}
	return true;
}