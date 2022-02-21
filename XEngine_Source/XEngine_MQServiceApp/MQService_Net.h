#pragma once

BOOL __stdcall MessageQueue_Callback_TCPLogin(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam);   //用户连接
void __stdcall MessageQueue_Callback_TCPRecv(LPCTSTR lpszClientAddr, SOCKET hSocket, LPCTSTR lpszRecvMsg,int nMsgLen,LPVOID lParam); //接受到数据
void __stdcall MessageQueue_Callback_TCPLeave(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam);          //用户离开
void __stdcall MessageQueue_Callback_TCPHeart(LPCSTR lpszClientAddr, SOCKET hSocket, int nStatus, LPVOID lParam);

BOOL __stdcall MessageQueue_Callback_HttpLogin(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam);  
void __stdcall MessageQueue_Callback_HttpRecv(LPCTSTR lpszClientAddr, SOCKET hSocket, LPCTSTR lpszRecvMsg, int nMsgLen, LPVOID lParam); 
void __stdcall MessageQueue_Callback_HttpLeave(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam);     

BOOL __stdcall MessageQueue_Callback_WSLogin(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam);
void __stdcall MessageQueue_Callback_WSRecv(LPCTSTR lpszClientAddr, SOCKET hSocket, LPCTSTR lpszRecvMsg, int nMsgLen, LPVOID lParam);
void __stdcall MessageQueue_Callback_WSLeave(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam);
void __stdcall MessageQueue_Callback_WSHeart(LPCSTR lpszClientAddr, SOCKET hSocket, int nStatus, LPVOID lParam);

void XEngine_MQXService_Close(LPCTSTR lpszClientAddr, int nIPProto, BOOL bHeart);
BOOL XEngine_MQXService_Send(LPCTSTR lpszClientAddr, LPCTSTR lpszMsgBuffer, int nMsgLen, int nIPProto);