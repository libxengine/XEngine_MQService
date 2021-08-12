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
  可空：N
  意思：输出解析到的头协议
 参数.四：pSt_MQProtocol
  In/Out：Out
  类型：数据结构指针
  可空：N
  意思：输出消息协议
 参数.五：pptszMsgBuffer
  In/Out：Out
  类型：二级指针
  可空：Y
  意思：输出消息内容,需要释放内存
 参数.六：pInt_MsgLen
  In/Out：Out
  类型：整数型指针
  可空：Y
  意思：输出内容大小
 参数.七：pInt_Type
  In/Out：Out
  类型：整数型指针
  可空：Y
  意思：输出负载类型
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CProtocolModule_Parse::ProtocolModule_Parse_Http(LPCTSTR lpszMsgBuffer, int nMsgLen, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, XENGINE_PROTOCOLHDR* pSt_ProtocolHdr /* = NULL */, TCHAR** pptszMsgBuffer /* = NULL */, int* pInt_MsgLen /* = NULL */, int* pInt_Type /* = NULL */)
{
	Protocol_IsErrorOccur = FALSE;

	if ((NULL == lpszMsgBuffer) || (NULL == pSt_MQProtocol))
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

	if (NULL != pSt_ProtocolHdr)
	{
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
	}
	_tcscpy(pSt_MQProtocol->tszMQKey, st_JsonMQProtocol["tszMQKey"].asCString());
	pSt_MQProtocol->nSerial = st_JsonMQProtocol["nSerial"].asInt();
	pSt_MQProtocol->nKeepTime = st_JsonMQProtocol["nKeepTime"].asInt();
	pSt_MQProtocol->nGetTimer = st_JsonMQProtocol["nGetTimer"].asInt();

	if (!st_JsonRoot["st_Payload"].isNull())
	{
		Json::Value st_JsonPayLoad = st_JsonRoot["st_Payload"];

		*pInt_Type = st_JsonPayLoad["nPayType"].asInt();
		*pInt_MsgLen = st_JsonPayLoad["nPayLen"].asInt();

		*pptszMsgBuffer = (TCHAR*)malloc(*pInt_MsgLen);
		if (NULL != *pptszMsgBuffer)
		{
			Protocol_IsErrorOccur = TRUE;
			Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_MALLOC;
			return FALSE;
		}
		memcpy(*pptszMsgBuffer, st_JsonPayLoad["tszPayData"].asCString(), *pInt_MsgLen);
	}
	return TRUE;
}