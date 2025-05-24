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
bool MessageQueue_HttpTask_Get(LPCXSTR lpszClientAddr, LPCXSTR lpszFuncName, XCHAR*** pptszListHdr, int nUrlCount)
{
	int nSDLen = 0;
	XCHAR tszSDBuffer[1024] = {};
	XCHAR tszKeyStr[MAX_PATH] = {};

	LPCXSTR lpszAPILogin = _X("login");
	LPCXSTR lpszAPIUPDate = _X("update");
	LPCXSTR lpszAPIClose = _X("close");

	if (0 == _tcsxnicmp(lpszAPILogin, lpszFuncName, _tcsxlen(lpszAPILogin)))
	{
		//http://app.xyry.org:5202/api?function=login&user=123123aa&pass=123123
		XNETHANDLE xhToken = 0;
		XENGINE_PROTOCOL_USERINFO st_UserInfo = {};

		BaseLib_String_GetKeyValue((*pptszListHdr)[1], "=", tszKeyStr, st_UserInfo.tszUserName);
		BaseLib_String_GetKeyValue((*pptszListHdr)[2], "=", tszKeyStr, st_UserInfo.tszUserPass);

		if (!DBModule_MQUser_UserQuery(&st_UserInfo))
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_NOTFOUND, "user or pass is incorrect");
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求本地验证失败,用户或者密码不正确,错误:%lX"), lpszClientAddr, SessionModule_GetLastError());
			return false;
		}
		//用户是否存在会话,存在就返回,并且更新TOKEN
		if (Session_Token_GetUser(st_UserInfo.tszUserName, st_UserInfo.tszUserPass, &xhToken))
		{
			XCHAR tszTokenStr[128] = {};
			_xstprintf(tszTokenStr, _X("%lld"), xhToken);
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, 0, tszTokenStr);
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,请求登录发现会话已经存在,直接返回TOKEN:%lld 成功"), lpszClientAddr, xhToken);
			return true;
		}
		//权限是否正确
		if (0 != st_UserInfo.nUserLevel)
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_AUTHORIZE, "permission error");
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端：%s，用户名：%s，登录失败，客户端权限不足够"), lpszClientAddr, st_UserInfo.tszUserName);
			return false;
		}
		if (0 == xhToken)
		{
			BaseLib_Handle_Create(&xhToken);
		}
		Session_Token_Insert(xhToken, &st_UserInfo);

		XCHAR tszTokenStr[128] = {};
		_xstprintf(tszTokenStr, _X("%lld"), xhToken);
		ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, 0, tszTokenStr);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,请求登录获得TOKEN:%lld 成功,用户级别:%d"), lpszClientAddr, xhToken, st_UserInfo.nUserLevel);
	}
	else if (0 == _tcsxnicmp(lpszAPIUPDate, lpszFuncName, _tcsxlen(lpszAPIUPDate)))
	{
		//http://app.xyry.org:5202/api?function=update&token=1000112345
		XCHAR tszUserToken[128];
		memset(tszUserToken, '\0', sizeof(tszUserToken));

		BaseLib_String_GetKeyValue((*pptszListHdr)[1], "=", tszKeyStr, tszUserToken);

		if (!Session_Token_UPDate(_ttxoll(tszUserToken)))
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_NOTFOUND, "token not found");
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端：%s，更新TOKEN失败，不存在的Token:%s"), lpszClientAddr, tszUserToken);
			return false;
		}
		ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, 0, tszUserToken);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,请求更新TOKEN:%s 成功"), lpszClientAddr, tszUserToken);
	}
	else if (0 == _tcsxnicmp(lpszAPIClose, lpszFuncName, _tcsxlen(lpszAPIClose)))
	{
		//http://app.xyry.org:5202/api?function=close&token=1000112345
		XCHAR tszUserToken[128];
		memset(tszUserToken, '\0', sizeof(tszUserToken));

		BaseLib_String_GetKeyValue((*pptszListHdr)[1], "=", tszKeyStr, tszUserToken);
		//主动关闭
		Session_Token_Delete(_ttxoll(tszUserToken));
		ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, 0, tszUserToken);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,请求关闭TOKEN:%s 成功"), lpszClientAddr, tszUserToken);
	}
	
	return true;
}