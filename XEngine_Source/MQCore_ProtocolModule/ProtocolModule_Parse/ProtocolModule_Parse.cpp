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
函数名称：ProtocolModule_Parse_Websocket
函数功能：websocket协议解析
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
bool CProtocolModule_Parse::ProtocolModule_Parse_Websocket(LPCXSTR lpszMsgBuffer, int nMsgLen, XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XCHAR* ptszMsgBuffer, int* pInt_MsgLen)
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
		if (!st_JsonRoot["byIsReply"].isNull())
		{
			pSt_ProtocolHdr->byIsReply = st_JsonRoot["byIsReply"].asInt();
		}
	}
	
	int nPos = 0;
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
		if (!st_JsonMQProtocol["tszMQUsr"].isNull())
		{
			_tcsxcpy(st_MQProtocol.tszMQUsr, st_JsonMQProtocol["tszMQUsr"].asCString());
		}
		if (!st_JsonMQProtocol["nSerial"].isNull())
		{
			st_MQProtocol.nSerial = st_JsonMQProtocol["nSerial"].asInt();
		}
		if (!st_JsonMQProtocol["nKeepTime"].isNull())
		{
			st_MQProtocol.nKeepTime = st_JsonMQProtocol["nKeepTime"].asInt();
		}
		if (!st_JsonMQProtocol["nPubTime"].isNull())
		{
			st_MQProtocol.nPubTime = st_JsonMQProtocol["nPubTime"].asInt64();
		}
		if (!st_JsonMQProtocol["nMSGAttr"].isNull())
		{
			XSHOT nMSGAttr = st_JsonMQProtocol["nMSGAttr"].asUInt();
			memcpy(&st_MQProtocol.st_MSGAttr, &nMSGAttr, sizeof(XENGINE_PROTOCOL_MSGATTR));
		}
		*pInt_MsgLen += sizeof(XENGINE_PROTOCOL_XMQ);
		memcpy(ptszMsgBuffer + nPos, &st_MQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));
		nPos += sizeof(XENGINE_PROTOCOL_XMQ);
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
		memcpy(ptszMsgBuffer + nPos, &st_ProtocolAuth, sizeof(XENGINE_PROTOCOL_USERAUTH));
		nPos += sizeof(XENGINE_PROTOCOL_USERAUTH);
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
		memcpy(ptszMsgBuffer + nPos, &st_ProtocolInfo, sizeof(XENGINE_PROTOCOL_USERINFO));
		nPos += sizeof(XENGINE_PROTOCOL_USERINFO);
	}
	//或者包含附加内容
	if (!st_JsonRoot["st_Payload"].isNull())
	{
		Json::Value st_JsonPayLoad = st_JsonRoot["st_Payload"];
		
		if (ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_BIN == pSt_ProtocolHdr->byVersion)
		{
			int nBLen = st_JsonPayLoad["nPayLen"].asInt();
			Cryption_Codec_Base64(st_JsonPayLoad["tszPayData"].asCString(), ptszMsgBuffer + nPos, &nBLen, false);
			*pInt_MsgLen += nBLen;
		}
		else
		{
			memcpy(ptszMsgBuffer + nPos, st_JsonPayLoad["tszPayData"].asCString(), st_JsonPayLoad["nPayLen"].asInt());
			*pInt_MsgLen += st_JsonPayLoad["nPayLen"].asInt();
		}
	}
	return true;
}
/********************************************************************
函数名称：ProtocolModule_Parse_Register
函数功能：解析用户信息
 参数.一：lpszMsgBuffer
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要解析的缓冲区
 参数.二：nMsgLen
  In/Out：In
  类型：整数型
  可空：N
  意思：输入要解析的大小
 参数.三：pSt_UserInfo
  In/Out：Out
  类型：数据结构指针
  可空：N
  意思：输出解析的数据
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CProtocolModule_Parse::ProtocolModule_Parse_Register(LPCXSTR lpszMsgBuffer, int nMsgLen, XENGINE_PROTOCOL_USERINFO* pSt_UserInfo)
{
	Protocol_IsErrorOccur = false;

	if ((NULL == lpszMsgBuffer) || (NULL == pSt_UserInfo))
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
	Json::Value st_JsonProtocol = st_JsonRoot["st_UserInfo"];
	if (!st_JsonProtocol["tszUserName"].isNull())
	{
		_tcsxcpy(pSt_UserInfo->tszUserName, st_JsonProtocol["tszUserName"].asCString());
	}
	if (!st_JsonProtocol["tszUserPass"].isNull())
	{
		_tcsxcpy(pSt_UserInfo->tszUserPass, st_JsonProtocol["tszUserPass"].asCString());
	}
	if (!st_JsonProtocol["tszEMailAddr"].isNull())
	{
		_tcsxcpy(pSt_UserInfo->tszEMailAddr, st_JsonProtocol["tszEMailAddr"].asCString());
	}
	if (!st_JsonProtocol["tszLoginTime"].isNull())
	{
		_tcsxcpy(pSt_UserInfo->tszLoginTime, st_JsonProtocol["tszLoginTime"].asCString());
	}
	if (!st_JsonProtocol["tszCreateTime"].isNull())
	{
		_tcsxcpy(pSt_UserInfo->tszCreateTime, st_JsonProtocol["tszCreateTime"].asCString());
	}
	if (!st_JsonProtocol["nPhoneNumber"].isNull())
	{
		pSt_UserInfo->nPhoneNumber = st_JsonProtocol["nPhoneNumber"].asInt64();
	}
	if (!st_JsonProtocol["nIDNumber"].isNull())
	{
		pSt_UserInfo->nIDNumber = st_JsonProtocol["nIDNumber"].asInt64();
	}
	if (!st_JsonProtocol["nUserLevel"].isNull())
	{
		pSt_UserInfo->nUserLevel = st_JsonProtocol["nUserLevel"].asInt();
	}
	if (!st_JsonProtocol["nUserState"].isNull())
	{
		pSt_UserInfo->nUserState = st_JsonProtocol["nUserState"].asInt();
	}
	return true;
}
/********************************************************************
函数名称：ProtocolModule_Parse_Token
函数功能：解析TOKEN
 参数.一：lpszMsgBuffer
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要解析的缓冲区
 参数.二：nMsgLen
  In/Out：In
  类型：整数型
  可空：N
  意思：输入要解析的大小
 参数.三：pxhToken
  In/Out：Out
  类型：句柄
  可空：N
  意思：输出解析到的TOKEN值
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CProtocolModule_Parse::ProtocolModule_Parse_Token(LPCXSTR lpszMsgBuffer, int nMsgLen, XNETHANDLE* pxhToken)
{
	Protocol_IsErrorOccur = false;

	if ((NULL == lpszMsgBuffer) || (NULL == pxhToken))
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
	if (st_JsonRoot["xhToken"].isNull())
	{
		Protocol_IsErrorOccur = true;
		Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_PARSE;
		return false;
	}
	*pxhToken = st_JsonRoot["xhToken"].asUInt64();
	return true;
}
/********************************************************************
函数名称：ProtocolModule_Parse_Type
函数功能：解析负载的类型字段
 参数.一：lpszMsgBuffer
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：要解析的数据
 参数.二：nMsgLen
  In/Out：In
  类型：整数型
  可空：N
  意思：要解析的大小
 参数.三：pInt_Type
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出解析到的类型
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CProtocolModule_Parse::ProtocolModule_Parse_Type(LPCXSTR lpszMsgBuffer, int nMsgLen, int* pInt_Type)
{
	Protocol_IsErrorOccur = false;

	if ((NULL == lpszMsgBuffer))
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
	Json::Value st_JsonObject = st_JsonRoot["Object"];
	if (!st_JsonObject["nType"].isNull())
	{
		*pInt_Type = st_JsonObject["nType"].asInt();
	}
	
	return true;
}
/********************************************************************
函数名称：ProtocolModule_Parse_Name
函数功能：解析负载的类型字段
 参数.一：lpszMsgBuffer
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：要解析的数据
 参数.二：nMsgLen
  In/Out：In
  类型：整数型
  可空：N
  意思：要解析的大小
 参数.三：pSt_XMQProtocol
  In/Out：Out
  类型：数据结构指针
  可空：N
  意思：输出解析的XMQ协议
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CProtocolModule_Parse::ProtocolModule_Parse_XMQ(LPCXSTR lpszMsgBuffer, int nMsgLen, XENGINE_PROTOCOL_XMQ* pSt_XMQProtocol)
{
	Protocol_IsErrorOccur = false;

	if ((NULL == lpszMsgBuffer))
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
	Json::Value st_JsonObject = st_JsonRoot["Object"];
	if (!st_JsonObject["tszMQKey"].isNull())
	{
		_tcsxcpy(pSt_XMQProtocol->tszMQKey, st_JsonObject["tszMQKey"].asCString());
	}
	if (!st_JsonObject["tszMQUsr"].isNull())
	{
		_tcsxcpy(pSt_XMQProtocol->tszMQUsr, st_JsonObject["tszMQUsr"].asCString());
	}
	if (!st_JsonObject["nKeepTime"].isNull())
	{
		pSt_XMQProtocol->nKeepTime = st_JsonObject["nKeepTime"].asInt();
	}
	if (!st_JsonObject["nPubTime"].isNull())
	{
		pSt_XMQProtocol->nPubTime = st_JsonObject["nPubTime"].asInt();
	}
	if (!st_JsonObject["nSerial"].isNull())
	{
		pSt_XMQProtocol->nSerial = st_JsonObject["nSerial"].asInt();
	}
	return true;
}