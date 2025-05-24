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
	XNETHANDLE xhToken = 0;
	XCHAR tszSDBuffer[1024] = {};
	XCHAR tszKeyStr[MAX_PATH] = {};
	XCHAR tszVluStr[MAX_PATH] = {};
	LPCXSTR lpszAPIRegister = _X("register");
	LPCXSTR lpszAPIGetUser = _X("getuser");
	LPCXSTR lpszAPIGetTopic = _X("gettopic");
	LPCXSTR lpszAPIGetList = _X("getlist");
	LPCXSTR lpszAPIGetOnline = _X("getonline");
	LPCXSTR lpszAPICreateTopic = _X("createtopic");
	LPCXSTR lpszAPIDelTopic = _X("deletetopic");
	LPCXSTR lpszAPIDelUser = _X("deleteuser");

	//判断是否需要验证，不是注册协议
	if (st_ServiceCfg.st_XAuthorize.bHTTPAuth && (0 != _tcsxnicmp(lpszAPIRegister, lpszFuncName, _tcsxlen(lpszAPIRegister))))
	{
		if (ProtocolModule_Parse_Token(lpszMsgBuffer, nMsgLen, &xhToken))
		{
			if (!Session_Token_Get(xhToken))
			{
				ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_AUTHORIZE, "not authorize");
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求的API:%s 失败,因为没有经过验证"), lpszClientAddr, lpszFuncName);
				return false;
			}
		}
	}
	//判断请求
	if (0 == _tcsxnicmp(lpszAPIRegister, lpszFuncName, _tcsxlen(lpszAPIRegister)))
	{
		XENGINE_PROTOCOL_USERINFO st_UserInfo = {};
		if (!ProtocolModule_Parse_Register(lpszMsgBuffer, nMsgLen, &st_UserInfo))
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_PARSE, _X("json load parse is failure"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求注册消息失败,负载内容错误:%s"), lpszClientAddr, lpszMsgBuffer);
			return false;
		}
		if (DBModule_MQUser_UserQuery(&st_UserInfo))
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_EXISTED, _X("user is existed"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求用户注册失败,用户已经存在,错误:%lX"), lpszClientAddr, SessionModule_GetLastError());
			return false;
		}
		st_UserInfo.nUserLevel = ENUM_XENGINE_PROTOCOLHDR_LEVEL_TYPE_USER;
		if (!DBModule_MQUser_UserInsert(&st_UserInfo))
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_SERVICE, _X("service is failure"));
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
				ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_APIREG, _X("service is failure"));
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
		ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,请求用户注册成功,用户名:%s,密码:%s"), lpszClientAddr, st_UserInfo.tszUserName, st_UserInfo.tszUserPass);
	}
	else if (0 == _tcsxnicmp(lpszAPIGetUser, lpszFuncName, _tcsxlen(lpszAPIGetUser)))
	{
		//用户 http://127.0.0.1:5202/api?function=getuser
		int nListCount = 0;
		XENGINE_PROTOCOL_USERINFO** ppSt_UserInfo;
		DBModule_MQUser_UserList(&ppSt_UserInfo, &nListCount);
		ProtocolModule_Packet_UserList(tszSDBuffer, &nSDLen, &ppSt_UserInfo, nListCount);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
		BaseLib_Memory_Free((XPPPMEM)&ppSt_UserInfo, nListCount);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,发送的获取用户列表请求成功,获取到的用户列表个数:%d"), lpszClientAddr, nListCount);
	}
	else if (0 == _tcsxnicmp(lpszAPIGetOnline, lpszFuncName, _tcsxlen(lpszAPIGetOnline)))
	{
		//获取在线用户 http://127.0.0.1:5202/api?function=getonline
		int nType = 0;
		int nListCount = 0;
		XCHAR** pptszListAddr;

		ProtocolModule_Parse_Type(lpszMsgBuffer, nMsgLen, &nType);
		SessionModule_Client_GetListAddr(&pptszListAddr, &nListCount, nType);
		ProtocolModule_Packet_OnlineList(tszSDBuffer, &nSDLen, &pptszListAddr, nListCount);
		BaseLib_Memory_Free((XPPPMEM)&pptszListAddr, nListCount);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
	}
	else if (0 == _tcsxnicmp(lpszAPIGetTopic, lpszFuncName, _tcsxlen(lpszAPIGetTopic)))
	{
		//主题 http://127.0.0.1:5202/api?function=gettopic
		int nDBCount = 0;
		XCHAR tszTopicName[MAX_PATH] = {};
		ProtocolModule_Parse_Name(lpszMsgBuffer, nMsgLen, tszTopicName);
		DBModule_MQData_GetLeftCount(tszTopicName, 0, &nDBCount);
		ProtocolModule_Packet_TopicName(tszSDBuffer, &nSDLen, tszTopicName, nDBCount);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
	}
	else if (0 == _tcsxnicmp(lpszAPIGetList, lpszFuncName, _tcsxlen(lpszAPIGetList)))
	{
		//主题 http://127.0.0.1:5202/api?function=getlist
		int nListCount = 0;
		XCHAR** ppszTableName;
		DBModule_MQData_ShowTable(&ppszTableName, &nListCount);
		ProtocolModule_Packet_TopicList(tszSDBuffer, &nSDLen, &ppszTableName, nListCount);
		BaseLib_Memory_Free((XPPPMEM)&ppszTableName, nListCount);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,发送的获取主题列表请求成功,获取到的主题列表个数:%d"), lpszClientAddr, nListCount);
	}
	else if (0 == _tcsxnicmp(lpszAPICreateTopic, lpszFuncName, _tcsxlen(lpszAPICreateTopic)))
	{
		//http://127.0.0.1:5202/api?function=createtopic

		XENGINE_PROTOCOL_XMQ st_MQProtocol = {};
		XENGINE_PROTOCOLHDR st_ProtocolHdr = {};

		ProtocolModule_Parse_Name(lpszMsgBuffer, nMsgLen, st_MQProtocol.tszMQKey);
		APIHelp_MQHelp_JsonToHex(&st_ProtocolHdr);

		st_ProtocolHdr.xhToken = xhToken;
		st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICCREATE;
		MessageQueue_TCP_Handle(&st_ProtocolHdr, lpszClientAddr, (LPCXSTR)&st_MQProtocol, sizeof(XENGINE_PROTOCOL_XMQ), XENGINE_MQAPP_NETTYPE_HTTP);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,请求主题删除成功,主题名:%s"), lpszClientAddr, st_MQProtocol.tszMQKey);
	}
	else if (0 == _tcsxnicmp(lpszAPIDelTopic, lpszFuncName, _tcsxlen(lpszAPIDelTopic)))
	{
		//http://127.0.0.1:5202/api?function=deletetopic

		XENGINE_PROTOCOL_XMQ st_MQProtocol = {};
		XENGINE_PROTOCOLHDR st_ProtocolHdr = {};
		
		ProtocolModule_Parse_Name(lpszMsgBuffer, nMsgLen, st_MQProtocol.tszMQKey);
		APIHelp_MQHelp_JsonToHex(&st_ProtocolHdr);

		st_ProtocolHdr.xhToken = xhToken;
		st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICDELETE;
		MessageQueue_TCP_Handle(&st_ProtocolHdr, lpszClientAddr, (LPCXSTR)&st_MQProtocol, sizeof(XENGINE_PROTOCOL_XMQ), XENGINE_MQAPP_NETTYPE_HTTP);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,请求主题删除成功,主题名:%s"), lpszClientAddr, st_MQProtocol.tszMQKey);
	}
	else if (0 == _tcsxnicmp(lpszAPIDelUser, lpszFuncName, _tcsxlen(lpszAPIDelUser)))
	{
		XENGINE_PROTOCOL_USERINFO st_UserInfo = {};
		if (!ProtocolModule_Parse_Register(lpszMsgBuffer, nMsgLen, &st_UserInfo))
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_PARSE, _X("json load parse is failure"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求注册消息失败,负载内容错误:%s"), lpszClientAddr, lpszMsgBuffer);
			return false;
		}
		if (!DBModule_MQUser_UserDelete(&st_UserInfo))
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_DELETE, _X("delete user failure"));
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
				ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_APIDEL, _X("delete user failure"));
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
		ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,请求用户删除成功,用户名:%s"), lpszClientAddr, st_UserInfo.tszUserName);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("HTTP客户端:%s,协议错误"), lpszClientAddr);
	}
	return true;
}