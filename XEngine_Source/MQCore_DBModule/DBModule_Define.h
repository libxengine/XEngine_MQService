#pragma once
/********************************************************************
//    Created:     2022/03/15  16:02:36
//    File Name:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule\DBModule_Define.h
//    File Path:   D:\XEngine_MQService\XEngine_Source\MQCore_DBModule
//    File Base:   DBModule_Define
//    File Ext:    h
//    Project:     XEngine(网络通信引擎)
//    Author:      qyt
//    Purpose:     导出的函数
//    History:
*********************************************************************/
typedef struct  
{
	TCHAR tszMsgBuffer[8192];                                             //消息内容
	TCHAR tszQueueName[256];                                              //此消息的KEY
	TCHAR tszQueueLeftTime[64];                                           //过期时间
	TCHAR tszQueuePublishTime[64];                                        //发布时间
	TCHAR tszQueueCreateTime[64];                                         //创建时间
	__int64x nQueueSerial;                                                //包序列号
	__int64x nQueueGetTime;                                               //可以获取的次数
	int nMsgLen;                                                          //消息大小
}XENGINE_DBMESSAGEQUEUE;
//////////////////////////////////////////////////////////////////////////
//                       导出的函数
//////////////////////////////////////////////////////////////////////////
extern "C" DWORD DBModule_GetLastError(int *pInt_SysError = NULL);
/*************************************************************************
                        消息队列导出函数
**************************************************************************/
/********************************************************************
函数名称：DBModule_MQData_Init
函数功能：初始化数据库管理器
 参数.一：pSt_DBConnector
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：数据MYSQL数据库连接信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL DBModule_MQData_Init(DATABASE_MYSQL_CONNECTINFO* pSt_DBConnector);
/********************************************************************
函数名称：DBModule_MQData_Destory
函数功能：销毁数据库管理器
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL DBModule_MQData_Destory();
/********************************************************************
函数名称：DBModule_MQData_Insert
函数功能：插入消息到队列中
 参数.一：pSt_DBInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：要插入的数据信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL DBModule_MQData_Insert(XENGINE_DBMESSAGEQUEUE * pSt_DBInfo);
/********************************************************************
函数名称：DBModule_MQData_Query
函数功能：查询数据
 参数.一：pSt_DBInfo
  In/Out：In/Out
  类型：数据结构指针
  可空：N
  意思：输入查询数据,输出查询到的数据
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL DBModule_MQData_Query(XENGINE_DBMESSAGEQUEUE* pSt_DBInfo);
/********************************************************************
函数名称：DBModule_MQData_CreateTable
函数功能：创建表
 参数.一：lpszQueueName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要创建的表名称
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL DBModule_MQData_CreateTable(LPCTSTR lpszQueueName);
/********************************************************************
函数名称：DBModule_MQData_DeleteTable
函数功能：删除表
 参数.一：lpszQueueName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要操作的名称
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL DBModule_MQData_DeleteTable(LPCTSTR lpszQueueName);
/*************************************************************************
						消息分发导出函数
**************************************************************************/
/********************************************************************
函数名称：DBModule_MQUser_Init
函数功能：初始化数据库管理器
 参数.一：pSt_DBConnector
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：数据MYSQL数据库连接信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL DBModule_MQUser_Init(DATABASE_MYSQL_CONNECTINFO* pSt_DBConnector);
/********************************************************************
函数名称：DBModule_MQUser_Destory
函数功能：销毁数据库管理器
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL DBModule_MQUser_Destory();