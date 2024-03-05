#ifdef _MSC_BUILD
#include <Windows.h>
#include <tchar.h>
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"x86/XEngine_BaseLib/XEngine_BaseLib.lib")
#pragma comment(lib,"x86/XEngine_Client/XClient_Socket.lib")
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
using namespace std;
#include <XEngine_Include/XEngine_CommHdr.h>
#include <XEngine_Include/XEngine_Types.h>
#include <XEngine_Include/XEngine_ProtocolHdr.h>
#include <XEngine_Include/XEngine_BaseLib/BaseLib_Define.h>
#include <XEngine_Include/XEngine_BaseLib/Algorithm_Error.h>
#include <XEngine_Include/XEngine_Client/XClient_Define.h>
#include <XEngine_Include/XEngine_Client/XClient_Error.h>
#include "../../XEngine_Source/XQueue_ProtocolHdr.h"

//g++ -std=c++17 -Wall -g MQCore_TCPApp.cpp -o MQCore_TCPApp.exe -I ../../XEngine_Source/XEngine_ThirdPart/jsoncpp -L ../../XEngine_Source/XEngine_ThirdPart/jsoncpp -lXEngine_BaseLib -lXClient_Socket

XSOCKET m_Socket;
__int64x nLastNumber = 0;
LPCXSTR lpszKey = _X("XEngine_NotifyKey");  //主题

void MQ_Register()
{
	int nLen = 0;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_USERINFO st_ProtocolInfo;
	XCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_ProtocolInfo, '\0', sizeof(XENGINE_PROTOCOL_USERINFO));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_AUTH;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERREG;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.byIsReply = true;           //获得处理返回结果
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	st_ProtocolInfo.nUserLevel = 0;
	st_ProtocolInfo.nUserState = 0;
	st_ProtocolInfo.nPhoneNumber = 13699999999;
	st_ProtocolInfo.nIDNumber = 511000000000101010;
	strcpy(st_ProtocolInfo.tszUserName, "123123aa");
	strcpy(st_ProtocolInfo.tszUserPass, "123123");
	strcpy(st_ProtocolInfo.tszEMailAddr, "486179@qq.com");

	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_USERINFO);

	nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_ProtocolInfo, sizeof(XENGINE_PROTOCOL_USERINFO));

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	nLen = 0;
	XCHAR* ptszMsgBuffer;
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	if (!XClient_TCPSelect_RecvPkt(m_Socket, &ptszMsgBuffer, &nLen, &st_ProtocolHdr))
	{
		_xtprintf("接受数据失败！\n");
		return;
	}
	_xtprintf("%d\n", st_ProtocolHdr.wReserve);
	if (nLen > 0)
	{
		BaseLib_OperatorMemory_FreeCStyle((XPPMEM)&ptszMsgBuffer);
	}
}
void MQ_Authorize()
{
	int nLen = 0;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_USERAUTH st_ProtocolAuth;
	XCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_ProtocolAuth, '\0', sizeof(XENGINE_PROTOCOL_USERAUTH));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_AUTH;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERLOG;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.byIsReply = true;           //获得处理返回结果
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	strcpy(st_ProtocolAuth.tszUserName, "123123aa");
	strcpy(st_ProtocolAuth.tszUserPass, "123123");

	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_USERAUTH);

	nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_ProtocolAuth, sizeof(XENGINE_PROTOCOL_USERAUTH));

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	nLen = 0;
	XCHAR* ptszMsgBuffer;
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	if (!XClient_TCPSelect_RecvPkt(m_Socket, &ptszMsgBuffer, &nLen, &st_ProtocolHdr))
	{
		_xtprintf("接受数据失败！\n");
		return;
	}
	_xtprintf("%d\n", st_ProtocolHdr.wReserve);
	if (nLen > 0)
	{
		BaseLib_OperatorMemory_FreeCStyle((XPPMEM)&ptszMsgBuffer);
	}
}
//获取未读消息
void MQ_GetUNRead(int nType = 0)
{
	int nLen = 0;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_XMQ st_XMQProtocol;
	XCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUNREAD;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.byIsReply = true;                  //必须为真
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;
	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ);

	nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_XMQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}

	while (true)
	{
		nLen = 0;
		XCHAR* ptszMsgBuffer;

		memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
		if (XClient_TCPSelect_RecvPkt(m_Socket, &ptszMsgBuffer, &nLen, &st_ProtocolHdr))
		{
			if (0 == st_ProtocolHdr.wReserve)
			{
				_xtprintf("接受到数据,长度：%d，内容：%s\n", st_ProtocolHdr.unPacketSize, ptszMsgBuffer);
			}
			else
			{
				_xtprintf("获取消息队列数据失败,错误码:%d\n", st_ProtocolHdr.wReserve);
			}
			BaseLib_OperatorMemory_FreeCStyle((XPPMEM)&ptszMsgBuffer);
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
//删除用户
void MQ_DeleteUser()
{
	int nLen = 0;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_USERINFO st_ProtocolInfo;
	XCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_ProtocolInfo, '\0', sizeof(XENGINE_PROTOCOL_USERINFO));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_AUTH;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQUSERDEL;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.byIsReply = true;    
	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_USERINFO);
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	st_ProtocolInfo.nUserLevel = 0;
	st_ProtocolInfo.nUserState = 0;
	st_ProtocolInfo.nPhoneNumber = 13699999999;
	st_ProtocolInfo.nIDNumber = 511000000000101010;
	strcpy(st_ProtocolInfo.tszUserName, "123123aa");
	strcpy(st_ProtocolInfo.tszUserPass, "123123");
	strcpy(st_ProtocolInfo.tszEMailAddr, "486179@qq.com");

	nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_ProtocolInfo, sizeof(XENGINE_PROTOCOL_USERINFO));

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	nLen = 2048;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (!XClient_TCPSelect_RecvMsg(m_Socket, tszMsgBuffer, &nLen))
	{
		_xtprintf("接受数据失败！\n");
		return;
	}
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memcpy(&st_ProtocolHdr, tszMsgBuffer, sizeof(XENGINE_PROTOCOLHDR));

	return;
}

void MQ_Create()
{
	int nLen = 0;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_XMQ st_XMQProtocol;
	XCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICCREATE;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.byIsReply = true;           //获得处理返回结果
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	strcpy(st_XMQProtocol.tszMQKey, lpszKey);

	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ);

	nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_XMQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	nLen = 2048;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (!XClient_TCPSelect_RecvMsg(m_Socket, tszMsgBuffer, &nLen))
	{
		_xtprintf("接受数据失败！\n");
		return;
	}
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	memcpy(&st_ProtocolHdr, tszMsgBuffer, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(&st_XMQProtocol, tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), sizeof(XENGINE_PROTOCOL_XMQ));
}

typedef struct
{
	XCHAR tszMsgBuffer[128];
	int a;
	int b;
}XENGINE_SQLBINARY;
void MQ_Post(LPCXSTR lpszMsgBuffer, int nType = 0, int nPubTime = -1, bool bSelf = false)
{
	int nLen = 0;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_XMQ st_XMQProtocol;
	XCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQPOST;
	st_ProtocolHdr.byIsReply = true;           //获得处理返回结果
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	st_XMQProtocol.nSerial = 0;          //序列号,0服务会自动处理
	st_XMQProtocol.nKeepTime = 0;      
	st_XMQProtocol.nPubTime = nPubTime;
	strcpy(st_XMQProtocol.tszMQKey, lpszKey);

	st_XMQProtocol.st_MSGAttr.byAttrSelf = bSelf;
	if (ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_BIN == nType)
	{
		XENGINE_SQLBINARY st_SQLBinary;
		memset(&st_SQLBinary, '\0', sizeof(XENGINE_SQLBINARY));

		st_SQLBinary.a = 1;
		st_SQLBinary.b = 2;
		strcpy(st_SQLBinary.tszMsgBuffer, "wdawandono0012d02d01d0j1");

		st_ProtocolHdr.byVersion = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_BIN;

		st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ) + sizeof(XENGINE_SQLBINARY);
		nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
		memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
		memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_XMQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));
		memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR) + sizeof(XENGINE_PROTOCOL_XMQ), &st_SQLBinary, sizeof(XENGINE_SQLBINARY));
	}
	else
	{
		st_ProtocolHdr.byVersion = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_STRING;

		st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ) + strlen(lpszMsgBuffer);
		nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
		memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
		memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_XMQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));
		memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR) + sizeof(XENGINE_PROTOCOL_XMQ), lpszMsgBuffer, strlen(lpszMsgBuffer));
	}

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	nLen = 2048;
	XCHAR* ptszMsgBuffer;
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));

	if (!XClient_TCPSelect_RecvPkt(m_Socket, &ptszMsgBuffer, &nLen, &st_ProtocolHdr))
	{
		_xtprintf("接受数据失败！\n");
		return;
	}
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));
	memcpy(&st_XMQProtocol, ptszMsgBuffer, sizeof(XENGINE_PROTOCOL_XMQ));
	BaseLib_OperatorMemory_FreeCStyle((XPPMEM)&ptszMsgBuffer);
}
void MQ_Get(int nType = 0)
{
	int nLen = 0;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_XMQ st_XMQProtocol;
	XCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQGET;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.byIsReply = true;                  //必须为真
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ);
	st_XMQProtocol.nSerial = 0;          //要获取的序列号,如果为0,服务会自动处理
	strcpy(st_XMQProtocol.tszMQKey, lpszKey);

	nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_XMQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	
	while (true)
	{
		nLen = 0;
		XCHAR* ptszMsgBuffer;

		memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
		memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));
		if (XClient_TCPSelect_RecvPkt(m_Socket, &ptszMsgBuffer, &nLen, &st_ProtocolHdr))
		{
			memcpy(&st_XMQProtocol, ptszMsgBuffer, sizeof(XENGINE_PROTOCOL_XMQ));

			if (0 == st_ProtocolHdr.wReserve)
			{
				if (ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_STRING == nType)
				{
					_xtprintf("接受到数据,主题:%s,序列:%lld,长度：%d，内容：%s\n", st_XMQProtocol.tszMQKey, st_XMQProtocol.nSerial, st_ProtocolHdr.unPacketSize - sizeof(XENGINE_PROTOCOL_XMQ), ptszMsgBuffer + sizeof(XENGINE_PROTOCOL_XMQ));
				}
				else
				{
					XENGINE_SQLBINARY st_SQLBinary;
					memset(&st_SQLBinary, '\0', sizeof(XENGINE_SQLBINARY));

					memcpy(&st_SQLBinary, ptszMsgBuffer + sizeof(XENGINE_PROTOCOL_XMQ), sizeof(XENGINE_SQLBINARY));
					break;
				}
			}
			else
			{
				_xtprintf("获取消息队列数据失败,错误码:%d\n", st_ProtocolHdr.wReserve);
			}
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void MQ_TimePublish()
{
	int nLen = 0;
	LPCXSTR lpszMsgBuffer = _X("hello world");
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_XMQ st_XMQProtocol;
	XCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQPOST;
	st_ProtocolHdr.byVersion = ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_STRING;
	st_ProtocolHdr.byIsReply = true;                
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ) + _tcsxlen(lpszMsgBuffer);
	st_XMQProtocol.nSerial = 0;          //要获取的序列号,如果为0,服务会自动处理
	st_XMQProtocol.nPubTime = time(NULL) + 30; //当前时间+60秒
	st_XMQProtocol.st_MSGAttr.byAttrSelf = 1;
	strcpy(st_XMQProtocol.tszMQKey, lpszKey);

	nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_XMQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR) + sizeof(XENGINE_PROTOCOL_XMQ), lpszMsgBuffer, _tcsxlen(lpszMsgBuffer));

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
	nLen = 0;
	XCHAR* ptszMsgBuffer = NULL;
	if (!XClient_TCPSelect_RecvPkt(m_Socket, &ptszMsgBuffer, &nLen, &st_ProtocolHdr))
	{
		_xtprintf("接受数据失败！\n");
		return;
	}
	BaseLib_OperatorMemory_FreeCStyle((XPPMEM)&ptszMsgBuffer);

	if (!XClient_TCPSelect_RecvPkt(m_Socket, &ptszMsgBuffer, &nLen, &st_ProtocolHdr, 60))
	{
		_xtprintf("接受数据失败！\n");
		return;
	}
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	memcpy(&st_XMQProtocol, ptszMsgBuffer, sizeof(st_XMQProtocol));

	_xtprintf("接受到通知消息,主题:%s,序列:%lld,长度：%d，内容：%s\n", st_XMQProtocol.tszMQKey, st_XMQProtocol.nSerial, st_ProtocolHdr.unPacketSize - sizeof(XENGINE_PROTOCOL_XMQ), ptszMsgBuffer + sizeof(XENGINE_PROTOCOL_XMQ));
	BaseLib_OperatorMemory_FreeCStyle((XPPMEM)&ptszMsgBuffer);
}

void MQ_GetNumber()
{
	int nLen = 0;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_XMQ st_XMQProtocol;
	XCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQNUMBER;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.byIsReply = true;            
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ);
	strcpy(st_XMQProtocol.tszMQKey, lpszKey);

	nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_XMQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}

	nLen = 2048;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (XClient_TCPSelect_RecvMsg(m_Socket, tszMsgBuffer, &nLen))
	{
		XENGINE_MQNUMBER st_MQNumber;
		memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
		memset(&st_MQNumber, '\0', sizeof(XENGINE_MQNUMBER));

		memcpy(&st_ProtocolHdr, tszMsgBuffer, sizeof(XENGINE_PROTOCOLHDR));
		memcpy(&st_MQNumber, tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), sizeof(XENGINE_MQNUMBER));

		if (0 == st_ProtocolHdr.wReserve)
		{
			nLastNumber = st_MQNumber.nLastNumber;
			_xtprintf("接受到消息信息,主题:%s,个数:%lld,起始编号:%lld,结束编号:%lld\n", st_MQNumber.tszMQKey, st_MQNumber.nCount, st_MQNumber.nFirstNumber, st_MQNumber.nLastNumber);
		}
		else
		{
			_xtprintf("接受到消息信息失败,错误码:%d\n", st_ProtocolHdr.wReserve);
		}
	}
}
void MQ_BindTopic()
{
	int nLen = 0;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_XMQ st_XMQProtocol;
	XCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICBIND;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.byIsReply = true;
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ);

	st_XMQProtocol.nSerial = 1; //设置为1开始读取
	strcpy(st_XMQProtocol.tszMQKey, lpszKey);

	nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_XMQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}

	nLen = 2048;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (XClient_TCPSelect_RecvMsg(m_Socket, tszMsgBuffer, &nLen))
	{
		memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
		memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

		memcpy(&st_ProtocolHdr, tszMsgBuffer, sizeof(XENGINE_PROTOCOLHDR));
		memcpy(&st_XMQProtocol, tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), sizeof(XENGINE_PROTOCOL_XMQ));

		if (0 == st_ProtocolHdr.wReserve)
		{
			_xtprintf("请求某个位置开始获取消息成功,主题:%s,序列号:%lld\n", st_XMQProtocol.tszMQKey, st_XMQProtocol.nSerial);
		}
		else
		{
			_xtprintf("请求某个位置开始获取消息失败,错误码:%d\n", st_ProtocolHdr.wReserve);
		}
	}
}

//删除主题
void MQ_DeleteTopic()
{
	int nLen = 0;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_XMQ st_XMQProtocol;
	XCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQTOPICDELETE;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.byIsReply = true;       //不获取结果
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ);
	strcpy(st_XMQProtocol.tszMQKey, lpszKey);

	nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_XMQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		_xtprintf("发送投递失败！\n");
		return;
	}
}

int main(int argc, char** argv)
{
#ifdef _MSC_BUILD
	WSADATA st_WSAData;
	WSAStartup(MAKEWORD(2, 2), &st_WSAData);
#endif
	LPCXSTR lpszServiceAddr = _X("127.0.0.1");
	LPCXSTR lpszMsgBuffer = _X("123456789aaa");
	if (!XClient_TCPSelect_Create(&m_Socket, lpszServiceAddr, 5200))
	{
		_xtprintf("连接失败！%ld\n", XClient_GetLastError());
		return -1;
	}
	_xtprintf("连接成功！\n");

	MQ_Register();
	MQ_Authorize();
	MQ_GetUNRead();
	MQ_Create();
	MQ_Post(NULL, ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_BIN, -1, true);
	MQ_BindTopic();
	MQ_Get(ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_BIN);

	MQ_Post(lpszMsgBuffer, ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_STRING, 0);
	for (int i = 0; i < 10; i++)
	{
		MQ_Post(lpszMsgBuffer, ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_STRING, -1, true);
	}
	MQ_GetNumber();
	MQ_Get(ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_STRING);
	MQ_Get(ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_STRING);
	MQ_Get(ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE_STRING);
	MQ_TimePublish();
	MQ_DeleteTopic();
	MQ_DeleteUser();

	std::this_thread::sleep_for(std::chrono::seconds(1));
	XClient_TCPSelect_Close(m_Socket);
#ifdef _MSC_BUILD
	WSACleanup();
#endif
	return 0;
}