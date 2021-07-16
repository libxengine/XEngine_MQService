#include "pch.h"
#include "SessionModule_Notify.h"
/********************************************************************
//    Created:     2021/07/09  13:37:50
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_SessionModule\SessionModule_Notify\SessionModule_Notify.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_SessionModule\SessionModule_Notify
//    File Base:   SessionModule_Notify
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     消息订阅服务
//    History:
*********************************************************************/
CSessionModule_Notify::CSessionModule_Notify()
{
}
CSessionModule_Notify::~CSessionModule_Notify()
{
}
///////////////////////////////////////////////////////////////////////////////
//                      公有函数
///////////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：SessionModule_Notify_Create
函数功能：创建主题 
 参数.一：lpszTopicName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入主题名
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CSessionModule_Notify::SessionModule_Notify_Create(LPCTSTR lpszTopicName)
{
    Session_IsErrorOccur = FALSE;

    if (NULL == lpszTopicName)
    {
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PARAMENT;
		return FALSE;
    }
	XENGINE_SESSIONNOTIFY st_SessionNotify;

	st_SessionNotify.pStl_ListClient = new list<tstring>;
	st_SessionNotify.st_Locker = make_shared<shared_mutex>();

    st_Locker.lock();
	stl_MapSession.insert(make_pair(lpszTopicName, st_SessionNotify));
    st_Locker.unlock();
    return TRUE;
}
/********************************************************************
函数名称：SessionModule_Notify_Destory
函数功能：销毁一个主题
 参数.一：lpszTopicName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入主题名
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CSessionModule_Notify::SessionModule_Notify_Destory(LPCTSTR lpszTopicName)
{
	Session_IsErrorOccur = FALSE;

	st_Locker.lock();
	unordered_map<tstring, XENGINE_SESSIONNOTIFY>::iterator stl_MapIterator = stl_MapSession.find(lpszTopicName);
	if (stl_MapIterator != stl_MapSession.end())
	{
		stl_MapIterator->second.st_Locker->lock();
		stl_MapIterator->second.pStl_ListClient->clear();

		delete stl_MapIterator->second.pStl_ListClient;
		stl_MapIterator->second.pStl_ListClient = NULL;
		stl_MapIterator->second.st_Locker->unlock();
        stl_MapSession.erase(stl_MapIterator);
	}
	st_Locker.unlock();

	return TRUE;
}
/************************************************************************
函数名称：SessionModule_Notify_Insert
函数功能：插入一个订阅客户端到指定主题
 参数.一：lpszTopicStr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入主题名
 参数.二：lpszClientAddr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要插入的客户端
返回值
  类型：逻辑型
  意思：是否成功
备注：
************************************************************************/
BOOL CSessionModule_Notify::SessionModule_Notify_Insert(LPCTSTR lpszTopicStr, LPCTSTR lpszClientAddr)
{
    Session_IsErrorOccur = FALSE;

    if ((NULL == lpszTopicStr) || (NULL == lpszClientAddr))
    {
        Session_IsErrorOccur = TRUE;
        Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PARAMENT;
        return FALSE;
    }
	st_Locker.lock_shared();
	unordered_map<tstring, XENGINE_SESSIONNOTIFY>::iterator stl_MapIterator = stl_MapSession.find(lpszTopicStr);
	if (stl_MapIterator == stl_MapSession.end())
	{
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_NOTFOUND;
		st_Locker.unlock_shared();
		return FALSE;
	}
	stl_MapIterator->second.st_Locker->lock();
	stl_MapIterator->second.pStl_ListClient->push_back(lpszClientAddr);
	stl_MapIterator->second.st_Locker->unlock();
	st_Locker.unlock_shared();
    return TRUE;
}
/************************************************************************
函数名称：SessionModule_Notify_Delete
函数功能：从一个主题中删除一个订阅客户端
 参数.一：lpszTopicStr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入主题名
 参数.二：lpszClientAddr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要删除的客户端
返回值
  类型：逻辑型
  意思：是否成功
备注：
************************************************************************/
BOOL CSessionModule_Notify::SessionModule_Notify_Delete(LPCTSTR lpszTopicStr, LPCTSTR lpszClientAddr)
{
    Session_IsErrorOccur = FALSE;

    if ((NULL == lpszTopicStr) || (NULL == lpszClientAddr))
    {
        Session_IsErrorOccur = TRUE;
        Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PARAMENT;
        return FALSE;
    }
    st_Locker.lock_shared();
    unordered_map<tstring, XENGINE_SESSIONNOTIFY>::iterator stl_MapIterator = stl_MapSession.find(lpszTopicStr);
	if (stl_MapIterator == stl_MapSession.end())
	{
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_NOTFOUND;
		st_Locker.unlock_shared();
		return FALSE;
	}
	stl_MapIterator->second.st_Locker->lock();
	list<tstring>::iterator stl_ListIterator = stl_MapIterator->second.pStl_ListClient->begin();
	for (; stl_ListIterator != stl_MapIterator->second.pStl_ListClient->end(); stl_ListIterator++)
	{
		if (0 == _tcsnicmp(lpszClientAddr, stl_ListIterator->c_str(), _tcslen(lpszClientAddr)))
		{
			stl_MapIterator->second.pStl_ListClient->erase(stl_ListIterator);
			break;
		}
	}
	stl_MapIterator->second.st_Locker->unlock();
    st_Locker.unlock_shared();
    return TRUE;
}
/********************************************************************
函数名称：SessionModule_Notify_GetList
函数功能：获取主题对应的订阅列表
 参数.一：lpszTopicStr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入主题名
 参数.二：ppptszListClient
  In/Out：Out
  类型：三级指针
  可空：N
  意思：输出客户端地址列表
 参数.三：pInt_ListCount
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出列表个数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
BOOL CSessionModule_Notify::SessionModule_Notify_GetList(LPCTSTR lpszTopicStr, TCHAR*** ppptszListClient, int* pInt_ListCount)
{
	Session_IsErrorOccur = FALSE;

	if ((NULL == lpszTopicStr) || (NULL == ppptszListClient) || (NULL == pInt_ListCount))
	{
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_PARAMENT;
		return FALSE;
	}
	st_Locker.lock_shared();
	unordered_map<tstring, XENGINE_SESSIONNOTIFY>::iterator stl_MapIterator = stl_MapSession.find(lpszTopicStr);
	if (stl_MapIterator == stl_MapSession.end())
	{
		Session_IsErrorOccur = TRUE;
		Session_dwErrorCode = ERROR_MQ_MODULE_SESSION_NOTFOUND;
		st_Locker.unlock_shared();
		return FALSE;
	}
	stl_MapIterator->second.st_Locker->lock_shared();
	BaseLib_OperatorMemory_Malloc((XPPPMEM)ppptszListClient, stl_MapIterator->second.pStl_ListClient->size(), MAX_PATH);
	list<tstring>::iterator stl_ListIterator = stl_MapIterator->second.pStl_ListClient->begin();
	for (int i = 0; stl_ListIterator != stl_MapIterator->second.pStl_ListClient->end(); stl_ListIterator++, i++)
	{
		_tcscpy((*ppptszListClient)[i], stl_ListIterator->c_str());
	}
	*pInt_ListCount = stl_MapIterator->second.pStl_ListClient->size();
	stl_MapIterator->second.st_Locker->unlock_shared();
	st_Locker.unlock_shared();
	return TRUE;
}