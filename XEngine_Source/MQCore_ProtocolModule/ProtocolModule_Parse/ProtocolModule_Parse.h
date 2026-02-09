#pragma once
/********************************************************************
//    Created:     2021/07/01  15:35:21
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_ProtocolModule\ProtocolModule_Parse\ProtocolModule_Parse.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_ProtocolModule\ProtocolModule_Parse
//    File Base:   ProtocolModule_Parse
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息队列协议解析器
//    History:
*********************************************************************/

class CProtocolModule_Parse
{
public:
    CProtocolModule_Parse();
    ~CProtocolModule_Parse();
public:
    bool ProtocolModule_Parse_Websocket(LPCXSTR lpszMsgBuffer, int nMsgLen, XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XCHAR* ptszMsgBuffer, int* pInt_MsgLen);
    bool ProtocolModule_Parse_Register(LPCXSTR lpszMsgBuffer, int nMsgLen, XENGINE_PROTOCOL_USERINFO* pSt_UserInfo);
    bool ProtocolModule_Parse_Token(LPCXSTR lpszMsgBuffer, int nMsgLen, XNETHANDLE* pxhToken);
    bool ProtocolModule_Parse_Type(LPCXSTR lpszMsgBuffer, int nMsgLen, int* pInt_Type);
    bool ProtocolModule_Parse_XMQ(LPCXSTR lpszMsgBuffer, int nMsgLen, XENGINE_PROTOCOL_XMQ* pSt_XMQProtocol);
protected:
private:
};
