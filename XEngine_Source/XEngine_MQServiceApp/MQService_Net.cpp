#include "MQService_Hdr.h"
//////////////////////////////////////////////////////////////////////////
BOOL __stdcall MessageQueue_Callback_TCPLogin(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam)
{
    SessionModule_Client_Create(lpszClientAddr);
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
void XEngine_MQXService_Close(LPCTSTR lpszClientAddr, int nIPProto, BOOL bHeart)
{
    if (XENGINE_MQAPP_NETTYPE_TCP == nIPProto)
    {
        HelpComponents_Datas_DeleteEx(xhTCPPacket, lpszClientAddr);
        SessionModule_Client_Delete(lpszClientAddr);
        
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
    else
    {
		RfcComponents_HttpServer_CloseClinetEx(xhHTTPPacket, lpszClientAddr);
        NetCore_TCPXCore_CloseForClientEx(xhHTTPSocket, lpszClientAddr);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("HTTP客户端离开，HTTP客户端地址：%s"), lpszClientAddr);
    }
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
		if (!NetCore_TCPXCore_SendEx(xhHTTPSocket, lpszClientAddr, lpszMsgBuffer, nMsgLen))
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("发送数据给HTTP客户端：%s，失败，错误：%lX"), lpszClientAddr, NetCore_GetLastError());
			return FALSE;
		}
    }
    return TRUE;
}