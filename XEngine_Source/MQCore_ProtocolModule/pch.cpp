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
BOOL Protocol_IsErrorOccur = FALSE;
DWORD Protocol_dwErrorCode = 0;
//////////////////////////////////////////////////////////////////////////
CProtocolModule_Parse m_ProtocolParse;
CProtocolModule_Packet m_ProtocolPacket;
//////////////////////////////////////////////////////////////////////////
//                        导出函数
//////////////////////////////////////////////////////////////////////////
extern "C" DWORD Protocol_GetLastError(int* pInt_SysError)
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
extern "C" BOOL ProtocolModule_Packet_Common(int nNetType, XENGINE_PROTOCOLHDR * pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ * pSt_MQProtocol, TCHAR * ptszMsgBuffer, int* pInt_MsgLen, LPCTSTR lpszMsgBuffer, int nMsgLen)
{
	return m_ProtocolPacket.ProtocolModule_Packet_Common(nNetType, pSt_ProtocolHdr, pSt_MQProtocol, ptszMsgBuffer, pInt_MsgLen, lpszMsgBuffer, nMsgLen);
}
extern "C" BOOL ProtocolModule_Packet_MQNumber(XENGINE_PROTOCOLHDR * pSt_ProtocolHdr, XENGINE_MQNUMBER * pSt_MQNumber, TCHAR * ptszMsgBuffer, int* pInt_MsgLen, int nNetType)
{
	return m_ProtocolPacket.ProtocolModule_Packet_MQNumber(pSt_ProtocolHdr, pSt_MQNumber, ptszMsgBuffer, pInt_MsgLen, nNetType);
}
extern "C" BOOL ProtocolModule_Packet_PassAuth(XENGINE_PROTOCOL_USERAUTH * pSt_ProtocolAuth, TCHAR * ptszMsgBuffer, int* pInt_MsgLen, int nCode)
{
	return m_ProtocolPacket.ProtocolModule_Packet_PassAuth(pSt_ProtocolAuth, ptszMsgBuffer, pInt_MsgLen, nCode);
}
extern "C" BOOL ProtocolModule_Packet_PassUser(XENGINE_PROTOCOL_USERINFO * pSt_ProtocolUser, TCHAR * ptszMsgBuffer, int* pInt_MsgLen, int nCode)
{
	return m_ProtocolPacket.ProtocolModule_Packet_PassUser(pSt_ProtocolUser, ptszMsgBuffer, pInt_MsgLen, nCode);
}
/************************************************************************/
/*                        解析类函数                                    */
/************************************************************************/
extern "C" BOOL ProtocolModule_Parse_Http(LPCTSTR lpszMsgBuffer, int nMsgLen, XENGINE_PROTOCOLHDR * pSt_ProtocolHdr, TCHAR *ptszMsgBuffer, int* pInt_MsgLen)
{
	return m_ProtocolParse.ProtocolModule_Parse_Http(lpszMsgBuffer, nMsgLen, pSt_ProtocolHdr, ptszMsgBuffer, pInt_MsgLen);
}