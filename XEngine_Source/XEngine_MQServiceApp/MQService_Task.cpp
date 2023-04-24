﻿#include "MQService_Hdr.h"

void CALLBACK MessageQueue_CBTask_TimePublish(LPCXSTR lpszQueueName, __int64x nIDMsg, __int64x nIDTime, XPVOID lParam)
{
	int nMsgLen = 0;
	XCHAR tszMsgBuffer[4096];
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_XMQ st_MQProtocol;
	XENGINE_DBMESSAGEQUEUE st_DBInfo;
	XENGINE_DBTIMERELEASE st_DBTime;

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_MQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));
	memset(&st_DBInfo, '\0', sizeof(XENGINE_DBMESSAGEQUEUE));
	memset(&st_DBTime, '\0', sizeof(XENGINE_DBTIMERELEASE));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_MSGNOTIFY;
	st_ProtocolHdr.byIsReply = false;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ);
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	st_MQProtocol.nPubTime = nIDTime;
	st_MQProtocol.nSerial = nIDMsg;
	_tcsxcpy(st_MQProtocol.tszMQKey, lpszQueueName);

	st_DBTime.nIDMsg = nIDMsg;
	st_DBInfo.nQueueSerial = nIDMsg;
	_tcsxcpy(st_DBInfo.tszQueueName, lpszQueueName);
	DBModule_MQData_Query(&st_DBInfo);
	//是否需要通知
	int nListCount = 0;
	XENGINE_DBUSERKEY** ppSt_ListUser;
	DBModule_MQUser_KeyList(NULL, st_MQProtocol.tszMQKey, &ppSt_ListUser, &nListCount);
	for (int i = 0; i < nListCount; i++)
	{
		int nNetType = 0;
		XCHAR tszUserAddr[128];
		memset(tszUserAddr, '\0', sizeof(tszUserAddr));
		//跳过自己
		if (0 == _tcsxncmp(st_DBInfo.tszUserName, ppSt_ListUser[i]->tszUserName, _tcsxlen(st_DBInfo.tszUserName)))
		{
			continue;
		}
		SessionModule_Client_GetAddr(ppSt_ListUser[i]->tszUserName, tszUserAddr);
		SessionModule_Client_GetType(tszUserAddr, &nNetType);
		ProtocolModule_Packet_Common(nNetType, &st_ProtocolHdr, &st_MQProtocol, tszMsgBuffer, &nMsgLen, st_DBInfo.tszMsgBuffer, st_DBInfo.nMsgLen);
		XEngine_MQXService_Send(tszUserAddr, tszMsgBuffer, nMsgLen, nNetType);
	}
	BaseLib_OperatorMemory_Free((XPPPMEM)&ppSt_ListUser, nListCount);
	
	if (st_DBConfig.st_MQUser.st_UserTime.bPubClear)
	{
		//移除这条消息
		DBModule_MQUser_TimeDelete(&st_DBTime);
	}
	else
	{
		//更新
		DBModule_MQUser_TimeUPDate(&st_DBTime);
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("定时任务,消息主题:%s,序列:%lld,定时任务分发成功,客户端个数:%d"), lpszQueueName, nIDMsg, nListCount);
}