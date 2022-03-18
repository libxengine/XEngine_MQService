#pragma once

XHTHREAD CALLBACK MessageQueue_WebsocketThread(LPVOID lParam);
BOOL MessageQueue_Websocket_Handle(LPCTSTR lpszClientAddr, LPCTSTR lpszMsgBuffer, int nMsgLen, ENUM_XENGINE_RFCOMPONENTS_WEBSOCKET_OPCODE enOPCode);