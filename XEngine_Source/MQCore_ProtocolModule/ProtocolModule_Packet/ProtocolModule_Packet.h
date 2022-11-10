#pragma once
/********************************************************************
//    Created:     2021/07/01  15:37:29
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_ProtocolModule\ProtocolModule_Packet\ProtocolModule_Packet.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_ProtocolModule\ProtocolModule_Packet
//    File Base:   ProtocolModule_Packet
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息队列协议组包器
//    History:
*********************************************************************/

class CProtocolModule_Packet
{
public:
    CProtocolModule_Packet();
    ~CProtocolModule_Packet();
public:
    BOOL ProtocolModule_Packet_Common(int nNetType, XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, TCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCTSTR lpszMsgBuffer = NULL, int nMsgLen = 0);
    BOOL ProtocolModule_Packet_TCPCommon(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, TCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCTSTR lpszMsgBuffer = NULL, int nMsgLen = 0);
    BOOL ProtocolModule_Packet_HttpCommon(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, TCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCTSTR lpszMsgBuffer = NULL, int nMsgLen = 0);
    BOOL ProtocolModule_Packet_MQNumber(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_MQNUMBER* pSt_MQNumber, TCHAR* ptszMsgBuffer, int* pInt_MsgLen, int nNetType);
    BOOL ProtocolModule_Packet_PassAuth(XENGINE_PROTOCOL_USERAUTH* pSt_ProtocolAuth, TCHAR* ptszMsgBuffer, int* pInt_MsgLen, int nCode);
    BOOL ProtocolModule_Packet_PassUser(XENGINE_PROTOCOL_USERINFO* pSt_ProtocolUser, TCHAR* ptszMsgBuffer, int* pInt_MsgLen, int nCode);
protected:
private:
};
