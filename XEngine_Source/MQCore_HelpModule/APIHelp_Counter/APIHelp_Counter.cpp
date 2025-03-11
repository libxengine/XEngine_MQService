#include "pch.h"
#include "APIHelp_Counter.h"
/********************************************************************
//    Created:     2025/03/11  14:49:15
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_HelpModule\APIHelp_Counter\APIHelp_Counter.cpp
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_HelpModule\APIHelp_Counter
//    File Base:   APIHelp_Counter
//    File Ext:    cpp
//    Project:     XEngine
//    Author:      qyt
//    Purpose:     帮助模块计数器
//    History:
*********************************************************************/
CAPIHelp_Counter::CAPIHelp_Counter()
{
}
CAPIHelp_Counter::~CAPIHelp_Counter()
{

}
//////////////////////////////////////////////////////////////////////////
//                         公有函数
//////////////////////////////////////////////////////////////////////////
/********************************************************************
函数名称：MemoryCache_DBData_DataSerialSet
函数功能：设置当前序列号
 参数.一：lpszQueueName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要插入的队列名称
 参数.二：nSerial
  In/Out：In
  类型：整数型
  可空：N
  意思：输入设置的队列序列号
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CAPIHelp_Counter::APIHelp_Counter_SerialSet(LPCXSTR lpszQueueName, __int64x nSerial)
{
	APIHelp_IsErrorOccur = false;

	if (NULL == lpszQueueName)
	{
		APIHelp_IsErrorOccur = true;
		APIHelp_dwErrorCode = ERROR_XENGINE_MQCORE_APIHELP_COUNTER_PARAMENT;
		return false;
	}
	st_Locker.lock();
	stl_MapSerial[lpszQueueName] = nSerial;
	st_Locker.unlock();
	return true;
}
/********************************************************************
函数名称：APIHelp_Counter_SerialGet
函数功能：获取当前序列号
 参数.一：lpszQueueName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要操作的队列名称
 参数.二：pInt_Serial
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出当前队列序列号
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CAPIHelp_Counter::APIHelp_Counter_SerialGet(LPCXSTR lpszQueueName, __int64x *pInt_Serial)
{
	APIHelp_IsErrorOccur = false;

	if (NULL == pInt_Serial)
	{
		APIHelp_IsErrorOccur = true;
		APIHelp_dwErrorCode = ERROR_XENGINE_MQCORE_APIHELP_COUNTER_PARAMENT;
		return false;
	}
	st_Locker.lock_shared();
	auto stl_MapIterator = stl_MapSerial.find(lpszQueueName);
	if (stl_MapIterator == stl_MapSerial.end())
	{
		APIHelp_IsErrorOccur = true;
		APIHelp_dwErrorCode = ERROR_XENGINE_MQCORE_APIHELP_COUNTER_NOTFOUND;
		st_Locker.unlock_shared();
		return false;
	}
	stl_MapIterator->second++;
	*pInt_Serial = stl_MapIterator->second;
	st_Locker.unlock_shared();
	return true;
}
/********************************************************************
函数名称：APIHelp_Counter_SerialDel
函数功能：删除当前队列
 参数.一：lpszQueueName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要操作的队列名称
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
bool CAPIHelp_Counter::APIHelp_Counter_SerialDel(LPCXSTR lpszQueueName)
{
	APIHelp_IsErrorOccur = false;

	st_Locker.lock();
	auto stl_MapIterator = stl_MapSerial.find(lpszQueueName);
	if (stl_MapIterator != stl_MapSerial.end())
	{
		stl_MapSerial.erase(stl_MapIterator);
	}
	st_Locker.unlock();
	return true;
}