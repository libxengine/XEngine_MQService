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
				BaseLib_OperatorMemory_Free((XPPPMEM)&ppszHdrList, nHdrCount);
			}
		}
		BaseLib_OperatorMemory_Free((XPPPMEM)&ppSst_ListAddr, nListCount);
	}
	return 0;
}
void CALLBACK MessageQueue_HttpTime(LPCSTR lpszSessionStr, LPVOID lParam)
{
	XENGINE_PROTOCOL_USERINFO st_UserInfo;
	memset(&st_UserInfo, '\0', sizeof(XENGINE_PROTOCOL_USERINFO));
	if (SessionModule_Client_GetUser(lpszSessionStr, st_UserInfo.tszUserName))
	{
		DBModule_MQUser_UserUPDate(&st_UserInfo);
	}
	SessionModule_Client_Delete(lpszSessionStr);
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("HTTP消息端:%s,会话ID:%s,用户会话超时!"), st_UserInfo.tszUserName, lpszSessionStr);
}
BOOL MessageQueue_Http_Handle(RFCCOMPONENTS_HTTP_REQPARAM* pSt_HTTPParam, LPCTSTR lpszClientAddr, LPCTSTR lpszMsgBuffer, int nMsgLen, TCHAR** pptszListHdr, int nHdrCount)
{
	LPCTSTR lpszMethod = _T("POST");
	if (0 == _tcsnicmp(lpszMethod, pSt_HTTPParam->tszHttpMethod, _tcslen(lpszMethod)))
	{
		int nPLen = 0;
		TCHAR* ptszMsgBuffer = NULL;
		XENGINE_PROTOCOLHDR st_ProtocolHdr;
		memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));

		ProtocolModule_Parse_Http(lpszMsgBuffer, nMsgLen, &st_ProtocolHdr, &ptszMsgBuffer, &nPLen);
		MessageQueue_TCP_Handle(&st_ProtocolHdr, lpszClientAddr, ptszMsgBuffer, nPLen, XENGINE_MQAPP_NETTYPE_HTTP);
		BaseLib_OperatorMemory_FreeCStyle((XPPMEM)&ptszMsgBuffer);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _T("HTTP消息端:%s,协议错误"), lpszClientAddr);
	}
	return TRUE;
}