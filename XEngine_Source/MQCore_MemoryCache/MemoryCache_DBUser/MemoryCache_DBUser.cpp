#include "pch.h"
#include "MemoryCache_DBUser.h"
/********************************************************************
//    Created:     2025/01/17  10:09:09
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_MemoryCache\MemoryCache_DBUser\MemoryCache_DBUser.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_MemoryCache\MemoryCache_DBUser
//    File Base:   MemoryCache_DBUser
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     数据库高速缓存
//    History:
*********************************************************************/
CMemoryCache_DBUser::CMemoryCache_DBUser()
{
}
CMemoryCache_DBUser::~CMemoryCache_DBUser()
{

}
//////////////////////////////////////////////////////////////////////////
//                         公有函数
//////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：MemoryCache_DBUser_Init
函数功能：初始化高速缓存管理器
 参数.一：nTimeLast
  In/Out：In
  类型：整数型
  可空：N
  意思：允许的最后更新时间存在秒数
 参数.二：nTimeCount
  In/Out：In
  类型：整数型
  可空：N
  意思：允许的最大保留时间.将不关心是否使用,0不启用
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CMemoryCache_DBUser::MemoryCache_DBUser_Init(int nTimeLast, int nTimeCount, CALLBACK_MESSAGEQUEUE_MODULE_DATABASE_CACHE fpCall_MemoryCache, XPVOID lParam /* = NULL */)
{
    MemoryCache_IsErrorOccur = false;

	bIsRun = true;

	m_nTimeLast = nTimeLast;
	m_nTimeCount = nTimeCount;
	m_lParam = lParam;
	lpCall_MemoryCache = fpCall_MemoryCache;

	pSTDThread_Query = std::make_unique<std::thread>(DBModule_MQUser_TimeThread, this);
	if (NULL == pSTDThread_Query)
	{
		MemoryCache_IsErrorOccur = true;
		MemoryCache_dwErrorCode = ERROR_XENGINE_MQCORE_MEMORYCACHE_DBDATA_THREAD;
		return false;
	}
    return true;
}
/********************************************************************
函数名称：MemoryCache_DBUser_SetHandle
函数功能：设置句柄
 参数.一：xhDBSQL
  In/Out：In
  类型：句柄
  可空：N
  意思：数据库句柄
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CMemoryCache_DBUser::MemoryCache_DBUser_SetHandle(XNETHANDLE xhDBSQL)
{
	MemoryCache_IsErrorOccur = false;

	m_xhDBSQL = xhDBSQL;

	return true;
}
/********************************************************************
函数名称：MemoryCache_DBUser_Destory
函数功能：销毁高速缓存管理器
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CMemoryCache_DBUser::MemoryCache_DBUser_Destory()
{
    MemoryCache_IsErrorOccur = false;

	bIsRun = false;
	if (NULL != pSTDThread_Query)
	{
		pSTDThread_Query->join();
	}
    return true;
}
/********************************************************************
函数名称：MemoryCache_DBUser_DataInsert
函数功能：插入一条数据到高速缓存
 参数.一：pSt_DBUserInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要插入的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CMemoryCache_DBUser::MemoryCache_DBUser_DataInsert(XENGINE_PROTOCOL_USERINFO* pSt_DBUserInfo)
{
    MemoryCache_IsErrorOccur = false;

	if (NULL == pSt_DBUserInfo)
	{
		MemoryCache_IsErrorOccur = true;
		MemoryCache_dwErrorCode = ERROR_XENGINE_MQCORE_MEMORYCACHE_DBDATA_PARAMENT;
		return false;
	}

	st_LockerQuery.lock();
	auto stl_MapIterator = stl_MapQuery.find(pSt_DBUserInfo->tszUserName);
	if (stl_MapIterator == stl_MapQuery.end())
	{
		MEMORYCACHE_DBUSER st_DBInfo = {};

		st_DBInfo.nTimeLast = st_DBInfo.nTimeStart = time(NULL);
		st_DBInfo.st_DBUserInfo = *pSt_DBUserInfo;
		stl_MapQuery[pSt_DBUserInfo->tszUserName] = st_DBInfo;
	}
	else
	{
		stl_MapIterator->second.st_DBUserInfo = *pSt_DBUserInfo;
	}
	st_LockerQuery.unlock();
    return true;
}
/********************************************************************
函数名称：MemoryCache_DBUser_DataQuery
函数功能：查询高速缓存
 参数.一：pSt_DBUserInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要查询的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CMemoryCache_DBUser::MemoryCache_DBUser_DataQuery(XENGINE_PROTOCOL_USERINFO* pSt_DBUserInfo)
{
	MemoryCache_IsErrorOccur = false;

	if (NULL == pSt_DBUserInfo)
	{
		MemoryCache_IsErrorOccur = true;
		MemoryCache_dwErrorCode = ERROR_XENGINE_MQCORE_MEMORYCACHE_DBDATA_PARAMENT;
		return false;
	}

	st_LockerQuery.lock_shared();
	auto stl_MapIterator = stl_MapQuery.find(pSt_DBUserInfo->tszUserName);
	if (stl_MapIterator == stl_MapQuery.end())
	{
		MemoryCache_IsErrorOccur = true;
		MemoryCache_dwErrorCode = ERROR_XENGINE_MQCORE_MEMORYCACHE_DBDATA_NOTFOUND;
		st_LockerQuery.unlock_shared();
		return false;
	}
	stl_MapIterator->second.nTimeLast = time(NULL);
	*pSt_DBUserInfo = stl_MapIterator->second.st_DBUserInfo;
	st_LockerQuery.unlock_shared();
	return true;
}
/********************************************************************
函数名称：MemoryCache_DBUser_DataDelete
函数功能：删除高速缓存
 参数.一：pSt_DBUserInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要操作的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CMemoryCache_DBUser::MemoryCache_DBUser_DataDelete(XENGINE_PROTOCOL_USERINFO* pSt_DBUserInfo)
{
	MemoryCache_IsErrorOccur = false;

	if (NULL == pSt_DBUserInfo)
	{
		MemoryCache_IsErrorOccur = true;
		MemoryCache_dwErrorCode = ERROR_XENGINE_MQCORE_MEMORYCACHE_DBDATA_PARAMENT;
		return false;
	}

	st_LockerQuery.lock();
	auto stl_MapIterator = stl_MapQuery.find(pSt_DBUserInfo->tszUserName);
	if (stl_MapIterator != stl_MapQuery.end())
	{
		stl_MapQuery.erase(stl_MapIterator);
	}
	st_LockerQuery.unlock();
	return true;
}
//////////////////////////////////////////////////////////////////////////
//                         线程函数
//////////////////////////////////////////////////////////////////////////
XHTHREAD CALLBACK CMemoryCache_DBUser::DBModule_MQUser_TimeThread(XPVOID lParam)
{
	CMemoryCache_DBUser* pClass_This = (CMemoryCache_DBUser*)lParam;

	std::list<XENGINE_PROTOCOL_USERINFO> stl_ListDelete;
	while (pClass_This->bIsRun)
	{
		time_t nTimeEnd = time(NULL);
		pClass_This->st_LockerQuery.lock_shared();
		for (auto stl_MapIterator = pClass_This->stl_MapQuery.begin(); stl_MapIterator != pClass_This->stl_MapQuery.end(); stl_MapIterator++)
		{
			if ((nTimeEnd - stl_MapIterator->second.nTimeLast) > pClass_This->m_nTimeLast)
			{
				stl_ListDelete.push_back(stl_MapIterator->second.st_DBUserInfo);
			}
			else if (pClass_This->m_nTimeCount > 0 && ((nTimeEnd - stl_MapIterator->second.nTimeLast) > pClass_This->m_nTimeCount))
			{
				stl_ListDelete.push_back(stl_MapIterator->second.st_DBUserInfo);
			}
		}
		pClass_This->st_LockerQuery.unlock_shared();

		for (auto stl_ListIterator = stl_ListDelete.begin(); stl_ListIterator != stl_ListDelete.end(); stl_ListIterator++)
		{
			XENGINE_PROTOCOL_USERINFO st_DBUser = *stl_ListIterator;
			if (pClass_This->MemoryCache_DBUser_DataDelete(&st_DBUser))
			{
				pClass_This->lpCall_MemoryCache(ENUM_MEMORYCACHE_CALLBACK_TYPE_USER_QUERY, true, pClass_This->stl_MapQuery.size(), (XPVOID)&st_DBUser, pClass_This->m_lParam);
			}
			else
			{
				pClass_This->lpCall_MemoryCache(ENUM_MEMORYCACHE_CALLBACK_TYPE_USER_QUERY, false, pClass_This->stl_MapQuery.size(), (XPVOID)&st_DBUser, pClass_This->m_lParam);
			}
		}
		stl_ListDelete.clear();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	return 0;
}