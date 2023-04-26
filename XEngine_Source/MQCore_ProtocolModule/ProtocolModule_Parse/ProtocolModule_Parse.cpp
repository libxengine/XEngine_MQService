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
  类型：字符指针
  可空：Y
  意思：输出消息内容
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
bool CProtocolModule_Parse::ProtocolModule_Parse_Http(LPCXSTR lpszMsgBuffer, int nMsgLen, XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XCHAR* ptszMsgBuffer, int* pInt_MsgLen)
{
	Protocol_IsErrorOccur = false;

	if (NULL == lpszMsgBuffer)
	{
		Protocol_IsErrorOccur = true;
		Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_PARAMENT;
		return false;
	}
	Json::Value st_JsonRoot;
	JSONCPP_STRING st_JsonError;
	Json::CharReaderBuilder st_ReaderBuilder;

	std::unique_ptr<Json::CharReader> const pSt_JsonReader(st_ReaderBuilder.newCharReader());
	if (!pSt_JsonReader->parse(lpszMsgBuffer, lpszMsgBuffer + nMsgLen, &st_JsonRoot, &st_JsonError))
	{
		Protocol_IsErrorOccur = true;
		Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_PARSE;
		return false;
	}

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
		if (!st_JsonRoot["xhToken"].isNull())
		{
			pSt_ProtocolHdr->xhToken = st_JsonRoot["xhToken"].asUInt64();
		}
		if (!st_JsonRoot["wReserve"].isNull())
		{
			pSt_ProtocolHdr->wReserve = st_JsonRoot["wReserve"].asInt();
		}
		if (!st_JsonRoot["byVersion"].isNull())
		{
			pSt_ProtocolHdr->byVersion = st_JsonRoot["byVersion"].asInt();
		}
	}
	
	*pInt_MsgLen = 0;
	XENGINE_PROTOCOL_XMQ st_MQProtocol;
	XENGINE_PROTOCOL_USERAUTH st_ProtocolAuth;
	XENGINE_PROTOCOL_USERINFO st_ProtocolInfo;

	memset(&st_MQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));
	memset(&st_ProtocolAuth, '\0', sizeof(XENGINE_PROTOCOL_USERAUTH));
	memset(&st_ProtocolInfo, '\0', sizeof(XENGINE_PROTOCOL_USERINFO));
	//如果负载的是消息
	if (!st_JsonRoot["st_MQProtocol"].isNull())
	{
		Json::Value st_JsonMQProtocol = st_JsonRoot["st_MQProtocol"];

		if (!st_JsonMQProtocol["tszMQKey"].isNull())
		{
			_tcsxcpy(st_MQProtocol.tszMQKey, st_JsonMQProtocol["tszMQKey"].asCString());
		}
		if (!st_JsonMQProtocol["nSerial"].isNull())
		{
			st_MQProtocol.nSerial = st_JsonMQProtocol["nSerial"].asInt();
		}
		if (!st_JsonMQProtocol["nGetTimer"].isNull())
		{
			st_MQProtocol.nGetTimer = st_JsonMQProtocol["nGetTimer"].asInt();
		}
		if (!st_JsonMQProtocol["nKeepTime"].isNull())
		{
			st_MQProtocol.nKeepTime = st_JsonMQProtocol["nKeepTime"].asInt();
		}
		if (!st_JsonMQProtocol["nPubTime"].isNull())
		{
			st_MQProtocol.nPubTime = st_JsonMQProtocol["nPubTime"].asInt64();
		}
		*pInt_MsgLen += sizeof(XENGINE_PROTOCOL_XMQ);
	}
	//后者负载的是验证协议
	if (!st_JsonRoot["st_Auth"].isNull())
	{
		Json::Value st_JsonAuth = st_JsonRoot["st_Auth"];
		_tcsxcpy(st_ProtocolAuth.tszUserName, st_JsonAuth["tszUserName"].asCString());
		_tcsxcpy(st_ProtocolAuth.tszUserPass, st_JsonAuth["tszUserPass"].asCString());

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
	if (!st_JsonRoot["st_User"].isNull())
	{
		Json::Value st_JsonUser = st_JsonRoot["st_User"];

		if (!st_JsonUser["nUserLevel"].isNull())
		{
			st_ProtocolInfo.nUserLevel = st_JsonUser["nUserLevel"].asInt();
		}
		if (!st_JsonUser["nUserState"].isNull())
		{
			st_ProtocolInfo.nUserState = st_JsonUser["nUserState"].asInt();
		}
		if (!st_JsonUser["nPhoneNumber"].isNull())
		{
			st_ProtocolInfo.nPhoneNumber = st_JsonUser["nPhoneNumber"].asInt64();
		}
		if (!st_JsonUser["nIDNumber"].isNull())
		{
			st_ProtocolInfo.nIDNumber = st_JsonUser["nIDNumber"].asInt64();
		}
		if (!st_JsonUser["tszUserName"].isNull())
		{
			_tcsxcpy(st_ProtocolInfo.tszUserName, st_JsonUser["tszUserName"].asCString());
		}
		if (!st_JsonUser["tszUserPass"].isNull())
		{
			_tcsxcpy(st_ProtocolInfo.tszUserPass, st_JsonUser["tszUserPass"].asCString());
		}
		if (!st_JsonUser["tszEMailAddr"].isNull())
		{
			_tcsxcpy(st_ProtocolInfo.tszEMailAddr, st_JsonUser["tszEMailAddr"].asCString());
		}
		*pInt_MsgLen += sizeof(XENGINE_PROTOCOL_USERINFO);
	}
	//或者包含附加内容
	if (!st_JsonRoot["st_Payload"].isNull())
	{
		Json::Value st_JsonPayLoad = st_JsonRoot["st_Payload"];
		*pInt_MsgLen += st_JsonPayLoad["nPayLen"].asInt();
	}

	int nPos = 0;
	if (!st_JsonRoot["st_MQProtocol"].isNull())
	{
		memcpy(ptszMsgBuffer + nPos, &st_MQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));
		nPos += sizeof(XENGINE_PROTOCOL_XMQ);
	}
	if (!st_JsonRoot["st_Auth"].isNull())
	{
		memcpy(ptszMsgBuffer + nPos, &st_ProtocolAuth, sizeof(XENGINE_PROTOCOL_USERAUTH));
		nPos += sizeof(XENGINE_PROTOCOL_USERAUTH);
	}
	if (!st_JsonRoot["st_User"].isNull())
	{
		memcpy(ptszMsgBuffer + nPos, &st_ProtocolInfo, sizeof(XENGINE_PROTOCOL_USERINFO));
		nPos += sizeof(XENGINE_PROTOCOL_USERINFO);
	}
	if (!st_JsonRoot["st_Payload"].isNull())
	{
		Json::Value st_JsonPayLoad = st_JsonRoot["st_Payload"];
		memcpy(ptszMsgBuffer + nPos, st_JsonPayLoad["tszPayData"].asCString(), st_JsonPayLoad["nPayLen"].asInt());
	}
	return true;
}