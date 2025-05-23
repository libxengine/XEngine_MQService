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
		XENGINE_MANAGEPOOL_TASKEVENT** ppSst_ListAddr;

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
					BaseLib_Memory_FreeCStyle((VOID**)&ptszMsgBuffer);
					BaseLib_Memory_Free((XPPPMEM)&ppszHdrList, nHdrCount);
				}
			}
		}
		BaseLib_Memory_Free((XPPPMEM)&ppSst_ListAddr, nListCount);
	}
	return 0;
}
bool MessageQueue_Http_Handle(RFCCOMPONENTS_HTTP_REQPARAM* pSt_HTTPParam, LPCXSTR lpszClientAddr, LPCXSTR lpszMsgBuffer, int nMsgLen, XCHAR** pptszListHdr, int nHdrCount)
{
	int nPKTLen = 4096;
	XCHAR tszPKTBuffer[4096] = {};
	XCHAR tszKey[MAX_PATH] = {};
	XCHAR tszValue[MAX_PATH] = {};

	LPCXSTR lpszPostMethod = _X("POST");
	LPCXSTR lpszGetMethod = _X("GET");

	int nUrlCount = 0;
	XCHAR** ppSt_ListUrl;
	HttpProtocol_ServerHelp_GetParament(pSt_HTTPParam->tszHttpUri, &ppSt_ListUrl, &nUrlCount);
	if (nUrlCount < 1)
	{
		ProtocolModule_Packet_Http(tszPKTBuffer, &nPKTLen, ERROR_XENGINE_MESSAGE_HTTP_PARAMENT, "url parament is incorrent");
		XEngine_MQXService_Send(lpszClientAddr, tszPKTBuffer, nPKTLen, XENGINE_MQAPP_NETTYPE_HTTP);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求HTTP管理接口失败,参数错误:%s"), lpszClientAddr, pSt_HTTPParam->tszHttpUri);
		return false;
	}
	//判断请求
	if (0 == _tcsxnicmp(lpszPostMethod, pSt_HTTPParam->tszHttpMethod, _tcsxlen(lpszPostMethod)))
	{
		BaseLib_String_GetKeyValue(ppSt_ListUrl[0], _X("="), tszKey, tszValue);
		MessageQueue_HttpTask_Post(lpszClientAddr, tszValue, lpszMsgBuffer, nMsgLen);
	}
	else if (0 == _tcsxnicmp(lpszGetMethod, pSt_HTTPParam->tszHttpMethod, _tcsxlen(lpszGetMethod)))
	{
		BaseLib_String_GetKeyValue(ppSt_ListUrl[0], _X("="), tszKey, tszValue);
		MessageQueue_HttpTask_Get(lpszClientAddr, tszValue, &ppSt_ListUrl, nUrlCount);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("HTTP客户端:%s,协议错误"), lpszClientAddr);
	}
	return true;
}