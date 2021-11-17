#include "pch.h"
#include "SessionModule_Auth.h"
/********************************************************************
//    Created:     2021/11/16  15:33:13
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_SessionModule\SessionModule_Auth\SessionModule_Auth.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_SessionModule\SessionModule_Auth
//    File Base:   SessionModule_Auth
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     授权验证
//    History:
*********************************************************************/
CSessionModule_Auth::CSessionModule_Auth()
{
}
CSessionModule_Auth::~CSessionModule_Auth()
{
}
///////////////////////////////////////////////////////////////////////////////
//                      公有函数
///////////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：SessionModule_Auth_Init
函数功能：初始化授权会话管理器
 参数.一：lpszAuthFile
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入授权文件地址
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CSessionModule_Auth::SessionModule_Auth_Init(LPCTSTR lpszAuthFile)
{
	Session_IsErrorOccur = FALSE;

	FILE* pSt_File = _tfopen(lpszAuthFile, _T("rb"));
	if (NULL == pSt_File)
	{
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_OPENFILE;
		return FALSE;
	}
	int nCount = 0;
	TCHAR tszMsgBuffer[4096];
	memset(tszMsgBuffer, '\0', sizeof(tszMsgBuffer));

	while (TRUE)
	{
		int nRet = fread(tszMsgBuffer + nCount, 1, 1024, pSt_File);
		if (nRet <= 0)
		{
			break;
		}
		nCount += nRet;
	}
	fclose(pSt_File);

	LPCTSTR lpszLineStr = _T("\r\n");
	TCHAR* ptszTokStr = _tcstok(tszMsgBuffer, lpszLineStr);
	while (1)
	{
		if (NULL == ptszTokStr)
		{
			break;
		}
		XENGINE_SESSIONAUTH st_UserInfo;
		memset(&st_UserInfo, '\0', sizeof(XENGINE_SESSIONAUTH));

		_stscanf(ptszTokStr, _T("%s %s"), st_UserInfo.tszUserName, st_UserInfo.tszUserPass);
		stl_MapSession.insert(make_pair(st_UserInfo.tszUserName, st_UserInfo));

		ptszTokStr = _tcstok(NULL, lpszLineStr);
	}
	return TRUE;
}
/************************************************************************
函数名称：SessionModule_Auth_Destory
函数功能：销毁授权管理器
返回值
  类型：逻辑型
  意思：是否销毁成功
备注：
************************************************************************/
BOOL CSessionModule_Auth::SessionModule_Auth_Destory()
{
    Session_IsErrorOccur = FALSE;

	stl_MapSession.clear();
    return TRUE;
}
/********************************************************************
函数名称：SessionModule_Auth_Exist
函数功能：是否验证通过
 参数.一：lpszUser
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入用户名
 参数.二：lpszPass
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入密码
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CSessionModule_Auth::SessionModule_Auth_Exist(LPCTSTR lpszUser, LPCTSTR lpszPass)
{
	Session_IsErrorOccur = FALSE;

	unordered_map<string, XENGINE_SESSIONAUTH>::const_iterator stl_MapIterator = stl_MapSession.find(lpszUser);
	if (stl_MapIterator == stl_MapSession.end())
	{
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_NOTFOUND;
		return FALSE;
	}

	if (_tcslen(lpszPass) != _tcslen(stl_MapIterator->second.tszUserPass))
	{
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PASSWORD;
		return FALSE;
	}
	if (0 != _tcsncmp(lpszPass, stl_MapIterator->second.tszUserPass, _tcslen(lpszPass)))
	{
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PASSWORD;
		return FALSE;
	}
	return TRUE;
}