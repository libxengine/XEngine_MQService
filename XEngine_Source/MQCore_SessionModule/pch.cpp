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
bool Session_IsErrorOccur = false;
XLONG Session_dwErrorCode = 0;
//////////////////////////////////////////////////////////////////////////
CSessionModule_Client m_SessionClient;
//////////////////////////////////////////////////////////////////////////
//                        导出函数实现
//////////////////////////////////////////////////////////////////////////
extern "C" XLONG SessionModule_GetLastError(int* pInt_SysError)
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
extern "C" bool SessionModule_Client_Init(int nSessionTime, CALLBACK_MESSAGEQUEUE_SESSIONMODULE_CLIENT_TIMEOUT fpCall_Timeout, XPVOID lParam)
{
	return m_SessionClient.SessionModule_Client_Init(nSessionTime, fpCall_Timeout, lParam);
}
extern "C" bool SessionModule_Client_Destory()
{
	return m_SessionClient.SessionModule_Client_Destory();
}
extern "C" bool SessionModule_Client_Create(LPCXSTR lpszClientAddr, XENGINE_PROTOCOL_USERINFO * pSt_UserInfo, int nNetType)
{
	return m_SessionClient.SessionModule_Client_Create(lpszClientAddr, pSt_UserInfo, nNetType);
}
extern "C" bool SessionModule_Client_Delete(LPCXSTR lpszClientAddr)
{
	return m_SessionClient.SessionModule_Client_Delete(lpszClientAddr);
}
extern "C" bool SessionModule_Client_GetUser(LPCXSTR lpszSessionStr, XCHAR * ptszUserName)
{
	return m_SessionClient.SessionModule_Client_GetUser(lpszSessionStr, ptszUserName);
}
extern "C" bool SessionModule_Client_GetAddr(LPCXSTR lpszUserName, XCHAR * ptszUserAddr)
{
	return m_SessionClient.SessionModule_Client_GetAddr(lpszUserName, ptszUserAddr);
}
extern "C" bool SessionModule_Client_GetInfoByUser(LPCXSTR lpszUserName, XENGINE_PROTOCOL_USERINFO * pSt_UserInfo)
{
	return m_SessionClient.SessionModule_Client_GetInfoByUser(lpszUserName, pSt_UserInfo);
}
extern "C" bool SessionModule_Client_GetType(LPCXSTR lpszSessionStr, int* pInt_NetType)
{
	return m_SessionClient.SessionModule_Client_GetType(lpszSessionStr, pInt_NetType);
}
extern "C" bool SessionModule_Client_Heart(LPCXSTR lpszClientAddr)
{
	return m_SessionClient.SessionModule_Client_Heart(lpszClientAddr);
}