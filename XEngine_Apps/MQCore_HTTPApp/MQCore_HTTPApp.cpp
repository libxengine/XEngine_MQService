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

void MQ_GetUserList()
{
	LPCXSTR lpszPostUrl = _X("http://127.0.0.1:5202/api?function=get&method=user");

	int nLen = 0;
	XCHAR* ptszMsgBody = NULL;
	if (!APIClient_Http_Request(_X("GET"), lpszPostUrl, NULL, NULL, &ptszMsgBody, &nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	_xtprintf("MQ_GetUserList:%s\n", ptszMsgBody);
	BaseLib_Memory_FreeCStyle((XPPMEM)&ptszMsgBody);
}
void MQ_GetOnlineList()
{
	LPCXSTR lpszPostUrl = _X("http://127.0.0.1:5202/api?function=get&method=online&type=0");

	int nLen = 0;
	XCHAR* ptszMsgBody = NULL;
	if (!APIClient_Http_Request(_X("GET"), lpszPostUrl, NULL, NULL, &ptszMsgBody, &nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	_xtprintf("MQ_GetOnlineList:%s\n", ptszMsgBody);
	BaseLib_Memory_FreeCStyle((XPPMEM)&ptszMsgBody);
}
void MQ_GetTopicList()
{
	LPCXSTR lpszPostUrl = _X("http://127.0.0.1:5202/api?function=get&method=topic");

	int nLen = 0;
	XCHAR* ptszMsgBody = NULL;
	if (!APIClient_Http_Request(_X("GET"), lpszPostUrl, NULL, NULL, &ptszMsgBody, &nLen))
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

	MQ_GetUserList();
	MQ_GetOnlineList();
	MQ_GetTopicList();

#ifdef _MSC_BUILD
	WSACleanup();
#endif
	return 0;
}