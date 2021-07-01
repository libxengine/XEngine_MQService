#include "pch.h"
#include "ProtocolModule_Packet.h"
/********************************************************************
//    Created:     2020/12/23  20:55:17
//    File Name:   H:\NetEngine_Windows\NetEngine_SourceCode\NetEngine_XMQCore\MQCore_XMQService\XMQService_Protocol\XMQService_Protocol.cpp
//    File Path:   H:\NetEngine_Windows\NetEngine_SourceCode\NetEngine_XMQCore\MQCore_XMQService\XMQService_Protocol
//    File Base:   XMQService_Protocol
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息队列协议组包器
//    History:
*********************************************************************/
CProtocolModule_Packet::CProtocolModule_Packet()
{
}
CProtocolModule_Packet::~CProtocolModule_Packet()
{
}
///////////////////////////////////////////////////////////////////////////////
//                      公有函数
///////////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：ProtocolModule_Packet_TCPCommon
函数功能：通用封包类
 参数.一：pSt_ProtocolHdr
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：协议头
 参数.二：ptszMsgBuffer
  In/Out：Out
  类型：字符指针
  可空：N
  意思：导出封装号的协议
 参数.三：pInt_MsgLen
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：导出封装的协议的长度
 参数.四：lpszMsgBuffer
  In/Out：In
  类型：常量字符指针
  可空：Y
  意思：要封装的数据
 参数.五：nMsgLen
  In/Out：In
  类型：整数型
  可空：Y
  意思：要封装数据的长度
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CProtocolModule_Packet::ProtocolModule_Packet_TCPCommon(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, TCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCTSTR lpszMsgBuffer, int nMsgLen)
{
    Protocol_IsErrorOccur = FALSE;

    if ((NULL == pSt_ProtocolHdr) || (NULL == ptszMsgBuffer) || (NULL == pInt_MsgLen))
    {
        Protocol_IsErrorOccur = TRUE;
        Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_PARAMENT;
        return FALSE;
    }
    pSt_ProtocolHdr->unPacketSize = nMsgLen;
    *pInt_MsgLen = sizeof(XENGINE_PROTOCOLHDR) + nMsgLen;
    memcpy(ptszMsgBuffer, pSt_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
    if (nMsgLen)
    {
        memcpy(ptszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), lpszMsgBuffer, nMsgLen);
	}
    return TRUE;
}