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
 参数.一：nNetType
  In/Out：In
  类型：整数型
  可空：N
  意思：输入网络类型
 参数.二：pSt_ProtocolHdr
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：协议头
 参数.三：pSt_MQProtocol
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：消息头
 参数.四：ptszMsgBuffer
  In/Out：Out
  类型：字符指针
  可空：N
  意思：导出封装号的协议
 参数.五：pInt_MsgLen
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：导出封装的协议的长度
 参数.六：lpszMsgBuffer
  In/Out：In
  类型：常量字符指针
  可空：Y
  意思：要封装的数据
 参数.七：nMsgLen
  In/Out：In
  类型：整数型
  可空：Y
  意思：要封装数据的长度
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CProtocolModule_Packet::ProtocolModule_Packet_Common(int nNetType, XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, TCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCTSTR lpszMsgBuffer /* = NULL */, int nMsgLen /* = 0 */)
{
	Protocol_IsErrorOccur = FALSE;

	if (XENGINE_MQAPP_NETTYPE_TCP == nNetType)
	{
		ProtocolModule_Packet_TCPCommon(pSt_ProtocolHdr, pSt_MQProtocol, ptszMsgBuffer, pInt_MsgLen, lpszMsgBuffer, nMsgLen);
	}
	else
	{
		ProtocolModule_Packet_HttpCommon(pSt_ProtocolHdr, pSt_MQProtocol, ptszMsgBuffer, pInt_MsgLen, lpszMsgBuffer, nMsgLen);
	}
	return TRUE;
}
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
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CProtocolModule_Packet::ProtocolModule_Packet_TCPCommon(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, TCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCTSTR lpszMsgBuffer /* = NULL */, int nMsgLen /* = 0 */)
{
    Protocol_IsErrorOccur = FALSE;

    if ((NULL == pSt_ProtocolHdr) || (NULL == ptszMsgBuffer) || (NULL == pInt_MsgLen))
    {
        Protocol_IsErrorOccur = TRUE;
        Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_PARAMENT;
        return FALSE;
    }
	
	if (NULL == pSt_MQProtocol)
	{
		pSt_ProtocolHdr->unPacketSize = nMsgLen;
		memcpy(ptszMsgBuffer, pSt_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));

		*pInt_MsgLen = sizeof(XENGINE_PROTOCOLHDR) + nMsgLen;
	}
	else
	{
		pSt_ProtocolHdr->unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ) + nMsgLen;
		memcpy(ptszMsgBuffer, pSt_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));

		memcpy(ptszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), pSt_MQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));
		if (NULL != lpszMsgBuffer)
		{
			memcpy(ptszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR) + sizeof(XENGINE_PROTOCOL_XMQ), lpszMsgBuffer, nMsgLen);
		}
		*pInt_MsgLen = sizeof(XENGINE_PROTOCOLHDR) + sizeof(XENGINE_PROTOCOL_XMQ) + nMsgLen;
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
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CProtocolModule_Packet::ProtocolModule_Packet_HttpCommon(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, TCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCTSTR lpszMsgBuffer /* = NULL */, int nMsgLen /* = 0 */)
{
    Json::Value st_JsonRoot;
    Json::Value st_JsonMQProtocol;
    Json::Value st_JsonPayload;
	Json::StreamWriterBuilder st_JsonBuilder;

	st_JsonRoot["unOperatorType"] = pSt_ProtocolHdr->unOperatorType;
	st_JsonRoot["unOperatorCode"] = pSt_ProtocolHdr->unOperatorCode;
	st_JsonRoot["xhToken"] = pSt_ProtocolHdr->xhToken;
	st_JsonRoot["wReserve"] = pSt_ProtocolHdr->wReserve;

	if (NULL != pSt_MQProtocol)
	{
		st_JsonMQProtocol["tszMQKey"] = pSt_MQProtocol->tszMQKey;
		st_JsonMQProtocol["nSerial"] = (Json::Value::Int64)pSt_MQProtocol->nSerial;
		st_JsonMQProtocol["nGetTimer"] = pSt_MQProtocol->nGetTimer;
		st_JsonMQProtocol["nKeepTime"] = pSt_MQProtocol->nKeepTime;
		st_JsonMQProtocol["nPubTime"] = (Json::Value::Int64)pSt_MQProtocol->nPubTime;
		st_JsonRoot["st_MQProtocol"] = st_JsonMQProtocol;
	}
	
    if (nMsgLen > 0)
    {
        st_JsonPayload["nPayType"] = (ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE)pSt_ProtocolHdr->byVersion;
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
 参数.五：nNetType
  In/Out：Out
  类型：整数型
  可空：N
  意思：输入网络类型
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CProtocolModule_Packet::ProtocolModule_Packet_MQNumber(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_MQNUMBER* pSt_MQNumber, TCHAR* ptszMsgBuffer, int* pInt_MsgLen, int nNetType)
{
	Protocol_IsErrorOccur = FALSE;

	if ((NULL == pSt_ProtocolHdr) || (NULL == ptszMsgBuffer) || (NULL == pInt_MsgLen))
	{
		Protocol_IsErrorOccur = TRUE;
		Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_PARAMENT;
		return FALSE;
	}

	if (XENGINE_MQAPP_NETTYPE_TCP == nNetType)
	{
		pSt_ProtocolHdr->unPacketSize = sizeof(XENGINE_MQNUMBER);
		memcpy(ptszMsgBuffer, pSt_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
		memcpy(ptszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), pSt_MQNumber, sizeof(XENGINE_MQNUMBER));
		*pInt_MsgLen = sizeof(XENGINE_PROTOCOLHDR) + sizeof(XENGINE_MQNUMBER);
	}
	else
	{
		Json::Value st_JsonRoot;
		Json::Value st_JsonObject;
		Json::StreamWriterBuilder st_JsonBuilder;

		st_JsonRoot["unOperatorType"] = pSt_ProtocolHdr->unOperatorType;
		st_JsonRoot["unOperatorCode"] = pSt_ProtocolHdr->unOperatorCode;
		st_JsonRoot["wReserve"] = pSt_ProtocolHdr->wReserve;

		st_JsonObject["tszMQKey"] = pSt_MQNumber->tszMQKey;
		st_JsonObject["nCount"] = (Json::Value::Int64)pSt_MQNumber->nCount;
		st_JsonObject["nFirstNumber"] = (Json::Value::Int64)pSt_MQNumber->nFirstNumber;
		st_JsonObject["nLastNumber"] = (Json::Value::Int64)pSt_MQNumber->nLastNumber;
		
		st_JsonRoot["st_MQNumber"] = st_JsonObject;

		st_JsonBuilder["emitUTF8"] = true;

		*pInt_MsgLen = Json::writeString(st_JsonBuilder, st_JsonRoot).length();
		memcpy(ptszMsgBuffer, Json::writeString(st_JsonBuilder, st_JsonRoot).c_str(), *pInt_MsgLen);
	}
	return TRUE;
}
/********************************************************************
函数名称：ProtocolModule_Packet_PassAuth
函数功能：HTTP代理打包函数
 参数.一：pSt_ProtocolAuth
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要打包的内容
 参数.二：ptszMsgBuffer
  In/Out：Out
  类型：字符指针
  可空：N
  意思：输出打好包的缓冲区
 参数.三：pInt_MsgLen
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出缓冲区大小
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CProtocolModule_Packet::ProtocolModule_Packet_PassAuth(XENGINE_PROTOCOL_USERAUTH* pSt_ProtocolAuth, TCHAR* ptszMsgBuffer, int* pInt_MsgLen)
{
	Protocol_IsErrorOccur = FALSE;

	if ((NULL == pSt_ProtocolAuth) || (NULL == ptszMsgBuffer) || (NULL == pInt_MsgLen))
	{
		Protocol_IsErrorOccur = TRUE;
		Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_PARAMENT;
		return FALSE;
	}
	Json::Value st_JsonRoot;
	Json::StreamWriterBuilder st_JsonBuilder;

	st_JsonRoot["tszUserName"] = pSt_ProtocolAuth->tszUserName;
	st_JsonRoot["tszUserPass"] = pSt_ProtocolAuth->tszUserPass;
	st_JsonRoot["enClientType"] = pSt_ProtocolAuth->enClientType;
	st_JsonRoot["enDeviceType"] = pSt_ProtocolAuth->enDeviceType;
	st_JsonBuilder["emitUTF8"] = true;

	*pInt_MsgLen = Json::writeString(st_JsonBuilder, st_JsonRoot).length();
	memcpy(ptszMsgBuffer, Json::writeString(st_JsonBuilder, st_JsonRoot).c_str(), *pInt_MsgLen);
	return TRUE;
}
/********************************************************************
函数名称：ProtocolModule_Packet_DDSTopic
函数功能：DDS主题协议打包函数
 参数.一：pSt_DDSProtocol
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要打包的DDS协议
 参数.二：ptszMsgBuffer
  In/Out：Out
  类型：字符指针
  可空：N
  意思：输出打好包的缓冲区
 参数.三：pInt_MsgLen
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出缓冲区大小
 参数.四：dwOPeratorCode
  In/Out：In
  类型：整数型
  可空：N
  意思：输入操作码
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CProtocolModule_Packet::ProtocolModule_Packet_DDSTopic(XENGINE_PROTOCOL_XDDS* pSt_DDSProtocol, TCHAR* ptszMsgBuffer, int* pInt_MsgLen, DWORD dwOPeratorCode)
{
	Protocol_IsErrorOccur = FALSE;
	//发布通知
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	//打包协议头
	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XDDS;
	st_ProtocolHdr.unOperatorCode = dwOPeratorCode;
	st_ProtocolHdr.xhToken = 0;
	st_ProtocolHdr.byIsReply = FALSE;
	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XDDS);
	st_ProtocolHdr.wPacketSerial = 0;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;
	//组成数据包
	*pInt_MsgLen = sizeof(XENGINE_PROTOCOLHDR) + sizeof(XENGINE_PROTOCOL_XDDS);
	memcpy(ptszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(ptszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), pSt_DDSProtocol, sizeof(XENGINE_PROTOCOL_XDDS));

	return TRUE;
}
/********************************************************************
函数名称：ProtocolModule_Packet_DDSQuery
函数功能：DDS查询回复协议
 参数.一：pppSt_DDSProtocol
  In/Out：In
  类型：三级指针
  可空：N
  意思：输入DDS信息列表
 参数.二：nListCount
  In/Out：In
  类型：整数型
  可空：N
  意思：输入列表个数
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
BOOL CProtocolModule_Packet::ProtocolModule_Packet_DDSQuery(XENGINE_PROTOCOL_XDDS*** pppSt_DDSProtocol, int nListCount, TCHAR* ptszMsgBuffer, int* pInt_MsgLen)
{
	Protocol_IsErrorOccur = FALSE;
	//发布通知
	Json::Value st_JsonRoot;
	Json::Value st_JsonArray;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));

	for (int i = 0; i < nListCount; i++)
	{
		Json::Value st_JsonObject;

		st_JsonObject["tszTopic"] = (*pppSt_DDSProtocol)[i]->tszTopic;
		st_JsonObject["tszDDSAddr"] = (*pppSt_DDSProtocol)[i]->tszDDSAddr;
		st_JsonObject["bCreater"] = (*pppSt_DDSProtocol)[i]->bCreater;
		st_JsonObject["bTcp"] = (*pppSt_DDSProtocol)[i]->bTcp;
		st_JsonObject["nPort"] = (*pppSt_DDSProtocol)[i]->nPort;
		st_JsonArray.append(st_JsonObject);
	}
	st_JsonRoot["ListArray"] = st_JsonArray;
	st_JsonRoot["ListCount"] = st_JsonArray.size();
	//打包协议头
	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XDDS;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_DDS_REPTOPICQUERY;
	st_ProtocolHdr.xhToken = 0;
	st_ProtocolHdr.byIsReply = FALSE;
	st_ProtocolHdr.unPacketSize = st_JsonRoot.toStyledString().length();
	st_ProtocolHdr.wPacketSerial = 0;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;
	//组成数据包
	*pInt_MsgLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(ptszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(ptszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), st_JsonRoot.toStyledString().c_str(), st_ProtocolHdr.unPacketSize);

	return TRUE;
}