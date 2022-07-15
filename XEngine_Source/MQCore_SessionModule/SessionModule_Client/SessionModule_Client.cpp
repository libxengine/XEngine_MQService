#include "pch.h"
#include "SessionModule_Client.h"
/********************************************************************
//    Created:     2021/07/02  10:15:27
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_SessionModule\SessionModule_Client\SessionModule_Client.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_SessionModule\SessionModule_Client
//    File Base:   SessionModule_Client
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息会话服务
//    History:
*********************************************************************/
CSessionModule_Client::CSessionModule_Client()
{
	bRun = FALSE;
	pSTDThread = NULL;
}
CSessionModule_Client::~CSessionModule_Client()
{
}
///////////////////////////////////////////////////////////////////////////////
//                      公有函数
///////////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：SessionModule_Client_Init
函数功能：初始化客户端会话管理器
 参数.一：nSessionTime
  In/Out：In
  类型：整数型
  可空：N
  意思：输入会话超时时间,单位秒
 参数.二：fpCall_Timeout
  In/Out：In/Out
  类型：回调函数
  可空：N
  意思：设置会话超时回调
 参数.三：lParam
  In/Out：In/Out
  类型：无类型指针
  可空：Y
  意思：回调函数自定义参数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CSessionModule_Client::SessionModule_Client_Init(int nSessionTime, CALLBACK_MESSAGEQUEUE_SESSIONMODULE_CLIENT_TIMEOUT fpCall_Timeout, LPVOID lParam /* = NULL */)
{
    Session_IsErrorOccur = FALSE;

	bRun = TRUE;
	m_lParam = lParam;
	lpCall_Timeout = fpCall_Timeout;
	pSTDThread = make_shared<thread>();
    return TRUE;
}
/************************************************************************
函数名称：SessionModule_Client_Destory
函数功能：销毁会话管理器
返回值
  类型：逻辑型
  意思：是否销毁成功
备注：
************************************************************************/
BOOL CSessionModule_Client::SessionModule_Client_Destory()
{
    Session_IsErrorOccur = FALSE;

	bRun = FALSE;

	if (pSTDThread->joinable())
	{
		pSTDThread->join();
	}
    return TRUE;
}
/********************************************************************
函数名称：SessionModule_Client_Create
函数功能：创建一个用户会话 
 参数.一：lpszClientAddr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入客户端地址
 参数.二：lpszUserName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入用户名
 参数.三：nNetType
  In/Out：In
  类型：整数型
  可空：N
  意思：客户端的网络连接类型
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CSessionModule_Client::SessionModule_Client_Create(LPCTSTR lpszClientAddr, LPCTSTR lpszUserName, int nNetType)
{
    Session_IsErrorOccur = FALSE;

    if (NULL == lpszClientAddr)
    {
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PARAMENT;
		return FALSE;
    }
	XENGINE_SESSIONINFO st_SessionInfo;
	memset(&st_SessionInfo, '\0', sizeof(XENGINE_SESSIONINFO));

	st_SessionInfo.nNetType = nNetType;
	st_SessionInfo.nTimeStart = time(NULL);
	_tcscpy(st_SessionInfo.tszUserName, lpszUserName);
	_tcscpy(st_SessionInfo.tszUserAddr, lpszClientAddr);

    st_Locker.lock();
	stl_MapSession.insert(make_pair(lpszClientAddr, st_SessionInfo));
    st_Locker.unlock();
    return TRUE;
}
/********************************************************************
函数名称：SessionModule_Client_Delete
函数功能：删除一个用户
 参数.一：lpszClientAddr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入用户名
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CSessionModule_Client::SessionModule_Client_Delete(LPCTSTR lpszClientAddr)
{
	Session_IsErrorOccur = FALSE;

	st_Locker.lock();
	unordered_map<tstring, XENGINE_SESSIONINFO>::iterator stl_MapIterator = stl_MapSession.find(lpszClientAddr);
	if (stl_MapIterator != stl_MapSession.end())
	{
        stl_MapSession.erase(stl_MapIterator);
	}
	st_Locker.unlock();

	return TRUE;
}
/************************************************************************
函数名称：SessionModule_Client_GetUser
函数功能：通过会话ID获取用户
 参数.一：lpszSessionStr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入会话ID
 参数.二：ptszUserName
  In/Out：Out
  类型：字符指针
  可空：Y
  意思：输出对应的用户名
返回值
  类型：逻辑型
  意思：是否成功
备注：
************************************************************************/
BOOL CSessionModule_Client::SessionModule_Client_GetUser(LPCTSTR lpszSessionStr, TCHAR* ptszUserName /* = NULL */)
{
	Session_IsErrorOccur = FALSE;

	if (NULL == lpszSessionStr)
	{
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PARAMENT;
		return FALSE;
	}
	st_Locker.lock_shared();
	unordered_map<tstring, XENGINE_SESSIONINFO>::iterator stl_MapIterator = stl_MapSession.find(lpszSessionStr);
	if (stl_MapIterator == stl_MapSession.end())
	{
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_NOTFOUND;
		st_Locker.unlock_shared();
		return FALSE;
	}
	if (NULL != ptszUserName)
	{
		_tcscpy(ptszUserName, stl_MapIterator->second.tszUserName);
	}
	st_Locker.unlock_shared();
	return TRUE;
}
/********************************************************************
函数名称：SessionModule_Client_Heart
函数功能：触发一次心跳
 参数.一：lpszClientAddr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要触发的客户端
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CSessionModule_Client::SessionModule_Client_Heart(LPCTSTR lpszClientAddr)
{
	Session_IsErrorOccur = FALSE;

	if (NULL == lpszClientAddr)
	{
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PARAMENT;
		return FALSE;
	}
	st_Locker.lock_shared();
	unordered_map<tstring, XENGINE_SESSIONINFO>::iterator stl_MapIterator = stl_MapSession.find(lpszClientAddr);
	if (stl_MapIterator == stl_MapSession.end())
	{
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_NOTFOUND;
		st_Locker.unlock_shared();
		return FALSE;
	}
	stl_MapIterator->second.nTimeStart = time(NULL);
	st_Locker.unlock_shared();
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
//                      线程函数
///////////////////////////////////////////////////////////////////////////////
XHTHREAD CALLBACK CSessionModule_Client::SessionModule_Client_Thread(LPVOID lParam)
{
	CSessionModule_Client* pClass_This = (CSessionModule_Client*)lParam;

	list<XENGINE_SESSIONINFO> stl_ListClient;
	while (pClass_This->bRun)
	{
		pClass_This->st_Locker.lock_shared();
		unordered_map<tstring, XENGINE_SESSIONINFO>::const_iterator stl_MapIterator = pClass_This->stl_MapSession.begin();
		for (; stl_MapIterator != pClass_This->stl_MapSession.end(); stl_MapIterator++)
		{
			time_t nTimeEnd = time(NULL);
			if ((nTimeEnd - stl_MapIterator->second.nTimeStart) > pClass_This->nSessionTime)
			{
				//移除客户端
				stl_ListClient.push_back(stl_MapIterator->second);
				break;
			}
		}
		pClass_This->st_Locker.unlock_shared();
		//是否有需要移除的客户端
		if (!stl_ListClient.empty())
		{
			for (auto stl_ListIterator = stl_ListClient.begin(); stl_ListIterator != stl_ListClient.end(); stl_ListIterator++)
			{
				pClass_This->lpCall_Timeout(stl_ListIterator->tszUserAddr, stl_ListIterator->tszUserName, (ENUM_MQCORE_SESSION_CLIENT_TYPE)stl_ListIterator->nNetType, pClass_This->m_lParam);
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	return 0;
}