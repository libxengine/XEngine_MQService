#include "MQService_Hdr.h"
//////////////////////////////////////////////////////////////////////////
BOOL __stdcall MessageQueue_Callback_TCPLogin(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam)
{
    SessionModule_Client_Create(lpszClientAddr, XENGINE_MQAPP_NETTYPE_TCP);
    SocketOpt_HeartBeat_InsertAddrEx(xhTCPHeart, lpszClientAddr);
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
    SocketOpt_HeartBeat_ActiveAddrEx(xhTCPHeart, lpszClientAddr);
    XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_DEBUG, _T("投递TCP数据包到消息队列成功,%d"), nMsgLen);
}
void __stdcall MessageQueue_Callback_TCPLeave(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam)
{
    XEngine_MQXService_Close(lpszClientAddr, XENGINE_MQAPP_NETTYPE_TCP, FALSE);
}
void __stdcall MessageQueue_Callback_TCPHeart(LPCSTR lpszClientAddr, SOCKET hSocket, int nStatus, LPVOID lParam)
{
    XEngine_MQXService_Close(lpszClientAddr, XENGINE_MQAPP_NETTYPE_TCP, TRUE);
}
//////////////////////////////////////////////////////////////////////////
BOOL __stdcall MessageQueue_Callback_HttpLogin(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam)
{
    SessionModule_Client_Create(lpszClientAddr, XENGINE_MQAPP_NETTYPE_HTTP);
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
    SessionModule_Client_Create(lpszClientAddr, XENGINE_MQAPP_NETTYPE_WEBSOCKET);
    SocketOpt_HeartBeat_InsertAddrEx(xhWSHeart, lpszClientAddr);
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
        SocketOpt_HeartBeat_ActiveAddrEx(xhWSHeart, lpszClientAddr);
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
void __stdcall MessageQueue_Callback_WSHeart(LPCSTR lpszClientAddr, SOCKET hSocket, int nStatus, LPVOID lParam)
{
    XEngine_MQXService_Close(lpszClientAddr, XENGINE_MQAPP_NETTYPE_WEBSOCKET, TRUE);
}
//////////////////////////////////////////////////////////////////////////
void XEngine_MQXService_Close(LPCTSTR lpszClientAddr, int nIPProto, BOOL bHeart)
{
    if (XENGINE_MQAPP_NETTYPE_TCP == nIPProto)
    {
        HelpComponents_Datas_DeleteEx(xhTCPPacket, lpszClientAddr);
        
        if (bHeart)
        {
            NetCore_TCPXCore_CloseForClientEx(xhTCPSocket, lpszClientAddr);
        }
        else
        {
            SocketOpt_HeartBeat_DeleteAddrEx(xhTCPHeart, lpszClientAddr);
        }
        XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("TCP客户端离开，TCP客户端地址：%s"), lpszClientAddr);
    }
    else if (XENGINE_MQAPP_NETTYPE_WEBSOCKET == nIPProto)
    {
        RfcComponents_WSPacket_DeleteEx(xhWSPacket, lpszClientAddr);

		if (bHeart)
		{
			NetCore_TCPXCore_CloseForClientEx(xhWSSocket, lpszClientAddr);
		}
		else
		{
            SocketOpt_HeartBeat_DeleteAddrEx(xhWSHeart, lpszClientAddr);
		}
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("Websocket客户端离开，Websocket客户端地址：%s"), lpszClientAddr);
    }
    else
	{
		RfcComponents_HttpServer_CloseClinetEx(xhHTTPPacket, lpszClientAddr);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("HTTP客户端离开，HTTP客户端地址：%s"), lpszClientAddr);
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
        SocketOpt_HeartBeat_ActiveAddrEx(xhTCPHeart, lpszClientAddr);
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
        SocketOpt_HeartBeat_ActiveAddrEx(xhWSHeart, lpszClientAddr);
    }
    return TRUE;
}