#pragma once

XHTHREAD CALLBACK MessageQueue_HttpThread(LPVOID lParam);
BOOL MessageQueue_Http_Handle(RFCCOMPONENTS_HTTP_REQPARAM* pSt_HTTPParam, LPCTSTR lpszClientAddr, LPCTSTR lpszMsgBuffer, int nMsgLen, TCHAR** pptszListHdr, int nHdrCount);