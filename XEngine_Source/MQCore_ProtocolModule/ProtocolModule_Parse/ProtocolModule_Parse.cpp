#include "pch.h"
#include "ProtocolModule_Parse.h"
/********************************************************************
//    Created:     2021/07/01  15:48:41
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_ProtocolModule\ProtocolModule_Parse\ProtocolModule_Parse.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_ProtocolModule\ProtocolModule_Parse
//    File Base:   ProtocolModule_Parse
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息队列协议解析器
//    History:
*********************************************************************/
CProtocolModule_Parse::CProtocolModule_Parse()
{
}
CProtocolModule_Parse::~CProtocolModule_Parse()
{
}
///////////////////////////////////////////////////////////////////////////////
//                      公有函数
///////////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：ProtocolModule_Parse_Http
函数功能：HTTP协议解析
 参数.一：lpszMsgBuffer
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要解析的缓冲区
 参数.二：nMsgLen
  In/Out：In
  类型：整数型
  可空：N
  意思：输入缓冲区大小
 参数.三：pSt_ProtocolHdr
  In/Out：Out
  类型：数据结构指针
  可空：Y
  意思：输出解析到的头协议
 参数.四：pptszMsgBuffer
  In/Out：Out
  类型：二级指针
  可空：Y
  意思：输出消息内容,需要释放内存
 参数.五：pInt_MsgLen
  In/Out：Out
  类型：整数型指针
  可空：Y
  意思：输出内容大小
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CProtocolModule_Parse::ProtocolModule_Parse_Http(LPCTSTR lpszMsgBuffer, int nMsgLen, XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, TCHAR** pptszMsgBuffer, int* pInt_MsgLen)
{
	Protocol_IsErrorOccur = FALSE;

	if (NULL == lpszMsgBuffer)
	{
		Protocol_IsErrorOccur = TRUE;
		Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_PARAMENT;
		return FALSE;
	}
	Json::Value st_JsonRoot;
	JSONCPP_STRING st_JsonError;
	Json::CharReaderBuilder st_ReaderBuilder;

	std::unique_ptr<Json::CharReader> const pSt_JsonReader(st_ReaderBuilder.newCharReader());
	if (!pSt_JsonReader->parse(lpszMsgBuffer, lpszMsgBuffer + nMsgLen, &st_JsonRoot, &st_JsonError))
	{
		Protocol_IsErrorOccur = TRUE;
		Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_PARSE;
		return FALSE;
	}
	Json::Value st_JsonMQProtocol = st_JsonRoot["st_MQProtocol"];

	if (!st_JsonRoot["unOperatorType"].isNull())
	{
		pSt_ProtocolHdr->unOperatorType = st_JsonRoot["unOperatorType"].asInt();
	}
	if (!st_JsonRoot["unOperatorCode"].isNull())
	{
		pSt_ProtocolHdr->unOperatorCode = st_JsonRoot["unOperatorCode"].asInt();
	}
	if (!st_JsonRoot["wReserve"].isNull())
	{
		pSt_ProtocolHdr->wReserve = st_JsonRoot["wReserve"].asInt();
	}
	if (!st_JsonRoot["byVersion"].isNull())
	{
		pSt_ProtocolHdr->byVersion = st_JsonRoot["byVersion"].asInt();
	}
	*pInt_MsgLen = 0;
	XENGINE_PROTOCOL_XMQ st_MQProtocol;
	XENGINE_PROTOCOL_USERAUTH st_ProtocolAuth;

	memset(&st_ProtocolAuth, '\0', sizeof(XENGINE_PROTOCOL_USERAUTH));
	memset(&st_MQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));
	//如果负载的是消息
	if (!st_JsonRoot["st_MQProtocol"].isNull())
	{
		_tcscpy(st_MQProtocol.tszMQKey, st_JsonMQProtocol["tszMQKey"].asCString());
		st_MQProtocol.nSerial = st_JsonMQProtocol["nSerial"].asInt();
		st_MQProtocol.nGetTimer = st_JsonMQProtocol["nGetTimer"].asInt();
		st_MQProtocol.nKeepTime = st_JsonMQProtocol["nKeepTime"].asInt();
		st_MQProtocol.nPubTime = st_JsonMQProtocol["nPubTime"].asInt64();
		*pInt_MsgLen += sizeof(XENGINE_PROTOCOL_XMQ);
	}
	//后者负载的是验证协议
	if (!st_JsonRoot["st_Auth"].isNull())
	{
		Json::Value st_JsonAuth = st_JsonRoot["st_Auth"];
		_tcscpy(st_ProtocolAuth.tszUserName, st_JsonAuth["tszUserName"].asCString());
		_tcscpy(st_ProtocolAuth.tszUserPass, st_JsonAuth["tszUserPass"].asCString());

		if (!st_JsonAuth["enClientType"].isNull())
		{
			st_ProtocolAuth.enClientType = (ENUM_PROTOCOLCLIENT_TYPE)st_JsonAuth["enClientType"].asInt();
		}
		if (!st_JsonAuth["enDeviceType"].isNull())
		{
			st_ProtocolAuth.enDeviceType = (ENUM_PROTOCOLDEVICE_TYPE)st_JsonAuth["enDeviceType"].asInt();
		}
		*pInt_MsgLen += sizeof(XENGINE_PROTOCOL_USERAUTH);
	}
	//或者包含附加内容
	if (!st_JsonRoot["st_Payload"].isNull())
	{
		Json::Value st_JsonPayLoad = st_JsonRoot["st_Payload"];
		*pInt_MsgLen += st_JsonPayLoad["nPayLen"].asInt();
	}

	*pptszMsgBuffer = (TCHAR*)malloc(*pInt_MsgLen);
	if (NULL == *pptszMsgBuffer)
	{
		Protocol_IsErrorOccur = TRUE;
		Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_MALLOC;
		return FALSE;
	}
	memset(*pptszMsgBuffer, '\0', *pInt_MsgLen);

	if (st_JsonRoot["st_MQProtocol"].isNull())
	{
		memcpy(*pptszMsgBuffer, &st_ProtocolAuth, sizeof(XENGINE_PROTOCOL_USERAUTH));
	}
	else
	{
		memcpy(*pptszMsgBuffer, &st_MQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));
		if (!st_JsonRoot["st_Payload"].isNull())
		{
			Json::Value st_JsonPayLoad = st_JsonRoot["st_Payload"];
			memcpy(*pptszMsgBuffer + sizeof(XENGINE_PROTOCOL_XMQ), st_JsonPayLoad["tszPayData"].asCString(), st_JsonPayLoad["nPayLen"].asInt());
		}
	}
	return TRUE;
}
/********************************************************************
函数名称：ProtocolModule_Parse_DDSQuery
函数功能：DDS查询解析协议
 参数.一：lpszMsgBuffer
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要解析的内容
 参数.二：nMsgLen
  In/Out：In
  类型：整数型
  可空：N
  意思：输入解析的大小
 参数.三：pppSt_DDSProtocol
  In/Out：In/Out
  类型：三级指针
  可空：N
  意思：输出DDS信息列表,此内存需要释放
 参数.四：pInt_ListCount
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出列表个数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CProtocolModule_Parse::ProtocolModule_Parse_DDSQuery(LPCTSTR lpszMsgBuffer, int nMsgLen, XENGINE_PROTOCOL_XDDS*** pppSt_DDSProtocol, int* pInt_ListCount)
{
	Protocol_IsErrorOccur = FALSE;

	if ((NULL == lpszMsgBuffer) || (NULL == pppSt_DDSProtocol))
	{
		Protocol_IsErrorOccur = TRUE;
		Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_PARAMENT;
		return FALSE;
	}
	Json::Value st_JsonRoot;
	JSONCPP_STRING st_JsonError;
	Json::CharReaderBuilder st_ReaderBuilder;

	std::unique_ptr<Json::CharReader> const pSt_JsonReader(st_ReaderBuilder.newCharReader());
	if (!pSt_JsonReader->parse(lpszMsgBuffer, lpszMsgBuffer + nMsgLen, &st_JsonRoot, &st_JsonError))
	{
		Protocol_IsErrorOccur = TRUE;
		Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_PARSE;
		return FALSE;
	}
	*pInt_ListCount = st_JsonRoot["ListCount"].asInt();
	BaseLib_OperatorMemory_Malloc((XPPPMEM)pppSt_DDSProtocol, st_JsonRoot["ListCount"].asInt(), sizeof(XENGINE_PROTOCOL_XDDS));

	Json::Value st_JsonArray = st_JsonRoot["ListArray"];
	for (int i = 0; i < st_JsonRoot["ListCount"].asInt(); i++)
	{
		(*pppSt_DDSProtocol)[i]->bCreater = st_JsonArray[i]["bCreater"].asInt();
		(*pppSt_DDSProtocol)[i]->bTcp = st_JsonArray[i]["bTcp"].asInt();
		(*pppSt_DDSProtocol)[i]->nPort = st_JsonArray[i]["nPort"].asInt();
		_tcscpy((*pppSt_DDSProtocol)[i]->tszTopic, st_JsonArray[i]["tszTopic"].asCString());
		_tcscpy((*pppSt_DDSProtocol)[i]->tszDDSAddr, st_JsonArray[i]["tszDDSAddr"].asCString());
	}
	
	return TRUE;
}