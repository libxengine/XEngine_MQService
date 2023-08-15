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
	int nPKTLen = 4096;
	XCHAR tszPKTBuffer[4096];
	XCHAR tszKey[MAX_PATH];
	XCHAR tszValue[MAX_PATH];

	memset(tszKey, '\0', MAX_PATH);
	memset(tszValue, '\0', MAX_PATH);
	memset(tszPKTBuffer, '\0', sizeof(tszPKTBuffer));

	LPCXSTR lpszPostMethod = _X("POST");
	LPCXSTR lpszGetMethod = _X("GET");
	LPCXSTR lpszAPIGet = _X("get");
	LPCXSTR lpszAPIUser = _X("user");
	LPCXSTR lpszAPITopic = _X("topic");

	if (0 == _tcsxnicmp(lpszPostMethod, pSt_HTTPParam->tszHttpMethod, _tcsxlen(lpszPostMethod)))
	{
	}
	else if (0 == _tcsxnicmp(lpszGetMethod, pSt_HTTPParam->tszHttpMethod, _tcsxlen(lpszGetMethod)))
	{
		int nUrlCount = 0;
		XCHAR** ppSt_ListUrl;
		HttpProtocol_ServerHelp_GetParament(pSt_HTTPParam->tszHttpUri, &ppSt_ListUrl, &nUrlCount);
		if (nUrlCount < 1)
		{
			ProtocolModule_Packet_Http(tszPKTBuffer, &nPKTLen, 400, "url parament is incorrent");
			XEngine_MQXService_Send(lpszClientAddr, tszPKTBuffer, nPKTLen, XENGINE_MQAPP_NETTYPE_HTTP);
			return false;
		}
		//获取函数
		BaseLib_OperatorString_GetKeyValue(ppSt_ListUrl[0], _X("="), tszKey, tszValue);
		if (0 == _tcsxnicmp(lpszAPIGet, tszValue, _tcsxlen(lpszAPIGet)))
		{
			memset(tszValue, '\0', MAX_PATH);
			BaseLib_OperatorString_GetKeyValue(ppSt_ListUrl[1], _X("="), tszKey, tszValue);
			if (0 == _tcsxnicmp(lpszAPIUser, tszValue, _tcsxlen(lpszAPIUser)))
			{
				//用户 http://127.0.0.1:5202/api?function=get&method=user
				int nListCount = 0;
				XENGINE_PROTOCOL_USERINFO** ppSt_UserInfo;
				DBModule_MQUser_UserList(&ppSt_UserInfo, &nListCount);
				ProtocolModule_Packet_UserList(tszPKTBuffer, &nPKTLen, &ppSt_UserInfo, nListCount);
				BaseLib_OperatorMemory_Free((XPPPMEM)&ppSt_UserInfo, nListCount);

				XEngine_MQXService_Send(lpszClientAddr, tszPKTBuffer, nPKTLen, XENGINE_MQAPP_NETTYPE_HTTP);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,发送的获取用户列表请求成功,获取到的用户列表个数:%d"), lpszClientAddr, nListCount);
			}
			else if (0 == _tcsxnicmp(lpszAPITopic, tszValue, _tcsxlen(lpszAPITopic)))
			{
				//主题 http://127.0.0.1:5202/api?function=get&method=topic
				int nListCount = 0;
				XCHAR** ppszTableName;
				DBModule_MQData_ShowTable(&ppszTableName, &nListCount);
				ProtocolModule_Packet_TopicList(tszPKTBuffer, &nPKTLen, &ppszTableName, nListCount);
				BaseLib_OperatorMemory_Free((XPPPMEM)&ppszTableName, nListCount);

				XEngine_MQXService_Send(lpszClientAddr, tszPKTBuffer, nPKTLen, XENGINE_MQAPP_NETTYPE_HTTP);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,发送的获取主题列表请求成功,获取到的主题列表个数:%d"), lpszClientAddr, nListCount);
			}
		}
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("HTTP消息端:%s,协议错误"), lpszClientAddr);
	}
	return true;
}