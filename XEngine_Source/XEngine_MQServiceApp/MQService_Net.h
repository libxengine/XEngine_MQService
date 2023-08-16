#pragma once

bool CALLBACK MessageQueue_Callback_TCPLogin(LPCXSTR lpszClientAddr, XSOCKET hSocket, XPVOID lParam);   //用户连接
void CALLBACK MessageQueue_Callback_TCPRecv(LPCXSTR lpszClientAddr, XSOCKET hSocket, LPCXSTR lpszRecvMsg,int nMsgLen,XPVOID lParam); //接受到数据
void CALLBACK MessageQueue_Callback_TCPLeave(LPCXSTR lpszClientAddr, XSOCKET hSocket, XPVOID lParam);          //用户离开

bool CALLBACK MessageQueue_Callback_HttpLogin(LPCXSTR lpszClientAddr, XSOCKET hSocket, XPVOID lParam);  
void CALLBACK MessageQueue_Callback_HttpRecv(LPCXSTR lpszClientAddr, XSOCKET hSocket, LPCXSTR lpszRecvMsg, int nMsgLen, XPVOID lParam); 
void CALLBACK MessageQueue_Callback_HttpLeave(LPCXSTR lpszClientAddr, XSOCKET hSocket, XPVOID lParam);     

bool CALLBACK MessageQueue_Callback_WSLogin(LPCXSTR lpszClientAddr, XSOCKET hSocket, XPVOID lParam);
void CALLBACK MessageQueue_Callback_WSRecv(LPCXSTR lpszClientAddr, XSOCKET hSocket, LPCXSTR lpszRecvMsg, int nMsgLen, XPVOID lParam);
void CALLBACK MessageQueue_Callback_WSLeave(LPCXSTR lpszClientAddr, XSOCKET hSocket, XPVOID lParam);

void XEngine_MQXService_Close(LPCXSTR lpszClientAddr, int nIPProto, bool bHeart);
bool XEngine_MQXService_Send(LPCXSTR lpszClientAddr, LPCXSTR lpszMsgBuffer, int nMsgLen, int nIPProto);