#pragma once

XHTHREAD CALLBACK MessageQueue_TCPThread(XPVOID lParam);
bool MessageQueue_TCP_Handle(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, LPCXSTR lpszClientAddr, LPCXSTR lpszMsgBuffer, int nMsgLen, int nNetType);