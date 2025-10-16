#include "MQService_Hdr.h"
//////////////////////////////////////////////////////////////////////////
bool XCALLBACK MessageQueue_Callback_TCPLogin(LPCXSTR lpszClientAddr, XSOCKET hSocket, XPVOID lParam)
{
	HelpComponents_Datas_CreateEx(xhTCPPacket, lpszClientAddr, 0);
    XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO,_X("TCP客户端连接，TCP客户端地址：%s"),lpszClientAddr);
	return true;
}
void XCALLBACK MessageQueue_Callback_TCPRecv(LPCXSTR lpszClientAddr, XSOCKET hSocket, LPCXSTR lpszRecvMsg,int nMsgLen,XPVOID lParam)
{
	if (!HelpComponents_Datas_PostEx(xhTCPPacket, lpszClientAddr, lpszRecvMsg, nMsgLen))
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("投递TCP数据包到消息队列失败，错误：%lX"), Packets_GetLastError());
		return;
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_DEBUG, _X("投递TCP数据包到消息队列成功,%d"), nMsgLen);
}
void XCALLBACK MessageQueue_Callback_TCPLeave(LPCXSTR lpszClientAddr, XSOCKET hSocket, XPVOID lParam)
{
    XEngine_MQXService_Close(lpszClientAddr, XENGINE_MQAPP_NETTYPE_TCP, false);
}
//////////////////////////////////////////////////////////////////////////
bool XCALLBACK MessageQueue_Callback_MQTTLogin(LPCXSTR lpszClientAddr, XSOCKET hSocket, XPVOID lParam)
{
	MQTTProtocol_Parse_Insert(lpszClientAddr);
	return true;
}
void XCALLBACK MessageQueue_Callback_MQTTRecv(LPCXSTR lpszClientAddr, XSOCKET hSocket, LPCXSTR lpszRecvMsg, int nMsgLen, XPVOID lParam)
{
	if (!MQTTProtocol_Parse_Send(lpszClientAddr, lpszRecvMsg, nMsgLen))
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("投递MQTT数据包到消息队列失败，错误：%lX"), MQTTProtocol_GetLastError());
		return;
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_DEBUG, _X("投递MQTT数据包到消息队列成功,%d"), nMsgLen);
}
void XCALLBACK MessageQueue_Callback_MQTTLeave(LPCXSTR lpszClientAddr, XSOCKET hSocket, XPVOID lParam)
{
	XEngine_MQXService_Close(lpszClientAddr, XENGINE_MQAPP_NETTYPE_MQTT, false);
}
//////////////////////////////////////////////////////////////////////////
bool XCALLBACK MessageQueue_Callback_HttpLogin(LPCXSTR lpszClientAddr, XSOCKET hSocket, XPVOID lParam)
{
    HttpProtocol_Server_CreateClientEx(xhHTTPPacket, lpszClientAddr, 0);
    XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端连接，HTTP客户端地址：%s"), lpszClientAddr);
    return true;
}
void XCALLBACK MessageQueue_Callback_HttpRecv(LPCXSTR lpszClientAddr, XSOCKET hSocket, LPCXSTR lpszRecvMsg, int nMsgLen, XPVOID lParam)
{
	if (!HttpProtocol_Server_InserQueueEx(xhHTTPPacket, lpszClientAddr, lpszRecvMsg, nMsgLen))
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("投递HTTP数据包到消息队列失败，错误：%lX"), HttpProtocol_GetLastError());
		return;
	}
}
void XCALLBACK MessageQueue_Callback_HttpLeave(LPCXSTR lpszClientAddr, XSOCKET hSocket, XPVOID lParam)
{
    XEngine_MQXService_Close(lpszClientAddr, XENGINE_MQAPP_NETTYPE_HTTP, false);
}
//////////////////////////////////////////////////////////////////////////
bool XCALLBACK MessageQueue_Callback_WSLogin(LPCXSTR lpszClientAddr, XSOCKET hSocket, XPVOID lParam)
{
    RfcComponents_WSPacket_CreateEx(xhWSPacket, lpszClientAddr, 0);
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("Websocket客户端连接，Websocket客户端地址：%s"), lpszClientAddr);
	return true;
}
void XCALLBACK MessageQueue_Callback_WSRecv(LPCXSTR lpszClientAddr, XSOCKET hSocket, LPCXSTR lpszRecvMsg, int nMsgLen, XPVOID lParam)
{
	bool bLogin = false;
	RfcComponents_WSPacket_GetLoginEx(xhWSPacket, lpszClientAddr, &bLogin);
	if (bLogin)
	{
		if (!RfcComponents_WSPacket_InsertQueueEx(xhWSPacket, lpszClientAddr, lpszRecvMsg, nMsgLen))
		{
            XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("投递Websocket数据包到消息队列失败，错误：%lX"), WSFrame_GetLastError());
            return;
		}
	}
	else
	{
		int nSDLen = nMsgLen;
		XCHAR tszHandsBuffer[1024];
		memset(tszHandsBuffer, '\0', sizeof(tszHandsBuffer));

		RfcComponents_WSConnector_HandShake(lpszRecvMsg, &nSDLen, tszHandsBuffer);
		RfcComponents_WSPacket_SetLoginEx(xhWSPacket, lpszClientAddr);
        NetCore_TCPXCore_SendEx(xhWSSocket, lpszClientAddr, tszHandsBuffer, nSDLen);
        XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("WEBSOCKET客户端:%s 与服务器握手成功"), lpszClientAddr);
	}
}
void XCALLBACK MessageQueue_Callback_WSLeave(LPCXSTR lpszClientAddr, XSOCKET hSocket, XPVOID lParam)
{
    XEngine_MQXService_Close(lpszClientAddr, XENGINE_MQAPP_NETTYPE_WEBSOCKET, false);
}
//////////////////////////////////////////////////////////////////////////
void XEngine_MQXService_Close(LPCXSTR lpszClientAddr, int nIPProto, bool bHeart)
{
    if (XENGINE_MQAPP_NETTYPE_TCP == nIPProto)
    {
		HelpComponents_Datas_DeleteEx(xhTCPPacket, lpszClientAddr);
        if (bHeart)
        {
			NetCore_TCPXCore_CloseForClientEx(xhTCPSocket, lpszClientAddr);
		}
        XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("TCP客户端离开，TCP客户端地址：%s"), lpszClientAddr);
    }
    else if (XENGINE_MQAPP_NETTYPE_WEBSOCKET == nIPProto)
    {
        RfcComponents_WSPacket_DeleteEx(xhWSPacket, lpszClientAddr);
		if (bHeart)
		{
			NetCore_TCPXCore_CloseForClientEx(xhWSSocket, lpszClientAddr);
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("Websocket客户端离开，Websocket客户端地址：%s"), lpszClientAddr);
    }
    else if (XENGINE_MQAPP_NETTYPE_HTTP == nIPProto)
	{
		HttpProtocol_Server_CloseClinetEx(xhHTTPPacket, lpszClientAddr);
		if (bHeart)
		{
			NetCore_TCPXCore_CloseForClientEx(xhHTTPSocket, lpszClientAddr);
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("HTTP客户端离开，HTTP客户端地址：%s"), lpszClientAddr);
    }
	else
	{
		MQTTProtocol_Parse_Delete(lpszClientAddr);
		if (bHeart)
		{
			NetCore_TCPXCore_CloseForClientEx(xhMQTTSocket, lpszClientAddr);
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("MQTT客户端离开，MQTT客户端地址：%s"), lpszClientAddr);
	}
	XENGINE_PROTOCOL_USERINFO st_UserInfo;
	memset(&st_UserInfo, '\0', sizeof(XENGINE_PROTOCOL_USERINFO));
    if (SessionModule_Client_GetUser(lpszClientAddr, st_UserInfo.tszUserName))
    {
        if (_tcsxlen(st_ServiceCfg.st_XPass.tszPassLogout) > 0)
        {
            int nSDLen = 0;
			int nHTTPCode = 0;
			XCHAR tszSDBuffer[1024];
            XCLIENT_APIHTTP st_HTTPParament;
			XENGINE_PROTOCOL_USERAUTH st_ProtocolAuth;

            memset(tszSDBuffer, '\0', sizeof(tszSDBuffer));
			memset(&st_HTTPParament, '\0', sizeof(XCLIENT_APIHTTP));
            memset(&st_ProtocolAuth, '\0', sizeof(XENGINE_PROTOCOL_USERAUTH));

			st_HTTPParament.nTimeConnect = 2;

			ProtocolModule_Packet_PassAuth(&st_ProtocolAuth, tszSDBuffer, &nSDLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_AUTH_REQLOGOUT);
			APIClient_Http_Request(_X("POST"), st_ServiceCfg.st_XPass.tszPassLogout, tszSDBuffer, &nHTTPCode, NULL, NULL, NULL, NULL, &st_HTTPParament);
			if (200 == nHTTPCode)
			{
                XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("%s客户端:%s,请求远程关闭连接成功"), nIPProto, lpszClientAddr);
			}
            else
            {
                XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("%s客户端:%s,请求远程关闭连接失败,HTTPCode:%d"), nIPProto, lpszClientAddr, nHTTPCode);
            }
        }
        DBModule_MQUser_UserUPDate(&st_UserInfo);
    }
    SessionModule_Client_Delete(lpszClientAddr);
}
//////////////////////////////////////////////////////////////////////////
bool XEngine_MQXService_Send(LPCXSTR lpszClientAddr, LPCXSTR lpszMsgBuffer, int nMsgLen, int nIPProto, XENGINE_PROTOCOL_MSGATTR* pSt_MSGAttr, LPCXSTR lpszUserName)
{
	if (XENGINE_MQAPP_NETTYPE_TCP == nIPProto)
	{
		if (!NetCore_TCPXCore_SendEx(xhTCPSocket, lpszClientAddr, lpszMsgBuffer, nMsgLen))
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("发送数据给TCP客户端：%s，失败，错误：%lX"), lpszClientAddr, NetCore_GetLastError());
			return false;
		}
	}
	else if (XENGINE_MQAPP_NETTYPE_HTTP == nIPProto)
	{
		CMQService_MemoryPool m_MQMemoryPool(XENGINE_MEMORY_SIZE_MAX);
		int nSDLen = XENGINE_MEMORY_SIZE_MAX;
		RFCCOMPONENTS_HTTP_HDRPARAM st_HTTPHdr = {};

		st_HTTPHdr.nHttpCode = 200;
		st_HTTPHdr.bIsClose = true;
		_tcsxcpy(st_HTTPHdr.tszMimeType, _X("json"));

		HttpProtocol_Server_SendMsgEx(xhHTTPPacket, m_MQMemoryPool.get(), &nSDLen, &st_HTTPHdr, lpszMsgBuffer, nMsgLen);
		if (!NetCore_TCPXCore_SendEx(xhHTTPSocket, lpszClientAddr, m_MQMemoryPool.get(), nSDLen))
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("发送数据给HTTP客户端：%s，失败，错误：%lX"), lpszClientAddr, NetCore_GetLastError());
			return false;
		}
	}
	else if (XENGINE_MQAPP_NETTYPE_WEBSOCKET == nIPProto)
	{
		CMQService_MemoryPool m_MQMemoryPool(XENGINE_MEMORY_SIZE_MAX);
		RfcComponents_WSCodec_EncodeMsg(lpszMsgBuffer, m_MQMemoryPool.get(), &nMsgLen, ENUM_XENGINE_RFCOMPONENTS_WEBSOCKET_OPCODE_TEXT);
		if (!NetCore_TCPXCore_SendEx(xhWSSocket, lpszClientAddr, m_MQMemoryPool.get(), nMsgLen))
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("发送数据给Websocket客户端：%s，失败，错误：%lX"), lpszClientAddr, NetCore_GetLastError());
			return false;
		}
	}
	else if (XENGINE_MQAPP_NETTYPE_MQTT == nIPProto)
	{
		if (!NetCore_TCPXCore_SendEx(xhMQTTSocket, lpszClientAddr, lpszMsgBuffer, nMsgLen))
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("发送数据给MQTT客户端：%s，失败，错误：%lX"), lpszClientAddr, NetCore_GetLastError());
			return false;
		}
	}

	if (NULL != pSt_MSGAttr)
	{
		XENGINE_PROTOCOL_USERINFO st_UserInfo = {};
		_tcsxcpy(st_UserInfo.tszUserName, lpszUserName);
		
		if (!DBModule_MQUser_UserQuery(&st_UserInfo))
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("客户端:%s,用户名:%s 请求消息通知失败,因为没有找到指定的用户信息,错误码:%lX"), lpszClientAddr,lpszUserName, DBModule_GetLastError());
			return false;
		}
		//是否需要邮件通知
		if (pSt_MSGAttr->byAttrEMail)
		{
			if (!APIAddr_EMail_IsEMailAddr(st_UserInfo.tszEMailAddr))
			{
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("客户端:%s,用户名:%s 请求消息通知失败,通知邮件地址不正确,错误码:%lX"), lpszClientAddr, lpszUserName, APIAddr_GetLastError());
				return false;
			}
			if (!MSGNotify_EMail_Send(st_ServiceCfg.st_XNotify.st_EMailNotify.tszServiceAddr, st_ServiceCfg.st_XNotify.st_EMailNotify.tszUser, st_ServiceCfg.st_XNotify.st_EMailNotify.tszPass, lpszClientAddr, st_ServiceCfg.st_XNotify.st_EMailNotify.tszEMailSubject, lpszMsgBuffer))
			{
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("发送数据给EMail地址:%s 失败,错误码:%lX"), lpszClientAddr, MSGNotify_GetLastError());
				return false;
			}
		}
	}
	
    return true;
}