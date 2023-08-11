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
	}
	else if (0 == _tcsxnicmp(lpszGetMethod, pSt_HTTPParam->tszHttpMethod, _tcsxlen(lpszGetMethod)))
	{
		//http://127.0.0.1:5201/api?function=get&token=112&key=xengine&serial=1
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("HTTP消息端:%s,协议错误"), lpszClientAddr);
	}
	return true;
}