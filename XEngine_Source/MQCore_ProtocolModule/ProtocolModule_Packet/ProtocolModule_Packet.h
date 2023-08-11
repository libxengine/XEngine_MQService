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
typedef struct  
{
    ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE nType;

	Json::Value st_JsonRoot;
	Json::Value st_JsonArray;
    XENGINE_PROTOCOLHDR st_ProtocolHdr;
}PROTOCOL_PACKETUNREAD;

class CProtocolModule_Packet
{
public:
    CProtocolModule_Packet();
    ~CProtocolModule_Packet();
public:
    bool ProtocolModule_Packet_Common(int nNetType, XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, XCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCXSTR lpszMsgBuffer = NULL, int nMsgLen = 0);
    bool ProtocolModule_Packet_TCPCommon(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, XCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCXSTR lpszMsgBuffer = NULL, int nMsgLen = 0);
    bool ProtocolModule_Packet_HttpCommon(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, XCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCXSTR lpszMsgBuffer = NULL, int nMsgLen = 0);
    bool ProtocolModule_Packet_MQNumber(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_MQNUMBER* pSt_MQNumber, XCHAR* ptszMsgBuffer, int* pInt_MsgLen, int nNetType);
    bool ProtocolModule_Packet_PassAuth(XENGINE_PROTOCOL_USERAUTH* pSt_ProtocolAuth, XCHAR* ptszMsgBuffer, int* pInt_MsgLen, int nCode);
    bool ProtocolModule_Packet_PassUser(XENGINE_PROTOCOL_USERINFO* pSt_ProtocolUser, XCHAR* ptszMsgBuffer, int* pInt_MsgLen, int nCode);
    bool ProtocolModule_Packet_Http(XCHAR* ptszMsgBuffer, int* pInt_MsgLen, int nCode = 0, LPCXSTR lpszMsgBuffer = NULL);
    bool ProtocolModule_Packet_UserList(XCHAR* ptszMsgBuffer, int* pInt_MsgLen, XENGINE_PROTOCOL_USERINFO*** pppSt_UserInfo, int nListCount);
public:
	XHANDLE ProtocolModule_Packet_UNReadCreate(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE enPayType);
	bool ProtocolModule_Packet_UNReadInsert(XHANDLE xhToken, XENGINE_DBMESSAGEQUEUE*** pppSt_DBMessage, int nListCount);
	bool ProtocolModule_Packet_UNReadDelete(XHANDLE xhToken, XCHAR* ptszMsgBuffer, int* pInt_MsgLen);
protected:
private:
};
