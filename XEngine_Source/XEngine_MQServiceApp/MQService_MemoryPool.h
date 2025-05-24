#pragma once
/********************************************************************
//    Created:     2025/05/24  10:55:24
//    File Name:   D:\XEngine_MQService\XEngine_Source\XEngine_MQServiceApp\MQService_MemoryPool.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\XEngine_MQServiceApp
//    File Base:   MQService_MemoryPool
//    File Ext:    h
//    Project:     XEngine
//    Author:      qyt
//    Purpose:     内存池封装
//    History:
*********************************************************************/
class CMQService_MemoryPool
{
public:
	CMQService_MemoryPool(size_t nSize)
	{
		lPtr = ManagePool_Memory_Alloc(xhMemPool, nSize);
		if (NULL == lPtr)
		{
			XLOG_PRINT(xhLog, XENGINE_HELPCOMPONENTS_XLOG_IN_LOGLEVEL_ALERT, _X("内存池分配失败，系统面临崩溃!"));
			return;
		}
	}
	~CMQService_MemoryPool()
	{
		if (NULL != lPtr)
		{
			ManagePool_Memory_Free(xhMemPool, lPtr);
		}
	}
public:
	// 支持类型转换
	template<typename T>
	T* as() {
		return static_cast<T*>(lPtr);
	}
	XCHAR* get()
	{
		return static_cast<XCHAR*>(lPtr);
	}
private:
	XPVOID lPtr = NULL;
};