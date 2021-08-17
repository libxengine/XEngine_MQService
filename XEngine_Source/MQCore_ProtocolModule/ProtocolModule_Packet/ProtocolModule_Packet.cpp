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
 参数.二：pSt_MQProtocol
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：消息头
 参数.三：ptszMsgBuffer
  In/Out：Out
  类型：字符指针
  可空：N
  意思：导出封装号的协议
 参数.四：pInt_MsgLen
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：导出封装的协议的长度
 参数.五：lpszMsgBuffer
  In/Out：In
  类型：常量字符指针
  可空：Y
  意思：要封装的数据
 参数.六：nMsgLen
  In/Out：In
  类型：整数型
  可空：Y
  意思：要封装数据的长度
 参数.七：enPayType
  In/Out：In
  类型：枚举型
  可空：Y
  意思：消息类型
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CProtocolModule_Packet::ProtocolModule_Packet_TCPCommon(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, TCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCTSTR lpszMsgBuffer /* = NULL */, int nMsgLen /* = 0 */, ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE enPayType /* = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_STRING */)
{
    Protocol_IsErrorOccur = FALSE;

    if ((NULL == pSt_ProtocolHdr) || (NULL == ptszMsgBuffer) || (NULL == pInt_MsgLen))
    {
        Protocol_IsErrorOccur = TRUE;
        Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_PARAMENT;
        return FALSE;
    }

    if (ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_BIN == pSt_ProtocolHdr->byVersion)
    {
		pSt_ProtocolHdr->unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ) + nMsgLen;
		memcpy(ptszMsgBuffer, pSt_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
		memcpy(ptszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), pSt_MQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));
		if (nMsgLen)
		{
			memcpy(ptszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR) + sizeof(XENGINE_PROTOCOL_XMQ), lpszMsgBuffer, nMsgLen);
		}
		*pInt_MsgLen = sizeof(XENGINE_PROTOCOLHDR) + sizeof(XENGINE_PROTOCOL_XMQ) + nMsgLen;
    }
    else
    {
		Json::Value st_JsonRoot;
		Json::Value st_JsonPayload;
		Json::StreamWriterBuilder st_JsonBuilder;

		st_JsonRoot["tszMQKey"] = pSt_MQProtocol->tszMQKey;
		st_JsonRoot["nSerial"] = pSt_MQProtocol->nSerial;
		st_JsonRoot["nKeepTime"] = pSt_MQProtocol->nKeepTime;
		st_JsonRoot["nGetTimer"] = pSt_MQProtocol->nGetTimer;
		if (nMsgLen > 0)
		{
			st_JsonPayload["nPayType"] = (ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE)enPayType;
			st_JsonPayload["nPayLen"] = nMsgLen;
			st_JsonPayload["tszPayData"] = lpszMsgBuffer;

			st_JsonRoot["st_Payload"] = st_JsonPayload;
		}
		st_JsonBuilder["emitUTF8"] = true;

		pSt_ProtocolHdr->unPacketSize = Json::writeString(st_JsonBuilder, st_JsonRoot).length();
		memcpy(ptszMsgBuffer, pSt_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
		memcpy(ptszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), Json::writeString(st_JsonBuilder, st_JsonRoot).c_str(), pSt_ProtocolHdr->unPacketSize);

		*pInt_MsgLen = sizeof(XENGINE_PROTOCOLHDR) + pSt_ProtocolHdr->unPacketSize;
    }
   
    return TRUE;
}
/********************************************************************
函数名称：ProtocolModule_Packet_HttpCommon
函数功能：HTTP通用封包类
 参数.一：pSt_ProtocolHdr
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：协议头
 参数.二：pSt_MQProtocol
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：消息头
 参数.三：ptszMsgBuffer
  In/Out：Out
  类型：字符指针
  可空：N
  意思：导出封装号的协议
 参数.四：pInt_MsgLen
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：导出封装的协议的长度
 参数.五：lpszMsgBuffer
  In/Out：In
  类型：常量字符指针
  可空：Y
  意思：要封装的数据
 参数.六：nMsgLen
  In/Out：In
  类型：整数型
  可空：Y
  意思：要封装数据的长度
 参数.七：enPayType
  In/Out：In
  类型：枚举型
  可空：Y
  意思：消息类型
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CProtocolModule_Packet::ProtocolModule_Packet_HttpCommon(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, TCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCTSTR lpszMsgBuffer /* = NULL */, int nMsgLen /* = 0 */, ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE enPayType /* = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_STRING */)
{
    Json::Value st_JsonRoot;
    Json::Value st_JsonMQProtocol;
    Json::Value st_JsonPayload;
	Json::StreamWriterBuilder st_JsonBuilder;

    st_JsonMQProtocol["tszMQKey"] = pSt_MQProtocol->tszMQKey;
    st_JsonMQProtocol["nSerial"] = pSt_MQProtocol->nSerial;
    st_JsonMQProtocol["nKeepTime"] = pSt_MQProtocol->nKeepTime;
    st_JsonMQProtocol["nGetTimer"] = pSt_MQProtocol->nGetTimer;

	st_JsonRoot["unOperatorType"] = pSt_ProtocolHdr->unOperatorType;
	st_JsonRoot["unOperatorCode"] = pSt_ProtocolHdr->unOperatorCode;
	st_JsonRoot["wReserve"] = pSt_ProtocolHdr->wReserve;

    st_JsonRoot["st_MQProtocol"] = st_JsonMQProtocol;
    if (nMsgLen > 0)
    {
        st_JsonPayload["nPayType"] = (ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE)enPayType;
        st_JsonPayload["nPayLen"] = nMsgLen;
        st_JsonPayload["tszPayData"] = lpszMsgBuffer;

        st_JsonRoot["st_Payload"] = st_JsonPayload;
    }
    st_JsonBuilder["emitUTF8"] = true;

    *pInt_MsgLen = Json::writeString(st_JsonBuilder, st_JsonRoot).length();
    memcpy(ptszMsgBuffer, Json::writeString(st_JsonBuilder, st_JsonRoot).c_str(), *pInt_MsgLen);

    return TRUE;
}
/********************************************************************
函数名称：ProtocolModule_Packet_MQNumber
函数功能：获取序列打包函数
 参数.一：pSt_ProtocolHdr
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入协议头
 参数.二：pSt_MQNumber
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要打包的序列号信息
 参数.三：ptszMsgBuffer
  In/Out：Out
  类型：字符指针
  可空：N
  意思：输出打好包的缓冲区
 参数.四：pInt_MsgLen
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出缓冲区大小
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CProtocolModule_Packet::ProtocolModule_Packet_MQNumber(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_MQNUMBER* pSt_MQNumber, TCHAR* ptszMsgBuffer, int* pInt_MsgLen)
{
	Protocol_IsErrorOccur = FALSE;

	if ((NULL == pSt_ProtocolHdr) || (NULL == ptszMsgBuffer) || (NULL == pInt_MsgLen))
	{
		Protocol_IsErrorOccur = TRUE;
		Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_PARAMENT;
		return FALSE;
	}

	if (ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_BIN == pSt_ProtocolHdr->byVersion)
	{
		pSt_ProtocolHdr->unPacketSize = sizeof(XENGINE_MQNUMBER);
		memcpy(ptszMsgBuffer, pSt_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
		memcpy(ptszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), pSt_MQNumber, sizeof(XENGINE_MQNUMBER));
		*pInt_MsgLen = sizeof(XENGINE_PROTOCOLHDR) + sizeof(XENGINE_MQNUMBER);
	}
	else
	{
		Json::Value st_JsonRoot;
		Json::Value st_JsonPayload;
		Json::StreamWriterBuilder st_JsonBuilder;

		st_JsonRoot["tszMQKey"] = pSt_MQNumber->tszMQKey;
		st_JsonRoot["nCount"] = pSt_MQNumber->nCount;
		st_JsonRoot["nFirstNumber"] = pSt_MQNumber->nFirstNumber;
		st_JsonRoot["nLastNumber"] = pSt_MQNumber->nLastNumber;
		st_JsonBuilder["emitUTF8"] = true;

		*pInt_MsgLen = Json::writeString(st_JsonBuilder, st_JsonRoot).length();
		memcpy(ptszMsgBuffer, Json::writeString(st_JsonBuilder, st_JsonRoot).c_str(), *pInt_MsgLen);
	}

	return TRUE;
}