#include "pch.h"
#include "MemoryCache_DBData.h"
/********************************************************************
//    Created:     2024/11/21  16:35:54
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_MemoryCache\MemoryCache_DBData\MemoryCache_DBData.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_MemoryCache\MemoryCache_DBData
//    File Base:   MemoryCache_DBData
//    File Ext:    cpp
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     数据库高速缓存
//    History:
*********************************************************************/
CMemoryCache_DBData::CMemoryCache_DBData()
{
}
CMemoryCache_DBData::~CMemoryCache_DBData()
{

}
//////////////////////////////////////////////////////////////////////////
//                         公有函数
//////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：MemoryCache_DBData_Init
函数功能：初始化高速缓存管理器
 参数.一：nTimeLast
  In/Out：In
  类型：整数型
  可空：N
  意思：允许的最后更新时间存在秒数
 参数.二：nTimeStart
  In/Out：In
  类型：整数型
  可空：N
  意思：允许的最大保留时间
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CMemoryCache_DBData::MemoryCache_DBData_Init(int nTimeLast /* = 3600 */, int nTimeStart /* = 0 */)
{
    MemoryCache_IsErrorOccur = false;

	bIsRun = true;

	
	m_nTimeLast = nTimeLast;
	m_nTimeStart = nTimeStart;

	pSTDThread_Query = std::make_unique<std::thread>(DBModule_MQUser_TimeThread, this);
	if (NULL == pSTDThread_Query)
	{
		MemoryCache_IsErrorOccur = true;
		MemoryCache_dwErrorCode = ERROR_XENGINE_MQCORE_MEMORYCACHE_DBDATA_THREAD;
		return false;
	}
	pSTDThread_Insert = std::make_unique<std::thread>(DBModule_MQUser_InsertThread, this);
	if (NULL == pSTDThread_Insert)
	{
		MemoryCache_IsErrorOccur = true;
		MemoryCache_dwErrorCode = ERROR_XENGINE_MQCORE_MEMORYCACHE_DBDATA_THREAD;
		return false;
	}
    return true;
}
/********************************************************************
函数名称：MemoryCache_DBData_SetHandle
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
bool CMemoryCache_DBData::MemoryCache_DBData_SetHandle(XNETHANDLE xhDBSQL)
{
	MemoryCache_IsErrorOccur = false;

	m_xhDBSQL = xhDBSQL;

	return true;
}
/********************************************************************
函数名称：MemoryCache_DBData_Destory
函数功能：销毁高速缓存管理器
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CMemoryCache_DBData::MemoryCache_DBData_Destory()
{
    MemoryCache_IsErrorOccur = false;

	bIsRun = false;
	if (NULL != pSTDThread_Query)
	{
		pSTDThread_Query->join();
	}
	if (NULL != pSTDThread_Insert)
	{
		pSTDThread_Insert->join();
	}
    return true;
}
/********************************************************************
函数名称：MemoryCache_DBData_DataInsert
函数功能：插入一条数据到高速缓存
 参数.一：pSt_DBMessageInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要插入的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CMemoryCache_DBData::MemoryCache_DBData_DataInsert(XENGINE_DBMESSAGEQUEUE* pSt_DBMessageInfo)
{
    MemoryCache_IsErrorOccur = false;

	if (NULL == pSt_DBMessageInfo)
	{
		MemoryCache_IsErrorOccur = true;
		MemoryCache_dwErrorCode = ERROR_XENGINE_MQCORE_MEMORYCACHE_DBDATA_PARAMENT;
		return false;
	}

	MEMORYCACHE_DBINFO st_DBInfo = {};

	st_DBInfo.nTimeStart = time(NULL);
	st_DBInfo.st_DBMessageInfo = *pSt_DBMessageInfo;

	std::tuple<__int64x, std::string> stl_Key(pSt_DBMessageInfo->nQueueSerial, pSt_DBMessageInfo->tszQueueName);

	st_LockerQuery.lock();
	auto stl_MapIterator = stl_MapQuery.find(stl_Key);
	if (stl_MapIterator == stl_MapQuery.end())
	{
		stl_MapQuery[stl_Key] = st_DBInfo;
	}
	else
	{
		stl_MapIterator->second.st_DBMessageInfo = *pSt_DBMessageInfo;
	}
	st_LockerQuery.unlock();
    return true;
}
/********************************************************************
函数名称：MemoryCache_DBData_DataQuery
函数功能：查询高速缓存
 参数.一：pSt_DBMessageInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要查询的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CMemoryCache_DBData::MemoryCache_DBData_DataQuery(XENGINE_DBMESSAGEQUEUE* pSt_DBMessageInfo)
{
	MemoryCache_IsErrorOccur = false;

	if (NULL == pSt_DBMessageInfo)
	{
		MemoryCache_IsErrorOccur = true;
		MemoryCache_dwErrorCode = ERROR_XENGINE_MQCORE_MEMORYCACHE_DBDATA_PARAMENT;
		return false;
	}

	std::tuple<__int64x, std::string> stl_Key(pSt_DBMessageInfo->nQueueSerial, pSt_DBMessageInfo->tszQueueName);

	st_LockerQuery.lock_shared();
	auto stl_MapIterator = stl_MapQuery.find(stl_Key);
	if (stl_MapIterator == stl_MapQuery.end())
	{
		MemoryCache_IsErrorOccur = true;
		MemoryCache_dwErrorCode = ERROR_XENGINE_MQCORE_MEMORYCACHE_DBDATA_NOTFOUND;
		st_LockerQuery.unlock_shared();
		return false;
	}
	stl_MapIterator->second.nTimeLast = time(NULL);
	*pSt_DBMessageInfo = stl_MapIterator->second.st_DBMessageInfo;
	st_LockerQuery.unlock_shared();
	return true;
}
/********************************************************************
函数名称：MemoryCache_DBData_DataDelete
函数功能：删除高速缓存
 参数.一：pSt_DBMessageInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要操作的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CMemoryCache_DBData::MemoryCache_DBData_DataDelete(XENGINE_DBMESSAGEQUEUE* pSt_DBMessageInfo)
{
	MemoryCache_IsErrorOccur = false;

	if (NULL == pSt_DBMessageInfo)
	{
		MemoryCache_IsErrorOccur = true;
		MemoryCache_dwErrorCode = ERROR_XENGINE_MQCORE_MEMORYCACHE_DBDATA_PARAMENT;
		return false;
	}

	st_LockerQuery.lock();
	if (-1 == pSt_DBMessageInfo->nQueueSerial)
	{
		for (auto stl_MapIterator = stl_MapQuery.begin(); stl_MapIterator != stl_MapQuery.end(); )
		{
			if (std::get<1>((stl_MapIterator->first)) == pSt_DBMessageInfo->tszQueueName)
			{
				stl_MapIterator = stl_MapQuery.erase(stl_MapIterator);
			}
			else
			{
				stl_MapIterator++;
			}
		}
	}
	else
	{
		std::tuple<__int64x, std::string> stl_Key(pSt_DBMessageInfo->nQueueSerial, pSt_DBMessageInfo->tszQueueName);
		auto stl_MapIterator = stl_MapQuery.find(stl_Key);
		if (stl_MapIterator != stl_MapQuery.end())
		{
			stl_MapQuery.erase(stl_Key);
		}
	}
	st_LockerQuery.unlock();
	return true;
}
/********************************************************************
函数名称：MemoryCache_DBData_QueueInsert
函数功能：队列插入工具
 参数.一：lpszSQLStr
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要插入的语句
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CMemoryCache_DBData::MemoryCache_DBData_QueueInsert(LPCXSTR lpszSQLStr)
{
	MemoryCache_IsErrorOccur = false;

	if (NULL == lpszSQLStr)
	{
		MemoryCache_IsErrorOccur = true;
		MemoryCache_dwErrorCode = ERROR_XENGINE_MQCORE_MEMORYCACHE_DBDATA_PARAMENT;
		return false;
	}

	st_LockerList.lock();
	stl_ListInsert.push_back(lpszSQLStr);
	st_LockerList.unlock();
	return true;
}
//////////////////////////////////////////////////////////////////////////
//                         线程函数
//////////////////////////////////////////////////////////////////////////
XHTHREAD CALLBACK CMemoryCache_DBData::DBModule_MQUser_TimeThread(XPVOID lParam)
{
	CMemoryCache_DBData* pClass_This = (CMemoryCache_DBData*)lParam;

	std::list<XENGINE_DBMESSAGEQUEUE> stl_ListDelete;
	while (pClass_This->bIsRun)
	{
		time_t nTimeEnd = time(NULL);
		pClass_This->st_LockerQuery.lock_shared();
		for (auto stl_MapIterator = pClass_This->stl_MapQuery.begin(); stl_MapIterator != pClass_This->stl_MapQuery.end(); stl_MapIterator++)
		{
			if (stl_MapIterator->second.nTimeLast > (nTimeEnd - pClass_This->m_nTimeLast))
			{
				stl_ListDelete.push_back(stl_MapIterator->second.st_DBMessageInfo);
			}
			else if (pClass_This->m_nTimeStart > 0 && (nTimeEnd - pClass_This->m_nTimeStart))
			{
				stl_ListDelete.push_back(stl_MapIterator->second.st_DBMessageInfo);
			}
		}
		pClass_This->st_LockerQuery.unlock_shared();

		for (auto stl_ListIterator = stl_ListDelete.begin(); stl_ListIterator != stl_ListDelete.end(); stl_ListIterator++)
		{
			XENGINE_DBMESSAGEQUEUE st_DBMessage = *stl_ListIterator;
			pClass_This->MemoryCache_DBData_DataDelete(&st_DBMessage);
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	return 0;
}
XHTHREAD CALLBACK CMemoryCache_DBData::DBModule_MQUser_InsertThread(XPVOID lParam)
{
	CMemoryCache_DBData* pClass_This = (CMemoryCache_DBData*)lParam;

	while (pClass_This->bIsRun)
	{
		if (!pClass_This->stl_ListInsert.empty())
		{
			pClass_This->st_LockerList.lock();
			std::string m_StrSQLInsert = pClass_This->stl_ListInsert.front();
			pClass_This->stl_ListInsert.pop_front();
			pClass_This->st_LockerList.unlock();

			if (!DataBase_MySQL_Execute(pClass_This->m_xhDBSQL, m_StrSQLInsert.c_str()))
			{

			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	return 0;
}