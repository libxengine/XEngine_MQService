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
//消息队列
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
//用户消息
typedef struct
{
	TCHAR tszUserName[MAX_PATH];                                          //用户名
	TCHAR tszKeyName[MAX_PATH];                                           //绑定的KEY
	TCHAR tszUPTime[64];                                                  //最后更新时间
	TCHAR tszCreateTime[64];                                              //创建的时间
	__int64x nKeySerial;                                                  //包序列号
}XENGINE_DBUSERKEY;
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
						消息用户导出函数
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
/********************************************************************
函数名称：DBModule_MQUser_UserInsert
函数功能：插入用户
 参数.一：pSt_UserInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要插入的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL DBModule_MQUser_UserInsert(XENGINE_PROTOCOL_USERINFO * pSt_UserInfo);
/********************************************************************
函数名称：DBModule_MQUser_UserQuery
函数功能：查询用户
 参数.一：pSt_UserInfo
  In/Out：In/Out
  类型：数据结构指针
  可空：N
  意思：输入要查询的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL DBModule_MQUser_UserQuery(XENGINE_PROTOCOL_USERINFO * pSt_UserInfo);
/********************************************************************
函数名称：DBModule_MQUser_UserDelete
函数功能：删除用户
 参数.一：pSt_UserInfo
  In/Out：In/Out
  类型：数据结构指针
  可空：N
  意思：输入要删除的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL DBModule_MQUser_UserDelete(XENGINE_PROTOCOL_USERINFO * pSt_UserInfo);
/********************************************************************
函数名称：DBModule_MQUser_UserUPDate
函数功能：更新用户
 参数.一：pSt_UserInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要处理的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL DBModule_MQUser_UserUPDate(XENGINE_PROTOCOL_USERINFO* pSt_UserInfo);
/*************************************************************************
						消息绑定导出函数
**************************************************************************/
/********************************************************************
函数名称：DBModule_MQUser_KeyInsert
函数功能：插入一个绑定的用户消息
 参数.一：pSt_UserKey
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要插入的内容
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL DBModule_MQUser_KeyInsert(XENGINE_DBUSERKEY* pSt_UserKey);
/********************************************************************
函数名称：DBModule_MQUser_KeyQuery
函数功能：通过用户查询绑定的消息信息
 参数.一：pSt_UserKey
  In/Out：In/Out
  类型：数据结构指针
  可空：N
  意思：输入要查询的信息,输出查询到的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL DBModule_MQUser_KeyQuery(XENGINE_DBUSERKEY* pSt_UserKey);
/********************************************************************
函数名称：DBModule_MQUser_KeyDelete
函数功能：删除绑定的消息队列
 参数.一：pSt_UserKey
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要删除的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL DBModule_MQUser_KeyDelete(XENGINE_DBUSERKEY* pSt_UserKey);
/********************************************************************
函数名称：DBModule_MQUser_KeyUPDate
函数功能：更新用户绑定的消息
 参数.一：pSt_UserKey
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要更新的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" BOOL DBModule_MQUser_KeyUPDate(XENGINE_DBUSERKEY* pSt_UserKey);