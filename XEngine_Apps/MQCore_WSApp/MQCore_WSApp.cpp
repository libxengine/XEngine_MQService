#ifdef _MSC_BUILD
#include <Windows.h>
#include <tchar.h>
#include <json/json.h>
#pragma comment(lib,"Ws2_32")
#pragma comment(lib,"x86/XEngine_Client/XClient_Socket")
#pragma comment(lib,"x86/XEngine_RfcComponents/RfcComponents_WSProtocol")
#pragma comment(lib,"../../XEngine_Source/Debug/jsoncpp")
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json/json.h>
#include <XEngine_Include/XEngine_CommHdr.h>
#include <XEngine_Include/XEngine_Types.h>
#include <XEngine_Include/XEngine_ProtocolHdr.h>
#include <XEngine_Include/XEngine_Client/XClient_Define.h>
#include <XEngine_Include/XEngine_Client/XClient_Error.h>
#include <XEngine_Include/XEngine_Core/ManagePool_Define.h>
#include <XEngine_Include/XEngine_RfcComponents/WSProtocol_Define.h>
#include <XEngine_Include/XEngine_RfcComponents/WSProtocol_Error.h>
#include "../../XEngine_Source/XQueue_ProtocolHdr.h"

//g++ -std=c++17 -Wall -g MQCore_WSApp.cpp -o MQCore_WSApp.exe -I ../../XEngine_Source/XEngine_ThirdPart/jsoncpp -L /usr/local/lib/XEngine_Release/XEngine_BaseLib -L /usr/local/lib/XEngine_Release/XEngine_Client -L /usr/local/lib/XEngine_Release/XEngine_RfcComponents -L ../../XEngine_Source/XEngine_ThirdPart/jsoncpp -lXEngine_BaseLib -lXEngine_Algorithm -lXClient_Socket -lRfcComponents_WSProtocol -ljsoncpp -Wl,-rpath=../../XEngine_Source/XEngine_ThirdPart/jsoncpp,--disable-new-dtags

XSOCKET m_Socket;
LPCXSTR lpszKey = _X("XEngine_Notify");  //主题

bool MQ_SendPacket(LPCXSTR lpszMsgBuffer, int nMsgLen)
{
	int nSDLen = nMsgLen;
	XCHAR tszSDBuffer[2048];
	memset(tszSDBuffer, '\0', sizeof(tszSDBuffer));
	//connect is ok.
	if (!RfcComponents_WSCodec_EncodeMsg(lpszMsgBuffer, tszSDBuffer, &nSDLen, ENUM_XENGINE_RFCOMPONENTS_WEBSOCKET_OPCODE_TEXT, true))
	{
		return false;
	}
	if (!XClient_TCPSelect_SendMsg(m_Socket, tszSDBuffer, nSDLen))
	{
		return false;
	}
	_xtprintf("MQ_SendPacket:%d\n", nSDLen);
	return true;
}
bool MQ_RecvPacket(XCHAR* ptszMsgBuffer, int* pInt_MsgLen)
{
	int nRVLen = 2048;
	XCHAR tszRVBuffer[2048];
	memset(tszRVBuffer, '\0', sizeof(tszRVBuffer));

	if (!XClient_TCPSelect_RecvMsg(m_Socket, tszRVBuffer, &nRVLen))
	{
		return false;
	}
	//你可以直接解码数据,也可以使用wspacket包管理器来获得高性能和自动包管理
	if (!RfcComponents_WSCodec_DecodeMsg(tszRVBuffer, &nRVLen, ptszMsgBuffer))
	{
		return false;
	}
	*pInt_MsgLen = nRVLen;
	return true;
}

void MQ_Authorize()
{
	int nLen = 0;
	XCHAR tszMsgBuffer[2048];
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
		_xtprintf("发送投递失败！\n");
		return;
	}
	nLen = 2048;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (!MQ_RecvPacket(tszMsgBuffer, &nLen))
	{
		_xtprintf("接受数据失败！\n");
		return;
	}
	_xtprintf("MQ_Authorize:%s\n", tszMsgBuffer);
}

void MQ_Create()
{
	int nLen = 0;
	XCHAR tszMsgBuffer[2048];
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));

	Json::Value st_JsonRoot;
	Json::Value st_JsonMQProtocol;
	st_JsonRoot["unOperatorType"] = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_JsonRoot["unOperatorCode"] = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICCREATE;
	st_JsonRoot["byVersion"] = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_JSON;
	st_JsonRoot["byIsReply"] = 1;

	st_JsonMQProtocol["tszMQKey"] = lpszKey;
	st_JsonMQProtocol["nSerial"] = 0;
	st_JsonMQProtocol["nKeepTime"] = 0;
	st_JsonMQProtocol["nGetTimer"] = 0;

	st_JsonRoot["st_MQProtocol"] = st_JsonMQProtocol;

	nLen = st_JsonRoot.toStyledString().length();
	memcpy(tszMsgBuffer, st_JsonRoot.toStyledString().c_str(), nLen);

	if (!MQ_SendPacket(tszMsgBuffer, nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	nLen = 2048;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (!MQ_RecvPacket(tszMsgBuffer, &nLen))
	{
		_xtprintf("接受数据失败！\n");
		return;
	}
	_xtprintf("MQ_Create:%s\n", tszMsgBuffer);
}

void MQ_Post(LPCXSTR lpszMsgBuffer)
{
	int nLen = 0;
	XCHAR tszMsgBuffer[2048];
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));

	Json::Value st_JsonRoot;
	Json::Value st_JsonMQProtocol;
	Json::Value st_JsonPayload;
	st_JsonRoot["unOperatorType"] = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_JsonRoot["unOperatorCode"] = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQPOST;
	st_JsonRoot["byVersion"] = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_BIN;
	st_JsonRoot["byIsReply"] = 1;

	st_JsonMQProtocol["tszMQKey"] = lpszKey;
	st_JsonMQProtocol["nSerial"] = 0;             //序列号,0服务会自动处理
	st_JsonMQProtocol["nKeepTime"] = -1;          //保存时间，单位秒，如果为0，获取一次后被抛弃。-1 永久存在，PacketKey不能为空
	st_JsonMQProtocol["nGetTimer"] = 0;

	st_JsonPayload["nPayLen"] = (Json::Value::UInt)strlen(lpszMsgBuffer);
	st_JsonPayload["tszPayData"] = lpszMsgBuffer;

	st_JsonRoot["st_MQProtocol"] = st_JsonMQProtocol;
	st_JsonRoot["st_Payload"] = st_JsonPayload;

	nLen = st_JsonRoot.toStyledString().length();
	memcpy(tszMsgBuffer, st_JsonRoot.toStyledString().c_str(), nLen);

	if (!MQ_SendPacket(tszMsgBuffer, nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	nLen = 2048;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (!MQ_RecvPacket(tszMsgBuffer, &nLen))
	{
		_xtprintf("接受数据失败！\n");
		return;
	}
	_xtprintf("MQ_Post:%s\n", tszMsgBuffer);
}


void MQ_BindTopic()
{
	int nLen = 0;
	XCHAR tszMsgBuffer[2048];
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));

	Json::Value st_JsonRoot;
	Json::Value st_JsonMQProtocol;
	Json::Value st_JsonPayload;
	st_JsonRoot["unOperatorType"] = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_JsonRoot["unOperatorCode"] = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICBIND;
	st_JsonRoot["byVersion"] = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_JSON;
	st_JsonRoot["byIsReply"] = 1;

	st_JsonMQProtocol["tszMQKey"] = lpszKey;
	st_JsonMQProtocol["nSerial"] = 1;

	st_JsonRoot["st_MQProtocol"] = st_JsonMQProtocol;

	nLen = st_JsonRoot.toStyledString().length();
	memcpy(tszMsgBuffer, st_JsonRoot.toStyledString().c_str(), nLen);

	if (!MQ_SendPacket(tszMsgBuffer, nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	nLen = 2048;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (!MQ_RecvPacket(tszMsgBuffer, &nLen))
	{
		_xtprintf("接受数据失败！\n");
		return;
	}
	_xtprintf("MQ_GetOrder:%s\n", tszMsgBuffer);
}

void MQ_GetNumber()
{
	int nLen = 0;
	XCHAR tszMsgBuffer[2048];
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));

	Json::Value st_JsonRoot;
	Json::Value st_JsonMQProtocol;
	Json::Value st_JsonPayload;
	st_JsonRoot["unOperatorType"] = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_JsonRoot["unOperatorCode"] = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQNUMBER;
	st_JsonRoot["byVersion"] = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_JSON;
	st_JsonRoot["byIsReply"] = 1;

	st_JsonMQProtocol["tszMQKey"] = lpszKey;

	st_JsonRoot["st_MQProtocol"] = st_JsonMQProtocol;

	nLen = st_JsonRoot.toStyledString().length();
	memcpy(tszMsgBuffer, st_JsonRoot.toStyledString().c_str(), nLen);

	if (!MQ_SendPacket(tszMsgBuffer, nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	nLen = 2048;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (!MQ_RecvPacket(tszMsgBuffer, &nLen))
	{
		_xtprintf("接受数据失败！\n");
		return;
	}
	_xtprintf("MQ_GetSerial:%s\n", tszMsgBuffer);
}
void MQ_Get()
{
	int nLen = 0;
	XCHAR tszMsgBuffer[2048];
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));

	Json::Value st_JsonRoot;
	Json::Value st_JsonMQProtocol;
	Json::Value st_JsonPayload;
	st_JsonRoot["unOperatorType"] = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_JsonRoot["unOperatorCode"] = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQGET;
	st_JsonRoot["byVersion"] = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_JSON;
	st_JsonRoot["byIsReply"] = 1;

	st_JsonMQProtocol["tszMQKey"] = lpszKey;
	st_JsonMQProtocol["nSerial"] = 0;      

	st_JsonRoot["st_MQProtocol"] = st_JsonMQProtocol;

	nLen = st_JsonRoot.toStyledString().length();
	memcpy(tszMsgBuffer, st_JsonRoot.toStyledString().c_str(), nLen);

	if (!MQ_SendPacket(tszMsgBuffer, nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	nLen = 2048;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (!MQ_RecvPacket(tszMsgBuffer, &nLen))
	{
		_xtprintf("接受数据失败！\n");
		return;
	}
	_xtprintf("MQ_Get:%s\n", tszMsgBuffer);
}

int main()
{
#ifdef _MSC_BUILD
	WSADATA st_WSAData;
	WSAStartup(MAKEWORD(2, 2), &st_WSAData);
#endif

	int nLen = 0;
	XCHAR tszKeyBuffer[1024];
	XCHAR tszMsgBuffer[1024];

	memset(tszKeyBuffer, '\0', sizeof(tszKeyBuffer));
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));

	if (!RfcComponents_WSConnector_Connect(tszKeyBuffer, tszMsgBuffer, &nLen))
	{
		_xtprintf("RfcComponents_WSConnector_Connect:%lX", WSFrame_GetLastError());
		return -1;
	}
	if (!XClient_TCPSelect_Create(&m_Socket, _X("127.0.0.1"), 5201))
	{
		_xtprintf("NetClient_TCPSelect_Create:%lX", XClient_GetLastError());
		return -1;
	}
	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		_xtprintf("NetClient_TCPSelect_SendMsg:%lX", XClient_GetLastError());
		return -1;
	}

	int nPos = 0;
	nLen = 1024;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (!XClient_TCPSelect_RecvMsg(m_Socket, tszMsgBuffer, &nLen))
	{
		_xtprintf("NetClient_TCPSelect_RecvMsg:%lX", XClient_GetLastError());
		return -1;
	}
	if (!RfcComponents_WSConnector_VerConnect(tszKeyBuffer, tszMsgBuffer, &nPos))
	{
		_xtprintf("RfcComponents_WSConnector_VerConnect:%lX", WSFrame_GetLastError());
		return -1;
	}

	if (nPos > 0)
	{
		_xtprintf("%s\n", tszMsgBuffer + nPos);
	}
	MQ_Authorize();
	MQ_Create();
	MQ_Post("MTIzMTIz");
	MQ_GetNumber();
	MQ_BindTopic();
	MQ_Get();

#ifdef _MSC_BUILD
	WSACleanup();
#endif
	return 0;
}