#include "../MQService_Hdr.h"
/********************************************************************
//    Created:     2025/05/20  16:33:21
//    File Name:   D:\XEngine_MQService\XEngine_Source\XEngine_MQServiceApp\MQService_HTTPTask\MQService_HTTPGet.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\XEngine_MQServiceApp\MQService_HTTPTask
//    File Base:   MQService_HTTPGet
//    File Ext:    cpp
//    Project:     XEngine
//    Author:      qyt
//    Purpose:     HTTP GET方法处理
//    History:
*********************************************************************/
bool MessageQueue_HttpTask_Get(LPCXSTR lpszClientAddr, LPCXSTR lpszFuncName, LPCXSTR lpszMsgBuffer, int nMsgLen, int nMethodPos, XCHAR*** pptszListHdr, int nUrlCount)
{
	int nSDLen = 0;
	XCHAR tszSDBuffer[1024] = {};
	XCHAR tszKeyStr[MAX_PATH] = {};
	XCHAR tszVluStr[MAX_PATH] = {};
	LPCXSTR lpszAPIGet = _X("get");
	LPCXSTR lpszAPIUser = _X("user");
	LPCXSTR lpszAPITopic = _X("topic");
	LPCXSTR lpszAPIOnline = _X("online");
	LPCXSTR lpszAPIDelete = _X("delete");
	
	if (0 == _tcsxnicmp(lpszAPIUser, lpszFuncName, _tcsxlen(lpszAPIUser)))
	{
		//用户 http://127.0.0.1:5202/api?function=user&token=0
		int nListCount = 0;
		XENGINE_PROTOCOL_USERINFO** ppSt_UserInfo;
		DBModule_MQUser_UserList(&ppSt_UserInfo, &nListCount);
		ProtocolModule_Packet_UserList(tszSDBuffer, &nSDLen, &ppSt_UserInfo, nListCount);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
		BaseLib_Memory_Free((XPPPMEM)&ppSt_UserInfo, nListCount);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,发送的获取用户列表请求成功,获取到的用户列表个数:%d"), lpszClientAddr, nListCount);
	}
	else if (0 == _tcsxnicmp(lpszAPIOnline, lpszFuncName, _tcsxlen(lpszAPIOnline)))
	{
		//获取在线用户 http://127.0.0.1:5202/api?function=online&token=0&type=0
		int nListCount = 0;
		XCHAR** pptszListAddr;

		BaseLib_String_GetKeyValue((*pptszListHdr)[nMethodPos + 1], _X("="), tszKeyStr, tszVluStr);
		SessionModule_Client_GetListAddr(&pptszListAddr, &nListCount, _ttxoi(tszVluStr));
		ProtocolModule_Packet_OnlineList(tszSDBuffer, &nSDLen, &pptszListAddr, nListCount);
		BaseLib_Memory_Free((XPPPMEM)&pptszListAddr, nListCount);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,发送的获取在线用户列表请求成功,获取到的列表个数:%d"), lpszClientAddr, nListCount);
	}
	else if (0 == _tcsxnicmp(lpszAPITopic, lpszFuncName, _tcsxlen(lpszAPITopic)))
	{
		//主题 http://127.0.0.1:5202/api?function=topic&name=comm
		if (2 == nUrlCount)
		{
			int nListCount = 0;
			XCHAR** ppszTableName;
			DBModule_MQData_ShowTable(&ppszTableName, &nListCount);
			ProtocolModule_Packet_TopicList(tszSDBuffer, &nSDLen, &ppszTableName, nListCount);
			BaseLib_Memory_Free((XPPPMEM)&ppszTableName, nListCount);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,发送的获取主题列表请求成功,获取到的主题列表个数:%d"), lpszClientAddr, nListCount);
		}
		else
		{
			int nDBCount = 0;
			BaseLib_String_GetKeyValue((*pptszListHdr)[nMethodPos + 1], _X("="), tszKeyStr, tszVluStr);
			DBModule_MQData_GetLeftCount(tszVluStr, 0, &nDBCount);
			ProtocolModule_Packet_TopicName(tszSDBuffer, &nSDLen, tszVluStr, nDBCount);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,发送的获取主题列表请求成功,获取到的主题消息个数:%d"), lpszClientAddr, nDBCount);
		}
	}
	else if (0 == _tcsxnicmp(lpszAPIDelete, lpszFuncName, _tcsxlen(lpszAPIDelete)))
	{
		//http://127.0.0.1:5202/api?function=delete&type=0&name=comm
		BaseLib_String_GetKeyValue((*pptszListHdr)[nMethodPos + 2], _X("="), tszKeyStr, tszVluStr);
		if (0 == _ttxoi(tszVluStr))
		{
			//type = 0 删除主题
			memset(tszVluStr, '\0', sizeof(tszVluStr));
			BaseLib_String_GetKeyValue((*pptszListHdr)[nMethodPos + 3], _X("="), tszKeyStr, tszVluStr);

			XENGINE_DBTOPICOWNER st_DBOwner = {};
			XENGINE_DBUSERKEY st_UserKey = {};
			XENGINE_DBTIMERELEASE st_DBInfo = {};

			_tcsxcpy(st_DBOwner.tszQueueName, tszVluStr);
			_tcsxcpy(st_UserKey.tszKeyName, tszVluStr);
			_tcsxcpy(st_DBInfo.tszQueueName, tszVluStr);
			if (!DBModule_MQUser_OwnerDelete(&st_DBOwner))
			{
				ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_NOTFOUND, "topic name not found");
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求HTTP删除主题失败,主题不存在:%s"), lpszClientAddr, tszVluStr);
				return false;
			}
			//清楚数据库
			APIHelp_Counter_SerialDel(tszVluStr);
			DBModule_MQData_DeleteTable(tszVluStr);
			DBModule_MQUser_KeyDelete(&st_UserKey);
			DBModule_MQUser_TimeDelete(&st_DBInfo);

			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,请求主题删除成功,主题名:%s"), lpszClientAddr, tszVluStr);
		}
		else
		{
			// 删除用户
			memset(tszVluStr, '\0', sizeof(tszVluStr));
			BaseLib_String_GetKeyValue((*pptszListHdr)[nMethodPos + 3], _X("="), tszKeyStr, tszVluStr);

			XENGINE_PROTOCOL_USERINFO st_UserInfo = {};
			XENGINE_DBUSERKEY st_UserKey = {};
			XENGINE_DBTOPICOWNER st_DBOwner = {};

			_tcsxcpy(st_UserInfo.tszUserName, tszVluStr);
			_tcsxcpy(st_UserKey.tszUserName, tszVluStr);
			_tcsxcpy(st_DBOwner.tszUserName, tszVluStr);

			if (!DBModule_MQUser_UserQuery(&st_UserInfo))
			{
				ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_NOTFOUND, "user name not found");
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求HTTP删除用户失败,用户不存在:%s"), lpszClientAddr, st_UserInfo.tszUserName);
				return false;
			}
			DBModule_MQUser_UserDelete(&st_UserInfo);
			DBModule_MQUser_KeyDelete(&st_UserKey);
			DBModule_MQUser_OwnerDelete(&st_DBOwner);

			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,请求用户删除成功,用户名:%s"), lpszClientAddr, st_UserInfo.tszUserName);
		}
	}
	return true;
}