#pragma once

BOOL __stdcall MessageQueue_Callback_TCPLogin(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam);   //�û�����
void __stdcall MessageQueue_Callback_TCPRecv(LPCTSTR lpszClientAddr, SOCKET hSocket, LPCTSTR lpszRecvMsg,int nMsgLen,LPVOID lParam); //���ܵ�����
void __stdcall MessageQueue_Callback_TCPLeave(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam);          //�û��뿪
void __stdcall MessageQueue_Callback_TCPHeart(LPCSTR lpszClientAddr, SOCKET hSocket, int nStatus, LPVOID lParam);

void XEngine_MQXService_Close(LPCTSTR lpszClientAddr, int nIPProto, BOOL bHeart);
BOOL XEngine_MQXService_Send(LPCTSTR lpszClientAddr, LPCTSTR lpszMsgBuffer, int nMsgLen, int nIPProto);