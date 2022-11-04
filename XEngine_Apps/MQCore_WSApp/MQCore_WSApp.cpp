#ifdef _WINDOWS
#include <Windows.h>
#include <tchar.h>
#include <json/json.h>
#pragma comment(lib,"Ws2_32")
#pragma comment(lib,"x86/XEngine_Client/XClient_Socket")
#pragma comment(lib,"x86/XEngine_RfcComponents/RfcComponents_WSProtocol")
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif
#include <json/json.h>
#include <XEngine_Include/XEngine_CommHdr.h>
#include <XEngine_Include/XEngine_ProtocolHdr.h>
#include <XEngine_Include/XEngine_Client/XClient_Define.h>
#include <XEngine_Include/XEngine_Client/XClient_Error.h>
#include <XEngine_Include/XEngine_RfcComponents/WSProtocol_Define.h>
#include <XEngine_Include/XEngine_RfcComponents/WSProtocol_Error.h>
#include "../../XEngine_Source/XQueue_ProtocolHdr.h"

#pragma comment(lib,"../../XEngine_Source/Debug/jsoncpp")
//g++ -std=c++17 -Wall -g MQCore_WSApp.cpp -o MQCore_WSApp.exe -I ../../XEngine_Source/XEngine_ThirdPart/jsoncpp -L /usr/local/lib/XEngine_Release/XEngine_BaseLib -L /usr/local/lib/XEngine_Release/XEngine_Client -L /usr/local/lib/XEngine_Release/XEngine_RfcComponents -L ../../XEngine_Source/XEngine_ThirdPart/jsoncpp -lXEngine_BaseLib -lXEngine_Algorithm -lXClient_Socket -lRfcComponents_WSProtocol -ljsoncpp

SOCKET m_Socket;
LPCTSTR lpszKey = _T("XEngine_Notify");  //主题

BOOL MQ_SendPacket(LPCTSTR lpszMsgBuffer, int nMsgLen)
{
	int nSDLen = nMsgLen;
	TCHAR tszSDBuffer[2048];
	memset(tszSDBuffer, '\0', sizeof(tszSDBuffer));
	//connect is ok.
	if (!RfcComponents_WSCodec_EncodeMsg(lpszMsgBuffer, tszSDBuffer, &nSDLen, ENUM_XENGINE_RFCOMPONENTS_WEBSOCKET_OPCODE_TEXT, TRUE))
	{
		return FALSE;
	}
	if (!XClient_TCPSelect_SendMsg(m_Socket, tszSDBuffer, nSDLen))
	{
		return FALSE;
	}
	printf("MQ_SendPacket:%d\n", nSDLen);
	return TRUE;
}
BOOL MQ_RecvPacket(TCHAR* ptszMsgBuffer, int* pInt_MsgLen)
{
	int nRVLen = 2048;
	TCHAR tszRVBuffer[2048];
	memset(tszRVBuffer, '\0', sizeof(tszRVBuffer));

	if (!XClient_TCPSelect_RecvMsg(m_Socket, tszRVBuffer, &nRVLen))
	{
		return FALSE;
	}
	//你可以直接解码数据,也可以使用wspacket包管理器来获得高性能和自动包管理
	if (!RfcComponents_WSCodec_DecodeMsg(tszRVBuffer, &nRVLen, ptszMsgBuffer))
	{
		return FALSE;
	}
	*pInt_MsgLen = nRVLen;
	return TRUE;
}

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

	if (!MQ_SendPacket(tszMsgBuffer, nLen))
	{
		printf("发送投递失败！\n");
		return;
	}
	nLen = 2048;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (!MQ_RecvPacket(tszMsgBuffer, &nLen))
	{
		printf("接受数据失败！\n");
		return;
	}
	printf("MQ_Authorize:%s\n", tszMsgBuffer);
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

	st_JsonMQProtocol["tszMQKey"] = lpszKey;
	st_JsonMQProtocol["nSerial"] = 0;
	st_JsonMQProtocol["nKeepTime"] = 0;
	st_JsonMQProtocol["nGetTimer"] = 0;

	st_JsonRoot["st_MQProtocol"] = st_JsonMQProtocol;

	nLen = st_JsonRoot.toStyledString().length();
	memcpy(tszMsgBuffer, st_JsonRoot.toStyledString().c_str(), nLen);

	if (!MQ_SendPacket(tszMsgBuffer, nLen))
	{
		printf("发送投递失败！\n");
		return;
	}
	nLen = 2048;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (!MQ_RecvPacket(tszMsgBuffer, &nLen))
	{
		printf("接受数据失败！\n");
		return;
	}
	printf("MQ_Create:%s\n", tszMsgBuffer);
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

	if (!MQ_SendPacket(tszMsgBuffer, nLen))
	{
		printf("发送投递失败！\n");
		return;
	}
	nLen = 2048;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (!MQ_RecvPacket(tszMsgBuffer, &nLen))
	{
		printf("接受数据失败！\n");
		return;
	}
	printf("MQ_Post:%s\n", tszMsgBuffer);
}


void MQ_GetSerial()
{
	int nLen = 0;
	TCHAR tszMsgBuffer[2048];
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));

	Json::Value st_JsonRoot;
	Json::Value st_JsonMQProtocol;
	Json::Value st_JsonPayload;
	st_JsonRoot["unOperatorType"] = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_JsonRoot["unOperatorCode"] = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQSERIAL;
	st_JsonRoot["byVersion"] = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_JSON;

	st_JsonMQProtocol["tszMQKey"] = lpszKey;
	st_JsonMQProtocol["nSerial"] = 10;

	st_JsonRoot["st_MQProtocol"] = st_JsonMQProtocol;

	nLen = st_JsonRoot.toStyledString().length();
	memcpy(tszMsgBuffer, st_JsonRoot.toStyledString().c_str(), nLen);

	if (!MQ_SendPacket(tszMsgBuffer, nLen))
	{
		printf("发送投递失败！\n");
		return;
	}
	nLen = 2048;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (!MQ_RecvPacket(tszMsgBuffer, &nLen))
	{
		printf("接受数据失败！\n");
		return;
	}
	printf("MQ_GetOrder:%s\n", tszMsgBuffer);
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

	st_JsonMQProtocol["tszMQKey"] = lpszKey;

	st_JsonRoot["st_MQProtocol"] = st_JsonMQProtocol;

	nLen = st_JsonRoot.toStyledString().length();
	memcpy(tszMsgBuffer, st_JsonRoot.toStyledString().c_str(), nLen);

	if (!MQ_SendPacket(tszMsgBuffer, nLen))
	{
		printf("发送投递失败！\n");
		return;
	}
	nLen = 2048;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (!MQ_RecvPacket(tszMsgBuffer, &nLen))
	{
		printf("接受数据失败！\n");
		return;
	}
	printf("MQ_GetSerial:%s\n", tszMsgBuffer);
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

	st_JsonMQProtocol["tszMQKey"] = lpszKey;
	st_JsonMQProtocol["nSerial"] = 0;      

	st_JsonRoot["st_MQProtocol"] = st_JsonMQProtocol;

	nLen = st_JsonRoot.toStyledString().length();
	memcpy(tszMsgBuffer, st_JsonRoot.toStyledString().c_str(), nLen);

	if (!MQ_SendPacket(tszMsgBuffer, nLen))
	{
		printf("发送投递失败！\n");
		return;
	}
	nLen = 2048;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (!MQ_RecvPacket(tszMsgBuffer, &nLen))
	{
		printf("接受数据失败！\n");
		return;
	}
	printf("MQ_Get:%s\n", tszMsgBuffer);
}

//订阅
void MQ_Subscribe()
{
	int nLen = 0;
	TCHAR tszMsgBuffer[2048];
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));

	Json::Value st_JsonRoot;
	Json::Value st_JsonMQProtocol;
	st_JsonRoot["unOperatorType"] = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_JsonRoot["unOperatorCode"] = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQNOTIFY;
	st_JsonRoot["wReserve"] = 1;       //1为请求订阅
	st_JsonRoot["byVersion"] = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_JSON;

	st_JsonMQProtocol["tszMQKey"] = lpszKey;

	st_JsonRoot["st_MQProtocol"] = st_JsonMQProtocol;

	nLen = st_JsonRoot.toStyledString().length();
	memcpy(tszMsgBuffer, st_JsonRoot.toStyledString().c_str(), nLen);

	if (!MQ_SendPacket(tszMsgBuffer, nLen))
	{
		printf("发送投递失败！\n");
		return;
	}
	nLen = 2048;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (!MQ_RecvPacket(tszMsgBuffer, &nLen))
	{
		printf("接受数据失败！\n");
		return;
	}
	printf("MQ_Subscribe:%s\n", tszMsgBuffer);
}

int main()
{
#ifdef _WINDOWS
	WSADATA st_WSAData;
	WSAStartup(MAKEWORD(2, 2), &st_WSAData);
#endif

	int nLen = 0;
	TCHAR tszKeyBuffer[1024];
	TCHAR tszMsgBuffer[1024];

	memset(tszKeyBuffer, '\0', sizeof(tszKeyBuffer));
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));

	if (!RfcComponents_WSConnector_Connect(tszKeyBuffer, tszMsgBuffer, &nLen, "127.0.0.1:5202"))
	{
		printf("RfcComponents_WSConnector_Connect:%lX", WSFrame_GetLastError());
		return -1;
	}
	if (!XClient_TCPSelect_Create(&m_Socket, _T("127.0.0.1"), 5202))
	{
		printf("NetClient_TCPSelect_Create:%lX", XClient_GetLastError());
		return -1;
	}
	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		printf("NetClient_TCPSelect_SendMsg:%lX", XClient_GetLastError());
		return -1;
	}

	int nPos = 0;
	nLen = 1024;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (!XClient_TCPSelect_RecvMsg(m_Socket, tszMsgBuffer, &nLen))
	{
		printf("NetClient_TCPSelect_RecvMsg:%lX", XClient_GetLastError());
		return -1;
	}
	if (!RfcComponents_WSConnector_VerConnect(tszKeyBuffer, tszMsgBuffer, &nPos))
	{
		printf("RfcComponents_WSConnector_VerConnect:%lX", WSFrame_GetLastError());
		return -1;
	}

	if (nPos > 0)
	{
		printf("%s\n", tszMsgBuffer + nPos);
	}
	MQ_Authorize();
	MQ_Create();
	MQ_Post("123hello");
	MQ_GetSerial();
	MQ_GetNumber();
	MQ_Get();
	MQ_Subscribe();

#ifdef _WINDOWS
	WSACleanup();
#endif
	return 0;
}