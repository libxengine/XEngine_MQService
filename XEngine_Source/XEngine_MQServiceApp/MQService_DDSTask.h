#pragma once

XHTHREAD CALLBACK MessageQueue_DDSMessage_ThreadDomain();
BOOL MQService_DDSTask_Handle(XENGINE_PROTOCOLHDR* pSt_ProtocolHdr, LPCTSTR lpszClientAddr, LPCTSTR lpszMsgBuffer, int nMsgLen);