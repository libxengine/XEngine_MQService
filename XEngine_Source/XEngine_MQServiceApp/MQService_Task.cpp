﻿#include "MQService_Hdr.h"

void CALLBACK MessageQueue_CBTask_TimePublish(XENGINE_DBTIMERELEASE* pSt_DBInfo, XPVOID lParam)
{
	int nMsgLen = 0;
	XCHAR tszMsgBuffer[4096];
	XENGINE_PROTOCOLHDR st_ProtocolHdr;
	XENGINE_PROTOCOL_XMQ st_MQProtocol;
	XENGINE_DBMESSAGEQUEUE st_DBInfo;

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
	memset(&st_MQProtocol, '\0', sizeof(XENGINE_PROTOCOL_XMQ));
	memset(&st_DBInfo, '\0', sizeof(XENGINE_DBMESSAGEQUEUE));

	st_ProtocolHdr.wHeader = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER;
	st_ProtocolHdr.unOperatorType = ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XMQ;
	st_ProtocolHdr.unOperatorCode = XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_MQ_MSGNOTIFY;
	st_ProtocolHdr.byIsReply = false;
	st_ProtocolHdr.byVersion = 1;
	st_ProtocolHdr.unPacketSize = sizeof(XENGINE_PROTOCOL_XMQ);
	st_ProtocolHdr.wTail = XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL;

	st_MQProtocol.nPubTime = pSt_DBInfo->nIDTime;
	st_MQProtocol.nSerial = pSt_DBInfo->nIDMsg;
	_tcsxcpy(st_MQProtocol.tszMQKey, pSt_DBInfo->tszQueueName);

	st_DBInfo.nQueueSerial = pSt_DBInfo->nIDMsg;
	_tcsxcpy(st_DBInfo.tszQueueName, pSt_DBInfo->tszQueueName);
	DBModule_MQData_Query(&st_DBInfo);

	memcpy(&st_MQProtocol.st_MSGAttr, &st_DBInfo.nMsgAttr, sizeof(st_DBInfo.nMsgAttr));
	//是否需要通知
	int nListCount = 0;
	XENGINE_DBUSERKEY** ppSt_ListUser;
	DBModule_MQUser_KeyList(NULL, st_MQProtocol.tszMQKey, &ppSt_ListUser, &nListCount);
	for (int i = 0; i < nListCount; i++)
	{
		int nNetType = 0;
		XCHAR tszUserAddr[128];
		memset(tszUserAddr, '\0', sizeof(tszUserAddr));
		//是否跳过自己
		if (0 == _tcsxncmp(st_DBInfo.tszUserName, ppSt_ListUser[i]->tszUserName, _tcsxlen(st_DBInfo.tszUserName)) && (0 == st_MQProtocol.st_MSGAttr.byAttrSelf))
		{
			continue;
		}
		SessionModule_Client_GetAddr(ppSt_ListUser[i]->tszUserName, tszUserAddr);
		SessionModule_Client_GetType(tszUserAddr, &nNetType);
		ProtocolModule_Packet_Common(nNetType, &st_ProtocolHdr, &st_MQProtocol, tszMsgBuffer, &nMsgLen, st_DBInfo.tszMsgBuffer, st_DBInfo.nMsgLen);
		XEngine_MQXService_Send(tszUserAddr, tszMsgBuffer, nMsgLen, nNetType);
	}
	BaseLib_Memory_Free((XPPPMEM)&ppSt_ListUser, nListCount);
	
	if (st_DBConfig.st_MQUser.st_UserTime.bPubClear)
	{
		//移除这条消息
		DBModule_MQUser_TimeDelete(pSt_DBInfo);
	}
	else
	{
		//更新
		DBModule_MQUser_TimeUPDate(pSt_DBInfo);
	}
	XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("定时任务,消息主题:%s,序列:%lld,定时任务分发成功,客户端个数:%d"), pSt_DBInfo->tszQueueName, pSt_DBInfo->nIDMsg, nListCount);
}
void CALLBACK MessageQueue_CBTask_MemoryCache(ENUM_MEMORYCACHE_CALLBACK_TYPE enMemoryType, bool bSuccess, size_t nListCount, XPVOID pSt_DBInfo, XPVOID lParam)
{
	if (ENUM_MEMORYCACHE_CALLBACK_TYPE_DATA_QUERY == enMemoryType)
	{
		if (bSuccess)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("高速缓存,消息主题:%s,序列:%lld,删除过期缓存成功,缓存剩余队列:%ld"), ((XENGINE_DBMESSAGEQUEUE*)pSt_DBInfo)->tszQueueName, ((XENGINE_DBMESSAGEQUEUE*)pSt_DBInfo)->nQueueSerial, nListCount);
		}
		else
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("高速缓存,消息主题:%s,序列:%lld,删除过期缓存成功,缓存剩余队列:%ld"), ((XENGINE_DBMESSAGEQUEUE*)pSt_DBInfo)->tszQueueName, ((XENGINE_DBMESSAGEQUEUE*)pSt_DBInfo)->nQueueSerial, nListCount);
		}
	}
	else if (ENUM_MEMORYCACHE_CALLBACK_TYPE_DATA_INSERT == enMemoryType)
	{
		if (bSuccess)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("高速缓存,消息主题:%s,序列:%lld,插入缓存队列到数据库成功,缓存剩余队列:%ld"), ((XENGINE_DBMESSAGEQUEUE*)pSt_DBInfo)->tszQueueName, ((XENGINE_DBMESSAGEQUEUE*)pSt_DBInfo)->nQueueSerial, nListCount);
		}
		else
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("高速缓存,消息主题:%s,序列:%lld,插入缓存队列到数据库失败,缓存剩余队列:%ld"), ((XENGINE_DBMESSAGEQUEUE*)pSt_DBInfo)->tszQueueName, ((XENGINE_DBMESSAGEQUEUE*)pSt_DBInfo)->nQueueSerial, nListCount);
		}
	}
	else if (ENUM_MEMORYCACHE_CALLBACK_TYPE_USER_QUERY == enMemoryType)
	{
		if (bSuccess)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _X("高速缓存,用户:%s,删除过期缓存成功,缓存剩余队列:%ld"), ((XENGINE_PROTOCOL_USERINFO*)pSt_DBInfo)->tszUserName, nListCount);
		}
		else
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _X("高速缓存,用户:%s,删除过期缓存失败,缓存剩余队列:%ld"), ((XENGINE_PROTOCOL_USERINFO*)pSt_DBInfo)->tszUserName, nListCount);
		}
	}
}