#include "pch.h"
#include "XMQModule_Packet/XMQModule_Packet.h"
/********************************************************************
//    Created:     2021/06/30  14:49:10
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_XMQModule\pch.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_XMQModule
//    File Base:   pch
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息队列导出函数实现
//    History:
*********************************************************************/
BOOL MQX_IsErrorOccur = FALSE;
DWORD MQX_dwErrorCode = 0;
///////////////////////////////////////////////////////////////////////////////
CXMQModule_Packet m_MQXPacket;
///////////////////////////////////////////////////////////////////////////////
//                      导出函数实现
///////////////////////////////////////////////////////////////////////////////
extern "C" DWORD XMQModule_GetLastError(int* pInt_SysError)
{
    if (NULL != pInt_SysError)
    {
        *pInt_SysError = errno;
    }
    return MQX_dwErrorCode;
}
/*************************************************************************
                        消息队列包管理导出函数
**************************************************************************/
extern "C" BOOL XMQModule_Packet_Init(LPCTSTR lpszCommonName, int nPacketCount)
{
    return m_MQXPacket.XMQModule_Packet_Init(lpszCommonName, nPacketCount);
}
extern "C" BOOL XMQModule_Packet_Destory()
{
    return m_MQXPacket.XMQModule_Packet_Destory();
}
extern "C" BOOL XMQModule_Packet_TopicCreate(LPCTSTR lpszKeyStr)
{
    return m_MQXPacket.XMQModule_Packet_TopicCreate(lpszKeyStr);
}
extern "C" BOOL XMQModule_Packet_TopicDelete(LPCTSTR lpszKeyStr)
{
    return m_MQXPacket.XMQModule_Packet_TopicDelete(lpszKeyStr);
}
extern "C" BOOL XMQModule_Packet_Post(XENGINE_PROTOCOL_XMQ * pSt_MQProtocol, LPCTSTR lpszMsgBuffer, int nMsgLen)
{
    return m_MQXPacket.XMQModule_Packet_Post(pSt_MQProtocol, lpszMsgBuffer, nMsgLen);
}
extern "C" BOOL XMQModule_Packet_Get(XENGINE_PROTOCOL_XMQ * pSt_MQProtocol, TCHAR * ptszMsgBuffer, int* pInt_Len)
{
    return m_MQXPacket.XMQModule_Packet_Get(pSt_MQProtocol, ptszMsgBuffer, pInt_Len);
}
extern "C" BOOL XMQModule_Packet_Del(XENGINE_PROTOCOL_XMQ * pSt_MQProtocol)
{
    return m_MQXPacket.XMQModule_Packet_Del(pSt_MQProtocol);
}