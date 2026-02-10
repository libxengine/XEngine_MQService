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
	LPCXSTR lpszAPIRegister = _X("register");
	LPCXSTR lpszAPIGetUser = _X("getuser");
	LPCXSTR lpszAPIGetTopic = _X("gettopic");
	LPCXSTR lpszAPIGetList = _X("getlist");
	LPCXSTR lpszAPIGetOnline = _X("getonline");
	LPCXSTR lpszAPIGetNumber = _X("getnumber");      //请求消息队列编号信息

	LPCXSTR lpszAPICreateTopic = _X("createtopic");
	LPCXSTR lpszAPIDelTopic = _X("deletetopic");
	LPCXSTR lpszAPIDelUser = _X("deleteuser");
	LPCXSTR lpszAPIDelMsg = _X("deletemsg");
	LPCXSTR lpszAPIModifyMsg = _X("modifymsg");
	LPCXSTR lpszAPIModifyTopic = _X("modifytopic");

	//判断请求
	if (0 == _tcsxncmp(lpszAPIRegister, lpszFuncName, _tcsxlen(lpszAPIRegister)))
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
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_EXIST, _X("user is existed"));
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
			ProtocolModule_Packet_PassUser(&st_UserInfo, tszSDBuffer, &nSDLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_AUTH_REQREGISTER);
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
	else if (0 == _tcsxncmp(lpszAPIGetUser, lpszFuncName, _tcsxlen(lpszAPIGetUser)))
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
	else if (0 == _tcsxncmp(lpszAPIGetOnline, lpszFuncName, _tcsxlen(lpszAPIGetOnline)))
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
	else if (0 == _tcsxncmp(lpszAPIGetNumber, lpszFuncName, _tcsxlen(lpszAPIGetNumber)))
	{
		//获取消息队列编号 http://127.0.0.1:5202/api?function=getnumber
		XENGINE_DBMESSAGEQUEUE st_DBStart = {};
		XENGINE_DBMESSAGEQUEUE st_DBEnd = {};
		XENGINE_MQNUMBER st_MQNumber = {};
		XENGINE_PROTOCOL_XMQ st_MQProtocol = {};

		if (!ProtocolModule_Parse_XMQ(lpszMsgBuffer, nMsgLen, &st_MQProtocol))
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_PARSE, _X("request json parse failure"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP消息端:%s,请求的获取消息队列编号失败,数据不正确:%s"), lpszClientAddr, lpszMsgBuffer);
			return false;
		}
		if (!DBModule_MQData_GetSerial(st_MQProtocol.tszMQKey, &st_MQNumber.nCount, &st_DBStart, &st_DBEnd))
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_NOTFOUND, _X("not found topic message"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP消息端:%s,获取消息队列序列属性失败,主题名称:%s,序列号:%lld,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nKeepTime == 1 ? "顺序" : "倒序", st_MQProtocol.nSerial, DBModule_GetLastError());
			return false;
		}

		st_MQNumber.nFirstNumber = st_DBStart.nQueueSerial;
		st_MQNumber.nLastNumber = st_DBEnd.nQueueSerial;
		_tcsxcpy(st_MQNumber.tszMQKey, st_MQProtocol.tszMQKey);
		ProtocolModule_Packet_MQNumber(tszSDBuffer, &nSDLen, &st_MQNumber);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP消息端:%s,获取主题序列编号成功,主题名称:%s,队列个数:%lld,开始编号:%lld,结尾编号:%lld"), lpszClientAddr, st_MQNumber.tszMQKey, st_MQNumber.nCount, st_MQNumber.nFirstNumber, st_MQNumber.nLastNumber);

		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
	}
	else if (0 == _tcsxncmp(lpszAPIGetTopic, lpszFuncName, _tcsxlen(lpszAPIGetTopic)))
	{
		//主题 http://127.0.0.1:5202/api?function=gettopic
		int nDBCount = 0;
		XENGINE_PROTOCOL_XMQ st_MQProtocol = {};

		ProtocolModule_Parse_XMQ(lpszMsgBuffer, nMsgLen, &st_MQProtocol);
		DBModule_MQData_GetLeftCount(st_MQProtocol.tszMQKey, 0, &nDBCount);
		ProtocolModule_Packet_TopicName(tszSDBuffer, &nSDLen, st_MQProtocol.tszMQKey, nDBCount);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
	}
	else if (0 == _tcsxncmp(lpszAPIGetList, lpszFuncName, _tcsxlen(lpszAPIGetList)))
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
	else if (0 == _tcsxncmp(lpszAPICreateTopic, lpszFuncName, _tcsxlen(lpszAPICreateTopic)))
	{
		//http://127.0.0.1:5202/api?function=createtopic
		XENGINE_PROTOCOL_XMQ st_MQProtocol = {};

		if (!ProtocolModule_Parse_XMQ(lpszMsgBuffer, nMsgLen, &st_MQProtocol))
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_PARSE, _X("request json parse failure"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP消息端:%s,请求的创建主题失败,数据不正确:%s"), lpszClientAddr, lpszMsgBuffer);
			return false;
		}
		int nListCount = 0;
		XCHAR** ppszTableName;
		//检查表是否存在
		DBModule_MQData_ShowTable(&ppszTableName, &nListCount);
		for (int i = 0; i < nListCount; i++)
		{
			if (0 == _tcsxnicmp(ppszTableName[i], st_MQProtocol.tszMQKey, _tcsxlen(ppszTableName[i])))
			{
				ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_EXIST, _X("topic name is exist"));
				XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
				BaseLib_Memory_Free((XPPPMEM)&ppszTableName, nListCount);
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP消息端:%s,创建主题失败,主题名称:%s,主题存在,无法继续"), lpszClientAddr, st_MQProtocol.tszMQKey);
				return false;
			}
		}
		BaseLib_Memory_Free((XPPPMEM)&ppszTableName, nListCount);
		//创建表
		if (!DBModule_MQData_CreateTable(st_MQProtocol.tszMQKey))
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_SERVICE, _X("create topic is failure"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP消息端:%s,创建主题失败,创建表失败,主题名称:%s,无法继续,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, DBModule_GetLastError());
			return false;
		}
		//插入所有者
		XENGINE_DBTOPICOWNER st_DBOwner;
		memset(&st_DBOwner, '\0', sizeof(XENGINE_DBTOPICOWNER));

		_tcsxcpy(st_DBOwner.tszUserName, st_MQProtocol.tszMQUsr);
		_tcsxcpy(st_DBOwner.tszQueueName, st_MQProtocol.tszMQKey);

		if (!DBModule_MQUser_OwnerInsert(&st_DBOwner))
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_SERVICE, _X("create topic bind with user is failure"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP消息端:%s,创建主题失败,插入所有者失败,主题名称:%s,无法继续,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, DBModule_GetLastError());
			return false;
		}
		//回复
		ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP消息端:%s,主题:%s,创建主题成功"), lpszClientAddr, st_MQProtocol.tszMQKey);
	}
	else if (0 == _tcsxncmp(lpszAPIDelTopic, lpszFuncName, _tcsxlen(lpszAPIDelTopic)))
	{
		//http://127.0.0.1:5202/api?function=deletetopic
		XENGINE_DBTOPICOWNER st_DBOwner = {};
		XENGINE_DBUSERKEY st_UserKey = {};
		XENGINE_DBTIMERELEASE st_DBInfo = {};
		XENGINE_PROTOCOL_XMQ st_MQProtocol = {};

		ProtocolModule_Parse_XMQ(lpszMsgBuffer, nMsgLen, &st_MQProtocol);

		_tcsxcpy(st_DBOwner.tszUserName, st_MQProtocol.tszMQUsr);
		_tcsxcpy(st_DBOwner.tszQueueName, st_MQProtocol.tszMQKey);
		_tcsxcpy(st_UserKey.tszKeyName, st_MQProtocol.tszMQKey);
		_tcsxcpy(st_DBInfo.tszQueueName, st_MQProtocol.tszMQKey);

		if (_tcsxlen(st_DBOwner.tszUserName) <= 0)
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_MISS, _X("user name missing"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP消息端:%s,删除主题失败,删除所有者失败,主题名称:%s,用户名为空"), lpszClientAddr, st_MQProtocol.tszMQKey, st_DBOwner.tszUserName);
			return false;
		}
		if (!DBModule_MQUser_OwnerDelete(&st_DBOwner))
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_SERVICE, _X("delete owner db failure"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP消息端:%s,删除主题失败,删除所有者失败,主题名称:%s,无法继续,错误：%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, DBModule_GetLastError());
			return false;
		}
		//清楚数据库
		APIHelp_Counter_SerialDel(st_MQProtocol.tszMQKey);
		DBModule_MQData_DeleteTable(st_MQProtocol.tszMQKey);
		DBModule_MQUser_KeyDelete(&st_UserKey);
		DBModule_MQUser_TimeDelete(&st_DBInfo);

		ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,请求主题删除成功,主题名:%s"), lpszClientAddr, st_MQProtocol.tszMQKey);
	}
	else if (0 == _tcsxncmp(lpszAPIDelMsg, lpszFuncName, _tcsxlen(lpszAPIDelMsg)))
	{
		//http://127.0.0.1:5202/api?function=deletemsg
		XENGINE_PROTOCOL_XMQ st_MQProtocol = {};
		XENGINE_DBMESSAGEQUEUE st_MessageQueue = {};

		ProtocolModule_Parse_XMQ(lpszMsgBuffer, nMsgLen, &st_MQProtocol);

		if (st_MQProtocol.nSerial <= 0)
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_MISS, _X("message serial not set"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP消息端:%s,主题:%s,删除消息数据失败,删除指定消息序列:%lld 失败"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial);
			return false;
		}
		st_MessageQueue.nQueueSerial = st_MQProtocol.nSerial;
		_tcsxcpy(st_MessageQueue.tszQueueName, st_MQProtocol.tszMQKey);
		if (!DBModule_MQData_Delete(&st_MessageQueue))
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_SERVICE, _X("message delete failure"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP消息端:%s,主题:%s,删除消息数据失败,删除指定消息序列:%lld 失败,错误码:%lX"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MQProtocol.nSerial, DBModule_GetLastError());
			return false;
		}

		ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,删除消息数据成功主题:%s,序列:%lld,"), lpszClientAddr, st_MQProtocol.tszMQKey, st_MessageQueue.nQueueSerial);
	}
	else if (0 == _tcsxncmp(lpszAPIDelUser, lpszFuncName, _tcsxlen(lpszAPIDelUser)))
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

			ProtocolModule_Packet_PassUser(&st_UserInfo, tszSDBuffer, &nSDLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_AUTH_REQDELETE);
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
	else if (0 == _tcsxncmp(lpszAPIModifyMsg, lpszFuncName, _tcsxlen(lpszAPIModifyMsg)))
	{
		XENGINE_DBMESSAGEQUEUE st_DBQueue = {};

		if (!ProtocolModule_Parse_MessageQueue(lpszMsgBuffer, nMsgLen, &st_DBQueue))
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_PARSE, _X("json load parse is failure"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求修改消息失败,负载内容错误:%s"), lpszClientAddr, lpszMsgBuffer);
			return false;
		}
		if (!DBModule_MQData_Modify(&st_DBQueue))
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_FAILURE, _X("modify message failure"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP消息端:%s,修改消息:%s,序列号:%lld,失败,错误：%lX"), lpszClientAddr, st_DBQueue.tszQueueName, st_DBQueue.nQueueSerial, DBModule_GetLastError());
			return false;
		}

		ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端:%s,请求修改消息成功,消息名:%s,序列号:%d"), lpszClientAddr, st_DBQueue.tszQueueName, st_DBQueue.nQueueSerial);
	}
	else if (0 == _tcsxncmp(lpszAPIModifyTopic, lpszFuncName, _tcsxlen(lpszAPIModifyTopic)))
	{
		XCHAR tszSrcTopic[XPATH_MIN] = {};
		XCHAR tszDstTopic[XPATH_MIN] = {};
		XCHAR tszUserName[XPATH_MIN] = {};

		if (!ProtocolModule_Parse_ModifyTopic(lpszMsgBuffer, nMsgLen, tszSrcTopic, tszDstTopic, tszUserName))
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_PARSE, _X("json load parse is failure"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP客户端:%s,请求修改主题失败,负载内容错误:%s"), lpszClientAddr, lpszMsgBuffer);
			return false;
		}
		XENGINE_DBTOPICOWNER st_DBOwner = {};
		//验证所有者
		_tcsxcpy(st_DBOwner.tszUserName, tszUserName);
		_tcsxcpy(st_DBOwner.tszQueueName, tszSrcTopic);

		if (!DBModule_MQUser_OwnerQuery(&st_DBOwner))
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_FAILURE, _X("query topic owner failure"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP消息端:%s,修改主题失败,可能用户不拥有主题,主题名称:%s,无法继续,错误：%lX"), lpszClientAddr, tszSrcTopic, DBModule_GetLastError());
			return false;
		}
		//修改主题
		if (!DBModule_MQData_ModifyTable(tszSrcTopic, tszDstTopic))
		{
			ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen, ERROR_XENGINE_MESSAGE_HTTP_FAILURE, _X("modify topic failure"));
			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("HTTP消息端:%s,修改主题名称失败,原名称:%s,目标名:%s,错误：%lX"), lpszClientAddr, tszSrcTopic, tszDstTopic, DBModule_GetLastError());
			return false;
		}
		DBModule_MQUser_KeyTopicUPDate(tszSrcTopic, tszDstTopic);
		DBModule_MQUser_TimeTopicUPDate(tszSrcTopic, tszDstTopic);
		DBModule_MQUser_OwnerTopicUPDate(tszSrcTopic, tszDstTopic);

		ProtocolModule_Packet_Http(tszSDBuffer, &nSDLen);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_HTTP);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP消息端:%s,修改主题名称成功,原名称:%s,目标名:%s"), lpszClientAddr, tszSrcTopic, tszDstTopic);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_WARN, _X("HTTP客户端:%s,协议错误"), lpszClientAddr);
	}
	return true;
}