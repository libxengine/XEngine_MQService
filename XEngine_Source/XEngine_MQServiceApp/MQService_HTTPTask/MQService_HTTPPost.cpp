#include "../MQService_Hdr.h"
/********************************************************************
//    Created:     2025/05/20  15:20:30
//    File Name:   D:\XEngine_MQService\XEngine_Source\XEngine_MQServiceApp\MQService_HTTPTask\MQService_HTTPPost.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\XEngine_MQServiceApp\MQService_HTTPTask
//    File Base:   MQService_HTTPPost
//    File Ext:    cpp
//    Project:     XEngine
//    Author:      qyt
//    Purpose:     HTTP POST方法处理
//    History:
*********************************************************************/
bool MessageQueue_HttpTask_Post(LPCXSTR lpszClientAddr, LPCXSTR lpszFuncName, LPCXSTR lpszMsgBuffer, int nMsgLen)
{
	int nSDLen = 0;
	XCHAR tszSDBuffer[1024] = {};
	LPCXSTR lpszAPIRegister = _X("register");
	LPCXSTR lpszAPIDelete = _X("delete");
	
	//判断请求
	if (0 == _tcsxnicmp(lpszAPIRegister, lpszFuncName, _tcsxlen(lpszAPIRegister)))
	{
		XENGINE_PROTOCOL_USERINFO st_UserInfo = {};
		if (!ProtocolModule_Parse_Register(lpszMsgBuffer, nMsgLen, &st_UserInfo))
		{
			ProtocolModule_Packet_HTTPCommon(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_PARSE, _X("json load parse is failure"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求注册消息失败,负载内容错误:%s"), lpszClientAddr, lpszMsgBuffer);
			return false;
		}
		if (DBModule_MQUser_UserQuery(&st_UserInfo))
		{
			ProtocolModule_Packet_HTTPCommon(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_EXISTED, _X("user is existed"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求用户注册失败,用户已经存在,错误:%lX"), lpszClientAddr, SessionModule_GetLastError());
			return false;
		}
		if (!DBModule_MQUser_UserInsert(&st_UserInfo))
		{
			ProtocolModule_Packet_HTTPCommon(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_SERVICE, _X("service is failure"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求用户注册失败,插入数据库失败,错误:%lX"), lpszClientAddr, SessionModule_GetLastError());
			return false;
		}
		//是否需要代理通知
		if (_tcsxlen(st_ServiceCfg.st_XPass.tszPassRegister) > 0)
		{
			int nHTTPCode = 0;
			XCLIENT_APIHTTP st_HTTPParament;
			memset(&st_HTTPParament, '\0', sizeof(XCLIENT_APIHTTP));

			st_HTTPParament.nTimeConnect = 2;
			ProtocolModule_Packet_PassUser(&st_UserInfo, tszSDBuffer, &nSDLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERREG);
			APIClient_Http_Request(_X("POST"), st_ServiceCfg.st_XPass.tszPassRegister, tszSDBuffer, &nHTTPCode, NULL, NULL, NULL, NULL, &st_HTTPParament);
			if (200 != nHTTPCode)
			{
				DBModule_MQUser_UserDelete(&st_UserInfo); //删除
				ProtocolModule_Packet_HTTPCommon(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_APIREG, _X("service is failure"));
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求远程注册失败,HTTPCode:%d"), lpszClientAddr, nHTTPCode);
				return false;
			}
		}
		//是否需要订阅公用消息队列
		if (st_DBConfig.st_MQData.bCommSub)
		{
			XENGINE_DBUSERKEY st_Userkey;
			memset(&st_Userkey, '\0', sizeof(XENGINE_DBUSERKEY));

			_tcsxcpy(st_Userkey.tszUserName, st_UserInfo.tszUserName);
			_tcsxcpy(st_Userkey.tszKeyName, st_ServiceCfg.tszTopic);
			//创建
			if (!DBModule_MQUser_KeyInsert(&st_Userkey))
			{
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("HTTP消息端:%s,绑定消息队列主题失败,主题名称:%s,错误：%lX"), lpszClientAddr, st_Userkey.tszKeyName, DBModule_GetLastError());
			}
		}
		ProtocolModule_Packet_HTTPCommon(tszSDBuffer, &nSDLen);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,请求用户注册成功,用户名:%s,密码:%s"), lpszClientAddr, st_UserInfo.tszUserName, st_UserInfo.tszUserPass);
	}
	else if (0 == _tcsxnicmp(lpszAPIDelete, lpszFuncName, _tcsxlen(lpszAPIDelete)))
	{
		XENGINE_PROTOCOL_USERINFO st_UserInfo = {};

		if (!DBModule_MQUser_UserDelete(&st_UserInfo))
		{
			ProtocolModule_Packet_HTTPCommon(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_DELETE, _X("delete user failure"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求用户删除失败,删除数据库失败,错误:%lX"), lpszClientAddr, SessionModule_GetLastError());
			return false;
		}
		if (_tcsxlen(st_ServiceCfg.st_XPass.tszPassUNReg) > 0)
		{
			int nHTTPCode = 0;
			XCLIENT_APIHTTP st_HTTPParament;
			memset(&st_HTTPParament, '\0', sizeof(XCLIENT_APIHTTP));

			st_HTTPParament.nTimeConnect = 2;

			ProtocolModule_Packet_PassUser(&st_UserInfo, tszSDBuffer, &nSDLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERDEL);
			APIClient_Http_Request(_X("POST"), st_ServiceCfg.st_XPass.tszPassUNReg, tszSDBuffer, &nHTTPCode, NULL, NULL, NULL, NULL, &st_HTTPParament);
			if (200 != nHTTPCode)
			{
				ProtocolModule_Packet_HTTPCommon(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_APIDEL, _X("delete user failure"));
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求远程注销失败,错误:%lX,HTTPCode:%d"), lpszClientAddr, nHTTPCode);
			}
		}
		XENGINE_DBUSERKEY st_UserKey;
		XENGINE_DBTOPICOWNER st_DBOwner;

		memset(&st_UserKey, '\0', sizeof(XENGINE_DBUSERKEY));
		memset(&st_DBOwner, '\0', sizeof(XENGINE_DBTOPICOWNER));

		_tcsxcpy(st_UserKey.tszUserName, st_UserInfo.tszUserName);
		_tcsxcpy(st_DBOwner.tszUserName, st_UserInfo.tszUserName);

		DBModule_MQUser_KeyDelete(&st_UserKey);
		DBModule_MQUser_OwnerDelete(&st_DBOwner);
		ProtocolModule_Packet_HTTPCommon(tszSDBuffer, &nSDLen);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,请求用户删除成功,用户名:%s"), lpszClientAddr, st_UserInfo.tszUserName);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("HTTP客户端:%s,协议错误"), lpszClientAddr);
	}
	return true;
}