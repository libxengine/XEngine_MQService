#include "MQService_Hdr.h"
//////////////////////////////////////////////////////////////////////////
BOOL __stdcall MessageQueue_Callback_TCPLogin(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam)
{
	HelpComponents_Datas_CreateEx(xhTCPPacket, lpszClientAddr, 0);
    XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO,_T("TCP客户端连接，TCP客户端地址：%s"),lpszClientAddr);
	return TRUE;
}
void __stdcall MessageQueue_Callback_TCPRecv(LPCTSTR lpszClientAddr, SOCKET hSocket, LPCTSTR lpszRecvMsg,int nMsgLen,LPVOID lParam)
{
    if (!HelpComponents_Datas_PostEx(xhTCPPacket,lpszClientAddr,lpszRecvMsg,nMsgLen))
    {
        XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR,_T("投递TCP数据包到消息队列失败，错误：%lX"),Packets_GetLastError());
        return;
    }
    XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_DEBUG, _T("投递TCP数据包到消息队列成功,%d"), nMsgLen);
}
void __stdcall MessageQueue_Callback_TCPLeave(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam)
{
    XEngine_MQXService_Close(lpszClientAddr, XENGINE_MQAPP_NETTYPE_TCP, FALSE);
}
//////////////////////////////////////////////////////////////////////////
BOOL __stdcall MessageQueue_Callback_HttpLogin(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam)
{
    RfcComponents_HttpServer_CreateClientEx(xhHTTPPacket, lpszClientAddr, 0);
    XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("HTTP客户端连接，HTTP客户端地址：%s"), lpszClientAddr);
    return TRUE;
}
void __stdcall MessageQueue_Callback_HttpRecv(LPCTSTR lpszClientAddr, SOCKET hSocket, LPCTSTR lpszRecvMsg, int nMsgLen, LPVOID lParam)
{
	if (!RfcComponents_HttpServer_InserQueueEx(xhHTTPPacket, lpszClientAddr, lpszRecvMsg, nMsgLen))
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("投递HTTP数据包到消息队列失败，错误：%lX"), HttpServer_GetLastError());
		return;
	}
}
void __stdcall MessageQueue_Callback_HttpLeave(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam)
{
    XEngine_MQXService_Close(lpszClientAddr, XENGINE_MQAPP_NETTYPE_HTTP, FALSE);
}
//////////////////////////////////////////////////////////////////////////
BOOL __stdcall MessageQueue_Callback_WSLogin(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam)
{
    RfcComponents_WSPacket_CreateEx(xhWSPacket, lpszClientAddr, 0);
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("Websocket客户端连接，Websocket客户端地址：%s"), lpszClientAddr);
	return TRUE;
}
void __stdcall MessageQueue_Callback_WSRecv(LPCTSTR lpszClientAddr, SOCKET hSocket, LPCTSTR lpszRecvMsg, int nMsgLen, LPVOID lParam)
{
	BOOL bLogin = FALSE;
	RfcComponents_WSPacket_GetLoginEx(xhWSPacket, lpszClientAddr, &bLogin);
	if (bLogin)
	{
		if (!RfcComponents_WSPacket_PostEx(xhWSPacket, lpszClientAddr, lpszRecvMsg, nMsgLen))
		{
            XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("投递Websocket数据包到消息队列失败，错误：%lX"), WSFrame_GetLastError());
            return;
		}
	}
	else
	{
		int nSDLen = nMsgLen;
		TCHAR tszHandsBuffer[1024];
		memset(tszHandsBuffer, '\0', sizeof(tszHandsBuffer));

		RfcComponents_WSConnector_HandShake(lpszRecvMsg, &nSDLen, tszHandsBuffer);
		RfcComponents_WSPacket_SetLoginEx(xhWSPacket, lpszClientAddr);
        NetCore_TCPXCore_SendEx(xhWSSocket, lpszClientAddr, tszHandsBuffer, nSDLen);
        XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("WEBSOCKET客户端:%s 与服务器握手成功"), lpszClientAddr);
	}
}
void __stdcall MessageQueue_Callback_WSLeave(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam)
{
    XEngine_MQXService_Close(lpszClientAddr, XENGINE_MQAPP_NETTYPE_WEBSOCKET, FALSE);
}
//////////////////////////////////////////////////////////////////////////
void __stdcall MessageQueue_Callback_Timeout(LPCSTR lpszClientAddr, LPCTSTR lpszClientUser, ENUM_MQCORE_SESSION_CLIENT_TYPE enClientType, LPVOID lParam)
{
	XEngine_MQXService_Close(lpszClientAddr, XENGINE_MQAPP_NETTYPE_HTTP, TRUE);
}
//////////////////////////////////////////////////////////////////////////
void XEngine_MQXService_Close(LPCTSTR lpszClientAddr, int nIPProto, BOOL bHeart)
{
    if (XENGINE_MQAPP_NETTYPE_TCP == nIPProto)
    {
        HelpComponents_Datas_DeleteEx(xhTCPPacket, lpszClientAddr);
        NetCore_TCPXCore_CloseForClientEx(xhTCPSocket, lpszClientAddr);
        XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("TCP客户端离开，TCP客户端地址：%s"), lpszClientAddr);
    }
    else if (XENGINE_MQAPP_NETTYPE_WEBSOCKET == nIPProto)
    {
        RfcComponents_WSPacket_DeleteEx(xhWSPacket, lpszClientAddr);
        NetCore_TCPXCore_CloseForClientEx(xhWSSocket, lpszClientAddr);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("Websocket客户端离开，Websocket客户端地址：%s"), lpszClientAddr);
    }
    else
	{
		RfcComponents_HttpServer_CloseClinetEx(xhHTTPPacket, lpszClientAddr);
        NetCore_TCPXCore_CloseForClientEx(xhHTTPSocket, lpszClientAddr);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("HTTP客户端离开，HTTP客户端地址：%s"), lpszClientAddr);
    }
	XENGINE_PROTOCOL_USERINFO st_UserInfo;
	memset(&st_UserInfo, '\0', sizeof(XENGINE_PROTOCOL_USERINFO));
    if (SessionModule_Client_GetUser(lpszClientAddr, st_UserInfo.tszUserName))
    {
        if (_tcslen(st_ServiceCfg.st_XPass.tszPassLogout) > 0)
        {
            int nSDLen = 0;
			int nHTTPCode = 0;
			TCHAR tszSDBuffer[1024];
            NETHELP_HTTPCLIENT st_HTTPParament;
			XENGINE_PROTOCOL_USERAUTH st_ProtocolAuth;

            memset(tszSDBuffer, '\0', sizeof(tszSDBuffer));
			memset(&st_HTTPParament, '\0', sizeof(NETHELP_HTTPCLIENT));
            memset(&st_ProtocolAuth, '\0', sizeof(XENGINE_PROTOCOL_USERAUTH));

			st_HTTPParament.nTimeConnect = 2;

			ProtocolModule_Packet_PassAuth(&st_ProtocolAuth, tszSDBuffer, &nSDLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSEROUT);
			APIClient_Http_Request(_T("POST"), st_ServiceCfg.st_XPass.tszPassLogout, tszSDBuffer, &nHTTPCode, NULL, NULL, NULL, NULL, &st_HTTPParament);
			if (200 == nHTTPCode)
			{
                XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("%s客户端:%s,请求远程关闭连接成功"), nIPProto, lpszClientAddr);
			}
            else
            {
                XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("%s客户端:%s,请求远程关闭连接失败,HTTPCode:%d"), nIPProto, lpszClientAddr, nHTTPCode);
            }
        }
        DBModule_MQUser_UserUPDate(&st_UserInfo);
    }
    SessionModule_Client_Delete(lpszClientAddr);
}
//////////////////////////////////////////////////////////////////////////
BOOL XEngine_MQXService_Send(LPCTSTR lpszClientAddr, LPCTSTR lpszMsgBuffer, int nMsgLen, int nIPProto)
{
    if (XENGINE_MQAPP_NETTYPE_TCP == nIPProto)
    {
        if (!NetCore_TCPXCore_SendEx(xhTCPSocket, lpszClientAddr, lpszMsgBuffer, nMsgLen))
        {
            XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("发送数据给TCP客户端：%s，失败，错误：%lX"), lpszClientAddr, NetCore_GetLastError());
            return FALSE;
        }
    }
    else if (XENGINE_MQAPP_NETTYPE_HTTP == nIPProto)
    {
        int nPKTLen = 8196;
        TCHAR tszPKTBuffer[8196];
        RFCCOMPONENTS_HTTP_HDRPARAM st_HTTPHdr;

        memset(tszPKTBuffer, '\0', sizeof(tszPKTBuffer));
        memset(&st_HTTPHdr, '\0', sizeof(RFCCOMPONENTS_HTTP_HDRPARAM));

        st_HTTPHdr.nHttpCode = 200;
        st_HTTPHdr.bIsClose = TRUE;
        _tcscpy(st_HTTPHdr.tszMimeType, _T("json"));

        RfcComponents_HttpServer_SendMsgEx(xhHTTPPacket, tszPKTBuffer, &nPKTLen, &st_HTTPHdr, lpszMsgBuffer, nMsgLen);
		if (!NetCore_TCPXCore_SendEx(xhHTTPSocket, lpszClientAddr, tszPKTBuffer, nPKTLen))
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("发送数据给HTTP客户端：%s，失败，错误：%lX"), lpszClientAddr, NetCore_GetLastError());
			return FALSE;
		}
    }
    else if (XENGINE_MQAPP_NETTYPE_WEBSOCKET == nIPProto)
    {
		TCHAR tszPKTBuffer[8196];
		memset(tszPKTBuffer, '\0', sizeof(tszPKTBuffer));

        RfcComponents_WSCodec_EncodeMsg(lpszMsgBuffer, tszPKTBuffer, &nMsgLen, ENUM_XENGINE_RFCOMPONENTS_WEBSOCKET_OPCODE_TEXT);
		if (!NetCore_TCPXCore_SendEx(xhWSSocket, lpszClientAddr, tszPKTBuffer, nMsgLen))
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("发送数据给Websocket客户端：%s，失败，错误：%lX"), lpszClientAddr, NetCore_GetLastError());
			return FALSE;
		}
    }
    return TRUE;
}