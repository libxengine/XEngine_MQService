#ifdef _WINDOWS
#include <Windows.h>
#include <tchar.h>
#include <json/json.h>
#pragma comment(lib,"Ws2_32")
#pragma comment(lib,"x86/XEngine_BaseLib/XEngine_BaseLib")
#pragma comment(lib,"x86/XEngine_NetHelp/NetHelp_APIHelp")
#pragma comment(lib,"../../XEngine_Source/Debug/jsoncpp")
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif
#include <json/json.h>
#include <XEngine_Include/XEngine_CommHdr.h>
#include <XEngine_Include/XEngine_ProtocolHdr.h>
#include <XEngine_Include/XEngine_BaseLib/BaseLib_Define.h>
#include <XEngine_Include/XEngine_BaseLib/BaseLib_Error.h>
#include <XEngine_Include/XEngine_NetHelp/APIHelp_Define.h>
#include <XEngine_Include/XEngine_NetHelp/APIHelp_Error.h>
#include "../../XEngine_Source/XQueue_ProtocolHdr.h"

//g++ -std=c++17 -Wall -g MQCore_HTTPApp.cpp -o MQCore_HTTPApp.exe -I ../../XEngine_Source/XEngine_ThirdPart/jsoncpp -L /usr/local/lib/XEngine_Release/XEngine_BaseLib -L /usr/local/lib/XEngine_Release/XEngine_NetHelp -L ../../XEngine_Source/XEngine_ThirdPart/jsoncpp -lXEngine_BaseLib -lNetHelp_APIHelp -ljsoncpp

SOCKET m_Socket;
XNETHANDLE xhToken = 0;
LPCTSTR lpszKey = _T("XEngine_Notify");  //主题
LPCTSTR lpszPostUrl = _T("http://127.0.0.1:5201");

void MQ_Authorize()
{
	int nLen = 0;
	TCHAR tszMsgBuffer[2048];
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));

	Json::Value st_JsonRoot;
	Json::Value st_JsonAuth;
	st_JsonRoot["unOperatorType"] = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_AUTH;
	st_JsonRoot["unOperatorCode"] = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERLOG;
	st_JsonRoot["byVersion"] = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_JSON;

	st_JsonAuth["tszUserName"] = "123123aa";
	st_JsonAuth["tszUserPass"] = "123123";

	st_JsonRoot["st_Auth"] = st_JsonAuth;

	nLen = st_JsonRoot.toStyledString().length();
	memcpy(tszMsgBuffer, st_JsonRoot.toStyledString().c_str(), nLen);

	TCHAR* ptszMsgBody = NULL;
	if (!APIHelp_HttpRequest_Custom(_T("POST"), lpszPostUrl, tszMsgBuffer, NULL, &ptszMsgBody, &nLen))
	{
		printf("发送投递失败！\n");
		return;
	}
	printf("MQ_Authorize:%s\n", ptszMsgBody);

	JSONCPP_STRING st_JsonError;
	Json::CharReaderBuilder st_ReaderBuilder;
	st_JsonRoot.clear();
	std::unique_ptr<Json::CharReader> const pSt_JsonReader(st_ReaderBuilder.newCharReader());
	if (!pSt_JsonReader->parse(ptszMsgBody, ptszMsgBody + nLen, &st_JsonRoot, &st_JsonError))
	{
		return;
	}
	if (!st_JsonRoot["xhToken"].isNull())
	{
		xhToken = st_JsonRoot["xhToken"].asUInt64();
	}
	BaseLib_OperatorMemory_FreeCStyle((XPPMEM)&ptszMsgBody);
}

void MQ_Create()
{
	int nLen = 0;
	TCHAR tszMsgBuffer[2048];
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));

	Json::Value st_JsonRoot;
	Json::Value st_JsonMQProtocol;
	st_JsonRoot["unOperatorType"] = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_JsonRoot["unOperatorCode"] = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQCREATE;
	st_JsonRoot["byVersion"] = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_JSON;
	st_JsonRoot["xhToken"] = xhToken;

	st_JsonMQProtocol["tszMQKey"] = lpszKey;
	st_JsonRoot["st_MQProtocol"] = st_JsonMQProtocol;

	nLen = st_JsonRoot.toStyledString().length();
	memcpy(tszMsgBuffer, st_JsonRoot.toStyledString().c_str(), nLen);

	TCHAR* ptszMsgBody = NULL;
	if (!APIHelp_HttpRequest_Custom(_T("POST"), lpszPostUrl, tszMsgBuffer, NULL, &ptszMsgBody))
	{
		printf("发送投递失败！\n");
		return;
	}
	printf("MQ_Create:%s\n", ptszMsgBody);
	BaseLib_OperatorMemory_FreeCStyle((XPPMEM)&ptszMsgBody);
}

void MQ_Post(LPCTSTR lpszMsgBuffer)
{
	int nLen = 0;
	TCHAR tszMsgBuffer[2048];
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));

	Json::Value st_JsonRoot;
	Json::Value st_JsonMQProtocol;
	Json::Value st_JsonPayload;
	st_JsonRoot["unOperatorType"] = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_JsonRoot["unOperatorCode"] = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQPOST;
	st_JsonRoot["byVersion"] = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_JSON;
	st_JsonRoot["xhToken"] = xhToken;

	st_JsonMQProtocol["tszMQKey"] = lpszKey;
	st_JsonMQProtocol["nSerial"] = 0;             //序列号,0服务会自动处理
	st_JsonMQProtocol["nKeepTime"] = -1;          //保存时间，单位秒，如果为0，获取一次后被抛弃。-1 永久存在，PacketKey不能为空
	st_JsonMQProtocol["nGetTimer"] = 0;

	st_JsonPayload["nPayType"] = 0;
	st_JsonPayload["nPayLen"] = (Json::Value::UInt)strlen(lpszMsgBuffer);
	st_JsonPayload["tszPayData"] = lpszMsgBuffer;

	st_JsonRoot["st_MQProtocol"] = st_JsonMQProtocol;
	st_JsonRoot["st_Payload"] = st_JsonPayload;

	nLen = st_JsonRoot.toStyledString().length();
	memcpy(tszMsgBuffer, st_JsonRoot.toStyledString().c_str(), nLen);

	TCHAR* ptszMsgBody = NULL;
	if (!APIHelp_HttpRequest_Custom(_T("POST"), lpszPostUrl, tszMsgBuffer, NULL, &ptszMsgBody))
	{
		printf("发送投递失败！\n");
		return;
	}
	printf("MQ_Post:%s\n", ptszMsgBody);
	BaseLib_OperatorMemory_FreeCStyle((XPPMEM)&ptszMsgBody);
}

void MQ_GetNumber()
{
	int nLen = 0;
	TCHAR tszMsgBuffer[2048];
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));

	Json::Value st_JsonRoot;
	Json::Value st_JsonMQProtocol;
	Json::Value st_JsonPayload;
	st_JsonRoot["unOperatorType"] = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_JsonRoot["unOperatorCode"] = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQNUMBER;
	st_JsonRoot["byVersion"] = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_JSON;
	st_JsonRoot["xhToken"] = xhToken;

	st_JsonMQProtocol["tszMQKey"] = lpszKey;
	st_JsonRoot["st_MQProtocol"] = st_JsonMQProtocol;

	nLen = st_JsonRoot.toStyledString().length();
	memcpy(tszMsgBuffer, st_JsonRoot.toStyledString().c_str(), nLen);

	TCHAR* ptszMsgBody = NULL;
	if (!APIHelp_HttpRequest_Custom(_T("POST"), lpszPostUrl, tszMsgBuffer, NULL, &ptszMsgBody))
	{
		printf("发送投递失败！\n");
		return;
	}
	printf("MQ_GetNumber:%s\n", ptszMsgBody);
	BaseLib_OperatorMemory_FreeCStyle((XPPMEM)&ptszMsgBody);
}
void MQ_Get()
{
	int nLen = 0;
	TCHAR tszMsgBuffer[2048];
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));

	Json::Value st_JsonRoot;
	Json::Value st_JsonMQProtocol;
	Json::Value st_JsonPayload;
	st_JsonRoot["unOperatorType"] = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_JsonRoot["unOperatorCode"] = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQGET;
	st_JsonRoot["byVersion"] = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_JSON;
	st_JsonRoot["xhToken"] = xhToken;

	st_JsonMQProtocol["tszMQKey"] = lpszKey;
	st_JsonMQProtocol["nSerial"] = 1;

	st_JsonRoot["st_MQProtocol"] = st_JsonMQProtocol;

	nLen = st_JsonRoot.toStyledString().length();
	memcpy(tszMsgBuffer, st_JsonRoot.toStyledString().c_str(), nLen);

	TCHAR* ptszMsgBody = NULL;
	if (!APIHelp_HttpRequest_Custom(_T("POST"), lpszPostUrl, tszMsgBuffer, NULL, &ptszMsgBody))
	{
		printf("发送投递失败！\n");
		return;
	}
	printf("MQ_Get:%s\n", ptszMsgBody);
	BaseLib_OperatorMemory_FreeCStyle((XPPMEM)&ptszMsgBody);
}

int main()
{
#ifdef _WINDOWS
	WSADATA st_WSAData;
	WSAStartup(MAKEWORD(2, 2), &st_WSAData);
#endif

	MQ_Authorize();
	MQ_Create();
	MQ_Post("123hello");
	MQ_GetNumber();
	MQ_Get();

#ifdef _WINDOWS
	WSACleanup();
#endif
	return 0;
}