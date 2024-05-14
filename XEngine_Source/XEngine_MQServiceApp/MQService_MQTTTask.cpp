#include "MQService_Hdr.h"

XHTHREAD CALLBACK MessageQueue_MQTTThread(XPVOID lParam)
{
	int nThreadPos = *(int*)lParam;
	nThreadPos++;
	while (bIsRun)
	{
		if (!MQTTProtocol_Parse_WaitEvent(nThreadPos))
		{
			continue;
		}
		int nListCount = 0;
		XENGINE_MANAGEPOOL_TASKEVENT** ppSst_ListAddr;

		MQTTProtocol_Parse_GetPool(nThreadPos, &ppSst_ListAddr, &nListCount);
		for (int i = 0; i < nListCount; i++)
		{
			for (int j = 0; j < ppSst_ListAddr[i]->nPktCount; j++)
			{
				MQTTPROTOCOL_INFORMATION st_MQTTProtcol = {};

				if (MQTTProtocol_Parse_Recv(ppSst_ListAddr[i]->tszClientAddr, &st_MQTTProtcol))
				{
					MQService_MQTT_Handle(ppSst_ListAddr[i]->tszClientAddr, &st_MQTTProtcol);
				}
			}
		}
		BaseLib_OperatorMemory_Free((XPPPMEM)&ppSst_ListAddr, nListCount);
	}
	return 0;
}
void Packet_Property(XCHAR* ptszMsgBuffer, int* pInt_Len, MQTTPROTOCOL_HDRPROPERTY*** pppSt_HDRProperty, int nListCount)
{
	BaseLib_OperatorMemory_Malloc((XPPPMEM)pppSt_HDRProperty, nListCount, sizeof(MQTTPROTOCOL_HDRPROPERTY));

	(*pppSt_HDRProperty)[0]->nProLen = 4;
	(*pppSt_HDRProperty)[0]->st_unValue.nValue = 1024000;
	(*pppSt_HDRProperty)[0]->byProFlag = XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_PROPERTY_PACKMAX;

	(*pppSt_HDRProperty)[1]->nProLen = 1;
	(*pppSt_HDRProperty)[1]->st_unValue.byValue = 1;
	(*pppSt_HDRProperty)[1]->byProFlag = XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_PROPERTY_REVERAVAI;

	(*pppSt_HDRProperty)[2]->nProLen = 1;
	(*pppSt_HDRProperty)[2]->st_unValue.byValue = 1;
	(*pppSt_HDRProperty)[2]->byProFlag = XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_PROPERTY_SHAREDSUBAVAI;

	(*pppSt_HDRProperty)[3]->nProLen = 1;
	(*pppSt_HDRProperty)[3]->st_unValue.byValue = 1;
	(*pppSt_HDRProperty)[3]->byProFlag = XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_PROPERTY_SUBIDAVAI;

	(*pppSt_HDRProperty)[4]->nProLen = 2;
	(*pppSt_HDRProperty)[4]->st_unValue.wValue = 65535;
	(*pppSt_HDRProperty)[4]->byProFlag = XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_PROPERTY_ALIASMAX;

	(*pppSt_HDRProperty)[5]->nProLen = 1;
	(*pppSt_HDRProperty)[5]->st_unValue.byValue = 1;
	(*pppSt_HDRProperty)[5]->byProFlag = XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_PROPERTY_WILDCARDSUBAVAI;
}
bool MQService_MQTT_Handle(LPCXSTR lpszClientAddr, MQTTPROTOCOL_INFORMATION* pSt_MQTTProtcol)
{
	int nSDLen = 0;
	int nRVLen = 0;
	XCHAR tszSDBuffer[1024];
	XCHAR tszRVBuffer[1024];

	memset(tszSDBuffer, '\0', sizeof(tszSDBuffer));
	memset(tszRVBuffer, '\0', sizeof(tszRVBuffer));
	//是不是连接
	if (XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_TYPE_CONNECT == pSt_MQTTProtcol->st_FixedHdr.byMsgType)
	{
		int nListCount = 6;
		MQTTPROTOCOL_HDRPROPERTY** ppSt_HDRProperty;

		Packet_Property(tszSDBuffer, &nSDLen, &ppSt_HDRProperty, nListCount);
		MQTTProtocol_Packet_REPConnect(tszRVBuffer, &nRVLen, 0, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_REASON_SUCCESS, &ppSt_HDRProperty, nListCount);
		MQTTProtocol_Packet_Header(tszSDBuffer, &nSDLen, nRVLen, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_TYPE_CONNACK, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_FLAG_CONNACK);
		memcpy(tszSDBuffer + nSDLen, tszRVBuffer, nRVLen);
		nSDLen += nRVLen;

		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_TCP);
	}
	else if (XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_TYPE_SUBSCRIBE == pSt_MQTTProtcol->st_FixedHdr.byMsgType)
	{
		MQTTProtocol_Packet_REPComm(tszRVBuffer, &nRVLen, pSt_MQTTProtcol->wMsgID, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_REASON_SUCCESS);
		MQTTProtocol_Packet_Header(tszSDBuffer, &nSDLen, nRVLen, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_TYPE_SUBACK, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_FLAG_SUBACK);
		memcpy(tszSDBuffer + nSDLen, tszRVBuffer, nRVLen);
		nSDLen += nRVLen;

		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_TCP);
	}
	else if (XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_TYPE_UNSUBSCRIBE == pSt_MQTTProtcol->st_FixedHdr.byMsgType)
	{
		MQTTProtocol_Packet_REPComm(tszRVBuffer, &nRVLen, pSt_MQTTProtcol->wMsgID, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_REASON_SUCCESS);
		MQTTProtocol_Packet_Header(tszSDBuffer, &nSDLen, nRVLen, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_TYPE_UNSUBACK, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_FLAG_UNSUBACK);
		memcpy(tszSDBuffer + nSDLen, tszRVBuffer, nRVLen);
		nSDLen += nRVLen;

		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_TCP);
	}
	else if (XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_TYPE_PUBLISH == pSt_MQTTProtcol->st_FixedHdr.byMsgType)
	{
		if ((XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_FLAG_PUBLISH_QOS1 == pSt_MQTTProtcol->st_FixedHdr.byMsgFlag) || (XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_FLAG_PUBLISH_QOS2 == pSt_MQTTProtcol->st_FixedHdr.byMsgFlag))
		{
			//需要回复
			MQTTProtocol_Packet_REPPublish(tszRVBuffer, &nRVLen, pSt_MQTTProtcol->wMsgID, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_REASON_SUCCESS);
			MQTTProtocol_Packet_Header(tszSDBuffer, &nSDLen, nRVLen, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_TYPE_PUBACK, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_FLAG_PUBACK);
			memcpy(tszSDBuffer + nSDLen, tszRVBuffer, nRVLen);
			nSDLen += nRVLen;

			XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_TCP);
		}
	}
	else if (XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_TYPE_PINGREQ == pSt_MQTTProtcol->st_FixedHdr.byMsgType)
	{
		MQTTProtocol_Packet_Header(tszSDBuffer, &nSDLen, 0, XENGINE_RFCCOMPONENTS_MQTT_PROTOCOL_TYPE_PINGREP, 0);
		XEngine_MQXService_Send(lpszClientAddr, tszSDBuffer, nSDLen, XENGINE_MQAPP_NETTYPE_TCP);
	}
	return true;
}