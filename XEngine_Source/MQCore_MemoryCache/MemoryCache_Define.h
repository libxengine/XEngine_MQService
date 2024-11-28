#pragma once
/********************************************************************
//    Created:     2024/11/21  15:09:57
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_MemoryCache\MemoryCache_Define.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_MemoryCache
//    File Base:   MemoryCache_Define
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     导出的函数
//    History:
*********************************************************************/
//////////////////////////////////////////////////////////////////////////
//                       导出的函数
//////////////////////////////////////////////////////////////////////////
extern "C" XLONG MemoryCache_GetLastError(int *pInt_SysError = NULL);
/*************************************************************************
                        高速缓存导出函数
**************************************************************************/
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
extern "C" bool MemoryCache_DBData_Init(int nTimeLast = 3600, int nTimeStart = 0);
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
extern "C" bool MemoryCache_DBData_SetHandle(XNETHANDLE xhDBSQL);
/********************************************************************
函数名称：MemoryCache_DBData_Destory
函数功能：销毁高速缓存管理器
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool MemoryCache_DBData_Destory();
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
extern "C" bool MemoryCache_DBData_DataInsert(XENGINE_DBMESSAGEQUEUE* pSt_DBMessageInfo);
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
extern "C" bool MemoryCache_DBData_DataQuery(XENGINE_DBMESSAGEQUEUE* pSt_DBMessageInfo);
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
extern "C" bool MemoryCache_DBData_DataDelete(XENGINE_DBMESSAGEQUEUE* pSt_DBMessageInfo);
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
extern "C" bool MemoryCache_DBData_QueueInsert(LPCXSTR lpszSQLStr);