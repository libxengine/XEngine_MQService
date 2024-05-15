#pragma once

XHTHREAD CALLBACK MessageQueue_MQTTThread(XPVOID lParam);
bool MQService_MQTT_Handle(LPCXSTR lpszClientAddr, MQTTPROTOCOL_FIXEDHEADER* pSt_MQTTHdr, LPCXSTR lpszMSGBuffer, int nMSGLen);