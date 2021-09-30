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
    BOOL ProtocolModule_Packet_TCPCommon(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, TCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCTSTR lpszMsgBuffer = NULL, int nMsgLen = 0, ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE enPayType = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_STRING);
    BOOL ProtocolModule_Packet_HttpCommon(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, TCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCTSTR lpszMsgBuffer = NULL, int nMsgLen = 0, ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE enPayType = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_STRING);
    BOOL ProtocolModule_Packet_MQNumber(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_MQNUMBER* pSt_MQNumber, TCHAR* ptszMsgBuffer, int* pInt_MsgLen);
public:
    BOOL ProtocolModule_Packet_DDSTopic(XENGINE_PROTOCOL_XDDS* pSt_DDSProtocol, TCHAR* ptszMsgBuffer, int* pInt_MsgLen, DWORD dwOPeratorCode);
    BOOL ProtocolModule_Packet_DDSQuery(XENGINE_PROTOCOL_XDDS*** pppSt_DDSProtocol, int nListCount, TCHAR* ptszMsgBuffer, int* pInt_MsgLen);
protected:
private:
};
