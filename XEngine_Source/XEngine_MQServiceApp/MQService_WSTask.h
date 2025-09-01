#pragma once

XHTHREAD XCALLBACK MessageQueue_WebsocketThread(XPVOID lParam);
bool MessageQueue_Websocket_Handle(LPCXSTR lpszClientAddr, LPCXSTR lpszMsgBuffer, int nMsgLen, ENUM_XENGINE_RFCOMPONENTS_WEBSOCKET_OPCODE enOPCode);