#pragma once

XHTHREAD CALLBACK MessageQueue_TCPThread(LPVOID lParam);
BOOL MessageQueue_TCP_Handle(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, LPCTSTR lpszClientAddr, LPCTSTR lpszMsgBuffer, int nMsgLen, int nNetType);