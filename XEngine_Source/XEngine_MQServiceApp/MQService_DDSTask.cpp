#include "MQService_Hdr.h"

XHTHREAD CALLBACK MessageQueue_DDSMessage_ThreadDomain()
{
	//初始化变量
	int nMsgLen = 2048;
	TCHAR tszMsgBuffer[2048];
	TCHAR tszClientAddr[128];
	XENGINE_PROTOCOLHDR st_ProtocolHdr;

	while (bIsRun)
	{
		nMsgLen = 2048;
		memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
		memset(tszClientAddr, '\0', sizeof(tszClientAddr));
		memset(&st_ProtocolHdr, '\0', sizeof(XENGINE_PROTOCOLHDR));
		
		//接收数据
		if (!NetCore_BroadCast_Recv(hRVSocket, tszMsgBuffer, &nMsgLen, tszClientAddr))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}
		//判断大小
		if (size_t(nMsgLen) >= sizeof(XENGINE_PROTOCOLHDR))
		{
			memcpy(&st_ProtocolHdr, tszMsgBuffer, sizeof(XENGINE_PROTOCOLHDR));
			//判断头
			if ((XENGIEN_COMMUNICATION_PACKET_PROTOCOL_HEADER != st_ProtocolHdr.wHeader) || (XENGIEN_COMMUNICATION_PACKET_PROTOCOL_TAIL != st_ProtocolHdr.wTail))
			{
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("消息分发端:%s,接受到的协议有问题,无法继续"), tszClientAddr);
				continue;
			}
			//判断是否是XDDS协议
			if (ENUM_XENGINE_COMMUNICATION_PROTOCOL_TYPE_XDDS != st_ProtocolHdr.unOperatorType)
			{
				XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ERROR, _T("消息分发端:%s,无法处理的协议:%X,无法继续"), tszClientAddr, st_ProtocolHdr.unOperatorType);
				continue;
			}
			MQService_DDSTask_Handle(&st_ProtocolHdr, tszClientAddr, tszMsgBuffer + sizeof(XENGINE_PROTOCOLHDR), nMsgLen - sizeof(XENGINE_PROTOCOLHDR));
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	return 0;
}
BOOL MQService_DDSTask_Handle(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, LPCTSTR lpszClientAddr, LPCTSTR lpszMsgBuffer, int nMsgLen)
{
	TCHAR tszMsgBuffer[2048];
	XENGINE_PROTOCOL_XDDS st_DDSProtocol;

	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));
	memset(&st_DDSProtocol, '\0', sizeof(XENGINE_PROTOCOL_XDDS));

	if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_DDS_REQTOPICCREATE == pSt_ProtocolHdr->unOperatorCode)
	{
		memcpy(&st_DDSProtocol, lpszMsgBuffer, sizeof(XENGINE_PROTOCOL_XDDS));
		DDSMessage_CommApi_TopicCreate(&st_DDSProtocol);
		ProtocolModule_Packet_DDSTopic(&st_DDSProtocol, tszMsgBuffer, &nMsgLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_DDS_REPTOPICCREATE);
		NetCore_BroadCast_Send(hSDSocket, tszMsgBuffer, nMsgLen);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("消息分发端:%s,请求创建主题成功,主题名:%s"), lpszClientAddr, st_DDSProtocol.tszTopic);
	}
	else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_DDS_REQTOPICDELETE == pSt_ProtocolHdr->unOperatorCode)
	{
		memcpy(&st_DDSProtocol, lpszMsgBuffer, sizeof(XENGINE_PROTOCOL_XDDS));
		DDSMessage_CommApi_TopicDelete(&st_DDSProtocol);
		ProtocolModule_Packet_DDSTopic(&st_DDSProtocol, tszMsgBuffer, &nMsgLen, XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_DDS_REPTOPICDELETE);
		NetCore_BroadCast_Send(hSDSocket, tszMsgBuffer, nMsgLen);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("消息分发端:%s,请求删除主题成功,主题名:%s"), lpszClientAddr, st_DDSProtocol.tszTopic);
	}
	else if (XENGINE_COMMUNICATION_PROTOCOL_OPERATOR_CODE_DDS_REQTOPICQUERY == pSt_ProtocolHdr->unOperatorCode)
	{
		int nListCount = 0;
		XENGINE_PROTOCOL_XDDS** ppSt_DDSProtocol;
		memcpy(&st_DDSProtocol, lpszMsgBuffer, sizeof(XENGINE_PROTOCOL_XDDS));

		DDSMessage_CommApi_TopicGet(st_DDSProtocol.tszTopic, &ppSt_DDSProtocol, &nListCount);
		ProtocolModule_Packet_DDSQuery(&ppSt_DDSProtocol, nListCount, tszMsgBuffer, &nMsgLen);
		NetCore_BroadCast_Send(hSDSocket, tszMsgBuffer, nMsgLen);
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("消息分发端:%s,请求获取主题信息成功,主题名:%s"), lpszClientAddr, st_DDSProtocol.tszTopic);
		BaseLib_OperatorMemory_Free((XPPPMEM)&ppSt_DDSProtocol, nListCount);
	}
	else
	{
		XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_INFO, _T("消息分发端:%s,请求的协议无法处理,协议类型:%X"), lpszClientAddr, pSt_ProtocolHdr->unOperatorCode);
	}

	return TRUE;
}