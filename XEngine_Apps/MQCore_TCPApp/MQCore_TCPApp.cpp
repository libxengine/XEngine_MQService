#ifdef _WINDOWS
#include <Windows.h>
#include <tchar.h>
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"x86/XEngine_BaseLib/XEngine_BaseLib.lib")
#pragma comment(lib,"x86/XEngine_Client/XClient_Socket.lib")
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif
#include <thread>
using namespace std;
#include <XEngine_Include/XEngine_CommHdr.h>
#include <XEngine_Include/XEngine_ProtocolHdr.h>
#include <XEngine_Include/XEngine_BaseLib/BaseLib_Define.h>
#include <XEngine_Include/XEngine_BaseLib/Algorithm_Error.h>
#include <XEngine_Include/XEngine_Client/XClient_Define.h>
#include <XEngine_Include/XEngine_Client/XClient_Error.h>
#include "../../XEngine_Source/XQueue_ProtocolHdr.h"
//g++ -std=c++17 -Wall -g MQCore_APPService.cpp -o MQCore_APPService.exe -L /usr/local/lib/XEngine_Release/XEngine_BaseLib -L /usr/local/lib/XEngine_Release/XEngine_Client -lXEngine_BaseLib -lXClient_Socket

SOCKET m_Socket;
__int64x nLastNumber = 0;
LPCTSTR lpszKey = _T("XEngine_Notify");  //主题

void MQ_Authorize()
{
	int nLen = 0;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_USERAUTH st_ProtocolAuth;
	TCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_ProtocolAuth, '\0', sizeof(XENGINE_PROTOCOL_USERAUTH));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_AUTH;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_AUTH_REQLOGIN;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.byIsReply = TRUE;           //获得处理返回结果
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	_tcscpy(st_ProtocolAuth.tszUserName, "123123aa");
	_tcscpy(st_ProtocolAuth.tszUserPass, "123123");

	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_USERAUTH);

	nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_ProtocolAuth, sizeof(XENGINE_PROTOCOL_USERAUTH));

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		printf("发送投递失败！\n");
		return;
	}
	nLen = 0;
	TCHAR* ptszMsgBuffer;
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	if (!XClient_TCPSelect_RecvPkt(m_Socket, &ptszMsgBuffer, &nLen, &st_ProtocolHdr))
	{
		printf("接受数据失败！\n");
		return;
	}
	printf("%d\n", st_ProtocolHdr.wReserve);
	if (nLen > 0)
	{
		BaseLib_OperatorMemory_FreeCStyle((XPPMEM)&ptszMsgBuffer);
	}
}

void MQ_Create()
{
	int nLen = 0;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_XMQ st_XMQProtocol;
	TCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQCREATE;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.byIsReply = TRUE;           //获得处理返回结果
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	strcpy(st_XMQProtocol.tszMQKey, lpszKey);

	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ);

	nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_XMQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		printf("发送投递失败！\n");
		return;
	}
	nLen = 2048;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (!XClient_TCPSelect_RecvMsg(m_Socket, tszMsgBuffer, &nLen, FALSE))
	{
		printf("接受数据失败！\n");
		return;
	}
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	memcpy(&st_ProtocolHdr, tszMsgBuffer, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(&st_XMQProtocol, tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), sizeof(XENGINE_PROTOCOL_XMQ));
}

void MQ_Post(LPCTSTR lpszMsgBuffer)
{
	int nLen = 0;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_XMQ st_XMQProtocol;
	TCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQPOST;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.byIsReply = TRUE;           //获得处理返回结果
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	st_XMQProtocol.nSerial = 0;          //序列号,0服务会自动处理
	st_XMQProtocol.nKeepTime = -1;        //保存时间，单位秒，如果为0，获取一次后被抛弃。-1 永久存在，PacketKey不能为空
	strcpy(st_XMQProtocol.tszMQKey, lpszKey);

	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ) + strlen(lpszMsgBuffer);

	nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_XMQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR) + sizeof(XENGINE_PROTOCOL_XMQ), lpszMsgBuffer, strlen(lpszMsgBuffer));

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		printf("发送投递失败！\n");
		return;
	}
	nLen = 2048;
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	if (!XClient_TCPSelect_RecvMsg(m_Socket, tszMsgBuffer, &nLen, FALSE))
	{
		printf("接受数据失败！\n");
		return;
	}
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	memcpy(&st_ProtocolHdr, tszMsgBuffer, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(&st_XMQProtocol, tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), sizeof(XENGINE_PROTOCOL_XMQ));
}
void MQ_Get()
{
	int nLen = 0;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_XMQ st_XMQProtocol;
	TCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQGET;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.byIsReply = TRUE;                  //必须为真
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ);
	st_XMQProtocol.nSerial = 0;          //要获取的序列号,如果为0,服务会自动处理
	strcpy(st_XMQProtocol.tszMQKey, lpszKey);

	nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_XMQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		printf("发送投递失败！\n");
		return;
	}
	
	while (TRUE)
	{
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
				printf("接受到数据,主题:%s,序列:%lld,长度：%d，内容：%s\n", st_XMQProtocol.tszMQKey, st_XMQProtocol.nSerial, st_ProtocolHdr.unPacketSize - sizeof(XENGINE_PROTOCOL_XMQ), tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR) + sizeof(XENGINE_PROTOCOL_XMQ));
			}
			else
			{
				printf("获取消息队列数据失败,错误码:%d\n", st_ProtocolHdr.wReserve);
			}
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
void MQ_GetNumber()
{
	int nLen = 0;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_XMQ st_XMQProtocol;
	TCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQNUMBER;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.byIsReply = TRUE;            
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ);
	strcpy(st_XMQProtocol.tszMQKey, lpszKey);

	nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_XMQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		printf("发送投递失败！\n");
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
			printf("接受到消息信息,主题:%s,个数:%lld,起始编号:%lld,结束编号:%lld\n", st_MQNumber.tszMQKey, st_MQNumber.nCount, st_MQNumber.nFirstNumber, st_MQNumber.nLastNumber);
		}
		else
		{
			printf("接受到消息信息失败,错误码:%d\n", st_ProtocolHdr.wReserve);
		}
	}
}
void MQ_GetOrder()
{
	int nLen = 0;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_XMQ st_XMQProtocol;
	TCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQSERIAL;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.byIsReply = TRUE;
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ);

	st_XMQProtocol.nKeepTime = 0;         //倒序
	st_XMQProtocol.nSerial = nLastNumber;
	strcpy(st_XMQProtocol.tszMQKey, lpszKey);

	nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_XMQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		printf("发送投递失败！\n");
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
			printf("请求某个位置开始获取消息成功,主题:%s,序列号:%lld,顺序:%s\n", st_XMQProtocol.tszMQKey, st_XMQProtocol.nSerial, st_XMQProtocol.nKeepTime == 1 ? "顺序" : "倒序");
		}
		else
		{
			printf("请求某个位置开始获取消息失败,错误码:%d\n", st_ProtocolHdr.wReserve);
		}
	}
}

void MQ_GetSerial()
{
	int nLen = 0;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_XMQ st_XMQProtocol;
	TCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQNUMBER;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.byIsReply = TRUE;
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ);
	strcpy(st_XMQProtocol.tszMQKey, lpszKey);

	nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_XMQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		printf("发送投递失败！\n");
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
			printf("接受到消息信息,主题:%s,个数:%lld,起始编号:%lld,结束编号:%lld\n", st_MQNumber.tszMQKey, st_MQNumber.nCount, st_MQNumber.nFirstNumber, st_MQNumber.nLastNumber);
		}
		else
		{
			printf("接受到消息信息失败,错误码:%d\n", st_ProtocolHdr.wReserve);
		}
	}
}
//离开包
void MQ_Delete()
{
	int nLen = 0;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_XMQ st_XMQProtocol;
	TCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQDEL;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.byIsReply = FALSE;       //不获取结果
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ);
	st_XMQProtocol.nSerial = 1;          //要删除的序列号,必填
	strcpy(st_XMQProtocol.tszMQKey, lpszKey);

	nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_XMQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		printf("发送投递失败！\n");
		return;
	}
}
//订阅
void MQ_Subscribe()
{
	int nLen = 0;
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_XMQ st_XMQProtocol;
	TCHAR tszMsgBuffer[2048];

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_XMQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_REQNOTIFY;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.wReserve = 1;            //1为请求订阅
	st_ProtocolHdr.byIsReply = FALSE;       //不获取结果
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ);
	strcpy(st_XMQProtocol.tszMQKey, lpszKey);

	nLen = sizeof(XENGINE_PROTOCOLHDR) + st_ProtocolHdr.unPacketSize;
	memcpy(tszMsgBuffer, &st_ProtocolHdr, sizeof(XENGINE_PROTOCOLHDR));
	memcpy(tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), &st_XMQProtocol, sizeof(XENGINE_PROTOCOL_XMQ));

	if (!XClient_TCPSelect_SendMsg(m_Socket, tszMsgBuffer, nLen))
	{
		printf("发送投递失败！\n");
		return;
	}
	while (TRUE)
	{
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
				printf("接受到数据,主题:%s,序列:%lld,长度：%d，内容：%s\n", st_XMQProtocol.tszMQKey, st_XMQProtocol.nSerial, st_ProtocolHdr.unPacketSize - sizeof(XENGINE_PROTOCOL_XMQ), tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR) + sizeof(XENGINE_PROTOCOL_XMQ));
			}
			else
			{
				printf("获取消息队列数据失败,错误码:%d\n", st_ProtocolHdr.wReserve);
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
int main(int argc, char** argv)
{
#ifdef _WINDOWS
	WSADATA st_WSAData;
	WSAStartup(MAKEWORD(2, 2), &st_WSAData);
#endif
	LPCTSTR lpszServiceAddr = _T("127.0.0.1");
	LPCTSTR lpszMsgBuffer = _T("123456789aaa");
	if (!XClient_TCPSelect_Create(&m_Socket, lpszServiceAddr, 5200))
	{
		printf("连接失败！%d\n",WSAGetLastError());
		return -1;
	}
	printf("连接成功！\n");

	//MQ_Authorize();
	if (argc > 1)
	{
		MQ_Create();
		MQ_Subscribe();
	}
	else
	{
		MQ_Create();

		MQ_Post(lpszMsgBuffer);
		MQ_Post(lpszMsgBuffer);

		MQ_GetNumber();
		MQ_GetOrder();
		MQ_Get();
		MQ_Get();
		MQ_Get();
		//MQ_Delete();
	}

	XClient_TCPSelect_Close(m_Socket);
#ifdef _WINDOWS
	WSACleanup();
#endif
	return 0;
}