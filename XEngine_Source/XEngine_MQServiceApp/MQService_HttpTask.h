#pragma once

XHTHREAD CALLBACK MessageQueue_HttpThread(XPVOID lParam);
bool MessageQueue_Http_Handle(RFCCOMPONENTS_HTTP_REQPARAM* pSt_HTTPParam, LPCXSTR lpszClientAddr, LPCXSTR lpszMsgBuffer, int nMsgLen, XCHAR** pptszListHdr, int nHdrCount);