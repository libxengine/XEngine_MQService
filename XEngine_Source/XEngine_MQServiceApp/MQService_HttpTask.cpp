﻿#include "MQService_Hdr.h"

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
	LPCXSTR lpszAPIOnline = _X("online");
	LPCXSTR lpszAPIDelete = _X("delete");

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
	int nMethodPos = 0;
	//判断是否需要验证
	if (st_ServiceCfg.st_XAuthorize.bHTTPAuth)
	{
		//http://127.0.0.1:5202/api?function=user&auth=XENGINE_MQPasskey
		BaseLib_String_GetKeyValue(ppSt_ListUrl[1], _X("="), tszKey, tszValue);
		if (_tcsxlen(st_ServiceCfg.st_XAuthorize.tszHTTPPass) > 0)
		{
			int nHTTPCode = 0;
			ProtocolModule_Packet_PassHttp(tszPKTBuffer, &nPKTLen, pptszListHdr[1], pptszListHdr[0], pSt_HTTPParam->tszHttpMethod);
			APIClient_Http_Request("POST", st_ServiceCfg.st_XAuthorize.tszHTTPPass, tszPKTBuffer, &nHTTPCode);
			if (200 != nHTTPCode)
			{
				ProtocolModule_Packet_Http(tszPKTBuffer, &nPKTLen, ERROR_XENGINE_MESSAGE_HTTP_AUTHORIZE, "auth key is incorrent");
				XEngine_MQXService_Send(lpszClientAddr, tszPKTBuffer, nPKTLen, XENGINE_MQAPP_NETTYPE_HTTP);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求HTTP管理接口失败,验证失败:%s"), lpszClientAddr, pSt_HTTPParam->tszHttpUri);
				return false;
			}
		}
		else
		{
			if (0 != _tcsxncmp(st_ServiceCfg.st_XAuthorize.tszToken, tszValue, _tcsxlen(st_ServiceCfg.st_XAuthorize.tszToken)))
			{
				ProtocolModule_Packet_Http(tszPKTBuffer, &nPKTLen, ERROR_XENGINE_MESSAGE_HTTP_AUTHORIZE, "auth key is incorrent");
				XEngine_MQXService_Send(lpszClientAddr, tszPKTBuffer, nPKTLen, XENGINE_MQAPP_NETTYPE_HTTP);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求HTTP管理接口失败,验证失败:%s"), lpszClientAddr, pSt_HTTPParam->tszHttpUri);
				return false;
			}
		}
		nMethodPos++;
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,请求HTTP管理接口验证成功,验证TOKEN:%s"), lpszClientAddr, ppSt_ListUrl[1]);
	}
	//判断请求
	if (0 == _tcsxnicmp(lpszPostMethod, pSt_HTTPParam->tszHttpMethod, _tcsxlen(lpszPostMethod)))
	{

	}
	else if (0 == _tcsxnicmp(lpszGetMethod, pSt_HTTPParam->tszHttpMethod, _tcsxlen(lpszGetMethod)))
	{
		//获取函数
		memset(tszValue, '\0', MAX_PATH);
		BaseLib_String_GetKeyValue(ppSt_ListUrl[0], _X("="), tszKey, tszValue);
		if (0 == _tcsxnicmp(lpszAPIUser, tszValue, _tcsxlen(lpszAPIUser)))
		{
			//用户 http://127.0.0.1:5202/api?function=user
			int nListCount = 0;
			XENGINE_PROTOCOL_USERINFO** ppSt_UserInfo;
			DBModule_MQUser_UserList(&ppSt_UserInfo, &nListCount);
			ProtocolModule_Packet_UserList(tszPKTBuffer, &nPKTLen, &ppSt_UserInfo, nListCount);
			BaseLib_Memory_Free((XPPPMEM)&ppSt_UserInfo, nListCount);

			XEngine_MQXService_Send(lpszClientAddr, tszPKTBuffer, nPKTLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,发送的获取用户列表请求成功,获取到的用户列表个数:%d"), lpszClientAddr, nListCount);
		}
		else if (0 == _tcsxnicmp(lpszAPIOnline, tszValue, _tcsxlen(lpszAPIOnline)))
		{
			//获取在线用户 http://127.0.0.1:5202/api?function=online&type=0
			int nListCount = 0;
			XCHAR** pptszListAddr;

			BaseLib_String_GetKeyValue(ppSt_ListUrl[nMethodPos + 1], _X("="), tszKey, tszValue);
			SessionModule_Client_GetListAddr(&pptszListAddr, &nListCount, _ttxoi(tszValue));
			ProtocolModule_Packet_OnlineList(tszPKTBuffer, &nPKTLen, &pptszListAddr, nListCount);
			BaseLib_Memory_Free((XPPPMEM)&pptszListAddr, nListCount);
			XEngine_MQXService_Send(lpszClientAddr, tszPKTBuffer, nPKTLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,发送的获取在线用户列表请求成功,获取到的列表个数:%d"), lpszClientAddr, nListCount);
		}
		else if (0 == _tcsxnicmp(lpszAPITopic, tszValue, _tcsxlen(lpszAPITopic)))
		{
			//主题 http://127.0.0.1:5202/api?function=topic&name=comm
			if (2 == nUrlCount)
			{
				int nListCount = 0;
				XCHAR** ppszTableName;
				DBModule_MQData_ShowTable(&ppszTableName, &nListCount);
				ProtocolModule_Packet_TopicList(tszPKTBuffer, &nPKTLen, &ppszTableName, nListCount);
				BaseLib_Memory_Free((XPPPMEM)&ppszTableName, nListCount);
				XEngine_MQXService_Send(lpszClientAddr, tszPKTBuffer, nPKTLen, XENGINE_MQAPP_NETTYPE_HTTP);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,发送的获取主题列表请求成功,获取到的主题列表个数:%d"), lpszClientAddr, nListCount);
			}
			else
			{
				int nDBCount = 0;
				BaseLib_String_GetKeyValue(ppSt_ListUrl[nMethodPos + 1], _X("="), tszKey, tszValue);
				DBModule_MQData_GetLeftCount(tszValue, 0, &nDBCount);
				ProtocolModule_Packet_TopicName(tszPKTBuffer, &nPKTLen, tszValue, nDBCount);
				XEngine_MQXService_Send(lpszClientAddr, tszPKTBuffer, nPKTLen, XENGINE_MQAPP_NETTYPE_HTTP);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,发送的获取主题列表请求成功,获取到的主题消息个数:%d"), lpszClientAddr, nDBCount);
			}
		}
		else if (0 == _tcsxnicmp(lpszAPIDelete, tszValue, _tcsxlen(lpszAPIDelete)))
		{
			//http://127.0.0.1:5202/api?function=delete&type=0&name=comm

			memset(tszValue, '\0', MAX_PATH);
			BaseLib_String_GetKeyValue(ppSt_ListUrl[nMethodPos + 2], _X("="), tszKey, tszValue);
			if (0 == _ttxoi(tszValue))
			{
				//type = 0 删除主题
				memset(tszValue, '\0', MAX_PATH);
				BaseLib_String_GetKeyValue(ppSt_ListUrl[nMethodPos + 3], _X("="), tszKey, tszValue);

				XENGINE_DBTOPICOWNER st_DBOwner = {};
				XENGINE_DBUSERKEY st_UserKey = {};
				XENGINE_DBTIMERELEASE st_DBInfo = {};

				_tcsxcpy(st_DBOwner.tszQueueName, tszValue);
				_tcsxcpy(st_UserKey.tszKeyName, tszValue);
				_tcsxcpy(st_DBInfo.tszQueueName, tszValue);
				if (!DBModule_MQUser_OwnerDelete(&st_DBOwner))
				{
					ProtocolModule_Packet_Http(tszPKTBuffer, &nPKTLen, ERROR_XENGINE_MESSAGE_HTTP_NOTFOUND, "topic name not found");
					XEngine_MQXService_Send(lpszClientAddr, tszPKTBuffer, nPKTLen, XENGINE_MQAPP_NETTYPE_HTTP);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求HTTP删除主题失败,主题不存在:%s"), lpszClientAddr, tszValue);
					return false;
				}
				//清楚数据库
				APIHelp_Counter_SerialDel(tszValue);
				DBModule_MQData_DeleteTable(tszValue);
				DBModule_MQUser_KeyDelete(&st_UserKey);
				DBModule_MQUser_TimeDelete(&st_DBInfo);

				ProtocolModule_Packet_Http(tszPKTBuffer, &nPKTLen);
				XEngine_MQXService_Send(lpszClientAddr, tszPKTBuffer, nPKTLen, XENGINE_MQAPP_NETTYPE_HTTP);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,请求主题删除成功,主题名:%s"), lpszClientAddr, tszValue);
			}
			else
			{
				// 删除用户
				memset(tszValue, '\0', MAX_PATH);
				BaseLib_String_GetKeyValue(ppSt_ListUrl[nMethodPos + 3], _X("="), tszKey, tszValue);

				XENGINE_PROTOCOL_USERINFO st_UserInfo = {};
				XENGINE_DBUSERKEY st_UserKey = {};
				XENGINE_DBTOPICOWNER st_DBOwner = {};

				_tcsxcpy(st_UserInfo.tszUserName, tszValue);
				_tcsxcpy(st_UserKey.tszUserName, tszValue);
				_tcsxcpy(st_DBOwner.tszUserName, tszValue);

				if (!DBModule_MQUser_UserQuery(&st_UserInfo))
				{
					ProtocolModule_Packet_Http(tszPKTBuffer, &nPKTLen, ERROR_XENGINE_MESSAGE_HTTP_NOTFOUND, "user name not found");
					XEngine_MQXService_Send(lpszClientAddr, tszPKTBuffer, nPKTLen, XENGINE_MQAPP_NETTYPE_HTTP);
					XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求HTTP删除用户失败,用户不存在:%s"), lpszClientAddr, st_UserInfo.tszUserName);
					return false;
				}
				DBModule_MQUser_UserDelete(&st_UserInfo);
				DBModule_MQUser_KeyDelete(&st_UserKey);
				DBModule_MQUser_OwnerDelete(&st_DBOwner);

				ProtocolModule_Packet_Http(tszPKTBuffer, &nPKTLen);
				XEngine_MQXService_Send(lpszClientAddr, tszPKTBuffer, nPKTLen, XENGINE_MQAPP_NETTYPE_HTTP);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,请求用户删除成功,用户名:%s"), lpszClientAddr, st_UserInfo.tszUserName);
			}
		}
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("HTTP客户端:%s,协议错误"), lpszClientAddr);
	}
	return true;
}