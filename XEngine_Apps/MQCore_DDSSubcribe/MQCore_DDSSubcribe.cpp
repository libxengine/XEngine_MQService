#ifdef _MSC_BUILD
#include <Windows.h>
#include <tchar.h>
#include <json/json.h>
#pragma comment(lib,"Ws2_32")
#pragma comment(lib,"XEngine_Core/XEngine_Core")
#pragma comment(lib,"XEngine_Client/XClient_Socket")
#pragma comment(lib,"../../XEngine_Source/Debug/MQCore_ProtocolModule")
#pragma comment(lib,"../../XEngine_Source/Debug/jsoncpp")
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _CENTOS
#include <json/json.h>
#else
#include <jsoncpp/json/json.h>
#endif
#endif
#include <XEngine_Include/XEngine_CommHdr.h>
#include <XEngine_Include/XEngine_ProtocolHdr.h>
#include <XEngine_Include/XEngine_Core/NetCore_Define.h>
#include <XEngine_Include/XEngine_Core/NetCore_Error.h>
#include <XEngine_Include/XEngine_Client/XClient_Define.h>
#include <XEngine_Include/XEngine_Client/XClient_Error.h>
#include "../../XEngine_Source/XQueue_ProtocolHdr.h"
#include "../../XEngine_Source/MQCore_ProtocolModule/Protocol_Define.h"
#include "../../XEngine_Source/MQCore_ProtocolModule/Protocol_Error.h"

//g++ -std=c++17 -Wall -g MQCore_DDSPublish.cpp -o MQCore_DDSPublish.exe -I ../../XEngine_Source/XEngine_ThirdPart/jsoncpp -L /usr/local/lib/XEngine_Release/XEngine_BaseLib -L /usr/local/lib/XEngine_Release/XEngine_Client -L /usr/local/lib/XEngine_Release/XEngine_RfcComponents -L ../../XEngine_Source/XEngine_ThirdPart/jsoncpp -lXEngine_BaseLib -lXEngine_Algorithm -lXClient_Socket -lRfcComponents_WSProtocol -ljsoncpp

SOCKET* phSocket;
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
	SOCKET hSDSocket;
	SOCKET hRVSocket;
	TCHAR tszMsgBuffer[2048];
	XENGINE_PROTOCOL_XDDS st_DDSProtocol;

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_DDSProtocol, '\0', sizeof(XENGINE_PROTOCOL_XDDS));

	//通过消息服务器来获得所有主题创建者,也可以直接优先约定通信端口
	NetCore_BroadCast_SendInit(&hSDSocket, 5210, lpszIPAddr);
	NetCore_BroadCast_RecvInit(&hRVSocket, 5211);

	strcpy(st_DDSProtocol.tszTopic, lpszKey);
	ProtocolModule_Packet_DDSTopic(&st_DDSProtocol, tszMsgBuffer, &nMsgLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_DDS_REQTOPICQUERY);
	NetCore_BroadCast_Send(hSDSocket, tszMsgBuffer, nMsgLen);

	nMsgLen = 2048;
	NetCore_BroadCast_Recv(hRVSocket, tszMsgBuffer, &nMsgLen);
	//用完后就可以关闭了
	NetCore_BroadCast_Close(hSDSocket);
	NetCore_BroadCast_Close(hRVSocket);

	int nListCount = 0;
	XENGINE_PROTOCOL_XDDS** ppSt_DDSProtocol;
	ProtocolModule_Parse_DDSQuery(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), nMsgLen - sizeof(XENGINE_PROTOCOLHDR), &ppSt_DDSProtocol, &nListCount);
	phSocket = new SOCKET[nListCount];
	for (int i = 0; i < nListCount; i++)
	{
		//创建订阅者(客户端),所有的主题发布者都需要创建
		if (ppSt_DDSProtocol[i]->bTcp)
		{
			if (!XClient_TCPSelect_Create(&phSocket[i], ppSt_DDSProtocol[i]->tszDDSAddr, ppSt_DDSProtocol[i]->nPort))
			{
				printf("XClient_TCPSelect_Create\n");
				return -1;
			}
		}
		else
		{
			if (!NetCore_GroupCast_RVCreate(&phSocket[i], ppSt_DDSProtocol[i]->tszDDSAddr, ppSt_DDSProtocol[i]->nPort))
			{
				printf("NetCore_GroupCast_RVCreate\n");
				return -1;
			}
		}
	}

	while (1)
	{
		//需要得到所有订阅者数据
		for (int i = 0; i < nListCount; i++)
		{
			//接受数据
			nMsgLen = 2048;
			TCHAR tszClientAddr[128];

			memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
			memset(tszClientAddr, '\0', sizeof(tszClientAddr));

			if (ppSt_DDSProtocol[i]->bTcp)
			{
				if (XClient_TCPSelect_RecvMsg(phSocket[i], tszMsgBuffer, &nMsgLen))
				{
					printf("XClient_TCPSelect_RecvMsg:%d,%s\n", nMsgLen, tszMsgBuffer);
				}
			}
			else
			{
				if (NetCore_GroupCast_RVecv(phSocket[i], tszMsgBuffer, &nMsgLen, tszClientAddr))
				{
					printf("NetCore_GroupCast_RVecv,%s:%d,%s\n", tszClientAddr, nMsgLen, tszMsgBuffer);
				}
			}
		}
	}

	for (int i = 0; i < nListCount; i++)
	{
		//关闭
		if (ppSt_DDSProtocol[i]->bTcp)
		{
			XClient_TCPSelect_Close(phSocket[i]);
		}
		else
		{
			NetCore_GroupCast_Close(phSocket[i]);
		}
	}
	delete[]phSocket;
#ifdef _MSC_BUILD
	WSACleanup();
#endif
	return 0;
}