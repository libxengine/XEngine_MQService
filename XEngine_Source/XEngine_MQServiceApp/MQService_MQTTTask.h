#pragma once

XHTHREAD CALLBACK MessageQueue_MQTTThread(XPVOID lParam);
bool MQService_MQTT_Handle(LPCXSTR lpszClientAddr, MQTTPROTOCOL_INFORMATION *pSt_MQTTProtcol);