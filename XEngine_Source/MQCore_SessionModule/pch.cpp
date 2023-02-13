#include "pch.h"
#include "SessionModule_Client/SessionModule_Client.h"
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
extern "C" BOOL SessionModule_Client_Init(int nSessionTime, CALLBACK_MESSAGEQUEUE_SESSIONMODULE_CLIENT_TIMEOUT fpCall_Timeout, LPVOID lParam)
{
	return m_SessionClient.SessionModule_Client_Init(nSessionTime, fpCall_Timeout, lParam);
}
extern "C" BOOL SessionModule_Client_Destory()
{
	return m_SessionClient.SessionModule_Client_Destory();
}
extern "C" BOOL SessionModule_Client_Create(LPCTSTR lpszClientAddr, LPCTSTR lpszUserName, int nNetType)
{
	return m_SessionClient.SessionModule_Client_Create(lpszClientAddr, lpszUserName, nNetType);
}
extern "C" BOOL SessionModule_Client_Delete(LPCTSTR lpszClientAddr)
{
	return m_SessionClient.SessionModule_Client_Delete(lpszClientAddr);
}
extern "C" BOOL SessionModule_Client_GetUser(LPCTSTR lpszSessionStr, TCHAR * ptszUserName)
{
	return m_SessionClient.SessionModule_Client_GetUser(lpszSessionStr, ptszUserName);
}
extern "C" BOOL SessionModule_Client_GetAddr(LPCTSTR lpszUserName, TCHAR * ptszUserAddr)
{
	return m_SessionClient.SessionModule_Client_GetAddr(lpszUserName, ptszUserAddr);
}
extern "C" BOOL SessionModule_Client_GetType(LPCTSTR lpszSessionStr, int* pInt_NetType)
{
	return m_SessionClient.SessionModule_Client_GetType(lpszSessionStr, pInt_NetType);
}
extern "C" BOOL SessionModule_Client_Heart(LPCTSTR lpszClientAddr)
{
	return m_SessionClient.SessionModule_Client_Heart(lpszClientAddr);
}