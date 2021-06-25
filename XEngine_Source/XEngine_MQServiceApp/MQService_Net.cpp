#include "MQService_Hdr.h"

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
	XMQService_User_Delete(lpszClientAddr);
    XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO,_T("TCP客户端离开，TCP客户端地址：%s"),lpszClientAddr);
}

void __stdcall MessageQueue_Callback_UDPRecv(LPCTSTR lpszClientAddr, SOCKET hSocket, LPCTSTR lpszRecvMsg,int nMsgLen,LPVOID lParam)
{
	NETENGIEN_MQTHREADPOOL st_ThreadPool;
	memset(&st_ThreadPool, '\0', sizeof(NETENGIEN_MQTHREADPOOL));

	st_ThreadPool.nIPProto = 2;
	st_ThreadPool.nMsgLen = nMsgLen - sizeof(XENGINE_PROTOCOLHDR);
	
	_tcscpy(st_ThreadPool.tszClientAddr, lpszClientAddr);
	memcpy(&st_ThreadPool.st_ProtocolHdr, lpszRecvMsg, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(st_ThreadPool.tszMsgBuffer, lpszRecvMsg + sizeof(XENGINE_PROTOCOLHDR), st_ThreadPool.nMsgLen);

	MessageQueue_Callback_ThreadPool(&st_ThreadPool);
}

BOOL NetEngine_MQXService_Send(LPCTSTR lpszClientAddr, LPCTSTR lpszMsgBuffer, int nMsgLen, int nIPProto)
{
    if (1 == nIPProto)
    {
        if (!NetCore_TCPXCore_SendEx(xhTCPSocket, lpszClientAddr, lpszMsgBuffer, nMsgLen))
        {
            XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("发送数据给TCP客户端：%s，失败，错误：%lX"), lpszClientAddr, NetCore_GetLastError());
            return FALSE;
        }
    }
    if (2 == nIPProto)
    {
        if (!NetCore_UDPXCore_SendMsgEx(xhUDPSocket, lpszClientAddr, lpszMsgBuffer, &nMsgLen))
        {
            XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("发送数据给UDP客户端：%s，失败，错误：%lX"), lpszClientAddr, NetCore_GetLastError());
            return FALSE;
        }
    }
    return TRUE;
}