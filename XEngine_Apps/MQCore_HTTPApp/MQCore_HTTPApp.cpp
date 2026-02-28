#ifdef _MSC_BUILD
#include <Windows.h>
#include <tchar.h>
#pragma comment(lib,"Ws2_32")
#pragma comment(lib,"XEngine_BaseLib/XEngine_BaseLib")
#pragma comment(lib,"XEngine_Client/XClient_APIHelp")
#pragma comment(lib,"../../XEngine_Source/Debug/jsoncpp")
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json/json.h>
#include <XEngine_Include/XEngine_CommHdr.h>
#include <XEngine_Include/XEngine_Types.h>
#include <XEngine_Include/XEngine_ProtocolHdr.h>
#include <XEngine_Include/XEngine_BaseLib/BaseLib_Define.h>
#include <XEngine_Include/XEngine_BaseLib/BaseLib_Error.h>
#include <XEngine_Include/XEngine_Client/APIClient_Define.h>
#include <XEngine_Include/XEngine_Client/APIClient_Error.h>
#include "../../XEngine_Source/XQueue_ProtocolHdr.h"

//VS2022 Debug x86 Compile
//g++ -std=c++17 -Wall -g MQCore_HTTPApp.cpp -o MQCore_HTTPApp.exe -lXEngine_BaseLib -lXClient_APIHelp
__int64x xhToken = 0;

//获取未读消息
void MQ_GetUNRead()
{
	LPCXSTR lpszPostUrl = _X("http://127.0.0.1:5202/api?function=unreadmsg");

	Json::Value st_JsonRoot;
	Json::Value st_JsonObject;

	st_JsonObject["tszMQUsr"] = "123123aa";
	st_JsonObject["tszMQKey"] = "XEngine_MSGKeyName";
	st_JsonRoot["xhToken"] = xhToken;
	st_JsonRoot["Object"] = st_JsonObject; 

	int nLen = 0;
	XCHAR* ptszMsgBody = NULL;
	if (!APIClient_Http_Request(_X("POST"), lpszPostUrl, st_JsonRoot.toStyledString().c_str(), NULL, &ptszMsgBody, &nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	_xtprintf("MQ_GetUNRead:%s\n", ptszMsgBody);
	BaseLib_Memory_FreeCStyle((XPPMEM)&ptszMsgBody);
}
void MQ_DelMsg()
{
	LPCXSTR lpszPostUrl = _X("http://127.0.0.1:5202/api?function=deletemsg");

	Json::Value st_JsonRoot;
	Json::Value st_JsonObject;

	st_JsonObject["nSerial"] = 1;
	st_JsonObject["tszMQKey"] = "XEngine_MSGKeyName";
	st_JsonRoot["xhToken"] = xhToken;
	st_JsonRoot["Object"] = st_JsonObject;

	int nLen = 0;
	XCHAR* ptszMsgBody = NULL;
	if (!APIClient_Http_Request(_X("POST"), lpszPostUrl, st_JsonRoot.toStyledString().c_str(), NULL, &ptszMsgBody, &nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	_xtprintf("MQ_DelMsg:%s\n", ptszMsgBody);
	BaseLib_Memory_FreeCStyle((XPPMEM)&ptszMsgBody);
}

void MQ_UserRegister()
{
	LPCXSTR lpszPostUrl = _X("http://127.0.0.1:5202/api?function=register");

	Json::Value st_JsonRoot;
	Json::Value st_JsonUser;
	st_JsonUser["tszUserName"] = "123123aa";
	st_JsonUser["tszUserPass"] = "123123";
	st_JsonUser["tszEMailAddr"] = "xxx@ad.com";
	st_JsonUser["nPhoneNumber"] = 1369999999;
	st_JsonUser["nIDNumber"] = 5111111111110;
	st_JsonRoot["st_UserInfo"] = st_JsonUser;
	int nLen = 0;
	XCHAR* ptszMsgBody = NULL;
	if (!APIClient_Http_Request(_X("POST"), lpszPostUrl, st_JsonRoot.toStyledString().c_str(), NULL, &ptszMsgBody, &nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	_xtprintf("MQ_UserRegister:%s\n", ptszMsgBody);
	BaseLib_Memory_FreeCStyle((XPPMEM)&ptszMsgBody);
}
void MQ_UserLogin()
{
	LPCXSTR lpszPostUrl = _X("http://127.0.0.1:5202/api?function=login&user=123123aa&pass=123123");

	int nLen = 0;
	XCHAR* ptszMsgBody = NULL;
	if (!APIClient_Http_Request(_X("GET"), lpszPostUrl, NULL, NULL, &ptszMsgBody, &nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	Json::Value st_JsonRoot;
	JSONCPP_STRING st_JsonError;
	Json::CharReaderBuilder st_ReaderBuilder;

	std::unique_ptr<Json::CharReader> const pSt_JsonReader(st_ReaderBuilder.newCharReader());
	if (!pSt_JsonReader->parse(ptszMsgBody, ptszMsgBody + nLen, &st_JsonRoot, &st_JsonError))
	{
		return;
	}
	xhToken = _ttxoll(st_JsonRoot["msg"].asCString());

	_xtprintf("MQ_UserLogin:%s\n", ptszMsgBody);
	BaseLib_Memory_FreeCStyle((XPPMEM)&ptszMsgBody);
}
void MQ_UserDelete()
{
	LPCXSTR lpszPostUrl = _X("http://127.0.0.1:5202/api?function=deleteuser");

	Json::Value st_JsonRoot;
	Json::Value st_JsonUser;
	st_JsonUser["tszUserName"] = "123123aa";
	st_JsonUser["tszUserPass"] = "123123";
	st_JsonUser["tszEMailAddr"] = "xxx@ad.com";
	st_JsonUser["nPhoneNumber"] = 1369999999;
	st_JsonUser["nIDNumber"] = 5111111111110;
	st_JsonRoot["st_UserInfo"] = st_JsonUser;
	int nLen = 0;
	XCHAR* ptszMsgBody = NULL;
	if (!APIClient_Http_Request(_X("POST"), lpszPostUrl, st_JsonRoot.toStyledString().c_str(), NULL, &ptszMsgBody, &nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	_xtprintf("MQ_UserDelete:%s\n", ptszMsgBody);
	BaseLib_Memory_FreeCStyle((XPPMEM)&ptszMsgBody);
}
void MQ_GetUserList()
{
	LPCXSTR lpszPostUrl = _X("http://127.0.0.1:5202/api?function=getuser");

	Json::Value st_JsonRoot;
	st_JsonRoot["xhToken"] = xhToken;

	int nLen = 0;
	XCHAR* ptszMsgBody = NULL;
	if (!APIClient_Http_Request(_X("POST"), lpszPostUrl, st_JsonRoot.toStyledString().c_str(), NULL, &ptszMsgBody, &nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	_xtprintf("MQ_GetUserList:%s\n", ptszMsgBody);
	BaseLib_Memory_FreeCStyle((XPPMEM)&ptszMsgBody);
}
void MQ_GetOnlineList()
{
	LPCXSTR lpszPostUrl = _X("http://127.0.0.1:5202/api?function=getonline");

	Json::Value st_JsonRoot;
	st_JsonRoot["xhToken"] = xhToken;

	int nLen = 0;
	XCHAR* ptszMsgBody = NULL;
	if (!APIClient_Http_Request(_X("POST"), lpszPostUrl, st_JsonRoot.toStyledString().c_str(), NULL, &ptszMsgBody, &nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	_xtprintf("MQ_GetOnlineList:%s\n", ptszMsgBody);
	BaseLib_Memory_FreeCStyle((XPPMEM)&ptszMsgBody);
}
void MQ_TopicCreate()
{
	LPCXSTR lpszPostUrl = _X("http://127.0.0.1:5202/api?function=createtopic");

	Json::Value st_JsonRoot;
	Json::Value st_JsonObject;
	
	st_JsonObject["tszMQUsr"] = "123123aa";
	st_JsonObject["tszMQKey"] = "topictest";

	st_JsonRoot["xhToken"] = xhToken;
	st_JsonRoot["Object"] = st_JsonObject;

	int nLen = 0;
	XCHAR* ptszMsgBody = NULL;
	if (!APIClient_Http_Request(_X("POST"), lpszPostUrl, st_JsonRoot.toStyledString().c_str(), NULL, &ptszMsgBody, &nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	_xtprintf("MQ_TopicCreate:%s\n", ptszMsgBody);
	BaseLib_Memory_FreeCStyle((XPPMEM)&ptszMsgBody);
}
void MQ_TopicModify()
{
	LPCXSTR lpszPostUrl = _X("http://127.0.0.1:5202/api?function=modifytopic");

	Json::Value st_JsonRoot;
	Json::Value st_JsonObject;

	st_JsonObject["tszMQUsr"] = "123123aa";
	st_JsonObject["tszSrcTopic"] = "topictest";
	st_JsonObject["tszDstTopic"] = "topic2test";

	st_JsonRoot["xhToken"] = xhToken;
	st_JsonRoot["Object"] = st_JsonObject;

	int nLen = 0;
	XCHAR* ptszMsgBody = NULL;
	if (!APIClient_Http_Request(_X("POST"), lpszPostUrl, st_JsonRoot.toStyledString().c_str(), NULL, &ptszMsgBody, &nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	_xtprintf("MQ_TopicCreate:%s\n", ptszMsgBody);
	BaseLib_Memory_FreeCStyle((XPPMEM)&ptszMsgBody);
}

void MQ_TopicDelete()
{
	LPCXSTR lpszPostUrl = _X("http://127.0.0.1:5202/api?function=deletetopic");

	Json::Value st_JsonRoot;
	Json::Value st_JsonObject;

	st_JsonObject["tszMQUsr"] = "123123aa";
	st_JsonObject["tszMQKey"] = "topic2test";

	st_JsonRoot["xhToken"] = xhToken;
	st_JsonRoot["Object"] = st_JsonObject;

	int nLen = 0;
	XCHAR* ptszMsgBody = NULL;
	if (!APIClient_Http_Request(_X("POST"), lpszPostUrl, st_JsonRoot.toStyledString().c_str(), NULL, &ptszMsgBody, &nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	_xtprintf("MQ_TopicDelete:%s\n", ptszMsgBody);
	BaseLib_Memory_FreeCStyle((XPPMEM)&ptszMsgBody);
}
void MQ_GetTopic()
{
	LPCXSTR lpszPostUrl = _X("http://127.0.0.1:5202/api?function=gettopic");

	Json::Value st_JsonRoot;
	Json::Value st_JsonObject;

	st_JsonRoot["xhToken"] = xhToken;
	st_JsonObject["tszMQKey"] = "topictest";
	st_JsonRoot["Object"] = st_JsonObject;

	int nLen = 0;
	XCHAR* ptszMsgBody = NULL;
	if (!APIClient_Http_Request(_X("POST"), lpszPostUrl, st_JsonRoot.toStyledString().c_str(), NULL, &ptszMsgBody, &nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	_xtprintf("MQ_GetTopic:%s\n", ptszMsgBody);
	BaseLib_Memory_FreeCStyle((XPPMEM)&ptszMsgBody);
}
void MQ_GetNumber()
{
	LPCXSTR lpszPostUrl = _X("http://127.0.0.1:5202/api?function=getnumber");

	Json::Value st_JsonRoot;
	Json::Value st_JsonObject;

	st_JsonObject["nKeepTime"] = 1;
	st_JsonObject["tszMQKey"] = "XEngine_CommKey";
	st_JsonRoot["xhToken"] = xhToken;
	st_JsonRoot["Object"] = st_JsonObject;

	int nLen = 0;
	XCHAR* ptszMsgBody = NULL;
	if (!APIClient_Http_Request(_X("POST"), lpszPostUrl, st_JsonRoot.toStyledString().c_str(), NULL, &ptszMsgBody, &nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	_xtprintf("MQ_GetNumber:%s\n", ptszMsgBody);
	BaseLib_Memory_FreeCStyle((XPPMEM)&ptszMsgBody);
}
void MQ_GetTopicList()
{
	LPCXSTR lpszPostUrl = _X("http://127.0.0.1:5202/api?function=getlist");

	Json::Value st_JsonRoot;
	st_JsonRoot["xhToken"] = xhToken;

	int nLen = 0;
	XCHAR* ptszMsgBody = NULL;
	if (!APIClient_Http_Request(_X("POST"), lpszPostUrl, st_JsonRoot.toStyledString().c_str(), NULL, &ptszMsgBody, &nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	_xtprintf("MQ_GetTopicList:%s\n", ptszMsgBody);
	BaseLib_Memory_FreeCStyle((XPPMEM)&ptszMsgBody);
}

int main()
{
#ifdef _MSC_BUILD
	WSADATA st_WSAData;
	WSAStartup(MAKEWORD(2, 2), &st_WSAData);
#endif

	MQ_UserRegister();
	MQ_UserLogin();
	MQ_GetUNRead();
	MQ_GetUserList();
	MQ_GetOnlineList();
	MQ_TopicCreate();
	MQ_GetTopic();
	MQ_GetNumber();
	MQ_GetTopicList();
	MQ_TopicModify();
	MQ_TopicDelete();
	MQ_UserDelete();

#ifdef _MSC_BUILD
	WSACleanup();
#endif
	return 0;
}