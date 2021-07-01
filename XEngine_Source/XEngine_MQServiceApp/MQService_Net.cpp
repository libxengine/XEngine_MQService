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
    }
}
void __stdcall MessageQueue_Callback_TCPLeave(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam)
{
	HelpComponents_Datas_DeleteEx(xhTCPPacket,lpszClientAddr);
    XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO,_T("TCP客户端离开，TCP客户端地址：%s"),lpszClientAddr);
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
    if (XENGINE_MQAPP_NETTYPE_HTTP == nIPProto)
    {
    }
    return TRUE;
}