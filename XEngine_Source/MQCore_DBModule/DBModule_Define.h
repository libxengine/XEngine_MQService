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
//////////////////////////////////////////////////////////////////////////
//                       导出的回调
//////////////////////////////////////////////////////////////////////////
typedef void(CALLBACK* CALLBACK_MESSAGEQUEUE_MODULE_DATABASE_TIMEPUBLISH)(LPCXSTR lpszQueueName, __int64x nIDMsg, __int64x nIDTime, XPVOID lParam);
//////////////////////////////////////////////////////////////////////////
//                       导出的数据结构
//////////////////////////////////////////////////////////////////////////
//消息队列
typedef struct  
{
	XCHAR tszMsgBuffer[8192];                                             //消息内容
	XCHAR tszUserName[256];                                               //谁发布的消息
	XCHAR tszQueueName[256];                                              //此消息的KEY
	XCHAR tszQueueLeftTime[64];                                           //过期时间
	XCHAR tszQueuePublishTime[64];                                        //发布时间
	XCHAR tszQueueCreateTime[64];                                         //创建时间
	__int64x nQueueSerial;                                                //包序列号
	__int64x nQueueGetTime;                                               //可以获取的次数
	int nMsgLen;                                                          //消息大小
	XBYTE byMsgType;                                                       //消息类型,参考:ENUM_XENGINE_PROTOCOLHDR_PAYLOAD_TYPE
}XENGINE_DBMESSAGEQUEUE;
typedef struct
{
	XCHAR tszQueueName[256];                                              //此消息的KEY
	XCHAR tszCreateTime[64];                                              //创建时间
	__int64x nIDMsg;                                                      //消息ID
	__int64x nIDTime;                                                     //发布时间
	bool bActive;                                                         //是否激活过
}XENGINE_DBTIMERELEASE;
//用户消息
typedef struct
{
	XCHAR tszUserName[MAX_PATH];                                          //用户名
	XCHAR tszKeyName[MAX_PATH];                                           //绑定的KEY
	XCHAR tszUPTime[64];                                                  //最后更新时间
	XCHAR tszCreateTime[64];                                              //创建的时间
	__int64x nKeySerial;                                                  //包序列号
}XENGINE_DBUSERKEY;
//主题所有者
typedef struct
{
	XCHAR tszQueueName[256];                                              //主题名称
	XCHAR tszUserName[256];                                               //主题所有者
	XCHAR tszCreateTime[64];                                              //创建时间
}XENGINE_DBTOPICOWNER;
//////////////////////////////////////////////////////////////////////////
//                       导出的函数
//////////////////////////////////////////////////////////////////////////
extern "C" XLONG DBModule_GetLastError(int *pInt_SysError = NULL);
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
extern "C" bool DBModule_MQData_Init(DATABASE_MYSQL_CONNECTINFO* pSt_DBConnector);
/********************************************************************
函数名称：DBModule_MQData_Destory
函数功能：销毁数据库管理器
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool DBModule_MQData_Destory();
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
extern "C" bool DBModule_MQData_Insert(XENGINE_DBMESSAGEQUEUE * pSt_DBInfo);
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
extern "C" bool DBModule_MQData_Query(XENGINE_DBMESSAGEQUEUE* pSt_DBInfo);
/********************************************************************
函数名称：DBModule_MQData_Modify
函数功能：数据修改
 参数.一：pSt_DBInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要操作的数据
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool DBModule_MQData_Modify(XENGINE_DBMESSAGEQUEUE* pSt_DBInfo);
/********************************************************************
函数名称：DBModule_MQData_List
函数功能：枚举指定主题序列号后的数据
 参数.一：lpszQueueName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要处理的主题
 参数.二：nSerial
  In/Out：In
  类型：整数型
  可空：N
  意思：输入主题序列号
 参数.三：pppSt_DBMessage
  In/Out：Out
  类型：三级指针
  可空：N
  意思：输出数据队列信息
 参数.四：pInt_ListCount
  In/Out：Out
  类型：整数型
  可空：N
  意思：输出数据队列大小
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool DBModule_MQData_List(LPCXSTR lpszQueueName, __int64x nSerial, XENGINE_DBMESSAGEQUEUE*** pppSt_DBMessage, int* pInt_ListCount);
/********************************************************************
函数名称：DBModule_MQData_GetSerial
函数功能：获取序列号
 参数.一：lpszName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要查找的名称
 参数.二：pInt_DBCount
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出统计的个数
 参数.三：pSt_DBStart
  In/Out：Out
  类型：数据结构指针
  可空：N
  意思：开始的记录
 参数.四：pSt_DBEnd
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：结尾的记录
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool DBModule_MQData_GetSerial(LPCXSTR lpszName, __int64x* pInt_DBCount, XENGINE_DBMESSAGEQUEUE* pSt_DBStart, XENGINE_DBMESSAGEQUEUE* pSt_DBEnd);
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
extern "C" bool DBModule_MQData_CreateTable(LPCXSTR lpszQueueName);
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
extern "C" bool DBModule_MQData_DeleteTable(LPCXSTR lpszQueueName);
/********************************************************************
函数名称：DBModule_MQData_ModifyTable
函数功能：修改表名称
 参数.一：lpszSrcTable
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要修改的表名
 参数.二：lpszDstTable
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入修改到的名称
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool DBModule_MQData_ModifyTable(LPCXSTR lpszSrcTable, LPCXSTR lpszDstTable);
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
 参数.二：fpCall_TimePublish
  In/Out：In
  类型：回调函数
  可空：N
  意思：定时消息发布回调函数
 参数.三：lParam
  In/Out：In
  类型：无类型指针
  可空：Y
  意思：回调函数自定义参数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool DBModule_MQUser_Init(DATABASE_MYSQL_CONNECTINFO* pSt_DBConnector, CALLBACK_MESSAGEQUEUE_MODULE_DATABASE_TIMEPUBLISH fpCall_TimePublish, XPVOID lParam = NULL);
/********************************************************************
函数名称：DBModule_MQUser_Destory
函数功能：销毁数据库管理器
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool DBModule_MQUser_Destory();
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
extern "C" bool DBModule_MQUser_UserInsert(XENGINE_PROTOCOL_USERINFO * pSt_UserInfo);
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
extern "C" bool DBModule_MQUser_UserQuery(XENGINE_PROTOCOL_USERINFO * pSt_UserInfo);
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
extern "C" bool DBModule_MQUser_UserDelete(XENGINE_PROTOCOL_USERINFO * pSt_UserInfo);
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
extern "C" bool DBModule_MQUser_UserUPDate(XENGINE_PROTOCOL_USERINFO* pSt_UserInfo);
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
extern "C" bool DBModule_MQUser_KeyInsert(XENGINE_DBUSERKEY* pSt_UserKey);
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
extern "C" bool DBModule_MQUser_KeyQuery(XENGINE_DBUSERKEY* pSt_UserKey);
/********************************************************************
函数名称：DBModule_MQUser_KeyList
函数功能：枚举用户关联的列表
 参数.一：lpszUser
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入用户名
 参数.二：lpszKeyName
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入要查询的KEY名称
 参数.三：pppSt_UserKey
  In/Out：Out
  类型：三级指针
  可空：N
  意思：输出用户列表信息
 参数.四：pInt_ListCount
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：输出列表个数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool DBModule_MQUser_KeyList(LPCXSTR lpszUser, LPCXSTR lpszKeyName, XENGINE_DBUSERKEY * **pppSt_UserKey, int* pInt_ListCount);
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
extern "C" bool DBModule_MQUser_KeyDelete(XENGINE_DBUSERKEY* pSt_UserKey);
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
extern "C" bool DBModule_MQUser_KeyUPDate(XENGINE_DBUSERKEY* pSt_UserKey);
/********************************************************************
函数名称：DBModule_MQUser_KeyTopicUPDate
函数功能：更新队列信息表
 参数.一：lpszSourceTable
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入原始表名
 参数.二：lpszDestTable
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入目标表名
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool DBModule_MQUser_KeyTopicUPDate(LPCXSTR lpszSourceTable, LPCXSTR lpszDestTable);
/*************************************************************************
						定时发布导出函数
**************************************************************************/
/********************************************************************
函数名称：DBModule_MQUser_TimeInsert
函数功能：定时发布插入
 参数.一：pSt_DBInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：要操作的数据信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool DBModule_MQUser_TimeInsert(XENGINE_DBTIMERELEASE * pSt_DBInfo);
/********************************************************************
函数名称：DBModule_MQUser_TimeQuery
函数功能：定时发布查询
 参数.一：pppSt_DBInfo
  In/Out：Out
  类型：三级指针
  可空：N
  意思：导出的列表
 参数.二：pInt_ListCount
  In/Out：Out
  类型：整数型指针
  可空：N
  意思：导出的个数
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool DBModule_MQUser_TimeQuery(XENGINE_DBTIMERELEASE * **pppSt_DBInfo, int* pInt_ListCount);
/********************************************************************
函数名称：DBModule_MQUser_TimeDelete
函数功能：定时发布删除
 参数.一：pSt_DBInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：要操作的数据信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool DBModule_MQUser_TimeDelete(XENGINE_DBTIMERELEASE * pSt_DBInfo);
/********************************************************************
函数名称：DBModule_MQUser_TimeUPDate
函数功能：更新定时发布表
 参数.一：pSt_DBInfo
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：输入要更新的信息
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool DBModule_MQUser_TimeUPDate(XENGINE_DBTIMERELEASE* pSt_DBInfo);
/********************************************************************
函数名称：DBModule_MQUser_TimeClaer
函数功能：清理超时通知
 参数.一：nTime
  In/Out：In
  类型：整数型
  可空：Y
  意思：输入要清理的日期
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool DBModule_MQUser_TimeClaer(time_t nTime = 0);
/********************************************************************
函数名称：DBModule_MQUser_TimeTopicUPDate
函数功能：更新定时发布表
 参数.一：lpszSourceTable
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入原始表名
 参数.二：lpszDestTable
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入目标表名
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool DBModule_MQUser_TimeTopicUPDate(LPCXSTR lpszSourceTable, LPCXSTR lpszDestTable);
/*************************************************************************
						主题所有者导出函数
**************************************************************************/
/********************************************************************
函数名称：DBModule_MQUser_OwnerInsert
函数功能：主题所有者插入
 参数.一：pSt_DBOwner
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：要操作的数据
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool DBModule_MQUser_OwnerInsert(XENGINE_DBTOPICOWNER* pSt_DBOwner);
/********************************************************************
函数名称：DBModule_MQUser_OwnerDelete
函数功能：主题所有者删除
 参数.一：pSt_DBOwner
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：要操作的数据
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool DBModule_MQUser_OwnerDelete(XENGINE_DBTOPICOWNER* pSt_DBOwner);
/********************************************************************
函数名称：DBModule_MQUser_OwnerQuery
函数功能：主题所有者查询
 参数.一：pSt_DBOwner
  In/Out：In
  类型：数据结构指针
  可空：N
  意思：要操作的数据
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool DBModule_MQUser_OwnerQuery(XENGINE_DBTOPICOWNER* pSt_DBOwner);
/********************************************************************
函数名称：DBModule_MQUser_OwnerTopicUPDate
函数功能：更新所有者表
 参数.一：lpszSourceTable
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入原始表名
 参数.二：lpszDestTable
  In/Out：In
  类型：常量字符指针
  可空：N
  意思：输入目标表名
返回值
  类型：逻辑型
  意思：是否成功
备注：
*********************************************************************/
extern "C" bool DBModule_MQUser_OwnerTopicUPDate(LPCXSTR lpszSourceTable, LPCXSTR lpszDestTable);