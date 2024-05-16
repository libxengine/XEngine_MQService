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
bool CProtocolModule_Packet::ProtocolModule_Packet_Common(int nNetType, XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, XCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCXSTR lpszMsgBuffer /* = NULL */, int nMsgLen /* = 0 */)
{
	Protocol_IsErrorOccur = false;

	if (XENGINE_MQAPP_NETTYPE_TCP == nNetType)
	{
		ProtocolModule_Packet_TCPCommon(pSt_ProtocolHdr, pSt_MQProtocol, ptszMsgBuffer, pInt_MsgLen, lpszMsgBuffer, nMsgLen);
	}
	else if (XENGINE_MQAPP_NETTYPE_WEBSOCKET == nNetType)
	{
		ProtocolModule_Packet_WSCommon(pSt_ProtocolHdr, pSt_MQProtocol, ptszMsgBuffer, pInt_MsgLen, lpszMsgBuffer, nMsgLen);
	}
	else
	{
		ProtocolModule_Packet_MQTTCommon(pSt_ProtocolHdr, pSt_MQProtocol, ptszMsgBuffer, pInt_MsgLen, lpszMsgBuffer, nMsgLen);
	}
	return true;
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
bool CProtocolModule_Packet::ProtocolModule_Packet_MQNumber(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_MQNUMBER* pSt_MQNumber, XCHAR* ptszMsgBuffer, int* pInt_MsgLen, int nNetType)
{
	Protocol_IsErrorOccur = false;

	if ((NULL == pSt_ProtocolHdr) || (NULL == ptszMsgBuffer) || (NULL == pInt_MsgLen))
	{
		Protocol_IsErrorOccur = true;
		Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_PARAMENT;
		return false;
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
	return true;
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
 参数.四：nCode
  In/Out：In
  类型：整数型
  可空：N
  意思：输入要操作的协议
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CProtocolModule_Packet::ProtocolModule_Packet_PassAuth(XENGINE_PROTOCOL_USERAUTH* pSt_ProtocolAuth, XCHAR* ptszMsgBuffer, int* pInt_MsgLen, int nCode)
{
	Protocol_IsErrorOccur = false;

	if ((NULL == pSt_ProtocolAuth) || (NULL == ptszMsgBuffer) || (NULL == pInt_MsgLen))
	{
		Protocol_IsErrorOccur = true;
		Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_PARAMENT;
		return false;
	}
	Json::Value st_JsonRoot;
	Json::Value st_JsonAuth;
	Json::StreamWriterBuilder st_JsonBuilder;

	st_JsonAuth["tszUserName"] = pSt_ProtocolAuth->tszUserName;
	st_JsonAuth["tszUserPass"] = pSt_ProtocolAuth->tszUserPass;
	st_JsonAuth["enClientType"] = pSt_ProtocolAuth->enClientType;
	st_JsonAuth["enDeviceType"] = pSt_ProtocolAuth->enDeviceType;

	st_JsonRoot["unOperatorType"] = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_AUTH;
	st_JsonRoot["unOperatorCode"] = nCode;
	st_JsonRoot["wReserve"] = 0;
	st_JsonRoot["byVersion"] = 2;
	st_JsonRoot["st_Auth"] = st_JsonAuth;

	st_JsonBuilder["emitUTF8"] = true;

	*pInt_MsgLen = Json::writeString(st_JsonBuilder, st_JsonRoot).length();
	memcpy(ptszMsgBuffer, Json::writeString(st_JsonBuilder, st_JsonRoot).c_str(), *pInt_MsgLen);
	return true;
}
/********************************************************************
函数名称：ProtocolModule_Packet_PassUser
函数功能：HTTP用户信息打包函数
 参数.一：pSt_ProtocolUser
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
 参数.四：nCode
  In/Out：In
  类型：整数型
  可空：N
  意思：输入要操纵的协议
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CProtocolModule_Packet::ProtocolModule_Packet_PassUser(XENGINE_PROTOCOL_USERINFO* pSt_ProtocolUser, XCHAR* ptszMsgBuffer, int* pInt_MsgLen, int nCode)
{
	Protocol_IsErrorOccur = false;

	if ((NULL == pSt_ProtocolUser) || (NULL == ptszMsgBuffer) || (NULL == pInt_MsgLen))
	{
		Protocol_IsErrorOccur = true;
		Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_PARAMENT;
		return false;
	}
	Json::Value st_JsonRoot;
	Json::Value st_JsonUser;
	Json::StreamWriterBuilder st_JsonBuilder;

	st_JsonUser["nIDNumber"] = (Json::Value::UInt64)pSt_ProtocolUser->nIDNumber;
	st_JsonUser["nPhoneNumber"] = (Json::Value::UInt64)pSt_ProtocolUser->nPhoneNumber;
	st_JsonUser["nUserLevel"] = pSt_ProtocolUser->nUserLevel;
	st_JsonUser["nUserState"] = pSt_ProtocolUser->nUserState;
	st_JsonUser["tszCreateTime"] = pSt_ProtocolUser->tszCreateTime;
	st_JsonUser["tszEMailAddr"] = pSt_ProtocolUser->tszEMailAddr;
	st_JsonUser["tszLoginTime"] = pSt_ProtocolUser->tszLoginTime;
	st_JsonUser["tszUserName"] = pSt_ProtocolUser->tszUserName;
	st_JsonUser["tszUserPass"] = pSt_ProtocolUser->tszUserPass;

	st_JsonRoot["unOperatorType"] = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_JsonRoot["unOperatorCode"] = nCode;
	st_JsonRoot["wReserve"] = 0;
	st_JsonRoot["byVersion"] = 2;
	st_JsonRoot["st_User"] = st_JsonUser;

	st_JsonBuilder["emitUTF8"] = true;

	*pInt_MsgLen = Json::writeString(st_JsonBuilder, st_JsonRoot).length();
	memcpy(ptszMsgBuffer, Json::writeString(st_JsonBuilder, st_JsonRoot).c_str(), *pInt_MsgLen);
	return true;
}
/********************************************************************
函数名称：ProtocolModule_Packet_Http
函数功能：HTTP封包类
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
 参数.三：nCode
  In/Out：In
  类型：整数型
  可空：Y
  意思：返回的状态值
 参数.四：lpszMsgBuffer
  In/Out：In
  类型：常量字符指针
  可空：Y
  意思：返回的消息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CProtocolModule_Packet::ProtocolModule_Packet_Http(XCHAR* ptszMsgBuffer, int* pInt_MsgLen, int nCode /* = 0 */, LPCXSTR lpszMsgBuffer /* = NULL */)
{
	Json::Value st_JsonRoot;
	Json::StreamWriterBuilder st_JsonBuilder;

	st_JsonRoot["msg"] = lpszMsgBuffer;
	st_JsonRoot["code"] = nCode;

	st_JsonBuilder["emitUTF8"] = true;

	*pInt_MsgLen = Json::writeString(st_JsonBuilder, st_JsonRoot).length();
	memcpy(ptszMsgBuffer, Json::writeString(st_JsonBuilder, st_JsonRoot).c_str(), *pInt_MsgLen);

	return true;
}
/********************************************************************
函数名称：ProtocolModule_Packet_UserList
函数功能：用户信息打包
 参数.一：ptszMsgBuffer
  In/Out：Out
  类型：字符指针
  可空：N
  意思：输出打包的内容
 参数.二：pInt_MsgLen
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出打包大小
 参数.三：pppSt_UserInfo
  In/Out：In
  类型：三级指针
  可空：N
  意思：输入要打包的数据
 参数.四：nListCount
  In/Out：In
  类型：整数型
  可空：N
  意思：输入要打包的数据的个数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CProtocolModule_Packet::ProtocolModule_Packet_UserList(XCHAR* ptszMsgBuffer, int* pInt_MsgLen, XENGINE_PROTOCOL_USERINFO*** pppSt_UserInfo, int nListCount)
{
	Protocol_IsErrorOccur = false;

	Json::Value st_JsonRoot;
	Json::Value st_JsonArray;
	Json::StreamWriterBuilder st_JsonBuilder;

	for (int i = 0; i < nListCount; i++)
	{
		Json::Value st_JsonObject;
		st_JsonObject["tszUserName"] = (*pppSt_UserInfo)[i]->tszUserName;
		st_JsonObject["tszUserPass"] = (*pppSt_UserInfo)[i]->tszUserPass;
		st_JsonObject["tszEMailAddr"] = (*pppSt_UserInfo)[i]->tszEMailAddr;
		st_JsonObject["nPhoneNumber"] = (Json::Value::Int64)(*pppSt_UserInfo)[i]->nPhoneNumber;
		st_JsonObject["nIDNumber"] = (Json::Value::Int64)(*pppSt_UserInfo)[i]->nIDNumber;
		st_JsonObject["nUserState"] = (*pppSt_UserInfo)[i]->nUserState;
		st_JsonObject["nUserLevel"] = (*pppSt_UserInfo)[i]->nUserLevel;
		st_JsonObject["tszLoginTime"] = (*pppSt_UserInfo)[i]->tszLoginTime;
		st_JsonObject["tszCreateTime"] = (*pppSt_UserInfo)[i]->tszCreateTime;
		st_JsonArray.append(st_JsonObject);
	}
	st_JsonRoot["code"] = 0;
	st_JsonRoot["Array"] = st_JsonArray;
	st_JsonRoot["Count"] = st_JsonArray.size();

	st_JsonBuilder["emitUTF8"] = true;
	*pInt_MsgLen = Json::writeString(st_JsonBuilder, st_JsonRoot).length();
	memcpy(ptszMsgBuffer, Json::writeString(st_JsonBuilder, st_JsonRoot).c_str(), *pInt_MsgLen);
	return true;
}
/********************************************************************
函数名称：ProtocolModule_Packet_TopicList
函数功能：主题列表打包
 参数.一：ptszMsgBuffer
  In/Out：Out
  类型：字符指针
  可空：N
  意思：输出打包的内容
 参数.二：pInt_MsgLen
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出打包大小
 参数.三：pppszTableName
  In/Out：In
  类型：三级指针
  可空：N
  意思：输入要打包的数据
 参数.四：nListCount
  In/Out：In
  类型：整数型
  可空：N
  意思：输入要打包的数据的个数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CProtocolModule_Packet::ProtocolModule_Packet_TopicList(XCHAR* ptszMsgBuffer, int* pInt_MsgLen, XCHAR*** pppszTableName, int nListCount)
{
	Protocol_IsErrorOccur = false;

	Json::Value st_JsonRoot;
	Json::Value st_JsonArray;
	Json::StreamWriterBuilder st_JsonBuilder;

	for (int i = 0; i < nListCount; i++)
	{
		Json::Value st_JsonObject;
		st_JsonObject["tszTopicName"] = (*pppszTableName)[i];
		st_JsonArray.append(st_JsonObject);
	}
	st_JsonRoot["code"] = 0;
	st_JsonRoot["Array"] = st_JsonArray;
	st_JsonRoot["Count"] = st_JsonArray.size();

	st_JsonBuilder["emitUTF8"] = true;
	*pInt_MsgLen = Json::writeString(st_JsonBuilder, st_JsonRoot).length();
	memcpy(ptszMsgBuffer, Json::writeString(st_JsonBuilder, st_JsonRoot).c_str(), *pInt_MsgLen);
	return true;
}
/********************************************************************
函数名称：ProtocolModule_Packet_OnlineList
函数功能：在线用户打包
 参数.一：ptszMsgBuffer
  In/Out：Out
  类型：字符指针
  可空：N
  意思：输出打包的内容
 参数.二：pInt_MsgLen
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出打包大小
 参数.三：ppptszListUser
  In/Out：In
  类型：三级指针
  可空：N
  意思：输入要打包的数据
 参数.四：nListCount
  In/Out：In
  类型：整数型
  可空：N
  意思：输入要打包的数据的个数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CProtocolModule_Packet::ProtocolModule_Packet_OnlineList(XCHAR* ptszMsgBuffer, int* pInt_MsgLen, XCHAR*** ppptszListUser, int nListCount)
{
	Protocol_IsErrorOccur = false;

	Json::Value st_JsonRoot;
	Json::Value st_JsonArray;
	Json::StreamWriterBuilder st_JsonBuilder;

	for (int i = 0; i < nListCount; i++)
	{
		Json::Value st_JsonObject;
		st_JsonObject["tszUserAddr"] = (*ppptszListUser)[i];
		st_JsonArray.append(st_JsonObject);
	}
	st_JsonRoot["code"] = 0;
	st_JsonRoot["Array"] = st_JsonArray;
	st_JsonRoot["Count"] = st_JsonArray.size();

	st_JsonBuilder["emitUTF8"] = true;
	*pInt_MsgLen = Json::writeString(st_JsonBuilder, st_JsonRoot).length();
	memcpy(ptszMsgBuffer, Json::writeString(st_JsonBuilder, st_JsonRoot).c_str(), *pInt_MsgLen);
	return true;
}
/********************************************************************
函数名称：ProtocolModule_Packet_TopicName
函数功能：主题信息打包函数
 参数.一：ptszMsgBuffer
  In/Out：Out
  类型：字符指针
  可空：N
  意思：输出打包的内容
 参数.二：pInt_MsgLen
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出打包大小
 参数.三：lpszTopicName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要打包的主题名
 参数.四：nTopicCount
  In/Out：In
  类型：整数型
  可空：N
  意思：输入要打包的数据的个数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CProtocolModule_Packet::ProtocolModule_Packet_TopicName(XCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCXSTR lpszTopicName, int nTopicCount)
{
	Protocol_IsErrorOccur = false;
	
	Json::Value st_JsonRoot;
	Json::StreamWriterBuilder st_JsonBuilder;

	st_JsonRoot["code"] = 0;
	st_JsonRoot["lpszTopicName"] = lpszTopicName;
	st_JsonRoot["Count"] = nTopicCount;

	st_JsonBuilder["emitUTF8"] = true;
	*pInt_MsgLen = Json::writeString(st_JsonBuilder, st_JsonRoot).length();
	memcpy(ptszMsgBuffer, Json::writeString(st_JsonBuilder, st_JsonRoot).c_str(), *pInt_MsgLen);
	return true;
}
/********************************************************************
函数名称：ProtocolModule_Packet_UNReadCreate
函数功能：未读消息打包创建函数
 参数.一：pSt_ProtocolHdr
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要打包的协议头
 参数.二：enPayType
  In/Out：In
  类型：枚举型
  可空：N
  意思：输入打包的负载类型
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
XHANDLE CProtocolModule_Packet::ProtocolModule_Packet_UNReadCreate(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE enPayType)
{
	Protocol_IsErrorOccur = false;

	if (NULL == pSt_ProtocolHdr)
	{
		Protocol_IsErrorOccur = true;
		Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_PARAMENT;
		return NULL;
	}
	//申请内存
	PROTOCOL_PACKETUNREAD* pSt_UNRead = new PROTOCOL_PACKETUNREAD;
	if (NULL == pSt_UNRead)
	{
		Protocol_IsErrorOccur = true;
		Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_MALLOC;
		return NULL;
	}
	memset(&pSt_UNRead->st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));

	pSt_UNRead->nType = enPayType;
	if (pSt_UNRead->nType == ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_BIN)
	{
		pSt_UNRead->st_ProtocolHdr.byVersion = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_JSON;
	}
	else
	{
		pSt_UNRead->st_JsonRoot["unOperatorType"] = pSt_ProtocolHdr->unOperatorType;
		pSt_UNRead->st_JsonRoot["unOperatorCode"] = pSt_ProtocolHdr->unOperatorCode;
		pSt_UNRead->st_JsonRoot["wReserve"] = pSt_ProtocolHdr->wReserve;
	}
	return pSt_UNRead;
}
/********************************************************************
函数名称：ProtocolModule_Packet_UNReadInsert
函数功能：消息打包数据插入
 参数.一：xhToken
  In/Out：In
  类型：句柄
  可空：N
  意思：输入要操作的句柄
 参数.二：lpszKeyName
  In/Out：In
  类型：三级指针
  可空：N
  意思：输入队列名称
 参数.三：nListCount
  In/Out：In
  类型：整数型
  可空：N
  意思：输入队列个数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CProtocolModule_Packet::ProtocolModule_Packet_UNReadInsert(XHANDLE xhToken, LPCXSTR lpszKeyName, int nListCount)
{
	Protocol_IsErrorOccur = false;

	PROTOCOL_PACKETUNREAD* pSt_UNRead = (PROTOCOL_PACKETUNREAD*)xhToken;
	if (NULL == pSt_UNRead)
	{
		Protocol_IsErrorOccur = true;
		Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_NOTFOUND;
		return false;
	}
	Json::Value st_JsonSubArray;

	st_JsonSubArray["Name"] = lpszKeyName;
	st_JsonSubArray["Count"] = nListCount;
	pSt_UNRead->st_JsonArray.append(st_JsonSubArray);
	return true;
}
/********************************************************************
函数名称：ProtocolModule_Packet_UNReadDelete
函数功能：删除数据并且导出
 参数.一：xhToken
  In/Out：In
  类型：句柄
  可空：N
  意思：输入要操作的句柄
 参数.二：ptszMsgBuffer
  In/Out：Out
  类型：字符指针
  可空：N
  意思：输出打好包的数据
 参数.三：pInt_MsgLen
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出数据大小
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CProtocolModule_Packet::ProtocolModule_Packet_UNReadDelete(XHANDLE xhToken, XCHAR* ptszMsgBuffer, int* pInt_MsgLen)
{
	Protocol_IsErrorOccur = false;

	PROTOCOL_PACKETUNREAD* pSt_UNRead = (PROTOCOL_PACKETUNREAD*)xhToken;
	if (NULL == pSt_UNRead)
	{
		Protocol_IsErrorOccur = true;
		Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_NOTFOUND;
		return false;
	}
	Json::StreamWriterBuilder st_JsonBuilder;
	st_JsonBuilder["emitUTF8"] = true;

	pSt_UNRead->st_JsonRoot["Array"] = pSt_UNRead->st_JsonArray;
	pSt_UNRead->st_JsonRoot["Count"] = pSt_UNRead->st_JsonArray.size();

	if (pSt_UNRead->nType == ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_BIN)
	{
		pSt_UNRead->st_ProtocolHdr.unPacketSize = Json::writeString(st_JsonBuilder, pSt_UNRead->st_JsonRoot).length();
		memcpy(ptszMsgBuffer, &pSt_UNRead->st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
		memcpy(ptszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), Json::writeString(st_JsonBuilder, pSt_UNRead->st_JsonRoot).c_str(), pSt_UNRead->st_ProtocolHdr.unPacketSize);
		*pInt_MsgLen = sizeof(XENGINE_PROTOCOLHDR) + pSt_UNRead->st_ProtocolHdr.unPacketSize;
	}
	else
	{
		*pInt_MsgLen = Json::writeString(st_JsonBuilder, pSt_UNRead->st_JsonRoot).length();
		memcpy(ptszMsgBuffer, Json::writeString(st_JsonBuilder, pSt_UNRead->st_JsonRoot).c_str(), *pInt_MsgLen);
	}

	delete pSt_UNRead;
	pSt_UNRead = NULL;
	return true;
}
///////////////////////////////////////////////////////////////////////////////
//                      保护函数
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
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CProtocolModule_Packet::ProtocolModule_Packet_TCPCommon(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, XCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCXSTR lpszMsgBuffer /* = NULL */, int nMsgLen /* = 0 */)
{
	Protocol_IsErrorOccur = false;

	if ((NULL == pSt_ProtocolHdr) || (NULL == ptszMsgBuffer) || (NULL == pInt_MsgLen))
	{
		Protocol_IsErrorOccur = true;
		Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_PARAMENT;
		return false;
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

	return true;
}
/********************************************************************
函数名称：ProtocolModule_Packet_WSCommon
函数功能：Websocket通用封包类
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
bool CProtocolModule_Packet::ProtocolModule_Packet_WSCommon(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, XCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCXSTR lpszMsgBuffer /* = NULL */, int nMsgLen /* = 0 */)
{
	Json::Value st_JsonRoot;
	Json::Value st_JsonMQProtocol;
	Json::Value st_JsonPayload;
	Json::StreamWriterBuilder st_JsonBuilder;

	st_JsonRoot["unOperatorType"] = pSt_ProtocolHdr->unOperatorType;
	st_JsonRoot["unOperatorCode"] = pSt_ProtocolHdr->unOperatorCode;
	st_JsonRoot["xhToken"] = (Json::Value::UInt64)pSt_ProtocolHdr->xhToken;
	st_JsonRoot["wReserve"] = pSt_ProtocolHdr->wReserve;
	st_JsonRoot["byVersion"] = pSt_ProtocolHdr->byVersion;

	if (NULL != pSt_MQProtocol)
	{
		st_JsonMQProtocol["tszMQKey"] = pSt_MQProtocol->tszMQKey;
		st_JsonMQProtocol["tszMQUsr"] = pSt_MQProtocol->tszMQUsr;
		st_JsonMQProtocol["nSerial"] = (Json::Value::Int64)pSt_MQProtocol->nSerial;
		st_JsonMQProtocol["nKeepTime"] = pSt_MQProtocol->nKeepTime;
		st_JsonMQProtocol["nPubTime"] = (Json::Value::Int64)pSt_MQProtocol->nPubTime;

		XSHOT nMSGAttr = 0;
		memcpy(&nMSGAttr, &pSt_MQProtocol->st_MSGAttr, sizeof(XENGINE_PROTOCOL_MSGATTR));
		st_JsonMQProtocol["nMSGAttr"] = nMSGAttr;
		st_JsonRoot["st_MQProtocol"] = st_JsonMQProtocol;
	}

	if (nMsgLen > 0)
	{
		if (ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_BIN == pSt_ProtocolHdr->byVersion)
		{
			int nBLen = nMsgLen;
			XCHAR* ptszBaseBuffer = (XCHAR*)malloc(XENGINE_MEMORY_SIZE_MAX);
			if (NULL == ptszBaseBuffer)
			{
				Protocol_IsErrorOccur = true;
				Protocol_dwErrorCode = ERROR_MQ_MODULE_PROTOCOL_MALLOC;
				return false;
			}
			memset(ptszBaseBuffer, '\0', XENGINE_MEMORY_SIZE_MAX);

			OPenSsl_Codec_Base64(lpszMsgBuffer, ptszBaseBuffer, &nBLen, true);
			st_JsonPayload["nPayLen"] = nBLen;
			st_JsonPayload["tszPayData"] = ptszBaseBuffer;
		}
		else
		{
			st_JsonPayload["nPayLen"] = nMsgLen;
			st_JsonPayload["tszPayData"] = lpszMsgBuffer;
		}
		st_JsonRoot["st_Payload"] = st_JsonPayload;
	}
	st_JsonBuilder["emitUTF8"] = true;

	*pInt_MsgLen = Json::writeString(st_JsonBuilder, st_JsonRoot).length();
	memcpy(ptszMsgBuffer, Json::writeString(st_JsonBuilder, st_JsonRoot).c_str(), *pInt_MsgLen);

	return true;
}
bool CProtocolModule_Packet::ProtocolModule_Packet_MQTTCommon(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, XENGINE_PROTOCOL_XMQ* pSt_MQProtocol, XCHAR* ptszMsgBuffer, int* pInt_MsgLen, LPCXSTR lpszMsgBuffer /* = NULL */, int nMsgLen /* = 0 */)
{
	int nRVLen = 0;
	XCHAR tszRVBuffer[1024] = {};

	if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPUSERLOG == pSt_ProtocolHdr->unOperatorCode)
	{
		if (0 == pSt_ProtocolHdr->wReserve)
		{
			int nRVLen = 0;
			int nListCount = 6;
			XCHAR tszRVBuffer[1024];
			MQTTPROTOCOL_HDRPROPERTY** ppSt_HDRProperty;

			BaseLib_OperatorMemory_Malloc((XPPPMEM)&ppSt_HDRProperty, nListCount, sizeof(MQTTPROTOCOL_HDRPROPERTY));

			ppSt_HDRProperty[0]->nProLen = 4;
			ppSt_HDRProperty[0]->st_unValue.nValue = 1024000;
			ppSt_HDRProperty[0]->byProFlag = XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_PROPERTY_PACKMAX;

			ppSt_HDRProperty[1]->nProLen = 1;
			ppSt_HDRProperty[1]->st_unValue.byValue = 1;
			ppSt_HDRProperty[1]->byProFlag = XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_PROPERTY_REVERAVAI;

			ppSt_HDRProperty[2]->nProLen = 1;
			ppSt_HDRProperty[2]->st_unValue.byValue = 1;
			ppSt_HDRProperty[2]->byProFlag = XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_PROPERTY_SHAREDSUBAVAI;

			ppSt_HDRProperty[3]->nProLen = 1;
			ppSt_HDRProperty[3]->st_unValue.byValue = 1;
			ppSt_HDRProperty[3]->byProFlag = XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_PROPERTY_SUBIDAVAI;

			ppSt_HDRProperty[4]->nProLen = 2;
			ppSt_HDRProperty[4]->st_unValue.wValue = 65535;
			ppSt_HDRProperty[4]->byProFlag = XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_PROPERTY_ALIASMAX;

			ppSt_HDRProperty[5]->nProLen = 1;
			ppSt_HDRProperty[5]->st_unValue.byValue = 1;
			ppSt_HDRProperty[5]->byProFlag = XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_PROPERTY_WILDCARDSUBAVAI;

			MQTTProtocol_Packet_REPConnect(tszRVBuffer, &nRVLen, 0, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_REASON_SUCCESS, &ppSt_HDRProperty, nListCount);
			MQTTProtocol_Packet_Header(ptszMsgBuffer, pInt_MsgLen, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_TYPE_CONNACK, tszRVBuffer, nRVLen);
			BaseLib_OperatorMemory_Free((XPPPMEM)&ppSt_HDRProperty, nListCount);
		}
		else
		{
			MQTTProtocol_Packet_REPConnect(tszRVBuffer, &nRVLen, 0, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_REASON_USERPASS);
			MQTTProtocol_Packet_Header(ptszMsgBuffer, pInt_MsgLen, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_TYPE_CONNACK, tszRVBuffer, nRVLen);
		}
	}
	else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPPOST == pSt_ProtocolHdr->unOperatorCode)
	{
		if (XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_FLAG_PUBLISH_QOS1 == pSt_ProtocolHdr->byIsReply)
		{
			//需要回复
			MQTTProtocol_Packet_REPPublish(tszRVBuffer, &nRVLen, pSt_ProtocolHdr->wPacketSerial, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_REASON_SUCCESS);
			MQTTProtocol_Packet_Header(ptszMsgBuffer, pInt_MsgLen, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_TYPE_PUBACK, tszRVBuffer, nRVLen);
		}
		else if (XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_FLAG_PUBLISH_QOS2 == pSt_ProtocolHdr->byIsReply)
		{

		}
	}
	else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICBIND == pSt_ProtocolHdr->unOperatorCode)
	{
		if (0 == pSt_ProtocolHdr->wReserve)
		{
			MQTTProtocol_Packet_REPComm(tszRVBuffer, &nRVLen, pSt_ProtocolHdr->wPacketSerial, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_REASON_SUCCESS);
		}
		else
		{
			MQTTProtocol_Packet_REPComm(tszRVBuffer, &nRVLen, pSt_ProtocolHdr->wPacketSerial, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_REASON_TOPICNAME);
		}
		MQTTProtocol_Packet_Header(ptszMsgBuffer, pInt_MsgLen, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_TYPE_SUBACK, tszRVBuffer, nRVLen);
	}
	else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REPTOPICUNBIND == pSt_ProtocolHdr->unOperatorCode)
	{
		MQTTProtocol_Packet_REPComm(tszRVBuffer, &nRVLen, pSt_ProtocolHdr->wPacketSerial, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_REASON_SUCCESS);
		MQTTProtocol_Packet_Header(ptszMsgBuffer, pInt_MsgLen, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_TYPE_UNSUBACK, tszRVBuffer, nRVLen);
	}
	return true;
}