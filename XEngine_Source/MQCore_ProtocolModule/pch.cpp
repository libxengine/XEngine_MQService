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
extern "C" BOOL ProtocolModule_Packet_TCPCommon(XENGINE_PROTOCOLHDR * pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ * pSt_MQProtocol, TCHAR * ptszMsgBuffer, int* pInt_MsgLen, LPCTSTR lpszMsgBuffer, int nMsgLen, ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE enPayType)
{
	return m_ProtocolPacket.ProtocolModule_Packet_TCPCommon(pSt_ProtocolHdr, pSt_MQProtocol, ptszMsgBuffer, pInt_MsgLen, lpszMsgBuffer, nMsgLen, enPayType);
}
extern "C" BOOL ProtocolModule_Packet_HttpCommon(XENGINE_PROTOCOLHDR * pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ * pSt_MQProtocol, TCHAR * ptszMsgBuffer, int* pInt_MsgLen, LPCTSTR lpszMsgBuffer, int nMsgLen, ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE enPayType)
{
	return m_ProtocolPacket.ProtocolModule_Packet_HttpCommon(pSt_ProtocolHdr, pSt_MQProtocol, ptszMsgBuffer, pInt_MsgLen, lpszMsgBuffer, nMsgLen, enPayType);
}
extern "C" BOOL ProtocolModule_Packet_MQNumber(XENGINE_PROTOCOLHDR * pSt_ProtocolHdr, XENGINE_MQNUMBER * pSt_MQNumber, TCHAR * ptszMsgBuffer, int* pInt_MsgLen)
{
	return m_ProtocolPacket.ProtocolModule_Packet_MQNumber(pSt_ProtocolHdr, pSt_MQNumber, ptszMsgBuffer, pInt_MsgLen);
}
extern "C" BOOL ProtocolModule_Packet_DDSTopic(XENGINE_PROTOCOL_XDDS * pSt_DDSProtocol, TCHAR * ptszMsgBuffer, int* pInt_MsgLen, DWORD dwOPeratorCode)
{
	return m_ProtocolPacket.ProtocolModule_Packet_DDSTopic(pSt_DDSProtocol, ptszMsgBuffer, pInt_MsgLen, dwOPeratorCode);
}
extern "C" BOOL ProtocolModule_Packet_DDSQuery(XENGINE_PROTOCOL_XDDS * **pppSt_DDSProtocol, int nListCount, TCHAR * ptszMsgBuffer, int* pInt_MsgLen)
{
	return m_ProtocolPacket.ProtocolModule_Packet_DDSQuery(pppSt_DDSProtocol, nListCount, ptszMsgBuffer, pInt_MsgLen);
}
/************************************************************************/
/*                        解析类函数                                    */
/************************************************************************/
extern "C" BOOL ProtocolModule_Parse_Http(LPCTSTR lpszMsgBuffer, int nMsgLen, XENGINE_PROTOCOL_XMQ * pSt_MQProtocol, XENGINE_PROTOCOLHDR * pSt_ProtocolHdr, TCHAR * *pptszMsgBuffer, int* pInt_MsgLen, int* pInt_Type)
{
	return m_ProtocolParse.ProtocolModule_Parse_Http(lpszMsgBuffer, nMsgLen, pSt_MQProtocol, pSt_ProtocolHdr, pptszMsgBuffer, pInt_MsgLen, pInt_Type);
}
extern "C" BOOL ProtocolModule_Parse_DDSQuery(LPCTSTR lpszMsgBuffer, int nMsgLen, XENGINE_PROTOCOL_XDDS * **pppSt_DDSProtocol, int* pInt_ListCount)
{
	return m_ProtocolParse.ProtocolModule_Parse_DDSQuery(lpszMsgBuffer, nMsgLen, pppSt_DDSProtocol, pInt_ListCount);
}