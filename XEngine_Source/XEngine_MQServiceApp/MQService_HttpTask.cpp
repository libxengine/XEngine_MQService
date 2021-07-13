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
		int nMsgLen = 4096;
		int nListCount = 0;
		TCHAR tszMsgBuffer[4096];
		RFCCOMPONENTS_HTTP_PKTCLIENT** ppSst_ListAddr;

		memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
		
		RfcComponents_HttpServer_GetPoolEx(xhHTTPPacket, nThreadPos, &ppSst_ListAddr, &nListCount);
		for (int i = 0; i < nListCount; i++)
		{
			for (int j = 0; j < ppSst_ListAddr[i]->nPktCount; j++)
			{
				int nHdrCount = 0;
				CHAR** ppszHdrList;
				RFCCOMPONENTS_HTTP_REQPARAM st_HTTPReqparam;

				memset(&st_HTTPReqparam, '\0', sizeof(RFCCOMPONENTS_HTTP_REQPARAM));

				if (!RfcComponents_HttpServer_GetClientEx(xhHTTPPacket, ppSst_ListAddr[i]->tszClientAddr, tszMsgBuffer, &nMsgLen, &st_HTTPReqparam, &ppszHdrList, &nHdrCount))
				{
					DWORD dwRet = HttpServer_GetLastError();
					if (ERROR_HELPCOMPONENTS_PACKETS_PROTOCOL_GET_ISNULL == dwRet)
					{
						XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("HTTP服务器获取消息失败，获取数据包失败，错误：%lX"), dwRet);
					}
					continue;
				}
				MessageQueue_Http_Handle(&st_HTTPReqparam, ppSst_ListAddr[i]->tszClientAddr, tszMsgBuffer, nMsgLen, ppszHdrList, nHdrCount);
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
	TCHAR tszSDBuffer[2048];
	TCHAR tszRVBuffer[2048];
	LPCTSTR lpszMethod = _T("POST");

	memset(tszSDBuffer, '\0', sizeof(tszSDBuffer));
	memset(tszRVBuffer, '\0', sizeof(tszRVBuffer));

	if (0 == _tcsnicmp(lpszMethod, pSt_HTTPParam->tszHttpMethod, _tcslen(lpszMethod)))
	{
		XENGINE_PROTOCOL_XMQ st_MQProtocol;
		XENGINE_PROTOCOL_XMQ st_MQClient;

		memset(&st_MQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));
		memset(&st_MQClient, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

		if (!SessionModule_Client_Get(lpszClientAddr, &st_MQClient))
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("HTTP消息端:%s,没有找到指定会话地址,无法继续,错误：%lX"), lpszClientAddr, SessionModule_GetLastError());
			return FALSE;
		}
		else
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("HTTP消息端:%s,协议错误"), lpszClientAddr);
		}
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("HTTP消息端:%s,协议错误"), lpszClientAddr);
	}
	return TRUE;
}