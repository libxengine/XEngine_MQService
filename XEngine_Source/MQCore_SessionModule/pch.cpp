#include "pch.h"
#include "SessionModule_Client/SessionModule_Client.h"
#include "SessionModule_Notify/SessionModule_Notify.h"
/********************************************************************
//    Created:     2021/07/02  10:39:24
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_SessionModule\pch.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_SessionModule
//    File Base:   pch
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     导出实现
//    History:
*********************************************************************/
BOOL Session_IsErrorOccur = FALSE;
DWORD Session_dwErrorCode = 0;
//////////////////////////////////////////////////////////////////////////
CSessionModule_Client m_SessionClient;
CSessionModule_Notify m_SessionNotify;
//////////////////////////////////////////////////////////////////////////
//                        导出函数实现
//////////////////////////////////////////////////////////////////////////
extern "C" DWORD SessionModule_GetLastError(int* pInt_SysError)
{
	if (NULL != pInt_SysError)
	{
		*pInt_SysError = errno;
	}
	return Session_dwErrorCode;
}
/************************************************************************/
/*                        客户端会话                                    */
/************************************************************************/
extern "C" BOOL SessionModule_Client_Init()
{
	return m_SessionClient.SessionModule_Client_Init();
}
extern "C" BOOL SessionModule_Client_Destory()
{
	return m_SessionClient.SessionModule_Client_Destory();
}
extern "C" BOOL SessionModule_Client_Create(LPCTSTR lpszClientAddr, int nNetType)
{
	return m_SessionClient.SessionModule_Client_Create(lpszClientAddr, nNetType);
}
extern "C" BOOL SessionModule_Client_Delete(LPCTSTR lpszClientAddr)
{
	return m_SessionClient.SessionModule_Client_Delete(lpszClientAddr);
}
extern "C" BOOL SessionModule_Client_Get(LPCTSTR lpszClientAddr, XENGINE_PROTOCOL_XMQ * pSt_MQProtocol, int* pInt_NetType)
{
	return m_SessionClient.SessionModule_Client_Get(lpszClientAddr, pSt_MQProtocol, pInt_NetType);
}
extern "C" BOOL SessionModule_Client_Set(LPCTSTR lpszClientAddr, XENGINE_PROTOCOL_XMQ * pSt_MQProtocol)
{
	return m_SessionClient.SessionModule_Client_Set(lpszClientAddr, pSt_MQProtocol);
}
extern "C" BOOL SessionModule_Client_SetOrder(LPCTSTR lpszClientAddr, LPCTSTR lpszKeyStr, BOOL bOrder, __int64x nMQSerial)
{
	return m_SessionClient.SessionModule_Client_SetOrder(lpszClientAddr, lpszKeyStr, bOrder, nMQSerial);
}
extern "C" BOOL SessionModule_Client_ADDDelSerial(LPCTSTR lpszClientAddr)
{
	return m_SessionClient.SessionModule_Client_ADDDelSerial(lpszClientAddr);
}
/************************************************************************/
/*                        订阅                                          */
/************************************************************************/
extern "C" BOOL SessionModule_Notify_Create(LPCTSTR lpszTopicName)
{
	return m_SessionNotify.SessionModule_Notify_Create(lpszTopicName);
}
extern "C" BOOL SessionModule_Notify_Destory(LPCTSTR lpszTopicName)
{
	return m_SessionNotify.SessionModule_Notify_Destory(lpszTopicName);
}
extern "C" BOOL SessionModule_Notify_Insert(LPCTSTR lpszTopicStr, LPCTSTR lpszClientAddr, ENUM_MQCORE_SESSION_CLIENT_TYPE enClientType)
{
	return m_SessionNotify.SessionModule_Notify_Insert(lpszTopicStr, lpszClientAddr, enClientType);
}
extern "C" BOOL SessionModule_Notify_Delete(LPCTSTR lpszTopicStr, LPCTSTR lpszClientAddr, ENUM_MQCORE_SESSION_CLIENT_TYPE enClientType)
{
	return m_SessionNotify.SessionModule_Notify_Delete(lpszTopicStr, lpszClientAddr, enClientType);
}
extern "C" BOOL SessionModule_Notify_GetList(LPCTSTR lpszTopicStr, SESSION_NOTIFYCLIENT * **pppSt_ListClient, int* pInt_ListCount)
{
	return m_SessionNotify.SessionModule_Notify_GetList(lpszTopicStr, pppSt_ListClient, pInt_ListCount);
}