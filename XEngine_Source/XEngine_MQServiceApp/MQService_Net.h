#pragma once

BOOL __stdcall MessageQueue_Callback_TCPLogin(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam);   //用户连接
void __stdcall MessageQueue_Callback_TCPRecv(LPCTSTR lpszClientAddr, SOCKET hSocket, LPCTSTR lpszRecvMsg,int nMsgLen,LPVOID lParam); //接受到数据
void __stdcall MessageQueue_Callback_TCPLeave(LPCTSTR lpszClientAddr, SOCKET hSocket, LPVOID lParam);          //用户离开
BOOL NetEngine_MQXService_Send(LPCTSTR lpszClientAddr, LPCTSTR lpszMsgBuffer, int nMsgLen, int nIPProto);