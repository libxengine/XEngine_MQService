#include "pch.h"
#include "ProtocolModule_Packet/ProtocolModule_Packet.h"
#include "ProtocolModule_Parse/ProtocolModule_Parse.h"
/********************************************************************
//    Created:     2021/07/01  16:41:38
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_ProtocolModule\pch.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_ProtocolModule
//    File Base:   pch
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     导出实现
//    History:
*********************************************************************/
bool Protocol_IsErrorOccur = false;
XLONG Protocol_dwErrorCode = 0;
//////////////////////////////////////////////////////////////////////////
CProtocolModule_Parse m_ProtocolParse;
CProtocolModule_Packet m_ProtocolPacket;
//////////////////////////////////////////////////////////////////////////
//                        导出函数
//////////////////////////////////////////////////////////////////////////
extern "C" XLONG Protocol_GetLastError(int* pInt_SysError)
{
	if (NULL != pInt_SysError)
	{
		*pInt_SysError = errno;
	}
	return Protocol_dwErrorCode;
}
/************************************************************************/
/*                        封装类函数                                    */
/************************************************************************/
extern "C" bool ProtocolModule_Packet_Common(int nNetType, XENGINE_PROTOCOLHDR * pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ * pSt_MQProtocol, XCHAR * ptszMsgBuffer, int* pInt_MsgLen, LPCXSTR lpszMsgBuffer, int nMsgLen)
{
	return m_ProtocolPacket.ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, pSt_MQProtocol, ptszMsgBuffer, pInt_MsgLen, lpszMsgBuffer, nMsgLen);
}
extern "C" bool ProtocolModule_Packet_HTTPCommon(XCHAR* ptszMsgBuffer, int* pInt_MsgLen, int nHTTPCode, LPCXSTR lpszMSGBuffer)
{
	return m_ProtocolPacket.ProtocolModule_Packet_HTTPCommon(ptszMsgBuffer, pInt_MsgLen, nHTTPCode, lpszMSGBuffer);
}
extern "C" bool ProtocolModule_Packet_MQNumber(XENGINE_PROTOCOLHDR * pSt_ProtocolHdr, XENGINE_MQNUMBER * pSt_MQNumber, XCHAR * ptszMsgBuffer, int* pInt_MsgLen, int nNetType)
{
	return m_ProtocolPacket.ProtocolModule_Packet_MQNumber(pSt_ProtocolHdr, pSt_MQNumber, ptszMsgBuffer, pInt_MsgLen, nNetType);
}
extern "C" bool ProtocolModule_Packet_PassAuth(XENGINE_PROTOCOL_USERAUTH * pSt_ProtocolAuth, XCHAR * ptszMsgBuffer, int* pInt_MsgLen, int nCode)
{
	return m_ProtocolPacket.ProtocolModule_Packet_PassAuth(pSt_ProtocolAuth, ptszMsgBuffer, pInt_MsgLen, nCode);
}
extern "C" bool ProtocolModule_Packet_PassUser(XENGINE_PROTOCOL_USERINFO * pSt_ProtocolUser, XCHAR * ptszMsgBuffer, int* pInt_MsgLen, int nCode)
{
	return m_ProtocolPacket.ProtocolModule_Packet_PassUser(pSt_ProtocolUser, ptszMsgBuffer, pInt_MsgLen, nCode);
}
extern "C" bool ProtocolModule_Packet_PassHttp(XCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCXSTR lpszToken, LPCXSTR lpszAPIUser, LPCXSTR lpszURIApi)
{
	return m_ProtocolPacket.ProtocolModule_Packet_PassHttp(ptszMsgBuffer, pInt_MsgLen, lpszToken, lpszAPIUser, lpszURIApi);
}
extern "C" bool ProtocolModule_Packet_Http(XCHAR * ptszMsgBuffer, int* pInt_MsgLen, int nCode, LPCXSTR lpszMsgBuffer)
{
	return m_ProtocolPacket.ProtocolModule_Packet_Http(ptszMsgBuffer, pInt_MsgLen, nCode, lpszMsgBuffer);
}
extern "C" bool ProtocolModule_Packet_UserList(XCHAR * ptszMsgBuffer, int* pInt_MsgLen, XENGINE_PROTOCOL_USERINFO * **pppSt_UserInfo, int nListCount)
{
	return m_ProtocolPacket.ProtocolModule_Packet_UserList(ptszMsgBuffer, pInt_MsgLen, pppSt_UserInfo, nListCount);
}
extern "C" bool ProtocolModule_Packet_TopicList(XCHAR * ptszMsgBuffer, int* pInt_MsgLen, XCHAR * **pppszTableName, int nListCount)
{
	return m_ProtocolPacket.ProtocolModule_Packet_TopicList(ptszMsgBuffer, pInt_MsgLen, pppszTableName, nListCount);
}
extern "C" bool ProtocolModule_Packet_OnlineList(XCHAR * ptszMsgBuffer, int* pInt_MsgLen, XCHAR * **ppptszListUser, int nListCount)
{
	return m_ProtocolPacket.ProtocolModule_Packet_OnlineList(ptszMsgBuffer, pInt_MsgLen, ppptszListUser, nListCount);
}
extern "C" bool ProtocolModule_Packet_TopicName(XCHAR * ptszMsgBuffer, int* pInt_MsgLen, LPCXSTR lpszTopicName, int nTopicCount)
{
	return m_ProtocolPacket.ProtocolModule_Packet_TopicName(ptszMsgBuffer, pInt_MsgLen, lpszTopicName, nTopicCount);
}
extern "C" XHANDLE ProtocolModule_Packet_UNReadCreate(XENGINE_PROTOCOLHDR * pSt_ProtocolHdr, ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE enPayType)
{
	return m_ProtocolPacket.ProtocolModule_Packet_UNReadCreate(pSt_ProtocolHdr, enPayType);
}
extern "C" bool ProtocolModule_Packet_UNReadInsert(XHANDLE xhToken, LPCXSTR lpszKeyName, int nListCount)
{
	return m_ProtocolPacket.ProtocolModule_Packet_UNReadInsert(xhToken, lpszKeyName, nListCount);
}
extern "C" bool ProtocolModule_Packet_UNReadDelete(XHANDLE xhToken, XCHAR * ptszMsgBuffer, int* pInt_MsgLen)
{
	return m_ProtocolPacket.ProtocolModule_Packet_UNReadDelete(xhToken, ptszMsgBuffer, pInt_MsgLen);
}
/************************************************************************/
/*                        解析类函数                                    */
/************************************************************************/
extern "C" bool ProtocolModule_Parse_Websocket(LPCXSTR lpszMsgBuffer, int nMsgLen, XENGINE_PROTOCOLHDR * pSt_ProtocolHdr, XCHAR *ptszMsgBuffer, int* pInt_MsgLen)
{
	return m_ProtocolParse.ProtocolModule_Parse_Websocket(lpszMsgBuffer, nMsgLen, pSt_ProtocolHdr, ptszMsgBuffer, pInt_MsgLen);
}
extern "C" bool ProtocolModule_Parse_Register(LPCXSTR lpszMsgBuffer, int nMsgLen, XENGINE_PROTOCOL_USERINFO* pSt_UserInfo)
{
	return m_ProtocolParse.ProtocolModule_Parse_Register(lpszMsgBuffer, nMsgLen, pSt_UserInfo);
}