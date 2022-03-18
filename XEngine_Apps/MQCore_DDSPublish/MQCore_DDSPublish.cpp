#ifdef _MSC_BUILD
#include <Windows.h>
#include <tchar.h>
#include <json/json.h>
#pragma comment(lib,"Ws2_32")
#pragma comment(lib,"XEngine_Core/XEngine_Core")
#pragma comment(lib,"../../XEngine_Source/Debug/MQCore_ProtocolModule")
#pragma comment(lib,"../../XEngine_Source/Debug/jsoncpp")
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <XEngine_Include/XEngine_CommHdr.h>
#include <XEngine_Include/XEngine_ProtocolHdr.h>
#include <XEngine_Include/XEngine_Core/NetCore_Define.h>
#include <XEngine_Include/XEngine_Core/NetCore_Error.h>
#include "../../XEngine_Source/XQueue_ProtocolHdr.h"
#include "../../XEngine_Source/MQCore_ProtocolModule/Protocol_Define.h"
#include "../../XEngine_Source/MQCore_ProtocolModule/Protocol_Error.h"

//g++ -std=c++17 -Wall -g MQCore_DDSPublish.cpp -o MQCore_DDSPublish.exe -I ../../XEngine_Source/XEngine_ThirdPart/jsoncpp -L /usr/local/lib/XEngine_Release/XEngine_BaseLib -L /usr/local/lib/XEngine_Release/XEngine_Core -L ../../XEngine_Source/MQCore_ProtocolModule -L ../../XEngine_Source/XEngine_ThirdPart/jsoncpp -lXEngine_BaseLib -lXEngine_Core -lMQCore_ProtocolModule -ljsoncpp

SOCKET m_Socket;
LPCTSTR lpszKey = _T("XEngine_Notify");  //主题
LPCTSTR lpszIPAddr = _T("192.168.1.7");
LPCTSTR lpszGroupAddr = _T("224.0.2.100");

#define XENGINE_MQCORE_DDSPUBLISH_ENABLE_MULTI 1

int main()
{
#ifdef _MSC_BUILD
	WSADATA st_WSAData;
	WSAStartup(MAKEWORD(2, 2), &st_WSAData);
#endif
	int nMsgLen = 0;
	XNETHANDLE xhClient = 0;
	TCHAR tszMsgBuffer[2048];
	XENGINE_PROTOCOL_XDDS st_DDSProtocol;
	
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_DDSProtocol, '\0', sizeof(XENGINE_PROTOCOL_XDDS));
	//创建发布者
	st_DDSProtocol.bCreater = TRUE;
	st_DDSProtocol.bTcp = FALSE;
	st_DDSProtocol.nPort = 10000;
	strcpy(st_DDSProtocol.tszTopic, lpszKey);
	
	if (st_DDSProtocol.bTcp)
	{
		strcpy(st_DDSProtocol.tszDDSAddr, lpszIPAddr);
		//创建服务器
		if (!NetCore_TCPSelect_StartEx(&xhClient, st_DDSProtocol.nPort))
		{
			printf("NetCore_TCPSelect_StartEx:error\n");
			return -1;
		}
	}
	else
	{
		strcpy(st_DDSProtocol.tszDDSAddr, lpszGroupAddr);
		//初始化组播发送服务
		if (!NetCore_GroupCast_SDCreate((SOCKET*)&xhClient, st_DDSProtocol.tszDDSAddr, st_DDSProtocol.nPort))
		{
			printf("NetCore_GroupCast_SDCreate:error\n");
			return -1;
		}
	}
	//发送请求给服务端
#ifdef XENGINE_MQCORE_DDSPUBLISH_ENABLE_MULTI
	SOCKET hSDSocket;
	SOCKET hRVSocket;
	NetCore_BroadCast_SendInit(&hSDSocket, 5210, lpszIPAddr);
	NetCore_BroadCast_RecvInit(&hRVSocket, 5211);

	ProtocolModule_Packet_DDSTopic(&st_DDSProtocol, tszMsgBuffer, &nMsgLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_DDS_REQTOPICCREATE);
	NetCore_BroadCast_Send(hSDSocket, tszMsgBuffer, nMsgLen);

	NetCore_BroadCast_Recv(hRVSocket, tszMsgBuffer, &nMsgLen);
	NetCore_BroadCast_Close(hSDSocket);
	NetCore_BroadCast_Close(hRVSocket);
#endif

	while (1)
	{
		//发送数据
		if (st_DDSProtocol.bTcp)
		{
			if (!NetCore_TCPSelect_SendAllEx(xhClient, "hello", 5))
			{
				printf("NetCore_TCPSelect_StartEx:error\n");
				return -1;
			}
		}
		else
		{
			if (!NetCore_GroupCast_SDend((SOCKET)xhClient, "hello", 5))
			{
				printf("NetCore_GroupCast_SDCreate:error\n");
				return -1;
			}
		}
		printf("1\n");
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

#ifdef _MSC_BUILD
	WSACleanup();
#endif
	return 0;
}